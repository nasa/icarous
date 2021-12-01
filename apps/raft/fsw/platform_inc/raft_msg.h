/**
 * @file raft_msg.h
 * @brief Defintion of messages used by raft_msg.h
 */

#ifndef ICAROUS_CFS_RAFT_MSG_H
#define ICAROUS_CFS_RAFT_MSG_H

#include "cfe.h"
#include "merger_msg.h"

#define MAX_NODES 5

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
   CCSDS_PriHdr_t hdr;                      /**< cFS header information */
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
   uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
   int32_t intersectionID;                  /**< Intersection ID for the current raft network */
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
  uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
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
  uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
  int32_t intersectionID;                 /**< Intersection for which election is taking place */
  uint32_t candidateID;                   /**< Candidate node's aircraft ID */
  uint32_t term;                          /**< Election term for current request */
}raftVoteRequest_t;

/**
 * @struct raftVoteResponse_t
 * @brief followers respond to vote reqeuests with this message
 */
typedef struct{
  uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
  int32_t intersectionID;                 /**< Intersection for which election is taking place */
  uint32_t candidateID;                   /**< Leader node's aircraft ID */
  uint32_t nodeID;                        /**< Follower node's aircraft ID */
  uint32_t term;                          /**< Election term for current request */
}raftVoteResponse_t;


#endif //ICAROUS_CFS_RAFT_MSG_H
