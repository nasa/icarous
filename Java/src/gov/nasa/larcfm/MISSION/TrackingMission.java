package gov.nasa.larcfm.MISSION;

import gov.nasa.larcfm.ICAROUS.*;
import gov.nasa.larcfm.ICAROUS.FlightManagementSystem.ARDUPILOT_MODES;
import gov.nasa.larcfm.Util.Position;

import java.util.*;

public class TrackingMission implements Mission {
	
	public enum mission_state_t {START, EXECUTE, STOP};
	
	public mission_state_t state;
	
	public TrackingMission(){
		state = mission_state_t.START;
	}
	
	public int Execute(FlightManagementSystem FMS){
		
		Position target = null;
		synchronized(FMS.FlightData){
			target = FMS.FlightData.traffic.get(0).pos;
		}
		
		switch(state){
		
		case START:
			FMS.SetMode(ARDUPILOT_MODES.GUIDED);
			state = mission_state_t.EXECUTE;
			FMS.devAllowed = true;
			break;
			
		case EXECUTE:
			FMS.TRACKING(target);
			break;
			
		case STOP:
			break;
		}
		
		return 0;
	}
	
	public int GetMissionState(){
		return 0;
	}

	public boolean isMissionComplete(){
		return true;
	}
}
