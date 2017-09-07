/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "IpcIcarousAdapter.h"

#include "Debug.hh"
#include "Error.hh"
#include "ThreadSpawn.hh"

#include <sys/time.h>

/**
 * @brief Constructor. Opens the connection and spawns a listener thread.
 */
IpcIcarousAdapter::IpcIcarousAdapter(const std::string& centralhost,Icarous_t* ic)
  : icarous(ic),
    m_stateUIDMap(),
    m_ipcFacade(),
    m_listener(*this)
{
  assertTrueMsg(m_ipcFacade.initialize(m_ipcFacade.getUID().c_str(), centralhost.c_str()) == IPC_OK,
      "IpcIcarousAdapter: Unable to initialize ipc to central server at " << centralhost);

  // Spawn listener thread
  assertTrueMsg(m_ipcFacade.start() == IPC_OK,
		"IpcIcarousAdapter constructor: Unable to start IPC dispatch thread");

  // Subscribe only to messages we care about
  m_ipcFacade.subscribe(&m_listener, PlexilMsgType_Command);
  m_ipcFacade.subscribe(&m_listener, PlexilMsgType_LookupNow);
  debugMsg("IpcIcarousAdapter:IpcIcarousAdapter", " succeeded");
}

/**
 * @brief Destructor. Shuts down the listener thread and closes the connection.
 */
IpcIcarousAdapter::~IpcIcarousAdapter()
{
}

/**
 * @brief Send a command to RoboSim
 */
void IpcIcarousAdapter::processCommand(const std::vector<const PlexilMsgBase*>& msgs)
{
  PlexilStringValueMsg const *cmdMsg = (PlexilStringValueMsg const *) msgs[0];
  const std::string cmdName(cmdMsg->stringValue);
  IpcMessageId transId = IpcMessageId(msgs[0]->senderUID, msgs[0]->serial);
	
  if (cmdName == "RunIdleChecks"){
	  //std::cout<<"running idle checks"<<std::endl;
	  PLEXIL::Value returnVal(PLEXIL::Integer(icarous->FMS->IDLE()));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "SetMode"){
	  PlexilStringValueMsg const *nameMsg = (PlexilStringValueMsg const *) msgs[1];
	  std::string mode(nameMsg->stringValue);
	  if(mode == "GUIDED")
		  icarous->FMS->SetMode(_ACTIVE_);
	  else if(mode =="AUTO")
		  icarous->FMS->SetMode(_PASSIVE_);
  }

  else if(cmdName == "ArmMotors"){
	  PlexilBooleanValueMsg const *valueMsg = (PlexilBooleanValueMsg const *) msgs[1];
	  bool val = valueMsg->boolValue;
	  icarous->FMS->ArmThrottles(val);
  }

  else if(cmdName == "ThrottleUp"){
	  PLEXIL::Value returnVal(PLEXIL::Boolean(icarous->FMS->ThrottleUp()?true:false));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "CheckKeepInGeofence"){
	  icarous->FMS->Detector.CheckGeofence();
	  PLEXIL::Value returnVal(PLEXIL::Boolean(icarous->FMS->Detector.keepInConflict));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "CheckKeepOutGeofence"){
	  // Only send conflict flag - keep out geofences are checked for conflicts in previous if condition
	  PLEXIL::Value returnVal(PLEXIL::Boolean(icarous->FMS->Detector.keepOutConflict));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "CheckTraffic"){
	  icarous->FMS->Detector.CheckTraffic();
	  PLEXIL::Value returnVal(PLEXIL::Boolean(icarous->FMS->Detector.trafficConflict));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "CheckFlightPlan"){
	  PlexilBooleanValueMsg const *valueMsg = (PlexilBooleanValueMsg const *) msgs[1];
	  bool val = valueMsg->boolValue;
	  icarous->FMS->Detector.CheckFlightPlanDeviation(val);
	  PLEXIL::Value returnVal(PLEXIL::Boolean(icarous->FMS->Detector.flightPlanDeviationConflict));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "SetYaw"){
	  PlexilRealValueMsg const *valueMsg = (PlexilRealValueMsg const *) msgs[1];
	  double val = valueMsg->doubleValue;
	  icarous->FMS->SetYaw(val);
	  PLEXIL::Value returnVal(PLEXIL::Boolean(true));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "ComputeKeepInResolution"){
	  bool val = icarous->FMS->Resolver.ResolveKeepInConflict();
	  PLEXIL::Value returnVal(PLEXIL::Boolean(val));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "ComputeKeepOutResolution"){
	  bool val = icarous->FMS->Resolver.ResolveKeepOutConflict_Astar();
	  PLEXIL::Value returnVal(PLEXIL::Boolean(val));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "ComputeTrafficResolution"){
	  std::cout<<"Computed traffic resolution"<<std::endl;
	  PLEXIL::Value returnVal(PLEXIL::Boolean(true));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "ComputeFlightPlanResolution"){
	  std::cout<<"Computed traffic resolution"<<std::endl;
	  PLEXIL::Value returnVal(PLEXIL::Boolean(true));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "FlyTrajectory"){
	  PlexilBooleanValueMsg const *valueMsg = (PlexilBooleanValueMsg const *) msgs[1];
	  bool val = valueMsg->boolValue;
	  uint8_t rval = icarous->FMS->FlyTrajectory(val);
	  PLEXIL::Value returnVal(PLEXIL::Boolean(rval?true:false));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "FlyManeuver"){
	  PlexilBooleanValueMsg const *valueMsg = (PlexilBooleanValueMsg const *) msgs[1];
	  bool val = valueMsg->boolValue;
	  uint8_t rval = icarous->FMS->FlyManeuver(val);
	  PLEXIL::Value returnVal(PLEXIL::Boolean(rval?true:false));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "SetNextWPParameters"){
	  icarous->FMS->SetNextWPParameters();
  }

  else if(cmdName == "ClearConflictData"){
	  std::cout<<"Clearing all conflicts"<<endl;
	  icarous->FMS->Detector.clear();
	  PLEXIL::Value returnVal(PLEXIL::Boolean(true));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "ComputeInterceptCourse"){
	  std::cout<<"Compute intercept course"<<endl;
	  icarous->FMS->ComputeInterceptCourse();
	  PLEXIL::Value returnVal(PLEXIL::Boolean(true));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(cmdName == "StartLandingSequence"){
	  std::cout<<"Starting landing sequence"<<endl;
	  icarous->FMS->LAND();
  }
}

/**
 * @brief Deal with a LookupNow request
 */

void IpcIcarousAdapter::processLookupNow(const std::vector<const PlexilMsgBase*>& msgs)
{
  std::string stateName(((const PlexilStringValueMsg*)msgs[0])->stringValue);
  //std::cout<<"Received lookup:"<<stateName<<std::endl;
  IpcMessageId transId = IpcMessageId(msgs[0]->senderUID, msgs[0]->serial);

  if(stateName == "altitudeAGL"){
	  PLEXIL::Value returnVal(PLEXIL::Real(icarous->FMS->FlightData->acState.positionLast().alt()));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(stateName == "totalWP"){
	  PLEXIL::Value returnVal(PLEXIL::Integer(icarous->FMS->FlightData->MissionPlan.size()));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }

  else if(stateName == "currentWP"){
	  PLEXIL::Value returnVal(PLEXIL::Integer(icarous->FMS->FlightData->nextMissionWP));
	  m_ipcFacade.publishReturnValues(transId.second, transId.first,returnVal);
  }
}


IpcIcarousAdapter::MessageListener::MessageListener(IpcIcarousAdapter& adapter) :
  m_adapter(adapter) {
}
IpcIcarousAdapter::MessageListener::~MessageListener() {
}
void IpcIcarousAdapter::MessageListener::ReceiveMessage(const std::vector<const PlexilMsgBase*>& msgs) {
  const PlexilMsgBase* leader = msgs[0];
  switch (leader->msgType)
    {
    case PlexilMsgType_Command:
      m_adapter.processCommand(msgs);
      break;

    case PlexilMsgType_LookupNow:
      m_adapter.processLookupNow(msgs);
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
            "IpcIcarousAdapter::ReceiveMessage: invalid leader message type " << leader->msgType);
    }
}
