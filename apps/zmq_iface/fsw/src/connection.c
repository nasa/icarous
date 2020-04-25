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
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}

static int s_recv (void * const socket, char * const buffer)
{
    int size = zmq_recv (socket, buffer, RECV_BUFFER_SIZE-1, 0);
    buffer[size] = '\0';
    if (size == -1)
        return 1;
    return 0;
}


/*
 * ZMQ Connection functions
 *
 */

void ZMQ_IFACE_StartTelemetryServer(ZMQ_IFACE_Connection_t* conn);

void ZMQ_IFACE_InitZMQServices(ZMQ_IFACE_Connection_t * const conn)
{
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

void ZMQ_IFACE_SendTelemetryMsg(ZMQ_IFACE_Connection_t * const conn, char const * const msg)
{
    APP_DEBUG("Sending telemetry message: %s...\n", msg);
    s_send(conn->telemetrySocket, msg);
    APP_DEBUG("...sent!\n");
}
