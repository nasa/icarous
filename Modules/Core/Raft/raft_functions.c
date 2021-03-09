//
// Created by Swee Balachandran on 2/27/19.
//
#define EXTERN extern

#include <raft_msg.h>
#include <raft.h>


struct timespec ts;
double timeNow;

#define GETTIME() timespec_get(&ts,TIME_UTC); \
timeNow = ts.tv_sec + (double)(ts.tv_nsec)/1E9;


void raft_loop(void){

   // Check role
   switch(raftAppData.nodeRole) {
       case NEUTRAL:
           raft_neutral();
           break;

       case FOLLOWER:
          raft_follower();
          break;

       case CANDIDATE:
           raft_candidate();
           break;

       case LEADER:
           raft_leader();
           break;

   }
}

void raft_neutral(void){

   RAFT_SetNeutralPipeConfiguration();

   if(raftAppData.nodeIntersection >= 0){
      raftAppData.nodeRole = FOLLOWER;
      raftAppData.neutralFollower = TRUE;
      raftAppData.totalNodes = 1;
      raftSendDataToClient();
      GETTIME();
      fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | ALIVE\n",timeNow,raftAppData.nodeIntersection);
   }else{
      raftAppData.nodeRole = NEUTRAL;
   }

}

void raft_follower(void){

  RAFT_SetFollowerPipeConfiguration();

  // Check if timeout has occured.
  if (raftAppData.hbeatTimeoutCounter % 20 == 0){

      // If there were more than one node in the network
      // a lost heartbeat means that the leader dropped off.
      // So reduce total nodes count
      if(raftAppData.totalNodes > 1 && !raftAppData.neutralFollower){
         raftAppData.totalNodes--;
      }

      raftAppData.nodeRole = CANDIDATE;
      GETTIME();
      fprintf(raftAppData.logFile,"%.5f | MFID %d | CANDIDATE | ALIVE\n",timeNow,raftAppData.nodeIntersection);
      return;
  }else if(raftAppData.hbeatTimeoutCounter % 3 == 0){
      if(raftAppData.neutralFollower){
              raftHbeatAck_t hbtack;
              CFE_SB_InitMsg(&hbtack,ICAROUS_RAFT_NEUTRAL_FOLLOWER,sizeof(raftHbeatAck_t),TRUE);
              hbtack.intersectionID = raftAppData.nodeIntersection;
              hbtack.followerID = raftAppData.nodeID;
              hbtack.logIndex = -1;
              GETTIME();
              fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | SEND_NEUTRAL_FOLLOWER ID:%d\n",
                      timeNow,raftAppData.nodeIntersection,hbtack.logIndex);
              SendSBMsg(hbtack);
      }
  }

  // Wait for heartbeat from leader
  int32_t status = CFE_SB_RcvMsg(&raftAppData.Raft_HbeatMsgPtr, raftAppData.Raft_HbeatPipe, CFE_SB_POLL);

  if(status == CFE_SUCCESS){
      switch(CFE_SB_GetMsgId(raftAppData.Raft_HbeatMsgPtr)){

          // Parse heartbeat and verify intersectionID
          case ICAROUS_RAFT_HBEAT:{
              raftAppData.neutralFollower = FALSE;
              raftHbeat_t *hbeat = (raftHbeat_t *) raftAppData.Raft_HbeatMsgPtr;

              // Check if this heartbeat is for the network in the current intersection
              if(raftAppData.nodeIntersection != hbeat->intersectionID)
                 break;
              else
                  raftAppData.hbeatTimeoutCounter = 1;

              GETTIME();
              fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | RECV_HBEAT ID:%d FROM LEADER NODE: %d\n",
                      timeNow,
                      raftAppData.nodeIntersection,
                      hbeat->logIndex,
                      hbeat->leaderID);

              // extract log data from heartbeat
              raftAppData.nodeTerm   = hbeat->term;
              raftAppData.totalNodes = hbeat->totalNodes;
              raftAppData.leaderID   = hbeat->leaderID;
              memcpy(raftAppData.mergingData,hbeat->log,sizeof(report_t)*MAX_NODES);
              for(int i=0;i<MAX_NODES;++i){
                  raftAppData.memberNodeID[i] = raftAppData.mergingData[i].aircraftID;
                  //fprintf(raftAppData.logFile,"Received data for %d\n",raftAppData.mergingData[i].aircraftID);
              }

              // Send heartbeat acknowledgements (this also includes client data)
              raftHbeatAck_t hbtack;
              CFE_SB_InitMsg(&hbtack,ICAROUS_RAFT_ACKHBEAT,sizeof(raftHbeatAck_t),TRUE);
              hbtack.intersectionID = raftAppData.nodeIntersection;
              hbtack.followerID = raftAppData.nodeID;
              hbtack.logIndex = hbeat->logIndex;
              memcpy(&hbtack.nodeData,&raftAppData.ownshipMergingData,sizeof(report_t));
              fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | SEND_HBEAT_ACK ID:%d\n",
                      timeNow,raftAppData.nodeIntersection,hbtack.logIndex);
              SendSBMsg(hbtack);

              // Send collected log data to client
              raftSendDataToClient();
              break;
          }

          case ICAROUS_RAFT_NEUTRAL_FOLLOWER:{
              raftHbeatAck_t *ack = (raftHbeatAck_t*) raftAppData.Raft_HbeatMsgPtr;

              bool avail = false;
              raftAppData.memberNodeID[0] = raftAppData.nodeID;
              for(int i=0;i<raftAppData.totalNodes;++i){
                  if(raftAppData.memberNodeID[i] == ack->followerID){
                       avail = true;
                       break;
                  }
              }
              if(!avail){
                  raftAppData.memberNodeID[raftAppData.totalNodes] = ack->followerID;
                  raftAppData.totalNodes++;
                  GETTIME();
                  fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | DISCOVERED NEUTRAL FOLLOWER %d\n",
                         timeNow,raftAppData.nodeIntersection,ack->followerID);

              }
              break;
          }

          default:
              break;
      }
  }

  // Check for vote requests if from the network
  status = CFE_SB_RcvMsg(&raftAppData.Raft_VoteMsgPtr, raftAppData.Raft_VotePipe, CFE_SB_POLL);

  if(status == CFE_SUCCESS){
     switch(CFE_SB_GetMsgId(raftAppData.Raft_VoteMsgPtr)){

         case ICAROUS_RAFT_VOTEREQUEST:{

            raftVoteRequest_t *voteRequest = (raftVoteRequest_t*) raftAppData.Raft_VoteMsgPtr;

            // Make sure that the vote request is for the current intersection network
            if(raftAppData.nodeIntersection != voteRequest->intersectionID){
                break;
            }
            GETTIME();
            fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | RECV_VOTE_REQUEST TERM:%d\n",timeNow,raftAppData.nodeIntersection,voteRequest->term);

            // Response with votes only if a vote for the same term hasn't been sent before.
            if(raftAppData.votedTerm != voteRequest->term){
               raftVoteResponse_t voteResponse;
               CFE_SB_InitMsg(&voteResponse,ICAROUS_RAFT_VOTERESPONSE,sizeof(raftVoteResponse_t),TRUE);

               voteResponse.term = voteRequest->term;
               voteResponse.candidateID = voteRequest->candidateID;
               voteResponse.intersectionID = voteRequest->intersectionID;
               SendSBMsg(voteResponse);
               fprintf(raftAppData.logFile,"%.5f | MFID %d | FOLLOWER | SEND_VOTE\n",timeNow,raftAppData.nodeIntersection);
            }else{
               //fprintf(raftAppData.logFile,"MFID: %d,FOLLOWER: Not Sending vote, Already voted: %d\n",raftAppData.nodeIntersection,raftAppData.votedTerm);
            }
            break;
         }
     }
  }
}

void raft_candidate(void){

  RAFT_SetCandidatePipeConfiguration();
  // The assumption is here is that there is already a valid intersection

  // If there are no other nodes at the intersection, this node becomes the leader
  if(raftAppData.totalNodes == 1){
      raftAppData.nodeRole = LEADER;
      GETTIME();
      fprintf(raftAppData.logFile,"%.5f | MFID %d | LEADER | ALIVE\n",timeNow,raftAppData.nodeIntersection);
      return;
  }

  // Start election
  if(!raftAppData.electionInitiated){
     raftAppData.electionInitiated = TRUE;
     raftAppData.electionTimeoutCounter = 1;

     // Increment term
     raftAppData.nodeTerm++;

     // Vote for self
     raftAppData.totalVotesReceived = 1;

     // Send vote request messages
     raftVoteRequest_t voteRequest;
     CFE_SB_InitMsg(&voteRequest,ICAROUS_RAFT_VOTEREQUEST,sizeof(raftVoteRequest_t),TRUE);
     voteRequest.intersectionID = raftAppData.nodeIntersection;
     voteRequest.term = raftAppData.nodeTerm;
     voteRequest.candidateID = raftAppData.nodeID;
     SendSBMsg(voteRequest);
     GETTIME();
     fprintf(raftAppData.logFile,"%.5f| MFID %d| CANDIDATE | SEND_VOTE_REQUEST TERM:%d, TOTAL NODES:%d |\n",timeNow,raftAppData.nodeIntersection,voteRequest.term,raftAppData.totalNodes);
  }

  // If election timeout occurs go back to being a follower
  if(raftAppData.electionTimeoutCounter % 3 == 0){
     raftAppData.electionInitiated = FALSE;
     fprintf(raftAppData.logFile,"%.5f | CANDIDATE | TIME_OUT\n",timeNow);
     raftAppData.nodeRole = FOLLOWER;
     return;
  }

  // Receive and count votes
  int32_t status = CFE_SB_RcvMsg(&raftAppData.Raft_VoteMsgPtr, raftAppData.Raft_VotePipe, CFE_SB_POLL);

  if(status == CFE_SUCCESS) {
      switch (CFE_SB_GetMsgId(raftAppData.Raft_VoteMsgPtr)) {
          case ICAROUS_RAFT_VOTERESPONSE: {
              raftVoteResponse_t *voteResponse = (raftVoteResponse_t *) raftAppData.Raft_VoteMsgPtr;

              if (voteResponse->candidateID == raftAppData.nodeID && voteResponse->term == raftAppData.nodeTerm) {
                  fprintf(raftAppData.logFile,"%.5f | MFID %d | CANDIDATE | RECV_VOTE\n",timeNow,raftAppData.nodeIntersection);
                  raftAppData.totalVotesReceived++;
                  if (raftAppData.totalVotesReceived > (int) raftAppData.totalNodes / 2) {
                      raftAppData.nodeRole = LEADER;
                      memset(raftAppData.mergingData,-1,sizeof(report_t)*MAX_NODES);
                      memset(raftAppData.memberNodeID,-1,sizeof(uint32_t)*MAX_NODES);
                      raftAppData.totalNodes = 1;
                  }
              }
              break;
          }

          default:
              break;

      }
  }

  // Check to see if someone if already providing heartbeats
  status = CFE_SB_RcvMsg(&raftAppData.Raft_HbeatMsgPtr, raftAppData.Raft_HbeatPipe, CFE_SB_POLL);

  if(status == CFE_SUCCESS) {
      switch (CFE_SB_GetMsgId(raftAppData.Raft_HbeatMsgPtr)) {
          case ICAROUS_RAFT_HBEAT: {
              raftAppData.nodeRole = FOLLOWER;
              raftAppData.hbeatTimeoutCounter = 1;
              break;
          }

          default:
              break;

      }
  }


}


void raft_leader(void){

  RAFT_SetLeaderPipeConfiguration();

  if(raftAppData.leaderTimeoutCounter % 2 == 0){

      // Copy ownship data into global log at first position
      raftAppData.memberNodeID[0] = raftAppData.nodeID;
      memcpy(raftAppData.mergingData,&raftAppData.ownshipMergingData,sizeof(report_t));

      // Send heartbeat at regular intervals
      // Heartbeat also contains log data for all nodes
      raftHbeat_t hbeat;
      CFE_SB_InitMsg(&hbeat,ICAROUS_RAFT_HBEAT,sizeof(raftHbeat_t),TRUE);
      hbeat.totalNodes = raftAppData.totalNodes;
      hbeat.intersectionID = raftAppData.nodeIntersection;
      hbeat.leaderID = raftAppData.nodeID;
      hbeat.term = raftAppData.nodeTerm;
      hbeat.logIndex = ++raftAppData.logDataIndex;

      for(int i=0;i<MAX_NODES;++i){
          if(raftAppData.memberNodeID[i] == -1){
              raftAppData.mergingData[i].aircraftID = -1;
          }
      }
      memcpy(hbeat.log,raftAppData.mergingData,sizeof(report_t)*MAX_NODES);
      SendSBMsg(hbeat);
      GETTIME();

      fprintf(raftAppData.logFile,"%.5f | MFID %d | LEADER | SEND_HBEAT ID:%d\n",timeNow,raftAppData.nodeIntersection,hbeat.logIndex);
      //for(int i=0;i<MAX_NODES;++i){
          //fprintf(raftAppData.logFile,"ac id: %d\n",hbeat.log[i].aircraftID);
      //}

      // Send data to client
      raftSendDataToClient();
  }

  // Wait for ack and determine nodes in the network
   int32_t status = CFE_SB_RcvMsg(&raftAppData.Raft_HbeatMsgPtr, raftAppData.Raft_HbeatPipe, 10);

   if(status == CFE_SUCCESS){
       switch(CFE_SB_GetMsgId(raftAppData.Raft_HbeatMsgPtr)){

           case ICAROUS_RAFT_ACKHBEAT:{

               raftHbeatAck_t *hbtack = (raftHbeatAck_t*) raftAppData.Raft_HbeatMsgPtr;

               //OS_printf("Heartbeat ack from %d\n",hbtack->followerID);
               if(hbtack->intersectionID != raftAppData.nodeIntersection) {
                   break;
               }

               if(hbtack->logIndex != raftAppData.logDataIndex){
                  //OS_printf("Stale heartbeat ack\n");
               }
               GETTIME();

               fprintf(raftAppData.logFile,"%.5f | MFID %d | LEADER | RECV_HBEAT_ACK ID :%d FROM NODE: %d\n",
               timeNow,
               raftAppData.nodeIntersection,
               hbtack->logIndex,
               hbtack->followerID);

               uint32_t totalNodes = 0;
               bool nodeExists = FALSE;

               for(int i=0;i<MAX_NODES;++i){
                    if(raftAppData.memberNodeID[i] >= 0){
                        if(raftAppData.memberNodeID[i] == hbtack->followerID){
                            nodeExists = TRUE;
                            memcpy(raftAppData.mergingData+i,&(hbtack->nodeData),sizeof(report_t));
                            //fprintf(raftAppData.logFile,"Updating node data for %d at index %d\n",raftAppData.memberNodeID[i],i);
                            break;
                        }
                    }else{
                        break;
                    }
               }

               if(!nodeExists){
                  fprintf(raftAppData.logFile,"%.5f | MFID %d | LEADER | ADD_NEW_NODE\n",timeNow,raftAppData.nodeIntersection);
                  totalNodes = raftAppData.totalNodes;
                  raftAppData.memberNodeID[totalNodes] = hbtack->followerID;
                  memcpy(raftAppData.mergingData+totalNodes,&(hbtack->nodeData),sizeof(report_t));
                  raftAppData.totalNodes++;
               }
               break;
           }

           case ICAROUS_RAFT_HBEAT:{

              raftHbeat_t* hbt = (raftHbeat_t*)raftAppData.Raft_HbeatMsgPtr;

              if(hbt->intersectionID != raftAppData.nodeIntersection) {
                  break;
              }

              GETTIME();
              if(hbt->leaderID != raftAppData.nodeID) {
                  fprintf(raftAppData.logFile,"%.5f | MFID %d | LEADER | RECV_HBEAT NODEID:%d |",timeNow,raftAppData.nodeIntersection,hbt->leaderID);
                  if (hbt->term > raftAppData.nodeTerm) {
                      fprintf(raftAppData.logFile,"Higher term");
                      raftAppData.nodeRole = FOLLOWER;
                      raftAppData.votedTerm = -1;
                      raftAppData.totalVotesReceived = 0;
                      raftAppData.hbeatTimeoutCounter = 1;
                      return;
                  } else if(hbt->leaderID < raftAppData.nodeID){
                      fprintf(raftAppData.logFile,"Elder leader :%d",hbt->leaderID);
                      //fprintf(raftAppData.logFile,"Becoming a follower\n");
                      raftAppData.nodeRole = FOLLOWER;
                      raftAppData.votedTerm = -1;
                      raftAppData.totalVotesReceived = 0;
                      return;
                      return;
                  }
                  fprintf(raftAppData.logFile,"\n");
              }

              break;
           }

           default:
               break;
       }
   }
}

void heartbeatTimeoutCallback(uint32_t timerId){
   raftAppData.hbeatTimeoutCounter++;
   if(raftAppData.nodeRole != FOLLOWER){
      raftAppData.hbeatTimeoutCounter = 1;
   }
}

void electionTimeoutCallback(uint32_t timerId){
   raftAppData.electionTimeoutCounter++;
   if(raftAppData.nodeRole != CANDIDATE){
      raftAppData.electionTimeoutCounter = 1;
   }
}

void leaderTimeoutCallback(uint32_t timerId){
   raftAppData.leaderTimeoutCounter++;
   if(raftAppData.nodeRole != LEADER){
      raftAppData.leaderTimeoutCounter = 1;
   }

}

void raftProcessClientData(void){

  // Client here is the ownship
  // Data from other aircraft are obtained through the hbeat ack messages
   report_t* clientData = CFE_SB_GetUserData(raftAppData.Raft_ClientMsgPtr);

   raftAppData.nodeIntersection = clientData->intersectionID;
   if(raftAppData.nodeIntersection >= 0) {
       memcpy(&raftAppData.ownshipMergingData, clientData, sizeof(report_t));
  
       GETTIME();
       fprintf(raftAppData.logFile,"%.5f | CLIENT_DATA_RECV | Int ID: %d, NumSchedules: %d, Zone: %d, Arrival: %f\n",
               timeNow,
               raftAppData.ownshipMergingData.intersectionID,
               raftAppData.ownshipMergingData.numSchedulesComputed,
               raftAppData.ownshipMergingData.zoneStatus,
               raftAppData.ownshipMergingData.currentArrivalTime);
   }else{
       raftAppData.nodeRole = NEUTRAL;
       RAFT_AppInitializeData();
       fprintf(raftAppData.logFile,"%.5f | MFID -1 | NEUTRAL | ALIVE \n",timeNow);
   }
}



void raftSendDataToClient(void){

  cDataLog_t dataLog;
  CFE_SB_InitMsg(&dataLog,ICAROUS_RAFT_DATALOG, sizeof(cDataLog_t),TRUE);
  dataLog.totalNodes = raftAppData.totalNodes;
  dataLog.intersectionID = raftAppData.nodeIntersection;
  dataLog.nodeRole = raftAppData.nodeRole;
  memcpy(dataLog.log,raftAppData.mergingData, sizeof(report_t)*MAX_NODES);

  // Sent aircraft ids to -1 for empty entries
  //for(int i=0;i<MAX_NODES;++i){
  //   if(i < raftAppData.totalNodes){
   //      continue;
    // }
     //dataLog.log[i].aircraftID = -1;
 // }

  GETTIME();
  SendSBMsg(dataLog);

  fprintf(raftAppData.logFile,"%.5f | CLIENT_DATA_SEND |",timeNow);
  
  for(int i=0;i<MAX_NODES;++i){
     if(dataLog.log[i].aircraftID >= 0){
        fprintf(raftAppData.logFile,"< intersection: %d, zone: %d, a/c id: %d, arrival time: %f >",
                dataLog.log[i].intersectionID,
                dataLog.log[i].zoneStatus,
                dataLog.log[i].aircraftID,
                dataLog.log[i].currentArrivalTime);
     }
  }
  fprintf(raftAppData.logFile,"\n");
}
