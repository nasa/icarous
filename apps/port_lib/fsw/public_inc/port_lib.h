/**
 * @file port_lib.h
 * @brief serial/socket port library
 */
#ifndef _port_lib_h_
#define _port_lib_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>   // File control definitions
#include <termios.h> // POSIX terminal control definitions
#include "network_includes.h"
#include "mavlink/ardupilotmega/mavlink.h"

#define BUFFER_LENGTH 1000  ///< Mavlink message receive buffer size

/**
 * @enum PortType_t
 * @brief Port type
 */
typedef enum {
    SOCKET,  ///< enum value SOCKET
    SERIAL   ///< enum value SERIAL
} portType_e;


/**
 *\struct port_t
 * @brief Structure to hold port attributes
 */
typedef struct{
    int id;                          ///< id
    portType_e portType;             ///< port type
    struct sockaddr_in target_addr;  ///< target address
    struct sockaddr_in self_addr;    ///< self address
    socklen_t recvlen;               ///< length of received host properties
    int sockId;                      ///< socket id
    int portin;                      ///< input socket
    int portout;                     ///< output socket
    char target[50];                 ///< target ip address/or name of serial port
    char recvbuffer[BUFFER_LENGTH];  ///< buffer for incoming data
    int baudrate;                    ///< baud rate only if a serial port
}port_t;

/**
 * Initialize port library
 * @return
 */
int32 Port_LibInit(void);

/**
 * Initialize a socket port
 * @param *prt pointer to port
 */
void InitializeSocketPort(port_t *prt);

/**
 * Initialize a serial port
 * @param *prt pointer to port
 */
int InitializeSerialPort(port_t* prt,bool should_block);

/**
 * Write mavlink message to a given port
 * @param *prt pointer to output port
 * @param *message pointer to mavlink message
 */
void writeMavlinkData(port_t* prt,mavlink_message_t *message);

/**
 * Read raw data from port
 * @param prt pointer to port to read from
 */
int readPort(port_t *prt);

/**
 * Write raw data to port
 * @param prt  pointer to port to read data from
 * @param sendbuffer pointer to char array contiaining data
 * @param datalength length of data to be used
 */
void writeData(port_t* prt,char* sendbuffer,int datalength);

#endif

