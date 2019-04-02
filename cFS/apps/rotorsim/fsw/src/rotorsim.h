/**
 * @file rotorsim.h
 * @brief Definition of defines, structs and functions for rotorsim monitoring app
 *
 * @author Swee Balachandran
 *
 */


#ifndef ICAROUS_CFS_ROTORSIM_H
#define ICAROUS_CFS_ROTORSIM_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>
#include "rotorsim_table.h"

#include "msgids/ardupilot_msgids.h"
#include "msgdef/ardupilot_msg.h"
#include "msgids/scheduler_msgids.h"

#define ROTORSIM_STARTUP_INF_EID 0
#define ROTORSIM_COMMAND_ERR_EID 1

#define ROTORSIM_PIPE_DEPTH 100
#define ROTORSIM_PIPE_NAME "ROTORSIM_PIPE"
#define ROTORSIM_MAJOR_VERSION 1
#define ROTORSIM_MINOR_VERSION 0

/**
 * @defgroup ROTORSIM
 * @brief application to simulate rotorcraft
 * @ingroup APPLICATIONS
 *
 * @details This application simulated a rotorocraft using the NASA Langley high fidelity model
 *
 * @see
 */

/**
 * @struct rotorsimAppData_t
 * @brief  Struct to hold rotor sim app data
 */
typedef struct{
    CFE_SB_PipeId_t    Rotorsim_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Rotorsim_MsgPtr;     ///< Pointer to SB message
    CFE_TBL_Handle_t   Rotorsim_tblHandle;  ///< Table handle
    RotorsimTable_t Rotorsim_Tbl;           ///< Rotor sim table;
    double externalCmdV[3];                 ///< External velocity command (N,E,D);
    double externalCmdP[3];                 ///< External position command;
    double position[3];                     ///< Current lat[deg], lon[deg], alt[m] position
    double velocity[3];                     ///< Current velocity: N, E ,D
    double attitude[3];                     ///< Current attitude
    double heading;
    bool passive;                           ///< Passive mode
    bool startMission;                      ///< Start mission
    bool positionControl;                   ///< position control mode
    bool velocityControl;                   ///< velocity control mode
    bool flightplanSent;                    ///< flight plan send status
    int nextWP;                             ///< Next waypoint index
    flightplan_t flightPlan;                ///< Input flight plan
    uint32_t timerId;                       ///< Timer id
    double time;                            ///< time
    double offsetdist;                      ///< Moving reference point on controller
}rotorsimAppData_t;

/**
 * App's main entry point
 */
void Rotorsim_AppMain(void);

/**
 * App initialization
 */
void Rotorsim_AppInit(void);

/**
 * App clean up
 */
void Rotorsim_AppCleanUp();

/**
 * Function to process received packets
 */
void Rotorsim_ProcessPacket();

/**
 * @var rotorsimAppData
 * @brief global variable to hold app data
 */
rotorsimAppData_t rotorsimAppData;

/**
 * Function to validate table parameters
 * @param TblPtr pointer to table
 * @return 0 if success
 */
int32_t RotorsimTableValidationFunc(void *TblPtr);

void Rotorsim_InitModules(RotorsimTable_t* params);

void Rotorsim_GetInputs();

void Rotorsim_GetOutputs();

void timer_callback(uint32_t timerId);

bool ComputeOffSetPositionOnPlan(double position[],int currentLeg,double output[]);

void GetCorrectIntersectionPoint(double _wpA[],double _wpB[],double heading,double r,double output[]);

double distance(double x1,double y1,double x2,double y2);

#endif //ICAROUS_CFS_ROTORSIM_H