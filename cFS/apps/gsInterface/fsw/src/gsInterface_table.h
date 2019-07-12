/**
 * @file ardupilot_table.h
 * @brief Ardupilot app table definition
 */
#ifndef _GSINTERFACE_TABLE_H
#define _GSINTERFACE_TABLE_H
#include <stdint.h>
#include "gsInterface.h"

/**
 * @defgroup GSINTERFACE_TABLES
 * Input parameters for ardupilot applications.
 * @ingroup TABLES
 */

/**
 * @struct gsInterfaceTable_t
 * @brief Input parameters for ardupilot application
 * @ingroup ARDUPILOT_TABLES
 */
typedef struct
{
  portType_e PortType; ///< connection type to autopilot
  uint64_t BaudRate;   ///< baudrate (only for serial)
  uint16_t Portin;     ///< input port number (only used for socket)
  uint16_t Portout;    ///< output port number (only used for socket)
  char Address[50];    ///< port address (ip address or serial port name)
}gsInterfaceTable_t;


#endif
