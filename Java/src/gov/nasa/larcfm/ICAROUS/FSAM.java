/**
 * Flight Safety Assessment and Management (FSAM)
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;
import java.io.*;
import com.MAVLink.common.*;
import com.MAVLink.enums.*;

import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ACCoRD.*;


enum FSAM_OUTPUT{
    CONFLICT,NOOP, TERMINATE
}

enum RESOLVE_STATE{

    COMPUTE, EXECUTE_MANEUVER, EXECUTE_PLAN, MONITOR, JOIN, RESUME, NOOP
}

enum EXECUTE_STATE{
    START, SEND_COMMAND, AWAIT_COMPLETION, COMPLETE, PAUSE
}

public class FSAM{

    private Aircraft UAS;
    private AircraftData FlightData;
    private MAVLinkMessages Inbox;
    private Mission mission;
    private Interface apIntf;
    private double distance2WP;
    private double heading2WP;

    private long timeEvent1;
    private long timeElapsed;
    private long timeCurrent;
    private double logtime;
    
    private FSAM_OUTPUT output;

    private List<Conflict> conflictList;
    
    private Plan ResolutionPlan;
    
    private int currentResolutionWP;

    public RESOLVE_STATE resolveState;
    public EXECUTE_STATE executeState;

    private boolean FenceKeepInConflict;
    private boolean FenceKeepOutConflict;
    private boolean TrafficConflict;    
    private boolean StandoffConflict;
    private boolean GotoNextWP;
    private boolean joined;
    private boolean UsePlan;
    
    private int currentConflicts;
    private boolean NominalPlan;

    
    private double Vn1,Vn2;
    private double Ve1,Ve2;
    private double Vu1,Vu2;
    private double RefHeading1;
    private double RefHeading2;
    
    public double crossTrackDeviation;
    private double crossTrackOffset;
    private Daidalus daa;
    private int daaTick;
    private KinematicMultiBands KMB;

    private double daaLookahead;

    private long pausetime_start;
    private double resolutionSpeed;
    private double gridsize;
    private double buffer;
    private double lookahead;
    private double proximityfactor;
    private double standoff;
    private String daidalusparam;
    
    public FSAM(Aircraft ac,Mission ms){
	UAS                      = ac;
	FlightData               = ac.FlightData;
	Inbox                    = FlightData.Inbox;
	mission                  = ms;
	timeEvent1               = 0;
	timeElapsed              = 0;
	apIntf                   = ac.apIntf;
	conflictList             = new ArrayList<Conflict>();
	ResolutionPlan           = new Plan();
	resolveState             = RESOLVE_STATE.NOOP;
	executeState             = EXECUTE_STATE.COMPLETE;
	FenceKeepInConflict      = false;
	FenceKeepOutConflict     = false;
	TrafficConflict          = false;
	StandoffConflict         = false;	
	NominalPlan              = true;
	UsePlan                  = true;
	currentResolutionWP      = 0;
	currentConflicts         = 0;
	joined                   = true;
	RefHeading1              = Double.NaN;
	standoff  = UAS.pData.getValue("STANDOFF");		
	resolutionSpeed   = (float) UAS.pData.getValue("RES_SPEED");
	gridsize          = UAS.pData.getValue("GRIDSIZE");
	buffer            = UAS.pData.getValue("BUFFER");
	lookahead         = UAS.pData.getValue("LOOKAHEAD");
	proximityfactor   = UAS.pData.getValue("PROXFACTOR");
	daidalusparam     = "params/DaidalusQuadConfig.txt";
	
	// Create an object of type Daidalus for a well-clear volume based on TAUMOD
	daa = new Daidalus();
	
	daa.parameters.loadFromFile(daidalusparam);

	daaLookahead = daa.parameters.getLookaheadTime("s");
	// Kinematic bands			
	daaTick = 0;
	KMB = null;	
    }

    public void SetDaaConfig(String filename){
	daa.parameters.loadFromFile(filename);
	daaLookahead = daa.parameters.getLookaheadTime("s");
    }

    

    static public AlertLevels AlertQuad(double radius,double height,double alerttime1,double alerttime2) {		
	AlertLevels alertor = new AlertLevels();
	alertor.setConflictAlertLevel(1);		

	alertor.addLevel(new AlertThresholds(new CDCylinder(radius,"m",height,"m"),alerttime1,alerttime2,BandsRegion.NEAR));

	return alertor;
    }

    // Function to monitor conflicts (Geofence, flight plan deviation, air traffic etc...)
    public FSAM_OUTPUT Monitor(){

	Plan FlightPlan       = FlightData.CurrentFlightPlan;

	if(FlightPlan == null){
	    return FSAM_OUTPUT.NOOP;
	}
	
	AircraftState acState = FlightData.acState;
	
	timeCurrent    = UAS.timeCurrent;	
	timeElapsed    = timeCurrent - timeEvent1;
	
	if(timeEvent1 == 0){
	    timeEvent1 = UAS.timeStart;
	}

	//Get distance to next waypoint
	Position wp         = FlightPlan.point(FlightData.FP_nextWaypoint).position();
	Position currentPos = acState.positionLast();	
	double distance2WP  = currentPos.distanceH(wp);
	
	// Get time of current position in nominal plan
	UAS.FlightData.getPlanTime();
		
	// Check for geofence resolutions.
	CheckGeoFences();
	
	// Check for deviation from prescribed flight profile only in the NOOP state.	
	CheckStandoff();
		    
	// Check for conflicts from DAIDALUS against other traffic.
	if(FlightData.traffic.size() > 0){
	    CheckTraffic();
	}	
		
	// If conflicts are detected, initialize the state machine for the resolution function	
	if(conflictList.size() != currentConflicts){
	    currentConflicts = conflictList.size();
	    resolveState = RESOLVE_STATE.COMPUTE;
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Conflict(s) detected");
	    return FSAM_OUTPUT.CONFLICT;	    
	}		 	
	
	if(resolveState == RESOLVE_STATE.NOOP){
	    return FSAM_OUTPUT.NOOP;
	}else{	    
	    return FSAM_OUTPUT.CONFLICT;	    
	}


	
    }

    // Function to compute resolutions for conflicts
    public int Resolve(){

	int status;
	Plan CurrentFP  = FlightData.CurrentFlightPlan;
	float resolutionSpeed   = (float) UAS.pData.getValue("RES_SPEED");
	
	switch(resolveState){

	case COMPUTE:
	    // Call the relevant resolution functions to resolve conflicts
	    // [TODO:] Add conflict resolution table to add prioritization/scheduling to handle multiple conflicts
	    if(TrafficConflict && (FenceKeepInConflict || FenceKeepOutConflict)){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for traffic & geofence conflict");
		UsePlan = false;
		ResolveTrafficFenceConflict();
	    }
	    else if(TrafficConflict){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for traffic conflict");		
		UsePlan = false;
		// ResolveTrafficConflict();
		// Resolution is computed during Traffic checks
	    }
	    else if(FenceKeepInConflict && UsePlan){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for keep in conflict");
		UAS.SetSpeed(resolutionSpeed);
		ResolveKeepInConflict();
		UsePlan = true;
	    }
	    else if(FenceKeepOutConflict && UsePlan){
	       UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for keep out conflict");
	       UAS.SetSpeed(resolutionSpeed);
	       ResolveKeepOutConflict();
	       UsePlan = true;
	    }
	    else if(StandoffConflict){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for stand off conflict");	    
		UsePlan = ResolveStandoffConflict();
	    }

	    // Two kinds of actions to resolve conflicts : 1. Plan based resolution, 2. Maneuver based resolution
	    if(UsePlan){
		resolveState = RESOLVE_STATE.EXECUTE_PLAN;
		executeState = EXECUTE_STATE.START;
		NominalPlan  = false;
		joined       = false;
	    }
	    else{
		resolveState = RESOLVE_STATE.EXECUTE_MANEUVER;
		executeState = EXECUTE_STATE.START;
	    }
	    	    
	    break;

	case EXECUTE_MANEUVER:
	    // Execute maneuver based resolution when appropriate
	    
	    if( executeState != EXECUTE_STATE.COMPLETE ){

		if(TrafficConflict){
		    //ResolveTrafficConflict();
		}
		else if(StandoffConflict){
		    ResolveStandoffConflict();
		}
		
		ExecuteManeuver();
	    }
	    else{
		UAS.apMode = Aircraft.AP_MODE.AUTO;
		UAS.SetMode(3);
		UAS.error.addWarning("[" + UAS.timeLog + "] MODE: AUTO");
		resolveState = RESOLVE_STATE.NOOP;		
	    }
	    
	    break;

	case EXECUTE_PLAN:
	    // Execute plan based resolution when appropriate
	    if( executeState != EXECUTE_STATE.COMPLETE ){
		ExecuteResolutionPlan();
	    }
	    else{
		if(joined){
		    resolveState = RESOLVE_STATE.RESUME;
		}else{
		    resolveState = RESOLVE_STATE.JOIN;
		}
	    }
	    
	    break;

	case JOIN:	    

	    // Once resolution is complete, join the original mission
	    ResolutionPlan.clear();
	    Position pos = FlightData.acState.positionLast();
	    Position nextWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();
	    
	    ResolutionPlan.add(new NavPoint(pos,0));	    	    	    
	    double distance = nextWP.distanceH(pos);
	    double ETA      = distance/resolutionSpeed;
	    ResolutionPlan.add(new NavPoint(nextWP,ETA));

	    resolveState = RESOLVE_STATE.EXECUTE_PLAN;
	    executeState = EXECUTE_STATE.START;
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Joining mission");
	    joined = true;

	    Iterator Itr = conflictList.iterator();
	    while(Itr.hasNext()){
		Conflict cf = (Conflict) Itr.next();
		Itr.remove();
	    }
	    currentConflicts = conflictList.size();
	    
	    break;
	    

	case RESUME:
	    // Continue original mission	  
	    msg_mission_set_current msgMission = new msg_mission_set_current();
	    msgMission.target_system     = 0;
	    msgMission.target_component  = 0;
	    msgMission.seq               = FlightData.FP_nextWaypoint;	    		
	    UAS.error.addWarning("[" + UAS.timeLog + "] CMD: Set next mission item: "+msgMission.seq);
	    UAS.apIntf.Write(msgMission);

	    UAS.apMode = Aircraft.AP_MODE.AUTO;
	    UAS.SetMode(3);
	    UAS.error.addWarning("[" + UAS.timeLog + "] MODE: AUTO");
	    resolveState = RESOLVE_STATE.NOOP;
	    NominalPlan  = true;
	    UsePlan      = true;
	    
	    break;

	case NOOP:

	    break;
	    
	}
	
	return 0;
    }

    public void ExecuteManeuver(){


	switch(executeState){
	    
	case START:
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Starting resolution");	    
	    UAS.apMode = Aircraft.AP_MODE.GUIDED;
	    UAS.SetMode(4);
	    try{
		Thread.sleep(500);
	    }
	    catch(InterruptedException e){
		System.out.println(e);
	    }
	    
	    currentResolutionWP = 0;
	    executeState = EXECUTE_STATE.SEND_COMMAND;
	    break;

	case SEND_COMMAND:

	    	    
	    if(TrafficConflict){
		
		UAS.SetYaw(RefHeading1);		
		UAS.SetVelocity(Vn1,Ve1,Vu1);
		System.out.format("Traffic: Vn,Ve,Vu = %f,%f,%f\n",Vn1,Ve1,Vu1);
	    }else{
		UAS.SetYaw(RefHeading2);
		UAS.SetVelocity(Vn2,Ve2,Vu2);
	    }
	    

	    if(!StandoffConflict && !TrafficConflict){
		executeState = EXECUTE_STATE.COMPLETE;

		Iterator Itr = conflictList.iterator();
		while(Itr.hasNext()){
		    Conflict cf = (Conflict) Itr.next();
		    Itr.remove();
		}
		currentConflicts = conflictList.size();
		System.out.println("Finished maneuver");
	    }
	    
	    break;

	case PAUSE:

	    UAS.SetVelocity(0.0,0.0,0.0);

	    System.out.println("PAUSE:"+(float)((UAS.timeCurrent - pausetime_start))/1E9);
	    if((float) ((UAS.timeCurrent - pausetime_start))/1E9 > 3){
		executeState =EXECUTE_STATE.SEND_COMMAND;
	    }

	    break;
	}

    
    }
    
    public void ExecuteResolutionPlan(){	
	
	switch(executeState){

	case START:
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Starting resolution");	    
	    UAS.apMode = Aircraft.AP_MODE.GUIDED;
	    UAS.SetMode(4);
	    try{
		Thread.sleep(500);
	    }
	    catch(InterruptedException e){
		System.out.println(e);
	    }
	    
	    currentResolutionWP = 0;
	    executeState = EXECUTE_STATE.SEND_COMMAND;
	    break;
	    
	case SEND_COMMAND:

	    NavPoint wp = ResolutionPlan.point(currentResolutionWP);
	    UAS.SetGPSPos(wp.lla().latitude(),wp.lla().longitude(),wp.alt());
	    executeState = EXECUTE_STATE.AWAIT_COMPLETION;
	    break;
	    
	case AWAIT_COMPLETION:
	    
	    Position pos   = ResolutionPlan.point(currentResolutionWP).position();
	    double dist    = pos.distanceH(UAS.FlightData.acState.positionLast());

	    if(dist < 1){

		currentResolutionWP = currentResolutionWP + 1;
		if(currentResolutionWP < ResolutionPlan.size()){
		    executeState = EXECUTE_STATE.SEND_COMMAND;
		}
		else{		    
		    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Resolution complete");
		    executeState = EXECUTE_STATE.COMPLETE;
		    float speed = UAS.GetSpeed();					
		    UAS.SetSpeed(speed);
		    UAS.error.addWarning("[" + UAS.timeLog + "] CMD:SPEED CHANGE TO "+speed+" m/s");
		    
		}
	    }
	    
	    break;
	    
	    
	}//end switch case


    }//end function

    // Returns time corresponding to the current position in the resolution flight plan
    // (assuming constant velocity throughtout flight plan)
    public double GetResolutionTime(){
	Plan FP = ResolutionPlan;

	
	double legDistance, legTime, lastWPDistance, currentTime;
	Position pos = FlightData.acState.positionLast();

	if(currentResolutionWP == 0){
	    return 0;
	}

	currentTime = 0;
	if(currentResolutionWP < ResolutionPlan.size()){	    	    
	    legTime        = FP.getTime(currentResolutionWP) - FP.getTime(currentResolutionWP-1);
	    legDistance    = FP.pathDistance(currentResolutionWP-1);
	    lastWPDistance = FP.point(currentResolutionWP-1).position().distanceH(pos);
	    currentTime    = FP.getTime(currentResolutionWP-1) + legTime/legDistance * lastWPDistance;
	}				
	

	return currentTime;
    }            

    // Check for geofence violations
    public void CheckGeoFences(){
	 FenceKeepInConflict  = false;
	 FenceKeepOutConflict = false;
	 Plan CurrentPlan;
	 double planTime;
	 if(resolveState != RESOLVE_STATE.EXECUTE_MANEUVER){
	     if(NominalPlan){
		 CurrentPlan =  UAS.FlightData.CurrentFlightPlan;
		 planTime    =  UAS.FlightData.planTime;
	     }
	     else{
		 CurrentPlan = ResolutionPlan;
		 planTime    = GetResolutionTime();
	     }
	     for(int i=0;i< FlightData.fenceList.size();i++){
		 GeoFence GF = (GeoFence) FlightData.fenceList.get(i);
		 GF.CheckViolation(FlightData.acState,planTime,CurrentPlan);
		 Conflict cf;	   
		 if(GF.conflict){
		     if(GF.Type == 0){
			 cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_IN,GF,FlightData.acState.positionLast());
			 FenceKeepInConflict = true;
		     }
		     else{
			 cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_OUT,GF,FlightData.acState.positionLast());
			 FenceKeepOutConflict = true;
		     }
		     Conflict.AddConflictToList(conflictList,cf);
		 }
		 else{
		     
		 }
	     }
	 }
	 else{
	     // While executing maneuvers, project current position based on track and ground speed and
	     // check if projection collides with geofences
	     Velocity V = FlightData.acState.velocityLast();
	     Position CurrentPos = FlightData.acState.positionLast();
	     boolean collision = false;
	     Conflict cf;
	     Vect2 vecV = Velocity.trkgs2v(V.trk(),V.gs());
	     for(int j=0;j<FlightData.fenceList.size();++j){
		GeoFence GF    = FlightData.fenceList.get(j);
 		collision      = GF.CollisionDetection(CurrentPos,vecV,0,5);		
		if(collision){
		    if(GF.Type == 0){
			 cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_IN,GF,FlightData.acState.positionLast());
			 FenceKeepInConflict = true;
		     }
		     else{
			 cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_OUT,GF,FlightData.acState.positionLast());
			 FenceKeepOutConflict = true;
		     }
		     Conflict.AddConflictToList(conflictList,cf);
		}
	     }	     
	 }
    }

    // Check standoff distance violation
    public void CheckStandoff(){
	
	double heading   = FlightData.heading;	    
	double heading_fp_pos;
	Plan CurrentPlan = null;
	double planTime = 0;
	if(NominalPlan){
	    CurrentPlan =  UAS.FlightData.CurrentFlightPlan;
	    planTime    =  UAS.FlightData.planTime;
	}
	else{
	    CurrentPlan = ResolutionPlan;
	    planTime    = GetResolutionTime();
	}		
	Position PrevWP     = CurrentPlan.point(FlightData.FP_nextWaypoint - 1).position();
	Position NextWP     = CurrentPlan.point(FlightData.FP_nextWaypoint).position();
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
	crossTrackDeviation = sgn*dist*Math.sin(Math.toRadians(bearing));
	crossTrackOffset    = dist*Math.cos(Math.toRadians(bearing));
	if(Math.abs(crossTrackDeviation) > standoff){
	    StandoffConflict = true;	    
	    Conflict cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.FLIGHTPLAN);
	    Conflict.AddConflictToList(conflictList,cf);
	}else if(Math.abs(crossTrackDeviation) < (standoff)/3){
	    StandoffConflict = false;
	}
    }

    public void CheckTraffic(){
	daaTick = daaTick+1;
	Position so = FlightData.acState.positionLast();
	Velocity vo = FlightData.acState.velocityLast();
	
	daa.reset();
	daa.setOwnshipState("Ownship",so,vo,0.0);

	double dist = Double.MAX_VALUE;
	for(int i=0;i<FlightData.traffic.size();++i){
	    synchronized(FlightData.traffic){
		Position si = FlightData.traffic.get(i).pos.copy();
		Velocity vi = FlightData.traffic.get(i).vel.mkAddTrk(0);
		
		daa.addTrafficState("traffic"+i,si,vi);

		double trafficdist = so.distanceH(si);

		if(trafficdist < dist){
		    dist = trafficdist;
		}
	    }
	    
	}	
		
	for (int ac=1; ac < daa.numberOfAircraft(); ac++) {
	    double tlos = daa.timeToViolation(ac);
	    if (tlos >= 0 && tlos <= daaLookahead ) {
		TrafficConflict = true;
		System.out.printf("Predicted violation with traffic aircraft %s in %.1f [s]\n",
				  daa.getAircraftState(ac).getId(),tlos);

		Conflict cf = new Conflict(PRIORITY_LEVEL.HIGH,CONFLICT_TYPE.TRAFFIC);
		Conflict.AddConflictToList(conflictList,cf);

		KMB = daa.getKinematicMultiBands();
		System.out.println(daa.toString());
		System.out.println(KMB.outputString());
		daaTick = 0;

		ResolveTrafficConflict();
	    }

	    /*
	    if(TrafficConflict){				
		List<TrafficState> TS = new ArrayList<TrafficState>();
		TS.add(daa.getAircraftState(1));
		System.out.println("size: "+TS.size()+","+daa.getOwnshipState().formattedTraffic(TS,0));
	    }
	    */
	}

	// Predict if traffic violations exist on the return path
	if(daaTick > 60){
	    if(TrafficConflict){
		ResolveStandoffConflict(); // Call resolve standoff conflict here to get next heading to turn back to
		daa.reset();

		double Vgs  = UAS.GetSpeed();
		Velocity vo2 = Velocity.makeTrkGsVs(RefHeading2,"degree",Vgs,"m/s",0,"m/s");
		System.out.println(vo2.toString());
		daa.setOwnshipState("Ownship",so,vo2,0.0);
	    	    
		for(int i=0;i<FlightData.traffic.size();++i){
		    synchronized(FlightData.traffic){
			Position si = FlightData.traffic.get(i).pos.copy();
			Velocity vi = FlightData.traffic.get(i).vel.mkAddTrk(0);		
			daa.addTrafficState("traffic"+i,si,vi);		
		    }
		
		}

		double CurrentHeading = FlightData.heading;
		
		double psi   = RefHeading2 - CurrentHeading;
		double psi_c = 360 - Math.abs(psi);
		boolean leftTurn = false, rightTurn = false;
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

		TrafficConflict = false;
		for (int ac=1; ac < daa.numberOfAircraft(); ac++) {
		    double tlos = daa.timeToViolation(ac);
		    System.out.println("TLOS:"+tlos);
		    if(tlos>=0 && tlos <= 50){
			TrafficConflict = true;
		    }
		    KinematicMultiBands KMB2 = daa.getKinematicMultiBands();
		    		    
		    for(int i=0;i<KMB2.trackLength();i++){
			Interval iv = KMB2.track(i,"deg"); //i-th band region
			double lower_trk = iv.low -5; //[deg]
			double upper_trk = iv.up + 5; //[deg]
			BandsRegion regionType = KMB2.trackRegion(i);		
			//System.out.println("low trk:"+lower_trk);
			//System.out.println("upper trk:"+upper_trk);
			if (regionType.toString() != "NONE" ){
			    System.out.println("L:"+lower_trk);
			    System.out.println("R:"+upper_trk);
			    System.out.println("CurrentHeading:"+CurrentHeading);
			    System.out.println("Ref heading:"+RefHeading2);
			    							    
			    if(psi > 0){
				if(rightTurn){
				    if( (CurrentHeading < lower_trk) && (RefHeading2 > upper_trk ) ){
					System.out.println("A");
					TrafficConflict  = true;
				    }else{
					System.out.println("B");
					TrafficConflict = false;
				    }
				}
				else{
				    if( (CurrentHeading > upper_trk) && (RefHeading2 < lower_trk ) ){
					System.out.println("C");
					TrafficConflict = true;
				    }
				    else{
					System.out.println("D");
					TrafficConflict = false;
				    }
				}
			    }
			    else{
				if(rightTurn){
				    if( (CurrentHeading > upper_trk) && (RefHeading2 < lower_trk ) ){
					System.out.println("E");
					TrafficConflict = true;
				    }
				    else{
					System.out.println("F");
					TrafficConflict = false;
				    }
				}
				else{
				    if( (CurrentHeading < lower_trk) && (RefHeading2 > upper_trk ) ){
					System.out.println("G");
					TrafficConflict  = true;
				    }else{
					System.out.println("H");
					TrafficConflict = false;
				    }
				}
			    }

			    if( (CurrentHeading >= lower_trk) && (CurrentHeading <= upper_trk)){
				TrafficConflict = true;
				System.out.println("I1");
			    }

			    if( (RefHeading2 >= lower_trk) && (RefHeading2 <= upper_trk)){
				TrafficConflict = true;
				System.out.println("I2");
			    }
			}
			
		    }
		    		    		    		   
		    if(!TrafficConflict){
			System.out.println("Safe to turnback");
			// Remove traffic conflict from list
			Conflict.RemoveTrafficConflict(conflictList);
		    }		    		    
		}			    
	    }
	}		
    }
    
    // Compute resolution for keep in conflict
    public void ResolveKeepInConflict(){

	Plan CurrentFP = FlightData.CurrentFlightPlan;
	GeoFence GF = null;

	for(int i=0;i<conflictList.size();i++){
	    Conflict CF = conflictList.get(i);
	    if(CF.conflictType == CONFLICT_TYPE.KEEP_IN){
		GF = CF.fence;
		break;
	    }
	}

	NavPoint wp = null;
	if(GF.violation){
	    wp = new NavPoint(GF.RecoveryPoint,0);
	}
	else{
	    wp = new NavPoint(GF.SafetyPoint,0);
	}
	
	
	ResolutionPlan.clear();
	currentResolutionWP = 0;
	
	ResolutionPlan.add(wp);
	
	NavPoint nextWP = CurrentFP.point(FlightData.FP_nextWaypoint);
	if(!GF.CheckWaypointFeasibility(wp.position(),nextWP.position())){
	    GotoNextWP = true;
	    FlightData.FP_nextWaypoint++;	    
	    
	}else{
	    GotoNextWP = false;
	}
	

    }

    // Compute resolution for keep out conflicts
    public void ResolveKeepOutConflict(){

	Plan CurrentFP;
	double currentTime;
		
	if(NominalPlan){
	    CurrentFP = FlightData.CurrentFlightPlan;
	    currentTime = UAS.FlightData.planTime;
	}
	else{
	    CurrentFP   = ResolutionPlan;
	    currentTime = GetResolutionTime();
	}

	double minTime = Double.MAX_VALUE;
	double maxTime = 0.0;

	ArrayList<PolyPath> LPP = new ArrayList<PolyPath>();
	ArrayList<PolyPath> CPP = new ArrayList<PolyPath>();
	WeatherUtil WU = new WeatherUtil();

	Position RecoveryPoint = null;
	boolean violation = false;
	
	// Get conflict start and end time
	for(int i=0;i<conflictList.size();i++){

	    if(conflictList.get(i).conflictType != CONFLICT_TYPE.KEEP_OUT){
		continue;
	    }
	    
	    GeoFence GF = (GeoFence) conflictList.get(i).fence;
	   	    
	    if(GF.entryTime <= minTime){
		minTime = GF.entryTime;
	    }
	    
	    if(GF.exitTime >= maxTime){
		maxTime = GF.exitTime;
	    }
	    
	    LPP.add(GF.geoPolyPath);

	    if(GF.violation){
		RecoveryPoint = GF.RecoveryPoint;
		violation = true;
	    }
	    
	}

	for(int i=0;i<FlightData.fenceList.size();i++){
	    GeoFence GF = FlightData.fenceList.get(i);
	    if(GF.Type == 0){
		CPP.add(GF.geoPolyPath);
	    }
	}
	
	minTime = minTime - lookahead;
	maxTime = maxTime + lookahead;
	
	if(minTime < currentTime){
	    minTime = currentTime+0.1;
	}

	if(maxTime > CurrentFP.getLastTime()){
	    maxTime = CurrentFP.getLastTime() - 0.1;
	}

	 
	if(NominalPlan){
	    FlightData.FP_nextWaypoint = CurrentFP.getSegment(maxTime)+1;
	}
	
	// Get flight plan between start time and end time (with a 3 second buffer on both sides)
	Plan ConflictFP = PlanUtil.cutDown(CurrentFP,minTime,maxTime);
	
	// Reroute flight plan
	UAS.SetMode(4); // Set mode to guided for quadrotor to hover before replanning

	// Create a bounding box based on containment geofence
	BoundingRectangle BR = new BoundingRectangle();

	SimplePoly CF = FlightData.fenceList.get(0).geoPolyLLA;

	for(int i=0;i<CF.size();i++){
	    BR.add(CF.getVertex(i));
	}	

	// Get start and end positions based on conflicted parts of the original flight plan
	
	NavPoint start = ConflictFP.point(0);
	Position end   = ConflictFP.getLastPoint().position();

	if(violation){
	    start = new NavPoint(RecoveryPoint,0);
	}

	
	// Instantiate a grid to search over
	DensityGrid dg = new DensityGrid(BR,start,end,(int)buffer,gridsize,true);
	dg.snapToStart();
	
	// Set weights for the search space and obstacles
	dg.setWeights(5.0);

	for(int i=1;i<FlightData.fenceList.size();i++){
	    GeoFence GF = FlightData.fenceList.get(i);
	    double hthreshold = UAS.pData.getValue("HTHRESHOLD");
	    double vthreshold = UAS.pData.getValue("VTHRESHOLD");
	    SimplePoly expfence = GF.pu.bufferedConvexHull(GF.geoPolyLLA,hthreshold,vthreshold);
	    dg.setWeightsInside(expfence,100.0);
	}

	// Perform A* seartch
	List<Pair <Integer,Integer>> GridPath = dg.optimalPath();

	List<Position> PlanPosition = new ArrayList<Position>();
	double currHeading = 0.0;
	double nextHeading = 0.0;

	// Reduce number of waypoints based on heading
	PlanPosition.add(start.position());
	double startAlt = start.position().alt();
	PlanPosition.add(dg.getPosition(GridPath.get(0)).mkAlt(startAlt));
	currHeading = dg.getPosition(GridPath.get(0)).track(dg.getPosition(GridPath.get(1)));

	for(int i=1;i<GridPath.size();i++){
	    Position pos1 = dg.getPosition(GridPath.get(i));
	    	    	    
	    if(i==GridPath.size()-1){
		PlanPosition.add(pos1.mkAlt(startAlt));
		break;
	    }
	    else{
		Position pos2 = dg.getPosition(GridPath.get(i+1));

		nextHeading = pos1.track(pos2);
		
		if(Math.abs(nextHeading - currHeading) > 0.01){		    		
		    PlanPosition.add(pos1.mkAlt(startAlt));
		    currHeading = nextHeading;
		}
	    }
	    
	}
	PlanPosition.add(end);

	// Create new flight plan based on waypoints
	ResolutionPlan.clear();
	currentResolutionWP = 0;
	double ETA   = 0.0;
	ResolutionPlan.add(new NavPoint(PlanPosition.get(0),ETA));
	for(int i=1;i<PlanPosition.size();i++){
	    Position pos = PlanPosition.get(i);
	    double distance = pos.distanceH(PlanPosition.get(i-1));
	    ETA      = ETA + distance/resolutionSpeed;

	    ResolutionPlan.add(new NavPoint(pos,ETA));
	}

	
	
    }

    // Compute resolution for stand off distance violation
    public boolean ResolveStandoffConflict(){


	double XtrkDevGain       = UAS.pData.getValue("XTRK_GAIN") ;

	if(XtrkDevGain < 0){
	    XtrkDevGain = - XtrkDevGain;
	}
	
	if(StandoffConflict){

	    if(crossTrackDeviation <= 2*standoff){
		double Vs = XtrkDevGain*crossTrackDeviation;
		double V  = UAS.GetSpeed();

		double sgn = 0;

		if(Vs >= 0){
		    sgn = 1;
		}
		else{
		    sgn = -1;
		}
	    
		if(Math.pow(Math.abs(Vs),2) >= Math.pow(V,2)){
		    Vs = V*sgn;
		}

		double Vf       = Math.sqrt( Math.pow(V,2) - Math.pow(Vs,2) );

		Position PrevWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint - 1).position();
		Position NextWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();

		double Trk = PrevWP.track(NextWP);

		Vn2 = Vf*Math.cos(Trk) - Vs*Math.sin(Trk);
		Ve2 = Vf*Math.sin(Trk) + Vs*Math.cos(Trk);
		Vu2 = 0;
		RefHeading2 = Math.toDegrees(Math.atan2(Ve2,Vn2));

		if(RefHeading2 < 0){
		    RefHeading2 = 360 + RefHeading2;
		}

		//System.out.println("Ref heading:"+RefHeading2);	    
		return false; // false indicated that we don't have to use a resolution plan
		
	    }
	    else{
		
		Plan CurrentPlan      = FlightData.CurrentFlightPlan;
		Position PrevWP       = CurrentPlan.point(FlightData.FP_nextWaypoint - 1).position();
		Position NextWP       = CurrentPlan.point(FlightData.FP_nextWaypoint).position();		
		Position CurrentPos   = FlightData.acState.positionLast();
		double headingNextWP  = PrevWP.track(NextWP);
		
		double dn             = crossTrackOffset*Math.cos(headingNextWP);
		double de             = crossTrackOffset*Math.sin(headingNextWP);
		    
		Position cp           = PrevWP.linearEst(dn,de);
									
		RefHeading2 = Math.toDegrees(CurrentPos.track(cp));

		if(RefHeading2 < 0){
		    RefHeading2 = 360 + RefHeading2;
		}

		System.out.println("Closest point:"+cp.toString());
		System.out.println("Ref heading in standoff resolution plan:"+RefHeading2);
		ResolutionPlan.clear();
		ResolutionPlan.add(new NavPoint(CurrentPos,0));
		double distance = CurrentPos.distanceH(cp);
		double ETA      = distance/resolutionSpeed;
		ResolutionPlan.add(new NavPoint(cp,ETA));

		return true; // true indicates that we should use a resolution plan
	    }
	}
	else{
	    Vn2 = 0;
	    Ve2 = 0;
	    Vu2 = 0;

	    return false;
	}

    }

    public void ResolveTrafficConflict(){		

	// Get resolution headings from DAA
	double heading_right = KMB.trackResolution(true);
	double heading_left  = KMB.trackResolution(false);
	double res_heading   = Double.NaN;
		
	System.out.println("resolution heading L:"+heading_left*180/3.142);
	System.out.println("resolution heading R:"+heading_right*180/3.142);

	heading_left  = heading_left*180/Math.PI;
	heading_right = heading_right*180/Math.PI;
	double d1,d2,h1,h2, diff = Double.MAX_VALUE;	

	// Determine which resolution to use (left/right) based on proximity to next waypoint heading.
	Position CurrentPos = FlightData.acState.positionLast();
	Position NextWP     = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();

	// Heading to next waypoint from current position
	double planTrack    = Math.toDegrees(CurrentPos.track(NextWP));

	// Resolution speed
	double V  = UAS.GetSpeed();

	// Use DAA ground speed when available
	double Vres1 = KMB.groundSpeedResolution(true);
	double Vres2 = KMB.groundSpeedResolution(false);
	//System.out.println("Vres:"+Vres);
	if(!Double.isNaN(Vres1) && !Double.isInfinite(Vres1)){
	    V = Math.ceil(Vres1);		
	}
	else if(!Double.isNaN(Vres2) && !Double.isInfinite(Vres2)){
	    V = Math.ceil(Vres2);		
	}
	
	// Get vertical speed from DAA resolution - use 0 if unavailable
	double Dres = KMB.verticalSpeedResolution(true);
	//System.out.println("Dres:"+Dres);
	if(!Double.isNaN(Dres) && !Double.isInfinite(Dres)){
	    //Vu1 = -Dres;		// APM -ve means climb
	}
	else{
	    Vu1 = 0;
	}	

	// Check which resolution to use by checking collision against geofence
	boolean collisionLeft  = false;
	boolean collisionRight = false;	
	if(!Double.isNaN(heading_right) && !Double.isNaN(heading_left)){
	    Vect2 Vproj1 = Velocity.trkgs2v(Math.toRadians(heading_left),V);
	    Vect2 Vproj2 = Velocity.trkgs2v(Math.toRadians(heading_right),V);	    
	    for(int j=0;j<FlightData.fenceList.size();++j){
		GeoFence GF    = FlightData.fenceList.get(j);
 		collisionLeft  =  collisionLeft  || GF.CollisionDetection(CurrentPos,Vproj1,0,60);
		collisionRight =  collisionRight || GF.CollisionDetection(CurrentPos,Vproj2,0,60);
	    }
	}

	if(collisionLeft && !collisionRight){
	    res_heading = heading_right;
	    System.out.println("Collision Left");
	}
	else if(collisionRight && !collisionLeft){
	    res_heading = heading_left;
	    System.out.println("Collision Right");
	}
	else{
	    //System.out.println("Collision Left:"+collisionLeft);
	    //System.out.println("Collision Right:"+collisionRight);
	    d1 = Math.abs(planTrack - heading_left);
	    d2 = Math.abs(planTrack - heading_right);
	
	
	    // Pick the resolution angle that is closest to the next waypoint heading
	    if(d1 <= d2){
		res_heading = heading_left-5;
	    }
	    else{
		res_heading = heading_right+5;
	    }
	}

	// If the next waypoing heading is within the <NONE> band, ignore resolution headings	
	for(int i=0;i<KMB.trackLength();i++){
	    Interval iv = KMB.track(i,"deg"); //i-th band region
	    double lower_trk = iv.low; //[deg]
	    double upper_trk = iv.up; //[deg]
	    BandsRegion regionType = KMB.trackRegion(i);		
	    System.out.println("R:low trk:"+lower_trk);
	    System.out.println("R:upper trk:"+upper_trk);
	    if (regionType.toString() == "NONE" ){		    
		if (planTrack >= lower_trk && planTrack <= upper_trk){
		    //res_heading = planTrack;
		    //System.out.println("res_heading set to:"+res_heading);
		    break;
		}
	    }	    
	}			
	//System.out.println("Ref heading:"+res_heading);
	// If resolution heading angles available, compute Vn,Ve based on resolution heading
	if(!Double.isNaN(res_heading)){	    
	    //System.out.println("Resolution speed:"+V);
	    Vn1 = V*Math.cos(Math.toRadians(res_heading));
	    Ve1 = V*Math.sin(Math.toRadians(res_heading));
	    RefHeading1 = res_heading;
	    System.out.println("resolution heading:"+res_heading);
	    //System.out.format("Vn1 = %f, Ve1 = %f, Vu1 = %f\n",Vn1,Ve1,Vu1);
	}
	else{
	    // If resolution heading unavailable, follow last know resolution heading
	    res_heading = RefHeading1;	    
	    Vn1 = V*Math.cos(Math.toRadians(res_heading));
	    Ve1 = V*Math.sin(Math.toRadians(res_heading));
	    System.out.println("resolution heading:"+res_heading);
	    //System.out.format("Vn1 = %f, Ve1 = %f, Vu1 = %f\n",Vn,Ve,Vu);
	}
    }
    

    public void ResolveTrafficFenceConflict(){

	Position currentPos = FlightData.acState.positionLast();
	double alt = currentPos.alt();
	double alt_upper = KMB.altitudeResolution(true,1);
	double alt_lower = KMB.altitudeResolution(false,1);
	System.out.println("Lower:"+alt_lower+" Upper:"+alt_upper);
	Vn1 = 0;
	Ve1 = 0;
	Vu1 = 0;
	if(!Double.isNaN(alt_upper)){
	    if(alt <= alt_upper){
		Vn1 = 0;
		Ve1 = 0;
		Vu1 = -3;
	    }	    
	}
	else if(!Double.isNaN(alt_lower)){
	    if(alt > alt_lower){
		Vn1 = 0;
		Ve1 = 0;
		Vu1 = 1;
	    }
	}
    }
        
}
