#ifndef PARAMDEF
#define PARAMDEF
#include <stdint.h>

#include "Icarous_msg.h"
#include "Icarous_msgids.h"


#define PARAM_COUNT 97    ///< Total number of ICAROUS parameters

#define MAX_PARAMETER_NAME_SIZE 17

typedef struct{
    char param_id[MAX_PARAMETER_NAME_SIZE];
    float value;
    uint8_t type;
}param_t;

#define MAX_ADSB_CALLSIGN_SIZE 9

typedef struct{
   char value[MAX_ADSB_CALLSIGN_SIZE];
} adsb_callsign;

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
int GetParams(char *filename, size_t numOfPairs, char (*params)[MAX_PARAMETER_NAME_SIZE],char (*val)[MAX_PARAMETER_NAME_SIZE]);
bool InitializeParams(char *filename,param_t* params,uint16_t paramCount);
char const * GetAircraftCallSign(void);
void adsb_callsign_from_callsign_t(adsb_callsign * dest, callsign_t * orig);
void callsign_t_zero(callsign_t * dest);
char const * callsign_t_get(callsign_t * dest);
void callsign_t_set(callsign_t * dest, char const * const source);
bool InitializeAircraftCallSign(callsign_t * callsign);
bool InitializePortConfig(char* filename,void* prt);

#ifdef APPDEF_TRAFFIC
void ConstructDAAParamString(void* tfparam,char *params);
#endif


#endif
