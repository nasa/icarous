//
// Created by Swee Balachandran on 11/13/17.
//

#define EXTERN

#include <cfs-data-format.hh>
#include <msgdef/ardupilot_msg.h>
#include "plexil.h"
#include "msgids/ardupilot_msgids.h"
#include "plexil_tbl.c"

CFE_EVS_BinFilter_t  PLEXIL_EventFilters[] =
        {  /* Event ID    mask */
                {PLEXIL_STARTUP_INF_EID,       0x0000},
                {PLEXIL_COMMAND_ERR_EID,       0x0000},
        }; /// Event ID definitions

/* Application entry points */
void PLEXIL_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    PLEXIL_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){

        // Receive plexil scheduler message and command/lookup returns
        status = CFE_SB_RcvMsg(&plexilAppData.PLEXIL_MsgPtr, plexilAppData.PLEXIL_Pipe,10);

        // run actions on wakeup
        if (status == CFE_SUCCESS)
        {
            PLEXIL_ProcessPacket(false);
        }

        // Lets process several messages in the other pipe
        for(int i=0;i<20;i++){
            status = CFE_SB_RcvMsg(&plexilAppData.DATA_MsgPtr, plexilAppData.DATA_Pipe,1);

            if (status == CFE_SUCCESS)
            {
                PLEXIL_ProcessPacket(true);
            }
        }
    }

    PLEXIL_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void PLEXIL_AppInit(void) {

    memset(&plexilAppData, 0, sizeof(plexilAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(PLEXIL_EventFilters,
                     sizeof(PLEXIL_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages


    status = CFE_SB_CreatePipe(&plexilAppData.PLEXIL_Pipe, /* Variable to hold Pipe ID */
                               PLEXIL_PIPE_DEPTH,              /* Depth of Pipe */
                               PLEXIL_PIPE_NAME);          /* Name of pipe */


    status = CFE_SB_CreatePipe(&plexilAppData.DATA_Pipe, /* Variable to hold Pipe ID */
                               PLEXIL_PIPE_DEPTH,          /* Depth of Pipe */
                               PLEXIL_DATA_PIPE_NAME);          /* Name of pipe */

    //Subscribe to command messages and kinematic band messages from the SB
    //Subscribe to scheduler messages
    CFE_SB_Subscribe(FREQ_10_WAKEUP_MID, plexilAppData.PLEXIL_Pipe);

    //Subscribe to command/lookup return messages other data messages
    //command/lookup return messages should be subscribed in the PLEXIL_Pipe and
    //the reset in data pipes.
    PLEXIL_CustomSubscription();

    // Send event indicating app initialization
    CFE_EVS_SendEvent(PLEXIL_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Plexil Initialized. Version %d.%d",
                      PLEXIL_MAJOR_VERSION,
                      PLEXIL_MINOR_VERSION);


    // Register table with table services
    status = CFE_TBL_Register(&plexilAppData.PLEXIL_tblHandle,
                              "PlexilTable",
                              sizeof(PLEXILTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &PlexilTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(plexilAppData.PLEXIL_tblHandle, CFE_TBL_SRC_ADDRESS,&Plexil_TblStruct);


    PLEXILTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, plexilAppData.PLEXIL_tblHandle);

    // copy data from table here
    int argc;
    char **inputParams;//[7][20];

    inputParams = (char **) malloc(7 * sizeof(char *));
    for (int i = 0; i < 7; i++) {
        inputParams[i] = (char *) malloc(50 * sizeof(char));
    }

    argc = TblPtr->argc;
    memset(inputParams[0], 0x0, sizeof(char) * 50);
    memcpy(inputParams[1], TblPtr->argv1, sizeof(TblPtr->argv1));
    memcpy(inputParams[2], TblPtr->argv2, sizeof(TblPtr->argv2));
    memcpy(inputParams[3], TblPtr->argv3, sizeof(TblPtr->argv3));
    memcpy(inputParams[4], TblPtr->argv4, sizeof(TblPtr->argv4));
    memcpy(inputParams[5], TblPtr->argv5, sizeof(TblPtr->argv5));
    memcpy(inputParams[6], TblPtr->argv6, sizeof(TblPtr->argv6));

    //argv = TblPtr->argv;

    //printf("argv %s\n",argv[1]);

    plexil_init(argc, inputParams, &plexilAppData.exec, &plexilAppData.adap);

    // Free table pointer
    status = CFE_TBL_ReleaseAddress(plexilAppData.PLEXIL_tblHandle);

    for (int i = 0; i < 7; i++) {
        free(inputParams[i]);
    }

    free(inputParams);

    PLEXIL_InitializeCustomData();
}



void PLEXIL_AppCleanUp(){
    // Do clean up here
    plexil_destroy(plexilAppData.exec);
}

void PLEXIL_ProcessPacket(bool data){

    CFE_SB_MsgId_t  MsgId;
    if(data) {
        MsgId = CFE_SB_GetMsgId(plexilAppData.DATA_MsgPtr);
    }else{
        MsgId = CFE_SB_GetMsgId(plexilAppData.PLEXIL_MsgPtr);
    }

    switch(MsgId){

        // Process the default packet
        case FREQ_10_WAKEUP_MID:
            PLEXIL_Run();
            break;
        // Pass everything else to the user defined function
        default:
            PLEXIL_ProcessCustomPackets(data);
            break;
    }
}

void PLEXIL_Run(){
    int n;
    plexil_run(plexilAppData.exec);

    n = 1;

    while(n>0){
        PlexilMsg msg1;
        memset(&msg1,0,sizeof(msg1));
        n = plexil_getCommand(plexilAppData.adap,&msg1);

        if(n>=0) {
            PLEXIL_HandleCustomCommands(&msg1);
        }
    }

    n = 1;
    while(n>0){
        PlexilMsg msg2;
        n = plexil_getLookup(plexilAppData.adap,&msg2);
        if(n>=0) {
            PLEXIL_HandleCustomLookups(&msg2);
        }
    }
}

int32_t PlexilTableValidationFunc(void *TblPtr){

    int32_t status = 0;

    return status;
}