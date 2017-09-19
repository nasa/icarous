/**
 * AircraftData
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
package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.ardupilotmega.*;
import com.MAVLink.enums.*;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Geofence;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Object;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Waypoint;
import gov.nasa.larcfm.Util.AircraftState;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Pair;

public class AircraftData{

	
	public enum GF{
		GF_READ, GF_FETCH, GF_READ_COMPLETE
	}

	public MAVLinkMessages RcvdMessages;

        public double acTime;
    
	// Aircraft attitude
	public double roll;
	public double pitch;
	public double yaw;
	public double heading;
	public double speed;

	public double maneuverHeading;
	public double maneuverVn,maneuverVe,maneuverVu;


	// Aircraft's current position (GPS)
	public AircraftState acState;

	//public FlightPlan NewFlightPlan;
	//public FlightPlan CurrentFlightPlan;
	public Plan MissionPlan;
	public List<Integer> WaypointIndices;
	public List<Pair<Integer,Integer>> WPMissionItemMapping;
	public Plan ResolutionPlan;

	public List<msg_Geofence> tempFenceList;
	public List<GeoFence> fenceList;
	public List<msg_Waypoint>InputFlightPlan;

	// List for various objects
	public List<GenericObject> obstacles;
	public List<GenericObject> traffic;
	public List<GenericObject> missionObj;

	// List for traffic information

	// Flight plan specific data
	public double FP_maxHorizontalDeviation;
	public double FP_maxVerticalDeviation;
	public double FP_standoffDistance;
	public int numMissionWP;
	public int nextMissionWP;
	public int nextResolutionWP;

	public int startMission; // -1: last command executed, 0 - stop mission, 1 - start mission
	public double planTime;
	public ParameterData pData;
	public boolean reset;
	
	public double missionSpeed;

	double crossTrackDeviation;
	double crossTrackOffset;

	public AircraftData(ParameterData pdata){

		RcvdMessages        = new MAVLinkMessages();
		acState             = new AircraftState();
		fenceList           = new ArrayList<GeoFence>();
		tempFenceList       = new ArrayList<msg_Geofence>();
		obstacles           = new ArrayList<GenericObject>();
		traffic             = new ArrayList<GenericObject>();
		missionObj          = new ArrayList<GenericObject>();
		InputFlightPlan     = new ArrayList<msg_Waypoint>();		
		WaypointIndices     = new ArrayList<Integer>();
		WPMissionItemMapping = new ArrayList<Pair<Integer,Integer>>();
		startMission        = -1;
		nextMissionWP       = 0;
		nextResolutionWP    = 0;
		numMissionWP        = 0;
		pData               = pdata;
		reset               = false;
		MissionPlan         = new Plan();
		ResolutionPlan      = new Plan();
		speed               = 1;
		missionSpeed        = 1;
	}
	
	public synchronized void AddMissionItem(msg_Waypoint waypoint){
		InputFlightPlan.add(waypoint);
	}
	
	public synchronized int GetStartMissionFlag(){
		int var = startMission;
		startMission = -1;
		return var;
	}
	
	public synchronized void SetStartMissionFlag(int param){
		startMission = param;
	}
	
	public synchronized int GetFlightPlanSize(){
		return InputFlightPlan.size();
	}
	
	public synchronized void ClearMissionList(){
		InputFlightPlan.clear();
	}
	
	public synchronized void AddTraffic(int id,float lat,float lon,float alt,float vx,float vy,float vz){
		GenericObject obj = new GenericObject(1,id,lat,lon,alt,vx,vy,vz);		
		GenericObject.AddObject(traffic,obj);
		
	}
	
	public synchronized void AddGeofence(msg_Geofence gf){
		if(gf.vertexIndex == 0){
			tempFenceList.clear();
		}
		
		tempFenceList.add(gf);
		
		if(gf.vertexIndex+1 == gf.totalVertices){
			
			GeoFence GF = new GeoFence(gf.index,gf.type,gf.totalVertices,gf.floor,gf.ceiling,pData);
			for(int i=0;i<tempFenceList.size();i++){
				msg_Geofence fence = tempFenceList.get(i);
				GF.AddVertex(i, fence.latitude, fence.longitude);
			}
			
			if(fenceList.size() <= gf.index){
				fenceList.add(GF);
				System.out.println("Received fence:"+gf.index);
			}else{
				fenceList.remove(gf.index);
				fenceList.add(gf.index, GF);
			}
		}
	}

	// Function to get flight plan
	public int GetGeoFence(Interface1 Intf,msg_command_long msg){

		boolean readComplete = false;
		int count = 0;
		int COUNT = (int) msg.param4;
		GeoFence fence1 = new GeoFence((int)msg.param2,(int)msg.param3,(int)msg.param4,msg.param5,msg.param6,pData);

		if(COUNT < 1){
			if( (fence1.ID + 1) <= fenceList.size() ){
				fenceList.remove(fence1.ID);	
			}
			return 1;
		}

		msg_fence_fetch_point msgFenceFetchPoint = new msg_fence_fetch_point();

		msgFenceFetchPoint.sysid            = 1;
		msgFenceFetchPoint.compid           = 1;
		msgFenceFetchPoint.target_system    = (short) msg.sysid;
		msgFenceFetchPoint.target_component = (short) msg.compid;

		GF state = GF.GF_FETCH;

		ArrayList<msg_fence_point> InputFence = new ArrayList<msg_fence_point>();

		long time1 = System.nanoTime();

		while(!readComplete){

			long time2 = System.nanoTime();
			if(time2 - time1 > 3E9){
				break;
			}

			switch(state){

			case GF_FETCH:

				msgFenceFetchPoint.idx = (byte)count;		    		    
				Intf.Write(msgFenceFetchPoint);					    		
				//System.out.println("Wrote count:"+count);
				state = GF.GF_READ;	       

				break;

			case GF_READ:

				Intf.Read();								
				msg_fence_point msgFencePoint = RcvdMessages.GetFencePoint();
				if(msgFencePoint != null){

					//System.out.println("geofence point received:"+msgFencePoint.idx);
					if(msgFencePoint.idx == count){
						InputFence.add(msgFencePoint);
						fence1.AddVertex(msgFencePoint.idx,msgFencePoint.lat,msgFencePoint.lng);			    
						count++;
						time1 = time2;
						state = GF.GF_FETCH;    	       
					}
					else{
						// Send failure acknowledgement
						//msg_command_ack msgCommandAck = new msg_command_ack();
						//msgCommandAck.result = 0;
						//Intf.Write(msgCommandAck);
					}
				}

				if(count >= COUNT){
					state = GF.GF_READ_COMPLETE;
				}

				break;

			case GF_READ_COMPLETE:

				readComplete = true;
				//System.out.println("Received fence");
				if( fence1.ID >= fenceList.size() ){
					fenceList.add(fence1);
				}
				else{
					fenceList.set(fence1.ID,fence1);
				}
				msg_command_ack msgCommandAck = new msg_command_ack();
				msgCommandAck.command = MAV_CMD.MAV_CMD_DO_FENCE_ENABLE;
				msgCommandAck.result = 1;
				Intf.Write(msgCommandAck);		
				//fence1.print();
				return 1;
			} // end of switch case
		}//end of while
		return 0;
	}//end of function

	public void ConstructFlightPlan(){
		// Make a new Plan using the input mission item
		MissionPlan = new Plan();
		WaypointIndices.clear();
		WPMissionItemMapping.clear();
		msg_Waypoint msgMissionItem;
		int count = 0;
		for(int i=0;i<InputFlightPlan.size();i++){
			msgMissionItem = InputFlightPlan.get(i);
														
				double wptime= 0;
				Position nextWP = Position.makeLatLonAlt(msgMissionItem.latitude,"degree",
														 msgMissionItem.longitude,"degree",
														 msgMissionItem.altitude,"m");				
				if(count > 0 ){
					//double vel = msgMissionItem.param4;
					double vel = msgMissionItem.speed;
					if(vel < 0.5){
						vel = 1;
					}

					double distance = MissionPlan.point(count - 1).position().distanceH(nextWP);
					if(distance < 0.01){
						distance = MissionPlan.point(i - 1).position().distanceV(nextWP);
						vel      = 1;
					}
					wptime          = MissionPlan.time(count-1) + distance/vel;

					//System.out.println("Times:"+wptime);
				}		     
				MissionPlan.addNavPoint(new NavPoint(nextWP,wptime));
				count++;			
		}
		numMissionWP           = MissionPlan.size();		
	}

	public float GetFlightPlanSpeed(Plan fp,int nextWP){

		Plan CurrentFlightPlan = fp;		
		float speed = (float)(CurrentFlightPlan.pathDistance(nextWP-1,true)/
				(CurrentFlightPlan.time(nextWP) -
						CurrentFlightPlan.time(nextWP-1)));

		return speed;
	}

	public synchronized void Reset(){
		fenceList.clear();
		obstacles.clear();
		traffic.clear();
		missionObj.clear();
		InputFlightPlan.clear();
		startMission = -1;
		nextMissionWP = 0;
		nextResolutionWP = 0;
		reset = true;
	}

}
