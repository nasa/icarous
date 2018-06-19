/**
 * @file ardupilot_table.h
 * @brief Ardupilot app table definition
 */
#ifndef _ICAROUS_TABLE_H
#define _ICAROUS_TABLE_H
#define EXTERN extern
#include <stdint.h>
#include "ardupilot.h"

/**
 * @defgroup ARDUPILOT_TABLES
 * Input parameters for ardupilot applications.
 * @ingroup TABLES
 */

/**
 * @struct ArdupilotTable_t
 * @brief Input parameters for ardupilot application
 * @ingroup ARDUPILOT_TABLES
 */
typedef struct
{
  portType_e apPortType; ///< connection type to autopilot
  uint16_t apBaudRate;   ///< baudrate (only for serial)
  uint16_t apPortin;     ///< input port number (only used for socket)
  uint16_t apPortout;    ///< output port number (only used for socket)
  char apAddress[50];    ///< port address (ip address or serial port name)
  portType_e gsPortType; ///< connection type to ground station
  uint16_t gsBaudRate;   ///< baudrate (only for serial)
  uint16_t gsPortin;     ///< input port number (only used for socket)
  uint16_t gsPortout;    ///< output port number (only used for socket)
  char gsAddress[50];    ///< port address (ip address or serial port name)

}ArdupilotTable_t;


#endif
