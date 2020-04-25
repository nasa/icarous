#include <json-c/json.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "cfe.h"

#include "connection.h"
#include "utils.h"

/*
 * ZMQ C helper functions
 *
 * from: https://github.com/booksbyus/zguide/blob/master/examples/C/zhelpers.h
 *
 */
static int s_send(void * const socket, char const * const string);
static int s_recv(void * const socket, char       * const buffer);

static int s_send (void * const socket, char const * const string)
{
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}

static int s_recv (void * const socket, char * const buffer)
{
    int size = zmq_recv(socket, buffer, RECV_BUFFER_SIZE-1, 0);
    buffer[size] = '\0';
    if (size == -1)
        return 1;
    return 0;
}


/*
 * ZMQ Connection initialization
 *
 */
void ZMQ_IFACE_StartTelemetryServer(ZMQ_IFACE_Connection_t* conn);

void ZMQ_IFACE_InitZMQServices(ZMQ_IFACE_Connection_t * const conn)
{
    InitializeAircraftCallSign(conn->callSign.value);
    ZMQ_IFACE_StartTelemetryServer(conn);
    conn->started = true;
    APP_DEBUG("ZMQ Services successfully initialized\n");
}

void ZMQ_IFACE_StartTelemetryServer(ZMQ_IFACE_Connection_t * const conn)
{
    conn->context = zmq_ctx_new ();
    conn->telemetrySocket = zmq_socket (conn->context, ZMQ_PUB);
    int rc = zmq_bind (conn->telemetrySocket, TCP_SERVER_ADDR);
    assert (rc == 0);
}


/*
 * ZMQ Send generic telemetry messages
 *
 */
void ZMQ_IFACE_SendTelemetry(ZMQ_IFACE_Connection_t * const conn, char const * const msg)
{
    APP_DEBUG("Sending telemetry message: %s...\n", msg);
    s_send(conn->telemetrySocket, msg);
    APP_DEBUG("...sent!\n");
}


/*
 * ZMQ Send Alert Report
 *
 */
static struct json_object * BuildAlertReportJSON(callsign_t const callSign, traffic_alerts_t const * const cfsAlerts);
static struct json_object * BuildAlertJSONArray(traffic_alerts_t const * const cfsAlerts);
static struct json_object * BuildAlertJSON(callsign_t const callSign, double time, int32_t level);

void ZMQ_IFACE_SendAlertReport(ZMQ_IFACE_Connection_t * const conn, traffic_alerts_t const * const cfsAlerts)
{
    struct json_object * alertReport = BuildAlertReportJSON(conn->callSign, cfsAlerts);
    char const * const msg = json_object_to_json_string_ext(alertReport, JSON_C_TO_STRING_PLAIN);
    APP_DEBUG("Sending Alerts message: %s...\n", msg);
    s_send(conn->telemetrySocket, msg);
    APP_DEBUG("...sent!\n");
    json_object_put(alertReport);
}

static struct json_object * BuildAlertReportJSON(callsign_t const callSign, traffic_alerts_t const * const cfsAlerts)
{
    struct json_object * report = json_object_new_object();
    json_object_object_add(report, "type", json_object_new_string("AlertReport"));
    json_object_object_add(report, "callSign", json_object_new_string(callSign.value));
    json_object_object_add(report, "alerts", BuildAlertJSONArray(cfsAlerts));
    return report;
}

static struct json_object * BuildAlertJSONArray(traffic_alerts_t const * const cfsAlerts)
{
    struct json_object * alertsArray = json_object_new_array();
    for (size_t i = 0; i < cfsAlerts->numAlerts; i++) {
        json_object_array_add(
            alertsArray,
            BuildAlertJSON(
                cfsAlerts->callsign[i],
                cfsAlerts->time,
                cfsAlerts->trafficAlerts[i]
            )
        );
    }
    return alertsArray;
}

static struct json_object * BuildAlertJSON(callsign_t const callSign, double time, int32_t level)
{
    struct json_object * alert = json_object_new_object();
    json_object_object_add(alert, "aircraft_id", json_object_new_string(callSign.value));
    json_object_object_add(alert, "time",        json_object_new_double(time));
    json_object_object_add(alert, "level",       json_object_new_int(level));
    return alert;
}

