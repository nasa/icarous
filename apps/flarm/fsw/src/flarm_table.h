/**
 * @file ardupilot_table.h
 * @brief Ardupilot app table definition
 * @author Swee Balachandran
 */
#ifndef _FLARM_TABLE_H
#define _FLARM_TABLE_H
#include <stdint.h>
#include "port_lib.h"


/**
 * @struct FlarmTable_t
 * @brief table data containing port settings
 */

typedef struct
{
  portType_e PortType; /**< connection type to autopilot */
  uint16_t BaudRate;   /**< baudrate (only for serial) */
  uint16_t Portin;     /**< input port number (only used for socket) */
  uint16_t Portout;    /**< output port number (only used for socket) */
  char Address[50];    /**< port address (ip address or serial port name) */
}FlarmTable_t;


#endif
