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


/*
 * ZMQ Send Kinematic Band Report
 */

static struct json_object * BuildBandReportJSON(callsign_t const callSign, band_report_t const * const cfsBands);
static struct json_object * BuildBandJSON(bands_t const * const cfsBand, char const * const units, const double min, const double max);
static struct json_object * BuildBandRangesJSON(bands_t const * const cfsBand);
static char const * const GetRegionName(enum Region region);

void ZMQ_IFACE_SendBandReport(ZMQ_IFACE_Connection_t * const conn, band_report_t const * const bands)
{
    struct json_object * bandReport = BuildBandReportJSON(conn->callSign, bands);
    char const * const msg = json_object_to_json_string_ext(bandReport, JSON_C_TO_STRING_PLAIN);
    APP_DEBUG("Sending Band Report message: %s...\n", msg);
    s_send(conn->telemetrySocket, msg);
    APP_DEBUG("...sent!\n");
    json_object_put(bandReport);
}

static struct json_object * BuildBandReportJSON(callsign_t const callSign, band_report_t const * const cfsBands)
{
    struct json_object * report = json_object_new_object();
    json_object_object_add(report, "type", json_object_new_string("BandReport"));
    json_object_object_add(report, "callSign", json_object_new_string(callSign.value));
    json_object_object_add(report, "time", json_object_new_string_from_double(cfsBands->trackBands.time));
    json_object_object_add(report, "HorizontalDirection", BuildBandJSON(&cfsBands->trackBands, "deg", 0, 360));
    json_object_object_add(report, "HorizontalSpeed", BuildBandJSON(&cfsBands->groundSpeedBands, "m/s", 0, 1000));
    json_object_object_add(report, "VerticalSpeed", BuildBandJSON(&cfsBands->verticalSpeedBands, "m/s", 0, 1000));
    json_object_object_add(report, "Altitude", BuildBandJSON(&cfsBands->altitudeBands, "m", 0, 20000));
    return report;
}

static struct json_object * BuildBandJSON(bands_t const * const cfsBand, char const * const units, const double min, const double max)
{
    struct json_object * band = json_object_new_object();
    json_object_object_add(band, "units", json_object_new_string(units));
    json_object_object_add(band, "min", json_object_new_double(min));
    json_object_object_add(band, "max", json_object_new_double(max));
    json_object_object_add(band, "ranges", BuildBandRangesJSON(cfsBand));
    return band;
}

static struct json_object * BuildBandRangesJSON(bands_t const * const cfsBand)
{
    struct json_object * ranges = json_object_new_array();
    struct json_object * range = NULL;
    for (size_t i = 0; i < cfsBand->numBands; i++) {
        range = json_object_new_object();
        json_object_object_add(range, "region", json_object_new_string(GetRegionName(cfsBand->type[i])));
        json_object_object_add(range, "lower_bound", json_object_new_double(cfsBand->min[i]));
        json_object_object_add(range, "upper_bound", json_object_new_double(cfsBand->max[i]));
        json_object_array_add(ranges, range);
    }
    return ranges;
}

static char const * const GetRegionName(enum Region region) // TODO: Possibly this should be an Icarouslib function
{
    static char const * const NAMES[END_OF_REGION] = {
        "UNKNOWN",
        "NONE",
        "FAR",
        "MID",
        "NEAR",
        "RECOVERY"
    };
    return NAMES[region];
}

