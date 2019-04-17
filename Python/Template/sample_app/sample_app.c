//
// Created by {{AUTHOR}} on {{DATE}}.
//
#include "{{APP_NAME}}.h"
#include "{{APP_NAME}}_tbl.c"

CFE_EVS_BinFilter_t  {{APP_NAME}}_EventFilters[] =
{  /* Event ID    mask */
        { {{APP_NAMEU}}_STARTUP_INF_EID,       0x0000},
        { {{APP_NAMEU}}_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

/* Application entry points */
void {{APP_NAMEU}}_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    {{APP_NAMEU}}_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&{{APP_NAME}}AppData.{{APP_NAME}}_MsgPtr, {{APP_NAME}}AppData.{{APP_NAME}}_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
		{{APP_NAMEU}}_ProcessPacket();
        }
    }

    {{APP_NAMEU}}_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void {{APP_NAMEU}}_AppInit(void) {

    memset(&{{APP_NAME}}AppData, 0, sizeof({{APP_NAME}}AppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register({{APP_NAME}}_EventFilters,
                     sizeof({{APP_NAME}}_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&{{APP_NAME}}AppData.{{APP_NAME}}_Pipe, /* Variable to hold Pipe ID */
		    {{APP_NAMEU}}_PIPE_DEPTH,       /* Depth of Pipe */
		    {{APP_NAMEU}}_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB

    // Initialize all messages that this App generates

    // Register table with table services
    status = CFE_TBL_Register(&{{APP_NAME}}AppData.{{APP_NAME}}_tblHandle,
                              "{{APP_NAME}}Table",
                              sizeof({{APP_NAME}}Table_t),
                              CFE_TBL_OPT_DEFAULT,
                              &{{APP_NAME}}TableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load({{APP_NAME}}AppData.{{APP_NAME}}_tblHandle,CFE_TBL_SRC_ADDRESS,&{{APP_NAME}}_TblStruct);

    {{APP_NAME}}Table_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, {{APP_NAME}}AppData.{{APP_NAME}}_tblHandle);

    // Send event indicating app initialization
    CFE_EVS_SendEvent({{APP_NAMEU}}_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "{{APP_NAME}} App Initialized. Version %d.%d",
                      {{APP_NAMEU}}_MAJOR_VERSION,
                      {{APP_NAMEU}}_MINOR_VERSION);
}

void {{APP_NAMEU}}_AppCleanUp(){
    // Do clean up here
}

void {{APP_NAMEU}}_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId({{APP_NAME}}AppData.{{APP_NAME}}_MsgPtr);

    switch(MsgId){

    }
}

int32_t {{APP_NAME}}TableValidationFunc(void *TblPtr){
    return 0;
}
