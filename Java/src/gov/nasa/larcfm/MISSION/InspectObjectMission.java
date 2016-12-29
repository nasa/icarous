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
package gov.nasa.larcfm.MISSION;

import gov.nasa.larcfm.ICAROUS.*;
import gov.nasa.larcfm.ICAROUS.FlightManagementSystem.ARDUPILOT_MODES;

import java.util.*;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;

public class InspectObjectMission implements Mission,ErrorReporter{

	public enum MISSION_STATE{
		IDLE, TURN1, TURN2, EXAMINE, CONTINUE;
	}

	double Event1;
	MISSION_STATE stateMission;

	public ErrorLog error;
	String timeLog;

	boolean missionComplete;
	int status = 0;
	int status2 = 0;
	public double StartTime = 0;

	public InspectObjectMission(){
		stateMission = MISSION_STATE.IDLE;
		missionComplete = false;
		error = new ErrorLog("Mission ");


	}

	public int Execute(FlightManagementSystem UAS){

		if(status2 == 0){
			StartTime = System.nanoTime()/1E9;
			status2 = 1;
		}

		AircraftData FlightData = UAS.FlightData;
		
		double CurrentTime =  System.nanoTime()/1E9;
		double ElapsedTime = 0;
		double Targetheading;


		ElapsedTime = CurrentTime - StartTime;
		
		synchronized(FlightData.missionObj){
			if(FlightData.missionObj.size() > 0 && stateMission == MISSION_STATE.IDLE){

				if(FlightData.missionObj.get(0).id == 0){
					FlightData.missionObj.remove(0);
				}
				else{
					stateMission = MISSION_STATE.TURN1;
				}
			}
		}

		switch(stateMission){

		case TURN1:
			error.addWarning("[" + timeLog + "] MSG: TURN GOPRO TOWARDS OBJECT");
			UAS.SetMode(ARDUPILOT_MODES.GUIDED);
			
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

			synchronized(FlightData.missionObj){
				Iterator<GenericObject> Itr = FlightData.missionObj.iterator();
				while(Itr.hasNext()){
					GenericObject obj = (GenericObject) Itr.next();
					Itr.remove();
				}
			}

			UAS.SetMode(3);
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
