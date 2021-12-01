//
// Created by Swee Balachandran on 12/22/17.
//
#define EXTERN
#include "raft.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sch_msgids.h>
#include <merger_msgids.h>
#include <raft_msg.h>

/// Event ID filter definition
CFE_EVS_BinFilter_t  RAFT_EventFilters[] =
        {  /* Event ID    mask */
                {RAFT_STARTUP_INF_EID,       0x0000},
                {RAFT_COMMAND_ERR_EID,       0x0000},
        };

/* Application entry points */
void RAFT_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    RAFT_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        // Wait for the wakeup call from the scheduler
        status = CFE_SB_RcvMsg(&raftAppData.Raft_SchMsgPtr, raftAppData.Raft_SchPipe, CFE_SB_PEND_FOREVER);

        // Run a single iteration of the raft loop
        if (status == CFE_SUCCESS)
        {
            raft_loop();
        }

        // Listen for updates from the client
        status = CFE_SB_RcvMsg(&raftAppData.Raft_ClientMsgPtr, raftAppData.Raft_ClientPipe, CFE_SB_POLL);

        if (status == CFE_SUCCESS)
        {
            raftProcessClientData();
        }

    }

    RAFT_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void RAFT_AppInit(void) {

    memset(&raftAppData, 0, sizeof(raftAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(RAFT_EventFilters,
                     sizeof(RAFT_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    // Pipe to receive heartbeats or heartbeat acknowledgements
    status = CFE_SB_CreatePipe(&raftAppData.Raft_HbeatPipe, /* Variable to hold Pipe ID */
                               RAFT_PIPE_DEPTH,             /* Depth of Pipe */
                               RAFT_HBEATPIPE_NAME);        /* Name of pipe */

    // Pipe to receive scheduler wakeup messages
    status = CFE_SB_CreatePipe(&raftAppData.Raft_SchPipe,
                               RAFT_PIPE_DEPTH,
                               RAFT_SCHPIPE_NAME);

    // Pipe to receive vote requests and vote responses
    status = CFE_SB_CreatePipe(&raftAppData.Raft_VotePipe,
                               RAFT_PIPE_DEPTH,
                               RAFT_VOTEPIPE_NAME);

    // Pipe to receive client data
    status = CFE_SB_CreatePipe(&raftAppData.Raft_ClientPipe,
                               RAFT_PIPE_DEPTH,
                               RAFT_DATAPIPE_NAME);

    //Subscribe to messages from the SB
    CFE_SB_SubscribeLocal(FREQ_30_WAKEUP_MID,raftAppData.Raft_SchPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_ARRIVALTIMES_MID,raftAppData.Raft_ClientPipe,CFE_SB_DEFAULT_MSG_LIMIT);

    CFE_SB_Subscribe(ICAROUS_RAFT_HBEAT,raftAppData.Raft_HbeatPipe);
    CFE_SB_Subscribe(ICAROUS_RAFT_ACKHBEAT,raftAppData.Raft_HbeatPipe);
    CFE_SB_Subscribe(ICAROUS_RAFT_NEUTRAL_FOLLOWER,raftAppData.Raft_HbeatPipe);
    CFE_SB_Subscribe(ICAROUS_RAFT_VOTEREQUEST,raftAppData.Raft_VotePipe);
    CFE_SB_Subscribe(ICAROUS_RAFT_VOTERESPONSE,raftAppData.Raft_VotePipe);


    // Send event indicating app initialization
    CFE_EVS_SendEvent(RAFT_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Raft App Initialized. Version %d.%d",
                      RAFT_MAJOR_VERSION,
                      RAFT_MINOR_VERSION);

    // Initialize timers
    uint32_t clockAccuracy1;
    uint32_t clockAccuracy2;
    uint32_t clockAccuracy3;

    srand(time(0));
    // Get random timeouts
    rand();
    uint32_t followerTimeout  = (rand() % 50000) + 40000;
    uint32_t candidateTimeout = (rand() % 300000) + 100000;
    uint32_t leaderTimeout    = (rand() % 20000) + 1000;

    status = OS_TimerCreate(&raftAppData.followerTimerId,"FOLL_TIMER",&clockAccuracy1,heartbeatTimeoutCallback);
    if(status != CFE_SUCCESS){
       OS_printf("Could not create FOLL_TIMER timer\n");
    }

    status = OS_TimerCreate(&raftAppData.candidateTimerId,"CAND_TIMER",&clockAccuracy2,electionTimeoutCallback);
    if(status != CFE_SUCCESS){
       OS_printf("Could not create CAND_TIMER timer\n");
    }

    status = OS_TimerCreate(&raftAppData.leaderTimerId,"LEAD_TIMER",&clockAccuracy3,leaderTimeoutCallback);
    if(status != CFE_SUCCESS){
       OS_printf("Could not create LEAD_TIMER timer\n");
    }

    status = OS_TimerSet(raftAppData.followerTimerId,100000,followerTimeout);
    if(status != CFE_SUCCESS){
        OS_printf("Could not set FOLL_TIMER timer\n");
    }

    status = OS_TimerSet(raftAppData.candidateTimerId,100000,candidateTimeout);
    if(status != CFE_SUCCESS){
        OS_printf("Could not set CAND_TIMER timer\n");
    }

    status = OS_TimerSet(raftAppData.leaderTimerId,100000,leaderTimeout);
    if(status != CFE_SUCCESS){
        OS_printf("Could not set LEAD_TIMER timer\n");
    }

    // Initialize all data required for this application
    RAFT_AppInitializeData();

    char filename[25];
    sprintf(filename,"log/raftLog_%d.log",CFE_PSP_GetSpacecraftId());
    raftAppData.logFile = fopen(filename,"w");

    fprintf(raftAppData.logFile,"follower timeout: %d\n",followerTimeout);
    fprintf(raftAppData.logFile,"candidate timeout: %d\n",candidateTimeout);
    fprintf(raftAppData.logFile,"leader timeout: %d\n",leaderTimeout);

}

void RAFT_AppInitializeData(void){

    raftAppData.nodeID            = CFE_PSP_GetSpacecraftId();
    raftAppData.nodeRole          = NEUTRAL;
    raftAppData.nodeIntersection  =  -1;
    raftAppData.totalNodes        =  0;
    raftAppData.nodeTerm          =  0;
    raftAppData.electionInitiated = FALSE;
    raftAppData.neutralFollower   = TRUE;
    memset(raftAppData.memberNodeID,-1,sizeof(uint32_t)*MAX_NODES);
    memset(raftAppData.mergingData,-1,sizeof(cMergingData_t)*MAX_NODES);
    for(int i=0;i<MAX_NODES;++i){
        raftAppData.mergingData[i].aircraftID = -1; 
    }


}

void RAFT_AppCleanUp(void){

    // Do clean up here

}


void RAFT_SetNeutralPipeConfiguration(void){

    if(!raftAppData.neutralPipeInitialized) {
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_HBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_ACKHBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_VOTEREQUEST, raftAppData.Raft_VotePipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_VOTERESPONSE, raftAppData.Raft_VotePipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_NEUTRAL_FOLLOWER, raftAppData.Raft_HbeatPipe);

        raftAppData.neutralPipeInitialized = TRUE;
        raftAppData.followerPipeInitialized = FALSE;
        raftAppData.candidatePipeInitialized = FALSE;
        raftAppData.leaderPipeInitialized = FALSE;
    }

}

void RAFT_SetFollowerPipeConfiguration(void){

    if(!raftAppData.followerPipeInitialized) {
        CFE_SB_Subscribe(ICAROUS_RAFT_HBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_Subscribe(ICAROUS_RAFT_NEUTRAL_FOLLOWER, raftAppData.Raft_HbeatPipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_ACKHBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_Subscribe(ICAROUS_RAFT_VOTEREQUEST, raftAppData.Raft_VotePipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_VOTERESPONSE, raftAppData.Raft_VotePipe);

        raftAppData.neutralPipeInitialized = FALSE;
        raftAppData.followerPipeInitialized = TRUE;
        raftAppData.candidatePipeInitialized = FALSE;
        raftAppData.leaderPipeInitialized = FALSE;
    }

}

void RAFT_SetCandidatePipeConfiguration(void){

    if(!raftAppData.candidatePipeInitialized) {
        CFE_SB_Subscribe(ICAROUS_RAFT_HBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_ACKHBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_VOTEREQUEST, raftAppData.Raft_VotePipe);
        CFE_SB_Subscribe(ICAROUS_RAFT_VOTERESPONSE, raftAppData.Raft_VotePipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_NEUTRAL_FOLLOWER, raftAppData.Raft_HbeatPipe);

        raftAppData.neutralPipeInitialized = FALSE;
        raftAppData.followerPipeInitialized = FALSE;
        raftAppData.candidatePipeInitialized = TRUE;
        raftAppData.leaderPipeInitialized = FALSE;
    }
}

void RAFT_SetLeaderPipeConfiguration(void){

    if(!raftAppData.leaderPipeInitialized) {
        CFE_SB_Subscribe(ICAROUS_RAFT_HBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_Subscribe(ICAROUS_RAFT_ACKHBEAT, raftAppData.Raft_HbeatPipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_VOTEREQUEST, raftAppData.Raft_VotePipe);
        CFE_SB_Subscribe(ICAROUS_RAFT_VOTERESPONSE, raftAppData.Raft_VotePipe);
        CFE_SB_UnsubscribeLocal(ICAROUS_RAFT_NEUTRAL_FOLLOWER, raftAppData.Raft_HbeatPipe);

        raftAppData.neutralPipeInitialized = FALSE;
        raftAppData.followerPipeInitialized = FALSE;
        raftAppData.candidatePipeInitialized = FALSE;
        raftAppData.leaderPipeInitialized = TRUE;
    }

}

