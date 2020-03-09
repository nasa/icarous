#ifndef PARAMDEF
#define PARAMDEF
#include <stdint.h>

#include "Icarous_msg.h"
#include "Icarous_msgids.h"
#include "port_lib.h"

#define PARAM_COUNT 94     ///< Total number of ICAROUS parameters

typedef struct{
    char param_id[16];
    float value;
    uint8_t type;
}param_t;

#ifdef INIT_PARAM

uint8_t sysid_ap = 1;
uint8_t compid_ap = 1;

uint8_t sysid_ic = 1;
uint8_t compid_ic = 5;

uint8_t sysid_gs = 255;
uint8_t compid_gs = 0;

#else
extern uint8_t sysid_ap;
extern uint8_t compid_ap;

extern uint8_t sysid_ic;
extern uint8_t compid_ic;

extern uint8_t sysid_gs;
extern uint8_t compid_gs;

#endif

void PublishParams(param_t *params);
int ReadFlightplanFromFile(char* filename,flightplan_t *fplan);
int GetParams(char *filename, char (*params)[16],char (*val)[16]);
bool InitializeParams(char *filename,param_t* params,uint16_t paramCount);
bool InitializeAircraftCallSign(char* callsign);
bool InitializePortConfig(char* filename,port_t* prt);
#endif