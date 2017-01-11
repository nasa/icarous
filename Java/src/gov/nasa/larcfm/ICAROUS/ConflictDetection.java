/**
 * Conflict
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

import com.MAVLink.icarous.msg_kinematic_bands;

import java.lang.*;

import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.ICAROUS.GeoFence.FENCE_TYPE;
import gov.nasa.larcfm.Util.*;

public class ConflictDetection{   

	private QuadFMS FMS;
	private AircraftData FlightData;
	public boolean keepInConflict;
	public boolean keepOutConflict;
	public boolean trafficConflict;
	public boolean flightPlanDeviationConflict;
	public double daaTimeElapsed;
	public double daaTimeStart;

	public GeoFence keepInFence;
	public GeoFence keepOutFence;
	public int numConflicts;

	public Daidalus DAA;
	public KinematicMultiBands KMB;
	public double daaLookAhead;
	public double timeStart;
	
	public ConflictDetection(QuadFMS fms){
		FMS = fms;
		FlightData = FMS.FlightData;	
		numConflicts = 0;
		DAA = new Daidalus();
		DAA.parameters.loadFromFile("params/DaidalusQuadConfig.txt");
		daaLookAhead = DAA.parameters.getLookaheadTime("s");
		timeStart    = (double)System.nanoTime()/1E9;
	}

	public int Size(){
		int count = 0;
		if(keepInFence != null){
			count++;
		}	
		if(keepOutFence != null){
			count++;
		}
		if(trafficConflict){
			count++;
		}
		if(flightPlanDeviationConflict){
			count++;
		}
		return count;
	}

	public void Clear(){
		keepInConflict = false;
		keepOutConflict = false;
		trafficConflict = false;
		flightPlanDeviationConflict = false;
		keepInFence = null;
		keepOutFence = null;
	}

	public void AddFenceConflict(GeoFence GF){
		if(GF.fType == FENCE_TYPE.KEEP_IN){
			keepInConflict = true;
			if(keepInFence == null){
				keepInFence = GF;
			}
			else if(keepInFence.ID != GF.ID){
				keepInFence = GF;
			}
		}
		else{
			keepOutConflict = true;
			if(keepOutFence == null){
				keepOutFence = GF;
			}
			else if(keepOutFence.ID != GF.ID){
				keepOutFence = GF;
			}
		}
	}

	public void CheckGeoFences(){
		keepInConflict  = false;
		keepOutConflict = false;
		Plan CurrentPlan;
		double elapsedTime;

		if(FMS.planType == QuadFMS.plan_type_t.MISSION ){
			CurrentPlan =  FlightData.MissionPlan;	
			elapsedTime = FMS.getApproxElapsedPlanTime(CurrentPlan,FlightData.nextMissionWP);
		}
		else if(FMS.planType == QuadFMS.plan_type_t.TRAJECTORY){
			CurrentPlan = FlightData.ResolutionPlan;
			elapsedTime = FMS.getApproxElapsedPlanTime(CurrentPlan,FlightData.nextResolutionWP);
		}
		else{
			return;
		}

		for(int i=0;i< FlightData.fenceList.size();i++){
			GeoFence GF = (GeoFence) FlightData.fenceList.get(i);
			GF.CheckViolation(FlightData.acState,elapsedTime,CurrentPlan);
			if(GF.conflict){
				AddFenceConflict(GF);
			}
		}
	}


	//Check standoff distance violation
	public void CheckFlightPlanDeviation(){

		double allowedDev   = FlightData.pData.getValue("XTRK_DEV");	//TODO: change parameter name in param file	
		Plan CurrentPlan = null;
		int nextWP;
		CurrentPlan =  FlightData.MissionPlan;	
		nextWP      = FlightData.nextMissionWP;
		Position CurrentPos = FlightData.acState.positionLast();
		double stats[] = ComputeCrossTrackDev(CurrentPos,CurrentPlan,nextWP);
		FlightData.crossTrackDeviation = stats[0];
		FlightData.crossTrackOffset    = stats[1];
		if(Math.abs(FlightData.crossTrackDeviation) > allowedDev){
			//System.out.println(FlightData.crossTrackDeviation);
			flightPlanDeviationConflict = true;
		}else if(Math.abs(FlightData.crossTrackDeviation) < (allowedDev)/3){
			flightPlanDeviationConflict = false;
		}
		
		if(FMS.planType == QuadFMS.plan_type_t.TRAJECTORY){
			flightPlanDeviationConflict = false;
			return;
		}
	}
	
	public double[] ComputeCrossTrackDev(Position pos,Plan fp,int nextWP){
		Plan CurrentPlan =  fp;	
		
		Position PrevWP     = CurrentPlan.point(nextWP - 1).position();
		Position NextWP     = CurrentPlan.point(nextWP).position();
		double psi1         = PrevWP.track(NextWP) * 180/Math.PI;
		double psi2         = PrevWP.track(pos) * 180/Math.PI;
		double sgn          = 0;	
		if( (psi1 - psi2) >= 0){
			sgn = 1;              // Vehicle left of the path
		}
		else if( (psi1 - psi2) <= 180){
			sgn = -1;             // Vehicle right of the path
		}
		else if( (psi1 - psi2) < 0 ){
			sgn = -1;             // Vehicle right of path
		}
		else if ( (psi1 - psi2) >= -180  ){
			sgn = 1;              // Vehicle left of path
		}
		double bearing = Math.abs(psi1 - psi2);
		double dist = PrevWP.distanceH(pos);
		double crossTrackDeviation = sgn*dist*Math.sin(Math.toRadians(bearing));
		double crossTrackOffset    = dist*Math.cos(Math.toRadians(bearing));
		
		double stats[] = new double[2];
		stats[0] = crossTrackDeviation;
		stats[1] = crossTrackOffset;
		
		return stats;
	}

	public void CheckTraffic(){
		
		KinematicBandsParameters kbParams = new KinematicBandsParameters();
 		kbParams.loadFromFile("params/DaidalusQuadConfig.txt");
 		double distance  = FlightData.pData.getValue("CYL_RADIUS");
 		double height    = FlightData.pData.getValue("CYL_HEIGHT");
 		double alertTime = FlightData.pData.getValue("ALERT_TIME");
 		double earlyAlertTime = FlightData.pData.getValue("EALERT_TIME");
 		double lookAheadTime = FlightData.pData.getValue("DAA_LOOKAHEAD");
 		
 		kbParams.alertor.getLevel(1).setDetector(new CDCylinder(distance, "m", height, "m"));
 		kbParams.alertor.getLevel(1).setAlertingTime(alertTime);
 		kbParams.alertor.getLevel(1).setEarlyAlertingTime(earlyAlertTime);
 		kbParams.setLookaheadTime(lookAheadTime);
 		
 		DAA.parameters = kbParams;
		
		double simTime = (double)System.nanoTime()/1E9 - timeStart;
		daaTimeElapsed = (double)System.nanoTime()/1E9 - daaTimeStart;

		
		Position so = FlightData.acState.positionLast();
		Velocity vo = FlightData.acState.velocityLast();
		
		DAA.setOwnshipState("Ownship",so,vo,simTime);

		double dist2traffic = Double.MAX_VALUE;
		for(int i=0;i<FlightData.traffic.size();i++){
			Position si = FlightData.traffic.get(i).pos;
			Velocity vi = FlightData.traffic.get(i).vel;
			DAA.addTrafficState("Traffic"+i,si,vi);
			double dist = so.distanceH(si);
			if(dist < dist2traffic){
				dist2traffic = dist;
			}
		}

		boolean daaViolation = false;		
		KMB = DAA.getKinematicMultiBands();
		if(KMB.regionOfTrack(DAA.getOwnshipState().track()).isConflictBand()){
			trafficConflict = true;
			daaTimeStart = (double)System.nanoTime()/1E9;
			daaViolation = true;
		}
	

		//TODO: this parameter depends on how long it takes to compute a resolution + start executing that resolution
		if(daaTimeElapsed > 5){
			if(!daaViolation){
				trafficConflict = FMS.Resolver.returnPathConflict;
			}
		}
		
		// Construct kinematic bands message to send to ground station
		msg_kinematic_bands msg = new msg_kinematic_bands();
		msg.sysid = 1;
		msg.numBands = (byte)KMB.trackLength();
		
		for(int i=0;i<msg.numBands;++i){
			Interval iv = KMB.track(i,"deg");
			BandsRegion br = KMB.trackRegion(i);
			int type = 0;
			if(br == BandsRegion.NONE){
				type = 0;
			}
			else if(br == BandsRegion.NEAR){
				type = 1;
			}
			 
			if(i==0){
				msg.type1 = (byte) type;
				msg.min1 = (float) iv.low;
				msg.max1 = (float) iv.up;
			}else if(i==1){
				msg.type2 = (byte) type;
				msg.min2 = (float) iv.low;
				msg.max2 = (float) iv.up;
			}else if(i==2){
				msg.type3 = (byte) type;
				msg.min3 = (float) iv.low;
				msg.max3 = (float) iv.up;
			}else if(i==3){
				msg.type4 = (byte) type;
				msg.min4 = (float) iv.low;
				msg.max4 = (float) iv.up;
			}else{
				msg.type5 = (byte) type;
				msg.min5 = (float) iv.low;
				msg.max5 = (float) iv.up;
			}
		}
		
		
		if(KMB.trackLength() > 1 || (KMB.trackLength() == 1 && KMB.trackRegion(0).isValidBand() &&
					     KMB.trackRegion(0) != BandsRegion.NONE)) { 
		    FlightData.RcvdMessages.AddKinematicBands(msg);
		}
		
		//System.out.println(KMB.outputString());
	}
	
	public boolean CheckTurnConflict(double low,double high,double newHeading,double oldHeading){
		
		if(newHeading < 0){
			newHeading = 360 + newHeading;
		}
		
		if(oldHeading < 0){
			oldHeading = 360 + oldHeading;
		}

		// Get direction of turn
		double psi   = newHeading - oldHeading;
		double psi_c = 360 - Math.abs(psi);
		boolean leftTurn = false;
		boolean rightTurn = false;
		if(psi > 0){
			if(Math.abs(psi) > Math.abs(psi_c)){
				leftTurn = true;
			}
			else{
				rightTurn = true;
			}
		}else{
			if(Math.abs(psi) > Math.abs(psi_c)){
				rightTurn = true;
			}
			else{
				leftTurn = true;
			}
		}
		
		double A,B,X,Y,diff;
		if(rightTurn){
			diff = oldHeading;
			A = oldHeading - diff;
			B = newHeading - diff;
			X = low - diff;
			Y = high - diff;
			
			if(B < 0){
				B = 360 + B;
			}
			
			if(X < 0){
				X = 360 + X;
			}
			
			if(Y < 0){
				Y = 360 + Y;
			}
			
			if(A < X && B > Y){
				return true;
			}
		}else{
			diff = 360 - oldHeading;
			A    = oldHeading + diff;
			B    = newHeading + diff;
			X = low + diff;
			Y = high + diff;
			
			if(B > 360){
				B = B - 360;
			}
			
			if(X > 360){
				X = X - 360;
			}
			
			if(Y > 360){
				Y = Y - 360;
			}
			
			if(A > Y && B < X){
				return true;
			}	
		}
		
		return false;
	}



}
