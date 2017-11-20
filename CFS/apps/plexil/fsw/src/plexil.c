//
// Created by Swee Balachandran on 11/13/17.
//

#define EXTERN
#include "plexil.h"

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
        status = CFE_SB_RcvMsg(&plexilAppdata.PLEXIL_MsgPtr, plexilAppdata.PLEXIL_Pipe, 1);

        if (status == CFE_SUCCESS)
        {
            PLEXIL_ProcessPacket();
        }
    }


    PLEXIL_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void PLEXIL_AppInit(void){

    memset(&plexilAppdata,0,sizeof(plexilAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(PLEXIL_EventFilters,
                     sizeof(PLEXIL_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe( &plexilAppdata.PLEXIL_Pipe, /* Variable to hold Pipe ID */
                                PLEXIL_PIPE_DEPTH,       /* Depth of Pipe */
                                PLEXIL_PIPE_NAME);       /* Name of pipe */


    //Subscribe to command messages and kinematic band messages from the SB
    CFE_SB_Subscribe(PLEXIL_RETURN_MID, plexilAppdata.PLEXIL_Pipe);


    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&plexilMsg,PLEXIL_COMMAND_MID,sizeof(PlexilCommandMsg),TRUE);


    // Send event indicating app initialization
    CFE_EVS_SendEvent (PLEXIL_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Plexil Initialized. Version %d.%d",
                       PLEXIL_MAJOR_VERSION,
                       PLEXIL_MINOR_VERSION);


    // Register table with table services
    status = CFE_TBL_Register(&plexilAppdata.PLEXIL_tblHandle,
                              "PlexilTable",
                              sizeof(PLEXILTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &PlexilTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(plexilAppdata.PLEXIL_tblHandle,CFE_TBL_SRC_FILE,"/cf/apps/plexil_tbl.tbl");


    PLEXILTable_t *TblPtr;
    status = CFE_TBL_GetAddress(&TblPtr,plexilAppdata.PLEXIL_tblHandle);

    // copy data from table here
    int argc;
    char **inputParams;//[7][20];

    inputParams = (char**)malloc(7* sizeof(char*));
    for(int i=0;i<7;i++){
        inputParams[i] = (char*)malloc(50* sizeof(char));
    }

    argc = TblPtr->argc;
    memset(inputParams[0],0x0,sizeof(char)*50);
    memcpy(inputParams[1],TblPtr->argv1,sizeof(TblPtr->argv1));
    memcpy(inputParams[2],TblPtr->argv2,sizeof(TblPtr->argv2));
    memcpy(inputParams[3],TblPtr->argv3,sizeof(TblPtr->argv3));
    memcpy(inputParams[4],TblPtr->argv4,sizeof(TblPtr->argv4));
    memcpy(inputParams[5],TblPtr->argv5,sizeof(TblPtr->argv5));
    memcpy(inputParams[6],TblPtr->argv6,sizeof(TblPtr->argv6));

    //argv = TblPtr->argv;

    //printf("argv %s\n",argv[1]);

    plexil_init(argc,inputParams,&plexilAppdata.exec,&plexilAppdata.adap);

    // Free table pointer
    status = CFE_TBL_ReleaseAddress(plexilAppdata.PLEXIL_tblHandle);

    for(int i=0;i<7;i++){
        free(inputParams[i]);
    }

    free(inputParams);

}

void PLEXIL_AppCleanUp(){
    // Do clean up here
    plexil_destroy(plexilAppdata.exec);
}

void PLEXIL_ProcessPacket(){
    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(plexilAppdata.PLEXIL_MsgPtr);

    PlexilCommandMsg *msg;

    msg = (PlexilCommandMsg*) plexilAppdata.PLEXIL_MsgPtr;

    switch(msg->mType){
        case _LOOKUP_RETURN_:
        case _COMMAND_RETURN_:
            plexil_return(plexilAppdata.adap,msg);
            break;

//        case PLEXIL_RUN:
//            PLEXIL_Run();
//            break;
    }

    return;
}


void PLEXIL_Run(){
    int n;
    plexil_run(plexilAppdata.exec);

    n = 1;
    while(n>0){
        PlexilCommandMsg msg;
        n = plexil_getCommand(plexilAppdata.adap,&msg);

        if(n>0) {
            memcpy(&plexilMsg, &msg, sizeof(plexilMsg));
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t * ) & plexilMsg);
            CFE_SB_SendMsg((CFE_SB_Msg_t * ) & plexilMsg);
        }
    }

    n = 1;
    while(n>0){
        PlexilCommandMsg msg;
        n = plexil_getLookup(plexilAppdata.adap,&msg);

        if(n>0) {
            memcpy(&plexilMsg, &msg, sizeof(plexilMsg));
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t * ) & plexilMsg);
            CFE_SB_SendMsg((CFE_SB_Msg_t * ) & plexilMsg);
        }
    }
}

int32_t PlexilTableValidationFunc(void *TblPtr){

    int32_t status = 0;

    return status;
}