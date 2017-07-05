#ifndef _ICAROUS_TABLE_H
#define _ICAROUS_TABLE_H

#include <stdint.h>

typedef struct
{
  uint8_t apPortType; // 0 - socket, 1 - serial
  uint16_t apPortin;  // port number
  uint16_t apPortout;
  char apAddress[50]; // port address (ip address or serial port name)
  uint8_t gsPortType; // 0 - socket, 1 - serial
  uint16_t gsPortin;
  uint16_t gsPortout;
  char gsAddress[50];

}InterfaceTable_t;


#endif
