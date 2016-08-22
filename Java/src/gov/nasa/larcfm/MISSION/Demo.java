/**
 * Demo
 * 
 * This class implements the Mission interface that will be used by ICAROUS.
 * 
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.MISSION;

import gov.nasa.larcfm.ICAROUS.*;
import java.util.*;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;

public class Demo implements Mission,ErrorReporter{

    public enum MISSION_STATE{
	IDLE, TURN1, TURN2, EXAMINE, CONTINUE;
    }
    
    double Event1;
    MISSION_STATE stateMission;

    public ErrorLog error;
    String timeLog;

    boolean missionComplete;
    int status = 0;
    
    public Demo(){
	stateMission = MISSION_STATE.IDLE;
	missionComplete = false;
	error = new ErrorLog("Mission ");
	
    }

    public int Execute(Aircraft UAS){

	AircraftData FlightData = UAS.FlightData;
	timeLog = UAS.timeLog;
	double CurrentTime =  System.nanoTime()/1E9;
	double ElapsedTime = 0;
	double Targetheading;
	double StartTime = 0; 
	
	if(status == 0){
	    UAS.SetMode(4);
	    UAS.SetVelocity(1.0,0.0,0.0);
	    status = 1;
	    StartTime = CurrentTime;
	}

	
       
	ElapsedTime = CurrentTime - StartTime;

	if(ElapsedTime > 2){
	    StartTime = CurrentTime;
	}
	
	
	if(FlightData.missionObj.size() > 0 && stateMission == MISSION_STATE.IDLE){
	    stateMission = MISSION_STATE.TURN1;
	}

	switch(stateMission){

	case TURN1:
	    error.addWarning("[" + timeLog + "] MSG: TURN GOPRO TOWARDS OBJECT");
	    UAS.SetMode(4);
	    UAS.apMode = Aircraft.AP_MODE.GUIDED;
	    // Turn so that the go pro can get a clear shot of the balloon
	    
	    Targetheading = FlightData.yaw - 90;

	    if(Targetheading < 0)
		Targetheading = 360 + Targetheading;
	    
	    UAS.SetYaw(Targetheading);
	    stateMission = MISSION_STATE.TURN2;
	    Event1 = CurrentTime;
	    break;

	case TURN2:

	    ElapsedTime = CurrentTime - Event1;

	    if(ElapsedTime > 10){
		error.addWarning("[" + timeLog + "] MSG: TURN FIREFLY TOWARDS OBJECT");
		// Turn so that the go pro can get a clear shot of the balloon
		Targetheading = FlightData.yaw + 90;
		UAS.SetYaw(Targetheading);
		stateMission = MISSION_STATE.EXAMINE;
		Event1 = CurrentTime;
	    }
	    
	    
	    break;

	case EXAMINE:

	    ElapsedTime = CurrentTime - Event1;
	    
	    if(ElapsedTime > 10){
		error.addWarning("[" + timeLog + "] MSG: CONTINUE MISSION");
		stateMission = MISSION_STATE.CONTINUE;
	    }
	    
	    break;

        case IDLE:

	    if(missionComplete){
		return 1;
	    }
	    
	    break;
	    
	    
	case CONTINUE:

	    Iterator Itr = FlightData.missionObj.iterator();
	    while(Itr.hasNext()){
		GenericObject obj = (GenericObject) Itr.next();
		Itr.remove();
	    }
	    
	    UAS.SetMode(3);
	    UAS.apMode = Aircraft.AP_MODE.AUTO;
	    stateMission = MISSION_STATE.IDLE;
	    missionComplete = true;
	    error.addWarning("[" + timeLog + "] MSG: Object examination completed");
	    break;
	}
	
	return 0;
    }

    public int GetMissionState(){
	return 0;
    }

    public boolean isMissionComplete(){
	return missionComplete;
    }

    public boolean hasError() {
	return error.hasError();
    }
    
    public boolean hasMessage() {
	return error.hasMessage();
    }
    
    public String getMessage() {
	return error.getMessage();
    }
    
    public String getMessageNoClear() {
	return error.getMessageNoClear();
    }

}
