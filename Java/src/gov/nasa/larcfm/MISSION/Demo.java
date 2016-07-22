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

public class Demo implements Mission{

    public enum MISSION_STATE{
	NOOP,TURN1, TURN2, EXAMINE, CONTINUE;
    }
    
    double Event1;
    MISSION_STATE stateMission;
    
    public Demo(){
	stateMission = MISSION_STATE.NOOP;
    }

    public int Execute(Aircraft UAS){

	AircraftData FlightData = UAS.FlightData;

	double CurrentTime =  System.nanoTime()/1E9;
	double ElapsedTime;
	double Targetheading;
	
	if(FlightData.missionObj.size() > 0 && stateMission == MISSION_STATE.NOOP){
	    stateMission = MISSION_STATE.TURN1;
	}

	switch(stateMission){

	case TURN1:
	    System.out.println("TURN 1");
	    UAS.SetMode(4);
	    UAS.apMode = Aircraft.AP_MODE.GUIDED;
	    // Turn so that the go pro can get a clear shot of the balloon
	    
	    Targetheading = FlightData.yaw - 90;

	    if(Targetheading < 0)
		Targetheading = 360 + Targetheading;
	    
	    
	    System.out.println("Target heading:"+Targetheading);
	    UAS.SetYaw(Targetheading);
	    stateMission = MISSION_STATE.TURN2;
	    Event1 = CurrentTime;
	    break;

	case TURN2:

	    ElapsedTime = CurrentTime - Event1;

	    if(ElapsedTime > 10){
		System.out.println("turning back");
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
		stateMission = MISSION_STATE.CONTINUE;
	    }
	    
	    break;

        case NOOP:

	    break;
	    
	    
	case CONTINUE:

	    Iterator Itr = FlightData.missionObj.iterator();
	    while(Itr.hasNext()){
		GenericObject obj = (GenericObject) Itr.next();
		Itr.remove();
	    }
	    
	    UAS.SetMode(3);
	    UAS.apMode = Aircraft.AP_MODE.AUTO;
	    stateMission = MISSION_STATE.NOOP;
	    break;
	}
	
	return 0;
    }

}
