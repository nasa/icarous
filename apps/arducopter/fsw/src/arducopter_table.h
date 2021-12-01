/**
 * @file arducopter_table.h
 * @brief ArduCopter app table definition
 */
#ifndef _ARDUCOPTER_TABLE_H
#define _ARDUCOPTER_TABLE_H
#include <stdint.h>
#include "port_lib.h"

/**
 * @struct ArducopterTable_t
 * @brief Input parameters for ardupilot application
 */
typedef struct
{
  portType_e PortType;       ///< connection type to autopilot
  uint64_t BaudRate;         ///< baudrate (only for serial)
  uint16_t Portin;           ///< input port number (only used for socket)
  uint16_t Portout;          ///< output port number (only used for socket)
  char Address[50];          ///< port address (ip address or serial port name)
  uint8_t icRcChannel;       ///< Channel for ICAROUS modes
  uint32_t pwmStart;         ///< start signal pwm (values should be 600 units apart) 
  uint32_t pwmReset;         ///< reset signal pwm (values should be 600 units apart) 
}ArducopterTable_t;


#endif
