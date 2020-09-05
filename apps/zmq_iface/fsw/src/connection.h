#include <zmq.h>

#include "Icarous.h"
#include "traffic_msg.h"

#define MAX_ZMQ_MESSAGE_SIZE  600000

#define TELEMETRY_SERVER_ADDR "tcp://*:5556"
#define COMMAND_SERVER_ADDR   "tcp://*:5557"

/**
 * @struct ZMQ_IFACE_Connection_t
 * @brief ZMQ Connection
 */
typedef struct {
    void *context;
    void *telemetrySocket;
    void *commandSocket;
    bool started;
    char msgBuffer[MAX_ZMQ_MESSAGE_SIZE];
    callsign_t callSign;
} ZMQ_IFACE_Connection_t;


void ZMQ_IFACE_InitZMQServices(ZMQ_IFACE_Connection_t * const conn);
void ZMQ_IFACE_SendTelemetry(ZMQ_IFACE_Connection_t * const conn, char const * const msg);
void ZMQ_IFACE_SendAlertReport(ZMQ_IFACE_Connection_t * const conn, traffic_alerts_t const * const cfsAlerts);
void ZMQ_IFACE_SendBandReport(ZMQ_IFACE_Connection_t * const conn, band_report_t const * const cfsBands);
bool ZMQ_IFACE_ReceiveCommand(ZMQ_IFACE_Connection_t * const conn, char * const buffer, size_t size);
void ZMQ_IFACE_SendEUTL(ZMQ_IFACE_Connection_t * const conn, stringdata_t * msg);
