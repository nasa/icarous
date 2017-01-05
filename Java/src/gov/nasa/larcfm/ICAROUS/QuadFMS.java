/**
 * Aircraft
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
import com.MAVLink.enums.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import gov.nasa.larcfm.Util.*;
import java.util.*;

public class QuadFMS extends FlightManagementSystem{

	public enum resolve_state_t {IDLE, COMPUTE, MANEUVER, TRAJECTORY, RESUME};
	public enum trajectory_state_t {IDLE, START, FIX, ENROUTE, STOP};
	public enum maneuver_state_t {START,GUIDE,IDLE};
	public enum plan_type_t {MISSION,TRAJECTORY,MANEUVER};

	public float takeoffAlt; 
	boolean landStarted;
	boolean resumeMission;
	plan_type_t planType;
	resolve_state_t resolveState;
	trajectory_state_t trajectoryState;
	maneuver_state_t maneuverState;

	Velocity lastVelocity;
	ConflictDetection Detector;
	Resolution Resolver;
	Mission mission;
	Position NextGoal;
	boolean GoalReached;

	public QuadFMS(Interface ap_Intf,Interface com_Intf,AircraftData acData,Mission mc,ParameterData pdata){
		super("QuadFMS",acData,ap_Intf,com_Intf);
		takeoffAlt = 0.0f;
		landStarted = false;
		Detector = new ConflictDetection(this);
		Resolver = new Resolution(this);
		resolveState = resolve_state_t.IDLE;
		trajectoryState = trajectory_state_t.IDLE;
		maneuverState = maneuver_state_t.IDLE;
		mission = mc;
		planType = plan_type_t.MISSION;
	}

	@Override
	public void TAKEOFF(){
		takeoffAlt             = (float)FlightData.pData.getValue("TAKEOFF_ALT");
		Position currPosition  = FlightData.acState.positionLast();

		// set mode to guided
		SetMode(ARDUPILOT_MODES.GUIDED);

		// arm the copter
		SendCommand(0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,0,
				1,0,0,0,0,0,0);

		// send takeoff command
		SendCommand(0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,0,
				1,0,0,0, (float) currPosition.latitude(),
				(float) currPosition.longitude(),
				takeoffAlt);

		int ack = CheckAcknowledgement(MAV_CMD.MAV_CMD_NAV_TAKEOFF);

		if(ack == 1){
			fmsState = FMS_STATE_t._CLIMB_;
			apIntf.SendStatusText("Starting climb");
		}
		else{
			fmsState = FMS_STATE_t._IDLE_;
		}
	}

	@Override
	public void CLIMB(){
		// Switch to auto once targetAlt [m] is reached and start mission in auto mode
		double currentAlt = FlightData.acState.positionLast().alt();
		double error      = Math.abs( Math.abs(currentAlt - takeoffAlt));

		if( error < 0.5 ){
			fmsState = FMS_STATE_t._CRUISE_;
			SetMode(ARDUPILOT_MODES.AUTO);
			// Set speed
			FlightData.nextMissionWP++;
			float speed = FlightData.GetFlightPlanSpeed(FlightData.MissionPlan,FlightData.nextMissionWP);
			SetSpeed(speed);
		}
	}

	@Override
	public void CRUISE(){
		
		
		// Check for conflicts and determine mode
		int confSize = Monitor();

		if(confSize != Detector.numConflicts){
			//System.out.println("numConflicts"+confSize);
			Detector.numConflicts = confSize;
			if(confSize > 0){
				resolveState = resolve_state_t.COMPUTE;
			}
		}

		if(resolveState != resolve_state_t.IDLE){
			Resolve();
		}
		else{
			mission.Execute(this);
		}


		if((FlightData.nextMissionWP >= FlightData.numMissionWP)){
			fmsState = FMS_STATE_t._LAND_;
		}

	}

	@Override
	public void LAND(){	
		Position currPosition = FlightData.acState.positionLast();
		if(!landStarted){
			// Set mode to guided
			log.addWarning("MSG: Landing started");
			gsIntf.SendStatusText("Landing");
			SetMode(ARDUPILOT_MODES.GUIDED);
			SendCommand(0,0,MAV_CMD.MAV_CMD_NAV_LAND,0,
					0,0,0,0,
					(float) currPosition.latitude(),
					(float) currPosition.longitude(),
					(float) currPosition.alt());
			landStarted = true;
		}
	}

	@Override
	public void Reset(){
		fmsState = FMS_STATE_t._IDLE_;
		landStarted = false;
	}

	public int Monitor(){

		Detector.CheckGeoFences();
		
		if(!devAllowed){
			Detector.CheckFlightPlanDeviation();
		}

		if(FlightData.traffic.size() > 0){
			Detector.CheckTraffic();
		}

		return Detector.Size();
	}

	public void Resolve(){
		int status;
		switch(resolveState){

		case COMPUTE:
			// Call the relevant resolution functions to resolve conflicts
			if(Detector.trafficConflict){
				log.addWarning("MSG: Computing resolution for traffic conflict");
				gsIntf.SendStatusText("Traffic conflict");
				int cheapDAA = FlightData.pData.getInt("CHEAP_DAA");
				if(cheapDAA == 1){
					Resolver.ResolveTrafficConflictDAA();
				}else{
					Resolver.ResolveTrafficConflictRRT();
				}
			}
			else if(Detector.keepInConflict){
				log.addWarning("MSG: Computing resolution for keep in conflict");
				gsIntf.SendStatusText("Keep in conflict");
				Resolver.ResolveKeepInConflict();		
			}
			else if(Detector.keepOutConflict){
				SetMode(ARDUPILOT_MODES.GUIDED);
				log.addWarning("MSG: Computing resolution for keep out conflict");
				gsIntf.SendStatusText("Keep out conflict");
				int cheapSearch = FlightData.pData.getInt("CHEAP_SEARCH");
				double tstart1 = (double)System.nanoTime()/1E9;
				if(cheapSearch == 1){					
					Resolver.ResolveKeepOutConflictRRT();					
				}else{
					Resolver.ResolveKeepOutConflictAstar();
				}
				double tstart2 = (double)System.nanoTime()/1E9;
				System.out.format("RRT time:%f\n",(tstart2 - tstart1));
			}
			else if(Detector.flightPlanDeviationConflict){
				log.addWarning("MSG: Computing resolution for stand off conflict");
				gsIntf.SendStatusText("Flight plan deviation conflict");
				Resolver.ResolveFlightPlanDeviationConflict();
			}
			else{
				log.addWarning("MSG: No resolution");	    
			}

			// Two kinds of actions to resolve conflicts : 1. Plan based resolution, 2. Maneuver based resolution
			if(planType == plan_type_t.TRAJECTORY){
				resolveState    = resolve_state_t.TRAJECTORY;
				trajectoryState = trajectory_state_t.START;
			}
			else{
				resolveState    = resolve_state_t.MANEUVER;
				maneuverState  = maneuver_state_t.START;
			}

			break;

		case MANEUVER:
			// Execute maneuver based resolution when appropriate
			status = FlyManeuver();
			if(status == 1){
				resolveState = resolve_state_t.IDLE;
				SetMode(ARDUPILOT_MODES.AUTO);
			}

			break;

		case TRAJECTORY:

			status = FlyTrajectory();

			if(status == 1){
				if(resumeMission){
					System.out.println("Resuming mission\n");
					resolveState = resolve_state_t.IDLE;
					planType = plan_type_t.MISSION;
					SetMissionItem(FlightData.nextMissionWP);
					SetMode(ARDUPILOT_MODES.AUTO);
				}
				else{
					resolveState = resolve_state_t.RESUME;
				}
			}

			break;

		case RESUME:	    

			// Once resolution is complete, join the original mission
			// Resume mission
			Detector.Clear();
			ComputeInterceptCourse();
			resolveState = resolve_state_t.TRAJECTORY;
			trajectoryState = trajectory_state_t.START;
			GoalReached = true;
			
			break;


		case IDLE:
			break;

		}

		return;
	}

	public int FlyTrajectory(){
		int status = 0;
		float resolutionSpeed;
		NavPoint wp;
		Position current, next;
		double distH,distV;

		switch(trajectoryState){

		case START:
			System.out.print("executing trajectory resolution\n");
			FlightData.nextResolutionWP = 0;
			resolutionSpeed = (float)FlightData.pData.getValue("RES_SPEED");
			SetMode(ARDUPILOT_MODES.GUIDED); 
			SetSpeed(resolutionSpeed);
			trajectoryState = trajectory_state_t.FIX;
			break;

		case FIX:
			wp = FlightData.ResolutionPlan.point(FlightData.nextResolutionWP);
			SetGPSPos(wp.lla().latitude(),wp.lla().longitude(),wp.lla().alt());
			trajectoryState = trajectory_state_t.ENROUTE;
			break;

		case ENROUTE:
			current = FlightData.acState.positionLast();
			next    = FlightData.ResolutionPlan.point(FlightData.nextResolutionWP).position();
			distH     = current.distanceH(next);
			distV     = current.distanceV(next);

			if(distH < 1 && distV < 0.5){

				FlightData.nextResolutionWP++;
				if(FlightData.nextResolutionWP >= FlightData.ResolutionPlan.size()){
					trajectoryState = trajectory_state_t.STOP;
					FlightData.nextResolutionWP = 0;
				}
				else{
					trajectoryState = trajectory_state_t.FIX;
				}
			}

			break;

		case STOP:
			FlightData.nextResolutionWP = 0;
			FlightData.ResolutionPlan.clear();
			trajectoryState = trajectory_state_t.IDLE;
			status = 1;
			break;

		case IDLE:
			break;
		}

		return status;
	}

	public int FlyManeuver(){
		int status = 0;
		float resolutionSpeed = (float) FlightData.pData.getValue("RES_SPEED");

		switch(maneuverState){

		case START:
			System.out.print("executing maneuver resolution\n");
			SetMode(ARDUPILOT_MODES.GUIDED);
			maneuverState = maneuver_state_t.GUIDE;
			//Sleep here to allow mode change before sending commands
			try{
				Thread.sleep(100);
			}
			catch(InterruptedException e){
				System.out.println(e);
			}
			// Avoid break here to goto GUIDE state immediately without waiting for next cycle
			//break;

		case GUIDE:
			
			if(Detector.trafficConflict){
				Resolver.ResolveTrafficConflictDAA();
				SetYaw(FlightData.maneuverHeading);
				SetVelocity(FlightData.maneuverVn,FlightData.maneuverVe,FlightData.maneuverVu);
			}		
			else if(Detector.flightPlanDeviationConflict){
				Resolver.ResolveFlightPlanDeviationConflict();
				SetYaw(FlightData.maneuverHeading);
				SetVelocity(FlightData.maneuverVn,FlightData.maneuverVe,FlightData.maneuverVu);
			}
			else{
				System.out.print("finished maneuver resolution\n");
				maneuverState = maneuver_state_t.IDLE;
				Detector.Clear();
			}
			break;

		case IDLE:
			status = 1;
			break;

		}

		return status;
	}

	public void ComputeInterceptCourse(){
		
		Position current = FlightData.acState.positionLast();
		Position next;
		double distH;
		float speed;
		double ETA;
		NavPoint wp1,wp2;
		if(GoalReached){
			next    = FlightData.MissionPlan.point(FlightData.nextMissionWP).position();
			resumeMission = true;
		}
		else{
			next    = NextGoal;
			resumeMission = false;
		}
		
		distH = current.distanceH(next);
		speed  = (float)FlightData.pData.getValue("RES_SPEED");
		ETA   = distH/speed;
		wp1 = new NavPoint(current,0);
		wp2 = new NavPoint(next,ETA);

		FlightData.ResolutionPlan.add(wp1);
		FlightData.ResolutionPlan.add(wp2);
		FlightData.nextResolutionWP = 0;
		planType      = plan_type_t.TRAJECTORY;
		
	}
	
	@Override
	public void TRACKING(Position Target){
		
		// Get coordinates of object to track
		Position CurrentPos = FlightData.acState.positionLast();
		
		// Get tracking position
		// This is determined by parameters - distance and heading
		double heading = FlightData.pData.getValue("TRK_HEADING");
		double distH   = FlightData.pData.getValue("TRK_DISTH");
		double distV   = FlightData.pData.getValue("TRK_DISTV");   

		double distHx  = distH*Math.sin(Math.toRadians(heading)); // Heading is measured from North
		double distHy  = distH*Math.cos(Math.toRadians(heading));

		double Kx_trk = 0.5;
		double Ky_trk = 0.5;
		double Kz_trk = 0.5;

		Vect3 delPos   = new Vect3(distHx,distHy,distV);
			
		// Project from LatLonAlt to cartesian coordinates
		
		EuclideanProjection proj = Projection.createProjection(CurrentPos.mkAlt(0));
		Vect3 vecCP  =  proj.project(CurrentPos);
		Vect3 vecTP  =  proj.project(Target);

		Vect3 vecTPf =  vecTP.Add(delPos);
		
		// Relative vector to object
		Vect3 Rel    = vecTPf.Sub(vecCP);
			
		// Compute velocity commands that will enure smooth tracking of object
		double dx = Rel.x();
		double dy = Rel.y();
		double dz = Rel.z();

		// Velocity is proportional to distance from object.
		double Vx = SaturateVelocity(Kx_trk * dx,2.0);
		double Vy = SaturateVelocity(Ky_trk * dy,2.0);
		double Vz = SaturateVelocity(Kz_trk * dz,2.0);

		//System.out.println(vecCP.toString());
		//System.out.println(vecTPf.toString());
		//System.out.format("dx,dy,dz = %f,%f,%f\n",dx,dy,dz);
		//System.out.format("Heading = %f, Velocities %1.3f, %1.3f, %1.3f\n",heading,Vx,Vy,Vz);

		// Vn, Ve, Vd
		SetVelocity(Vy,Vx,-Vz);

		double RefHeading = Math.toDegrees(Math.atan2(Vx,Vy));

		if(RefHeading < 0){
		    RefHeading = 360 + RefHeading;
		}

		if(Rel.norm() > 0.3){
			SetYaw(RefHeading);
		}
		
	}
	
	public double SaturateVelocity(double V, double Vsat){
		if(Math.abs(V) > Vsat){
			return Math.signum(V)*Vsat;
		}
		else{
			return V;
		}
	}

}
