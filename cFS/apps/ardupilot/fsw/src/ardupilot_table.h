#ifndef _ICAROUS_TABLE_H
#define _ICAROUS_TABLE_H

#include <stdint.h>

typedef enum{
    _SOCKET_,_SERIAL_
}PortType;

typedef struct
{
  PortType apPortType; // 0 - socket, 1 - serial
  uint16_t apBaudRate; // baudrate (only for serial)
  uint16_t apPortin;   // input port number (only used for socket)
  uint16_t apPortout;  // output port number (only used for socket)
  char apAddress[50];  // port address (ip address or serial port name)
  PortType gsPortType; // 0 - socket, 1 - serial
  uint16_t gsBaudRate; // baudrate (only for serial)
  uint16_t gsPortin;   // input port number (only used for socket)
  uint16_t gsPortout;  // output port number (only used for socket)
  char gsAddress[50];  // port address (ip address or serial port name)

}ArdupilotTable_t;


#endif
