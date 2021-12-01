/**
 * @file raft.h
 * @brief Definition of defines, structs and functions for raft app
 *
 * @author Swee Balachandran
 *
 */


#ifndef ICAROUS_CFS_RAFT_H
#define ICAROUS_CFS_RAFT_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raft_msg.h"
#include "raft_msgids.h"
#include "Icarous_msgids.h"

#define RAFT_STARTUP_INF_EID 0
#define RAFT_COMMAND_ERR_EID 1
#define RAFT_RECEIVED_FENCE_EID 3

#define RAFT_PIPE_DEPTH 100
#define RAFT_HBEATPIPE_NAME "RAFT_HBEAT_PIPE"
#define RAFT_SCHPIPE_NAME "RAFT_SCH_PIPE"
#define RAFT_VOTEPIPE_NAME "RAFT_VOTE_PIPE"
#define RAFT_DATAPIPE_NAME "RAFT_DATA_PIPE"

#define RAFT_MAJOR_VERSION 1
#define RAFT_MINOR_VERSION 0

typedef enum{
   NEUTRAL,
   FOLLOWER,
   CANDIDATE,
   LEADER
}Role_e;

/**
 * @defgroup RAFT_cFS_APP
 * @ingroup APPLICATIONS
 * 
 * @details Implements the Raft leader election protocol.
 *
 */

/**
 * @struct raftAppData_t
 * @brief  Struct to hold raft app data
 */
typedef struct{
    CFE_SB_PipeId_t    Raft_HbeatPipe;      ///< Pipe variable
    CFE_SB_PipeId_t    Raft_SchPipe;        ///< Pipe for scheduler
    CFE_SB_PipeId_t    Raft_VotePipe;       ///< Pipe for client data
    CFE_SB_PipeId_t    Raft_ClientPipe;     ///< Pipe for client data

    CFE_SB_MsgPtr_t    Raft_HbeatMsgPtr;    ///< Pointer to SB message
    CFE_SB_MsgPtr_t    Raft_SchMsgPtr;      ///< Pointer to SB message
    CFE_SB_MsgPtr_t    Raft_VoteMsgPtr;     ///< Pointer to SB message
    CFE_SB_MsgPtr_t    Raft_ClientMsgPtr;   ///< Pipe for client data

    Role_e  nodeRole;                       ///< Raft role of this node
    uint32_t nodeID;                        ///< Aircraft id of current node
    uint32_t leaderID;                      ///< Leader for the current network
    int32_t nodeTerm;                       ///< Raft node's term
    int32_t votedTerm;                      ///< Voted term
    uint32_t votedForNodeId;                ///< Node id of candidate voted for
    int32_t nodeIntersection;               ///< Current intersection of this node
    uint32_t totalVotesReceived;            ///< Total votes received
    uint32_t totalNodes;                    ///< Total nodes in the network
    int32_t memberNodeID[MAX_NODES];        ///< Member node identifications
    uint32_t followerTimerId;               ///< Follower heartbeat timer id
    uint32_t candidateTimerId;              ///< Candidate election timer id
    uint32_t leaderTimerId;                 ///< Leader timer id
    uint32_t hbeatTimeoutCounter;           ///< Heartbeat timeout counter
    uint32_t electionTimeoutCounter;        ///< Election timeout counter
    uint32_t leaderTimeoutCounter;          ///< Leader timeout counter

    report_t mergingData[MAX_NODES];   ///< Merging data from all nodes in the network
    uint32_t logDataIndex;                  ///< Log data index
    report_t ownshipMergingData;       ///< Latest merging data from ownship
    bool electionInitiated;                 ///< Flag indicating if election was initiated.
    bool neutralPipeInitialized;            ///< Pipe configuration for neutral role initialized.
    bool followerPipeInitialized;           ///< Pipe configuration for follower role initialized.
    bool candidatePipeInitialized;          ///< Pipe configuration for candidate role initialized.
    bool leaderPipeInitialized;             ///< Pipe configuration for leader role initialized.
    bool neutralFollower;                   ///< True if no leader is available

    FILE* logFile;                          ///< Log file for debugging purposes
}raftAppData_t;

/**
 * App's main entry point
 */
void RAFT_AppMain(void);

/**
 * App initialization
 */
void RAFT_AppInit(void);

/**
 * Initialize data for this app
 */
void RAFT_AppInitializeData(void);

/**
 * App clean up
 */
void RAFT_AppCleanUp(void);

/**
 * Function to process received packets
 */
void RAFT_ProcessPacket(void);

void heartbeatTimeoutCallback(uint32_t timerId);

void electionTimeoutCallback(uint32_t timerId);

void leaderTimeoutCallback(uint32_t timerId);

void raft_loop(void);

void raft_neutral(void);

void raft_follower(void);

void raft_candidate(void);

void raft_leader(void);

void raftProcessClientData(void);

void raftSendDataToClient(void);

void RAFT_SetNeutralPipeConfiguration(void);

void RAFT_SetFollowerPipeConfiguration(void);

void RAFT_SetCandidatePipeConfiguration(void);

void RAFT_SetLeaderPipeConfiguration(void);


EXTERN raftAppData_t raftAppData;


#endif //ICAROUS_CFS_RAFT_H
