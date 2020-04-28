#include <zmq.h>

#include "Icarous.h"
#include "traffic_msg.h"

#define MAX_PUB_CHAR 10000
#define MAX_INTR_NAME_CHAR 100
#define RECV_BUFFER_SIZE 600000

#define TCP_SERVER_ADDR "tcp://*:5556"
#define RESPONSE_ADDR "localhost:5560"
#define TCP_RESPONSE_ADDR "tcp://*:5560"

/**
 * @struct ZMQ_IFACE_Connection_t
 * @brief ZMQ Connection
 */
typedef struct {
    void *context;
    void *telemetrySocket;
    bool started;
    char msgBuffer[RECV_BUFFER_SIZE];
    callsign_t callSign;
} ZMQ_IFACE_Connection_t;


void ZMQ_IFACE_InitZMQServices(ZMQ_IFACE_Connection_t * const conn);
void ZMQ_IFACE_SendTelemetry(ZMQ_IFACE_Connection_t * const conn, char const * const msg);
void ZMQ_IFACE_SendAlertReport(ZMQ_IFACE_Connection_t * const conn, traffic_alerts_t const * const cfsAlerts);
void ZMQ_IFACE_SendBandReport(ZMQ_IFACE_Connection_t * const conn, band_report_t const * const cfsBands);

