#ifndef PARAMDEF
#define PARAMDEF
#include <stdint.h>
#define PARAM_COUNT 74      ///< Total number of ICAROUS parameters

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

param_t initialValues[PARAM_COUNT] = 
{
    {"TRAFFIC_SRC",0,10},
    {"RES_TYPE",2,10},
    {"LOGDAADATA",1,10},
    {"LOOKAHEAD_TIME",20.0,10},
    {"LEFT_TRK",180.0,10},
    {"RIGHT_TRK",180.0,10},
    {"MIN_GS",0.2,10},
    {"MAX_GS",30.0,10},
    {"MIN_VS",-1000,10},
    {"MAX_VS",1000,10},
    {"MIN_ALT",0,10},
    {"MAX_ALT",500,10},
    {"TRK_STEP",1,10},
    {"GS_STEP",0.1,10},
    {"VS_STEP",5,10},
    {"ALT_STEP",1,10},
    {"HORIZONTAL_ACCL",0.0,10},
    {"VERTICAL_ACCL",0.0,10},
    {"TURN_RATE",0.0,10},
    {"BANK_ANGLE",0.0,10},
    {"VERTICAL_RATE",0.0,10},
    {"RECOV_STAB_TIME",0.0,10},
    {"MIN_HORIZ_RECOV",30.0,10},
    {"MIN_VERT_RECOV",10.0,10},
    {"RECOVERY_TRK",1,10},
    {"RECOVERY_GS",1,10},
    {"RECOVERY_VS",1,10},
    {"RECOVERY_ALT",1,10},
    {"CA_BANDS",1,10},
    {"CA_FACTOR",0.2,10},
    {"HORIZONTAL_NMAC",5.0,10},
    {"VERTICAL_NMAC",5.5,10},
    {"CONFLICT_CRIT",0,10},
    {"RECOVERY_CRIT",0,10},
    {"CONTOUR_THR",180.0,10},
    {"AL_1_ALERT_T",10.0,10},
    {"AL_1_E_ALERT_T",15.0,10},
    {"AL_1_SPREAD_ALT",0,10},
    {"AL_1_SPREAD_GS",0,10},
    {"AL_1_SPREAD_TRK",0,10},
    {"AL_1_SPREAD_VS",0,10},
    {"CONFLICT_LEVEL",1,10},
    {"DET_1_WCV_DTHR",30.0,10},
    {"DET_1_WCV_TCOA",0.0,10},
    {"DET_1_WCV_TTHR",0.0,10},
    {"DET_1_WCV_ZTHR",200.0,10},
    {"COMMAND",1,10},
    {"TRACKINGOBJID",0,10},
    {"PGAINX",0.5,10},
    {"PGAINY",0.5,10},
    {"PGAINZ",0.5,10},
    {"HEADING",270,10},
    {"DISTH",2,10},
    {"DISTV",2,10},
    {"OBSBUFFER",1,10},
    {"MAXCEILING",50,10},
    {"ASTAR_ENABLE3D",0,10},
    {"ASTAR_GRIDSIZE",10.0,10},
    {"ASTAR_RESSPEED",1.0,10},
    {"ASTAR_LOOKAHEAD",5.0,10},
    {"RRT_RESSPEED",1.0,10},
    {"RRT_NITERATIONS",2000,10},
    {"RRT_DT",1.0,10},
    {"RRT_MACROSTEPS",5,10},
    {"RRT_CAPR",5.0,10},
    {"XTRKDEV",1000.0,10},
    {"XTRKGAIN",0.6,10},
    {"RESSPEED",1.0,10},
    {"SEARCHALGORITHM",1,10},
    {"LOOKAHEAD",4.0,10},
    {"HTHRESHOLD",2.0,10},
    {"VTHRESHOLD",2.0,10},
    {"HSTEPBACK",20.0,10},
    {"VSTEPBACK",20.0,10},
};
#else
extern uint8_t sysid_ap;
extern uint8_t compid_ap;

extern uint8_t sysid_ic;
extern uint8_t compid_ic;

extern uint8_t sysid_gs;
extern uint8_t compid_gs;

extern param_t initialValues[PARAM_COUNT];
#endif


#endif