/**
 * @file ardupilot_table.h
 * @brief Ardupilot app table definition
 * @author Swee Balachandran
 */
#ifndef _APINTERFACE_TABLE_H
#define _APINTERFACE_TABLE_H
#include <stdint.h>
#include "port_lib.h"

/**
 * @defgroup APINTERFACE_TABLES
 * @brief input tables used by the autopilot interface application
 * @details Use this table to define port settings. This table currently provides values
 * that are required for serial/socket connections. Add entries as needed. Values are defined in tables/apIntf_tbl.c
 * You can create as many tables as you want (one for each port you are planning to use).
 * @ingroup TABLES
 */

/**
 * @struct ApInterfaceTable_t
 * @brief table data containing port settings
 * @ingroup APINTERFACE_TABLES
 */

typedef struct
{
  portType_e PortType; /**< connection type to autopilot */
  uint16_t BaudRate;   /**< baudrate (only for serial) */
  uint16_t Portin;     /**< input port number (only used for socket) */
  uint16_t Portout;    /**< output port number (only used for socket) */
  char Address[50];    /**< port address (ip address or serial port name) */
}ApInterfaceTable_t;


#endif
