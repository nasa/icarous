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

	private Daidalus DAA;
	private KinematicMultiBands KMB;
	double daaLookAhead;
	
	public ConflictDetection(QuadFMS fms){
		FMS = fms;
		FlightData = FMS.FlightData;	
		numConflicts = 0;
		DAA = new Daidalus();
		DAA.parameters.loadFromFile("params/DaidalusQuadConfig.txt");
		daaLookAhead = DAA.parameters.getLookaheadTime("s");
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
		double elapsedTime = 0;
		int nextWP;

		if(FMS.planType == QuadFMS.plan_type_t.MISSION ){
			CurrentPlan =  FlightData.MissionPlan;	
			elapsedTime = FMS.getApproxElapsedPlanTime(CurrentPlan,FlightData.nextMissionWP);
			nextWP      = FlightData.nextMissionWP;
		}
		else{
			flightPlanDeviationConflict = false;
			return;
		}

		Position PrevWP     = CurrentPlan.point(nextWP - 1).position();
		Position NextWP     = CurrentPlan.point(nextWP).position();
		Position CurrentPos = FlightData.acState.positionLast();
		double psi1         = PrevWP.track(NextWP) * 180/Math.PI;
		double psi2         = PrevWP.track(CurrentPos) * 180/Math.PI;
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
		double dist = PrevWP.distanceH(CurrentPos);
		FlightData.crossTrackDeviation = sgn*dist*Math.sin(Math.toRadians(bearing));
		FlightData.crossTrackOffset    = dist*Math.cos(Math.toRadians(bearing));
		if(Math.abs(FlightData.crossTrackDeviation) > allowedDev){
			flightPlanDeviationConflict = true;
		}else if(Math.abs(FlightData.crossTrackDeviation) < (allowedDev)/3){
			flightPlanDeviationConflict = false;
		}
	}

	public void CheckTraffic(){
		daaTimeElapsed = (double)System.nanoTime()/1E9 - daaTimeStart;

		Position so = FlightData.acState.positionLast();
		Velocity vo = FlightData.acState.velocityLast();

		DAA.reset();
		DAA.setOwnshipState("Ownship",so,vo,0.0);

		for(int i=0;i<FlightData.traffic.size();i++){
			Position si = FlightData.traffic.get(i).pos;
			Velocity vi = FlightData.traffic.get(i).vel;
			DAA.addTrafficState("Traffic"+i,si,vi);
		}

		double qHeading = vo.track("degree");

		boolean daaViolation = false;
		for(int ac = 1;ac<DAA.numberOfAircraft();ac++){
			double tlos = DAA.timeToViolation(ac);
			if(tlos >=0 && tlos <= daaLookAhead){
				KMB = DAA.getKinematicMultiBands();
				for(int ib=0;ib<KMB.trackLength();++ib){
					if(KMB.trackRegion(ib) != BandsRegion.NONE ){
						Interval ii = KMB.track(ib,"deg");
						if(qHeading > ii.low && qHeading < ii.up){
							trafficConflict = true;
							daaTimeStart = (double)System.nanoTime()/1E9;
							daaViolation = true;
						}
					}
				}
			}
		}

		if(daaTimeElapsed > 10){
			if(!daaViolation){
				trafficConflict = false;
			}
		}
	}


}
