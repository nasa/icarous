#ifndef RAFT_HPP
#define RAFT_HPP

#include <cstdint>
#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <list>

#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
   NEUTRAL,
   FOLLOWER,
   CANDIDATE,
   LEADER
}Role_e;


#define MAX_NODES 10

typedef struct __attribute__((__packed__)){
   int8_t aircraftID;           /**< Aircraft ID */
   int8_t intersectionID;       /**< Current intersection for which scheduling is required */
   double earlyArrivalTime;     /**< Earliest a/c can reach the intersection (using max speed) */
   double currentArrivalTime;   /**< Current arrival time  (using current speed)*/
   double lateArrivalTime;      /**< Late arrival time (using slowest speed + allowed deviations) */
   int32_t numSchedulesComputed;/**< Number of times scheduler has been run */
   uint8_t zoneStatus;          /**< zone status */
}report_t;



/**
 * @struct dataLog_t
 * @brief log containing merging data from all nodes in the network
 */
typedef struct __attribute__((__packed__)){
   int32_t intersectionID;                  /**< Intersection ID for the current raft network */
   uint8_t nodeRole;                        /**< Raft node status of current vehicle */
   uint32_t totalNodes;                     /**< Total number of nodes in the network */
   report_t log[MAX_NODES];
}cDataLog_t;

/**
 * @struct raftHbeat_t
 * @brief raft heartbeat sent to followers from the leader node
 *
 * Published under the ICAROUS_RAFT_HBEAT_MID topic
 */
typedef struct{
   int32_t intersectionID;                 /**< Intersection ID of the participating node */
   uint32_t leaderID;                       /**< Leader node's aircraft ID*/
   uint32_t totalNodes;                     /**< Total number of nodes in the network */
   uint32_t logIndex;                       /**< Index (count) of the current log data */
   uint32_t term;                           /**< Term of current leader */
   report_t log[MAX_NODES];
}raftHbeat_t;

/**
 * @struct raftHbeatAck_t
 * @brief acknowledgement data from follower nodes
 *
 * Published under the ICAROUS_RAFT_HBEATACK_MID topic
 */
typedef struct{
  int32_t intersectionID;                 /**< Intersection ID of the participating node */
  uint32_t followerID;                    /**< Follower node's aircraft ID */
  report_t nodeData;                      /**< Merging data for the current node */
  uint32_t logIndex;                      /**< log index for which ack is issued */
}raftHbeatAck_t;


/**
 * @struct raftVoteRequest_t
 * @brief message leader uses to request votes from followers
 */
typedef struct{
  int32_t intersectionID;                 /**< Intersection for which election is taking place */
  uint32_t candidateID;                   /**< Candidate node's aircraft ID */
  uint32_t term;                          /**< Election term for current request */
}raftVoteRequest_t;

/**
 * @struct raftVoteResponse_t
 * @brief followers respond to vote reqeuests with this message
 */
typedef struct{
  int32_t intersectionID;                 /**< Intersection for which election is taking place */
  uint32_t candidateID;                   /**< Leader node's aircraft ID */
  uint32_t nodeID;                        /**< Follower node's aircraft ID */
  uint32_t term;                          /**< Election term for current request */
}raftVoteResponse_t;



typedef struct{
    

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
    uint32_t followerTime;
    uint32_t candidateTime;
    uint32_t leaderTime;
    uint32_t followerTimeout;               ///< Follower heartbeat timer id
    uint32_t candidateTimeout;              ///< Candidate election timer id
    uint32_t leaderTimeout;                 ///< Leader timer id
    uint32_t hbeatTimeoutCounter;           ///< Heartbeat timeout counter
    uint32_t electionTimeoutCounter;        ///< Election timeout counter
    uint32_t leaderTimeoutCounter;          ///< Leader timeout counter
    struct timespec ts;
    double timeNow;

    report_t mergingData[MAX_NODES];        ///< Merging data from all nodes in the network
    uint32_t logDataIndex;                  ///< Log data index
    report_t ownshipMergingData;            ///< Latest merging data from ownship
    bool electionInitiated;                 ///< Flag indicating if election was initiated.
    bool neutralPipeInitialized;            ///< Pipe configuration for neutral role initialized.
    bool followerPipeInitialized;           ///< Pipe configuration for follower role initialized.
    bool candidatePipeInitialized;          ///< Pipe configuration for candidate role initialized.
    bool leaderPipeInitialized;             ///< Pipe configuration for leader role initialized.
    bool neutralFollower;                   ///< True if no leader is available

    std::list<raftHbeat_t> heartBeatPipeIn;
    std::list<raftHbeat_t> heartBeatPipeOut;
    std::list<raftHbeatAck_t> heartBeatAckPipeIn;
    std::list<raftHbeatAck_t> heartBeatAckPipeOut;
    std::list<raftVoteRequest_t> voteRequestPipeIn;
    std::list<raftVoteRequest_t> voteRequestPipeOut;
    std::list<raftVoteResponse_t> voteResponsePipeIn;
    std::list<raftVoteResponse_t> voteResponsePipeOut;
    cDataLog_t dataLog;

    FILE* logFile;                          ///< Log file for debugging purposes
}raftAppData_t;


void* RAFT_AppInitializeData(void* obj0,int id,int followerTimeout,int candidateTimeout,int leaderTimeout);

void heartbeatTimeoutCallback(raftAppData_t* obj);

void electionTimeoutCallback(raftAppData_t* obj);

void leaderTimeoutCallback(raftAppData_t* obj);

void raft_loop(void* obj,double time);

void raft_neutral(raftAppData_t* obj);

void raft_follower(raftAppData_t* obj);

void raft_candidate(raftAppData_t* obj);

void raft_leader(raftAppData_t* obj);

void raftProcessClientData(void* obj,report_t* clientData);

void raftSendDataToClient(raftAppData_t* obj);

void GetMergingData(cDataLog_t* data);

int GetHeartbeat(void* obj,raftHbeat_t* hbeat);

int GetHeartbeatAck(void* obj,raftHbeatAck_t* hbtack);

int GetVoteRequest(void* obj,raftVoteRequest_t* voteReq);

int GetVoteResponse(void* obj,raftVoteResponse_t* voteRep);

void InputHeartbeat(raftAppData_t* obj,raftHbeat_t hbeat){((raftAppData_t*)obj)->heartBeatPipeIn.push_back(hbeat);};

void InputHeartbeatAck(raftAppData_t* obj,raftHbeatAck_t hbeatack){((raftAppData_t*)obj)->heartBeatAckPipeIn.push_back(hbeatack);};

void InputVoteResponse(raftAppData_t* obj,raftVoteResponse_t voteResponse){((raftAppData_t*)obj)->voteResponsePipeIn.push_back(voteResponse);};

void InputVoteRequest(raftAppData_t* obj,raftVoteRequest_t voteRequest){((raftAppData_t*)obj)->voteRequestPipeIn.push_back(voteRequest);};

void ClearPipes(raftAppData_t* obj);

#ifdef __cplusplus
}
#endif


#endif //ICAROUS_CFS_RAFT_H

