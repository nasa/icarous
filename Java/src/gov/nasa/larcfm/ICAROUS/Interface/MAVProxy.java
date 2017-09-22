package gov.nasa.larcfm.ICAROUS.Interface;

import java.util.ArrayList;

import com.MAVLink.Messages.MAVLinkMessage;
import com.MAVLink.common.msg_mission_count;
import com.MAVLink.common.msg_mission_item;
import com.MAVLink.enums.MAV_CMD;

import gov.nasa.larcfm.ICAROUS.Icarous;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Waypoint;

public class MAVProxy extends MAVLinkInterface {
	
	public MAVProxy(Icarous IC){
		super(IC);
	}
	
	public void HandleMissionCount(MAVLinkMessage msg){
		msg_mission_count msgMissionCount = (msg_mission_count) msg;	    
		pipe.SendMavlinkData(msg);		
		int numWaypoints = msgMissionCount.count;
		waypointType = new ArrayList<Integer>(numWaypoints);
		IC.InputClearFlightPlan();
	}
	
	public void HandleMissionItem(MAVLinkMessage msg){
		msg_mission_item msgMissionItem = (msg_mission_item) msg;
		pipe.SendMavlinkData(msg);
		
		waypointType.add(msgMissionItem.seq,msgMissionItem.command);		
		if(msgMissionItem.command == MAV_CMD.MAV_CMD_NAV_WAYPOINT ||
				msgMissionItem.command == MAV_CMD.MAV_CMD_NAV_SPLINE_WAYPOINT){
			msg_Waypoint wpdata = new msg_Waypoint(waypointType.size(), msgMissionItem.seq, 
													msgMissionItem.x, msgMissionItem.y, msgMissionItem.z, speed);
			IC.InputFlightPlanData(wpdata);
		}else if(msgMissionItem.command == MAV_CMD.MAV_CMD_DO_CHANGE_SPEED){
			speed = msgMissionItem.param2;
		}		
	}
}
