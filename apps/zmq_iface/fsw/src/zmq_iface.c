#include "zmq_iface.h"
#include "utils.h"

CFE_EVS_BinFilter_t  ZMQ_IFACE_EventFilters[] =
{  /* Event ID    mask */
        { ZMQ_IFACE_STARTUP_INF_EID,       0x0000},
        { ZMQ_IFACE_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

/**
 * @var Global variable to store all app related data
 */
static ZMQ_IFACE_AppData_t AppData;
static ZMQ_IFACE_Connection_t * const connPtr = &AppData.connection;

const char STUB_MESSAGE[] = "ZMQ_IFACE HELO";

/* Application entry points */
void ZMQ_IFACE_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    ZMQ_IFACE_AppInit();

    while(1){
        ZMQ_IFACE_SendTelemetryMsg(connPtr, STUB_MESSAGE);
        SleepOneSecond();
    }

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&AppData.msgPtr, AppData.pipe, 10);

        if (status == CFE_SUCCESS)
        {
		    ZMQ_IFACE_ProcessPacket();
        }
    }

    ZMQ_IFACE_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void ZMQ_IFACE_AppInit(void) {

    memset(&AppData, 0, sizeof(ZMQ_IFACE_AppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(ZMQ_IFACE_EventFilters,
                     sizeof(ZMQ_IFACE_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&AppData.pipe, /* Variable to hold Pipe ID */
		    ZMQ_IFACE_PIPE_DEPTH,       /* Depth of Pipe */
		    ZMQ_IFACE_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB

    // Initialize all messages that this App generates

    ZMQ_IFACE_InitZMQServices(connPtr);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(ZMQ_IFACE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "zmq_iface App Initialized. Version %d.%d",
                      ZMQ_IFACE_MAJOR_VERSION,
                      ZMQ_IFACE_MINOR_VERSION);
}

void ZMQ_IFACE_AppCleanUp(){
    // Do clean up here
}

void ZMQ_IFACE_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(AppData.msgPtr);

    switch(MsgId){

    }
}

