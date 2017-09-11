/**
 * Aircraft data
 * 
 * Shared data structure containing all flight relevant data and functions
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. 
 *  All rights reserved.
 *     
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, 
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, 
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT 
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS 
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND 
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE 
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

#ifndef AIRCRAFT_DATA_H
#define AIRCRAFT_DATA_H
#include <list>
#include "ParameterData.h"
#include "AircraftState.h"
#include "Position.h"
#include "Velocity.h"
#include "Plan.h"
#include "Geofence.h"
#include "time.h"
#include "Port.h"
#include "GenericObject.h"
#include "Icarous_msg.h"

class AircraftData_t{
    private:
        pthread_mutex_t lock;
        uint8_t startMission;

    public:
        //member variables
        double roll, pitch, yaw, heading;
		double crossTrackDeviation;
		double crossTrackOffset;
		double maneuverVn,maneuverVe,maneuverVu;
		double maneuverHeading;
		bool reset;
		double speed;
		double acTime;
		double missionSpeed;

        uint16_t nextMissionWP;
        uint16_t nextResolutionWP;
        larcfm::ParameterData* paramData;
        larcfm::AircraftState acState;
        larcfm::Plan MissionPlan;
        larcfm::Plan ResolutionPlan;
        std::list<Geofence_t> fenceList;
        std::list<Geofence_t>::iterator fenceListIt;
        std::list<GenericObject_t> trafficList;
	    std::list<ArgsCmd_t> outputList;
	    std::list<CmdAck_t> commandAckList;
	    std::list<waypoint_t> listMissionItem;
	    visbands_t visBands;

        //Member functions
        AircraftData_t(){};
        AircraftData_t(larcfm::ParameterData* pData);
        void AddMissionItem(waypoint_t* msg);
        uint8_t GetStartMissionFlag();
        void SetStartMissionFlag(uint8_t flag);
        uint16_t GetFlightPlanSize();
        void ConstructPlan();
        void AddTraffic(int id,double x,double y,double z,double vx,double vy,double vz);
        void GetTraffic(int id,larcfm::Position &pos,larcfm::Velocity &vel);
        void ClearMissionList();
        void Reset();
	    bool CheckAck(command_name_t command);
	    void InputAck(CmdAck_t* ack);
        double getFlightPlanSpeed(larcfm::Plan *fp,int nextWP);
};


 #endif
