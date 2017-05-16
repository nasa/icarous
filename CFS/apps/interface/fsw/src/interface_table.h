#ifndef _ICAROUS_TABLE_H
#define _ICAROUS_TABLE_H

#include <stdint.h>

typedef struct
{
  uint8_t apPortType; // 0 - socket, 1 - serial
  uint16_t apPort;     // port number
  char apAddress[50]; // port address (ip address or serial port name)
  uint8_t gsPortType; // 0 - socket, 1 - serial
  uint16_t gsPort;
  char gsAddress[50];

}InterfaceTable_t;

int32 IcarousTableValidationFunc(void *TblPtr){

  int32 status = 0;

  return status;
}


#endif
