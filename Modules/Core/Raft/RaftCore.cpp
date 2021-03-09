#include <RaftCore.hpp>



#define GETTIME() clock_gettime(CLOCK_REALTIME,&obj->ts); \
obj->ts.tv_sec + static_cast<float>(obj->ts.tv_nsec)/1E9;

void raft_loop(void* objIn,double time){

   raftAppData_t* obj =(raftAppData_t*) objIn; 
   obj->timeNow = time;
   // Check role
   switch(obj->nodeRole) {
       case NEUTRAL:
           raft_neutral(obj);
           break;

       case FOLLOWER:
          raft_follower(obj);
          break;

       case CANDIDATE:
           raft_candidate(obj);
           break;

       case LEADER:
           raft_leader(obj);
           break;

   }
}

void raft_neutral(raftAppData_t* obj){

   if(obj->nodeIntersection >= 0){
      obj->nodeRole = FOLLOWER;
      obj->neutralFollower = true;
      obj->totalNodes = 1;
      raftSendDataToClient(obj);
      fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | ALIVE\n",obj->timeNow,obj->nodeIntersection);
   }else{
      obj->nodeRole = NEUTRAL;
   }

}

void raft_follower(raftAppData_t* obj){

    if(obj->timeNow*1E9 - obj->followerTime > obj->followerTimeout){
         heartbeatTimeoutCallback(obj);    
         obj->followerTime = (uint32_t)obj->timeNow*1E9;
    }

    // Check if timeout has occured.
    if (obj->hbeatTimeoutCounter % 20 == 0){

        // If there were more than one node in the network
        // a lost heartbeat means that the leader dropped off.
        // So reduce total nodes count
        if(obj->totalNodes > 1 && !obj->neutralFollower){
           obj->totalNodes--;
        }

        obj->nodeRole = CANDIDATE;
        fprintf(obj->logFile,"%.5f | MFID %d | CANDIDATE | ALIVE\n",obj->timeNow,obj->nodeIntersection);
        ClearPipes(obj);
        return;
    }else if(obj->hbeatTimeoutCounter % 3 == 0){
        if(obj->neutralFollower){
                raftHbeatAck_t hbtack;
                hbtack.intersectionID = obj->nodeIntersection;
                hbtack.followerID = obj->nodeID;
                hbtack.logIndex = -1;
                fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | SEND_NEUTRAL_FOLLOWER ID:%d\n",
                        obj->timeNow,obj->nodeIntersection,hbtack.logIndex);
                obj->heartBeatAckPipeOut.push_back(hbtack);
        }
    }

    // Wait for heartbeat from leader
    for(auto &hbeat: obj->heartBeatPipeIn){

        obj->neutralFollower = false;

        // Check if this heartbeat is for the network in the current intersection
        if(obj->nodeIntersection == hbeat.intersectionID){
            obj->hbeatTimeoutCounter = 1;

            fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | RECV_HBEAT ID:%d FROM LEADER NODE: %d\n",
                    obj->timeNow,
                    obj->nodeIntersection,
                    hbeat.logIndex,
                    hbeat.leaderID);

            // extract log data from heartbeat
            obj->nodeTerm   = hbeat.term;
            obj->totalNodes = hbeat.totalNodes;
            obj->leaderID   = hbeat.leaderID;
            memcpy(obj->mergingData,hbeat.log,sizeof(report_t)*MAX_NODES);
            for(int i=0;i<MAX_NODES;++i){
                obj->memberNodeID[i] = obj->mergingData[i].aircraftID;
                //fprintf(obj->logFile,"Received data for %d\n",obj->mergingData[i].aircraftID);
            }

            // Send heartbeat acknowledgements (this also includes client data)
            raftHbeatAck_t hbtack;
            hbtack.intersectionID = obj->nodeIntersection;
            hbtack.followerID = obj->nodeID;
            hbtack.logIndex = hbeat.logIndex;
            memcpy(&hbtack.nodeData,&obj->ownshipMergingData,sizeof(report_t));
            fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | SEND_HBEAT_ACK ID:%d\n",
                    obj->timeNow,obj->nodeIntersection,hbtack.logIndex);
            obj->heartBeatAckPipeOut.push_back(hbtack);

            // Send collected log data to client
            raftSendDataToClient(obj);
        }
    }

    for(auto &ack: obj->heartBeatAckPipeIn){
        bool avail = false;
        obj->memberNodeID[0] = obj->nodeID;
        for(int i=0;i<obj->totalNodes;++i){
            if(obj->memberNodeID[i] == ack.followerID){
                avail = true;
            }
        }

        if(!avail){
            obj->memberNodeID[obj->totalNodes] = ack.followerID;
            obj->totalNodes++;
            fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | DISCOVERED NEUTRAL FOLLOWER %d\n",
                    obj->timeNow,obj->nodeIntersection,ack.followerID);

        }
    }

    // Check for vote requests if from the network
    for(auto &voteRequest: obj->voteRequestPipeIn){
        // Make sure that the vote request is for the current intersection network
        if(obj->nodeIntersection != voteRequest.intersectionID){
            break;
        }
        fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | RECV_VOTE_REQUEST TERM:%d\n",obj->timeNow,obj->nodeIntersection,voteRequest.term);

        // Response with votes only if a vote for the same term hasn't been sent before.
       if(obj->votedTerm != voteRequest.term){
            raftVoteResponse_t voteResponse;
            voteResponse.term = voteRequest.term;
            voteResponse.candidateID = voteRequest.candidateID;
            voteResponse.intersectionID = voteRequest.intersectionID;
            obj->voteResponsePipeOut.push_back(voteResponse);
            fprintf(obj->logFile,"%.5f | MFID %d | FOLLOWER | SEND_VOTE\n",obj->timeNow,obj->nodeIntersection);
        }else{
            //fprintf(obj->logFile,"MFID: %d,FOLLOWER: Not Sending vote, Already voted: %d\n",obj->nodeIntersection,obj->votedTerm);
         }
    }

}

void raft_candidate(raftAppData_t* obj){

  if(obj->timeNow*1E9 - obj->candidateTime > obj->candidateTimeout){
       electionTimeoutCallback(obj);    
       obj->candidateTime = (uint32_t)obj->timeNow*1E9;
  }
  // The assumption is here is that there is already a valid intersection

  // If there are no other nodes at the intersection, this node becomes the leader
  if(obj->totalNodes == 1){
      obj->nodeRole = LEADER;
      fprintf(obj->logFile,"%.5f | MFID %d | LEADER | ALIVE\n",obj->timeNow,obj->nodeIntersection);
      ClearPipes(obj);
      return;
  }

  // Start election
  if(!obj->electionInitiated){
     obj->electionInitiated = true;
     obj->electionTimeoutCounter = 1;

     // Increment term
     obj->nodeTerm++;

     // Vote for self
     obj->totalVotesReceived = 1;

     // Send vote request messages
     raftVoteRequest_t voteRequest;
     voteRequest.intersectionID = obj->nodeIntersection;
     voteRequest.term = obj->nodeTerm;
     voteRequest.candidateID = obj->nodeID;
     obj->voteRequestPipeOut.push_back(voteRequest);
     fprintf(obj->logFile,"%.5f| MFID %d| CANDIDATE | SEND_VOTE_REQUEST TERM:%d, TOTAL NODES:%d |\n",obj->timeNow,obj->nodeIntersection,voteRequest.term,obj->totalNodes);
  }

  // If election timeout occurs go back to being a follower
  if(obj->electionTimeoutCounter % 3 == 0){
     obj->electionInitiated = true;
     fprintf(obj->logFile,"%.5f | CANDIDATE | TIME_OUT\n",obj->timeNow);
     obj->nodeRole = FOLLOWER;
     ClearPipes(obj);
     return;
  }

  // Receive and count votes
  for(auto &voteResponse: obj->voteResponsePipeIn){
      if (voteResponse.candidateID == obj->nodeID && voteResponse.term == obj->nodeTerm) {
          fprintf(obj->logFile, "%.5f | MFID %d | CANDIDATE | RECV_VOTE\n", obj->timeNow, obj->nodeIntersection);
          obj->totalVotesReceived++;
          if (obj->totalVotesReceived > (int)obj->totalNodes / 2) {
              obj->nodeRole = LEADER;
              memset(obj->mergingData, -1, sizeof(report_t) * MAX_NODES);
              memset(obj->memberNodeID, -1, sizeof(uint32_t) * MAX_NODES);
              obj->totalNodes = 1;
              ClearPipes(obj);
          }
      }
  }

  // Check to see if someone if already providing heartbeats
  for(auto &hbeat: obj->heartBeatPipeIn){
        obj->nodeRole = FOLLOWER;
        obj->hbeatTimeoutCounter = 1;
        ClearPipes(obj);
  }

}


void raft_leader(raftAppData_t* obj){

  GETTIME(); 
  if(obj->timeNow*1E9 - obj->leaderPipeInitialized > obj->leaderTimeout){
       leaderTimeoutCallback(obj);    
       obj->leaderTime = (uint32_t)obj->timeNow*1E9;
  }

  if(obj->leaderTimeoutCounter % 2 == 0){

      // Copy ownship data into global log at first position
      obj->memberNodeID[0] = obj->nodeID;
      memcpy(obj->mergingData,&obj->ownshipMergingData,sizeof(report_t));

      // Send heartbeat at regular intervals
      // Heartbeat also contains log data for all nodes
      raftHbeat_t hbeat;
      hbeat.totalNodes = obj->totalNodes;
      hbeat.intersectionID = obj->nodeIntersection;
      hbeat.leaderID = obj->nodeID;
      hbeat.term = obj->nodeTerm;
      hbeat.logIndex = ++obj->logDataIndex;

      for(int i=0;i<MAX_NODES;++i){
          if(obj->memberNodeID[i] == -1){
              obj->mergingData[i].aircraftID = -1;
          }
      }
      memcpy(hbeat.log,obj->mergingData,sizeof(report_t)*MAX_NODES);
      obj->heartBeatPipeOut.push_back(hbeat);

      fprintf(obj->logFile,"%.5f | MFID %d | LEADER | SEND_HBEAT ID:%d\n",obj->timeNow,obj->nodeIntersection,hbeat.logIndex);
      //for(int i=0;i<MAX_NODES;++i){
          //fprintf(obj->logFile,"ac id: %d\n",hbeat.log[i].aircraftID);
      //}

      // Send data to client
      raftSendDataToClient(obj);
  }

   // Wait for ack and determine nodes in the network
   for(auto &hbtack: obj->heartBeatAckPipeIn){
        if(hbtack.intersectionID != obj->nodeIntersection) {
            continue;
        }


        fprintf(obj->logFile,"%.5f | MFID %d | LEADER | RECV_HBEAT_ACK ID :%d FROM NODE: %d\n",
               obj->timeNow,
               obj->nodeIntersection,
               hbtack.logIndex,
               hbtack.followerID);

        uint32_t totalNodes = 0;
        bool nodeExists = false;

        for(int i=0;i<MAX_NODES;++i){
             if(obj->memberNodeID[i] >= 0){
                 if(obj->memberNodeID[i] == hbtack.followerID){
                     nodeExists = true;
                     memcpy(obj->mergingData+i,&(hbtack.nodeData),sizeof(report_t));
                     break;
                 }
             }else{
                 break;
             }
        }

        if(!nodeExists){
           fprintf(obj->logFile,"%.5f | MFID %d | LEADER | ADD_NEW_NODE\n",obj->timeNow,obj->nodeIntersection);
           totalNodes = obj->totalNodes;
           obj->memberNodeID[totalNodes] = hbtack.followerID;
           memcpy(obj->mergingData+totalNodes,&(hbtack.nodeData),sizeof(report_t));
           obj->totalNodes++;
        }
   }


   for(auto &hbt: obj->heartBeatPipeIn){
        if(hbt.intersectionID != obj->nodeIntersection) {
                  continue;
        }

        if(hbt.leaderID != obj->nodeID) {
            fprintf(obj->logFile,"%.5f | MFID %d | LEADER | RECV_HBEAT NODEID:%d |",obj->timeNow,obj->nodeIntersection,hbt.leaderID);
            if (hbt.term > obj->nodeTerm) {
                fprintf(obj->logFile,"Higher term");
                obj->nodeRole = FOLLOWER;
                obj->votedTerm = -1;
                obj->totalVotesReceived = 0;
                obj->hbeatTimeoutCounter = 1;
                ClearPipes(obj);
                return;
            } else if(hbt.leaderID < obj->nodeID){
                fprintf(obj->logFile,"Elder leader :%d",hbt.leaderID);
                //fprintf(obj->logFile,"Becoming a follower\n");
                obj->nodeRole = FOLLOWER;
                obj->votedTerm = -1;
                obj->totalVotesReceived = 0;
                ClearPipes(obj);
                return;
            }
            fprintf(obj->logFile,"\n");
        }
   }
}

void heartbeatTimeoutCallback(raftAppData_t* obj){
   obj->hbeatTimeoutCounter++;
   if(obj->nodeRole != FOLLOWER){
      obj->hbeatTimeoutCounter = 1;
   }
}

void electionTimeoutCallback(raftAppData_t* obj){
   obj->electionTimeoutCounter++;
   if(obj->nodeRole != CANDIDATE){
      obj->electionTimeoutCounter = 1;
   }
}

void leaderTimeoutCallback(raftAppData_t* obj){
   obj->leaderTimeoutCounter++;
   if(obj->nodeRole != LEADER){
      obj->leaderTimeoutCounter = 1;
   }

}

void raftProcessClientData(void* objIn,report_t* clientData){

   raftAppData_t* obj = (raftAppData_t*)objIn;
   // Client here is the ownship
   // Data from other aircraft are obtained through the hbeat ack messages
   obj->nodeIntersection = clientData->intersectionID;
   if(obj->nodeIntersection >= 0) {
       memcpy(&obj->ownshipMergingData, clientData, sizeof(report_t));
  
       fprintf(obj->logFile,"%.5f | CLIENT_DATA_RECV | Int ID: %d, NumSchedules: %d, Zone: %d, Arrival: %f\n",
               obj->timeNow,
               obj->ownshipMergingData.intersectionID,
               obj->ownshipMergingData.numSchedulesComputed,
               obj->ownshipMergingData.zoneStatus,
               obj->ownshipMergingData.currentArrivalTime);
   }else{
       obj->nodeRole = NEUTRAL;
       RAFT_AppInitializeData(obj,obj->nodeID,0,0,0);
       fprintf(obj->logFile,"%.5f | MFID -1 | NEUTRAL | ALIVE \n",obj->timeNow);
   }
}

void raftSendDataToClient(raftAppData_t* obj){

  obj->dataLog.totalNodes = obj->totalNodes;
  obj->dataLog.intersectionID = obj->nodeIntersection;
  obj->dataLog.nodeRole = obj->nodeRole;
  memcpy(obj->dataLog.log,obj->mergingData, sizeof(report_t)*MAX_NODES);


  fprintf(obj->logFile,"%.5f | CLIENT_DATA_SEND |",obj->timeNow);
  
  for(int i=0;i<MAX_NODES;++i){
     if(obj->dataLog.log[i].aircraftID >= 0){
        fprintf(obj->logFile,"< intersection: %d, zone: %d, a/c id: %d, arrival time: %f >",
                obj->dataLog.log[i].intersectionID,
                obj->dataLog.log[i].zoneStatus,
                obj->dataLog.log[i].aircraftID,
                obj->dataLog.log[i].currentArrivalTime);
     }
  }
  fprintf(obj->logFile,"\n");
}

void* RAFT_AppInitializeData(void* obj0,int id,int followerTimeout,int candidateTimeout,int leaderTimeout){

    raftAppData_t* obj;
    if(obj0 == nullptr){
         obj = new raftAppData_t;
         srand(time(0));
         // Get random timeouts
         rand();
         obj->followerTimeout = followerTimeout;
         obj->candidateTimeout = candidateTimeout;
         obj->leaderTimeout = leaderTimeoutCallback;
    }else{
         obj = (raftAppData_t*)obj0;
    }
    obj->nodeID            = id;
    obj->nodeRole          = NEUTRAL;
    obj->nodeIntersection  =  -1;
    obj->totalNodes        =  0;
    obj->nodeTerm          =  0;
    obj->electionInitiated = false;
    obj->neutralFollower   = true;
    memset(obj->memberNodeID,-1,sizeof(uint32_t)*MAX_NODES);
    memset(obj->mergingData,-1,sizeof(report_t)*MAX_NODES);
    for(int i=0;i<MAX_NODES;++i){
        obj->mergingData[i].aircraftID = -1; 
    }
    

    return (void*)obj;
}

void GetMergingData(void* obj,cDataLog_t* data){
   memcpy(data,&((raftAppData_t*)obj)->dataLog,sizeof(cDataLog_t));
}


int GetHeartbeat(void* objIn,raftHbeat_t* hbeat){
    raftAppData_t* obj = (raftAppData_t*) objIn;
    if(obj->heartBeatPipeOut.size() > 0){
        memcpy(hbeat,&obj->heartBeatPipeOut.front(),sizeof(raftHbeat_t));
        obj->heartBeatPipeOut.pop_front();
        return 1;
    }else{
        return 0;
    }
}

int GetHeartbeatAck(void* objIn,raftHbeatAck_t* hbtack){
    raftAppData_t* obj = (raftAppData_t*) objIn;
    if(obj->heartBeatAckPipeOut.size() > 0){
        memcpy(hbtack,&obj->heartBeatAckPipeOut.front(),sizeof(raftHbeatAck_t));
        obj->heartBeatAckPipeOut.pop_front();
        return 1;
    }else{
        return 0;
    }
}

int GetVoteRequest(void* objIn,raftVoteRequest_t* voteReq){
    raftAppData_t* obj = (raftAppData_t*) objIn;
    if(obj->voteRequestPipeOut.size() > 0){
        memcpy(voteReq,&obj->voteRequestPipeOut.front(),sizeof(raftVoteRequest_t));
        obj->voteRequestPipeOut.pop_front();
        return 1;
    }else{
        return 0;
    }
}

int GetVoteResponse(void* objIn,raftVoteResponse_t* voteRep){
    raftAppData_t* obj = (raftAppData_t*) objIn;
    if(obj->voteResponsePipeOut.size() > 0){
        memcpy(voteRep,&obj->voteResponsePipeOut.front(),sizeof(raftVoteResponse_t));
        obj->voteResponsePipeOut.pop_front();
        return 1;
    }else{
        return 0;
    }
}

void ClearPipes(raftAppData_t* obj){
   obj->heartBeatPipeIn.clear(); 
   obj->heartBeatPipeOut.clear();
   obj->heartBeatAckPipeIn.clear();
   obj->heartBeatAckPipeOut.clear();
   obj->voteRequestPipeIn.clear();
   obj->voteRequestPipeOut.clear();
   obj->voteResponsePipeIn.clear();
   obj->voteResponsePipeOut.clear();
}
