package gov.nasa.larcfm.ICAROUS.Interface;

import java.util.ArrayList;

import com.MAVLink.MAVLinkPacket;
import com.MAVLink.ardupilotmega.msg_fence_fetch_point;
import com.MAVLink.ardupilotmega.msg_fence_point;
import com.MAVLink.common.msg_command_ack;
import com.MAVLink.common.msg_command_long;
import com.MAVLink.common.msg_mission_count;
import com.MAVLink.common.msg_mission_item;
import com.MAVLink.common.msg_mission_request_list;
import com.MAVLink.enums.MAV_CMD;

import gov.nasa.larcfm.ICAROUS.Icarous;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Geofence;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Object;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Waypoint;

public class MAVProxy extends MAVLinkInterface {
	
	msg_Geofence gfdata;
	
	public MAVProxy(Icarous IC){
		super(IC);	
		gfdata = new msg_Geofence();
	}
	
	@Override
	public void HandleMissionCount(MAVLinkPacket msg){
		System.out.println("Received mission count");
		msg_mission_count msgMissionCount = (msg_mission_count) msg.unpack();	    
		pipe.SendMavlinkData(msg);		
		int numWaypoints = msgMissionCount.count;
		waypointType = new ArrayList<Integer>(numWaypoints);
		IC.InputClearFlightPlan();
	}
	
	@Override
	public void HandleMissionItem(MAVLinkPacket msg){
		msg_mission_item msgMissionItem = (msg_mission_item) msg.unpack();
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
	
	@Override
	public void HandleMissionRequestList(MAVLinkPacket msg){
		msg_mission_request_list msgMissionRequestList = (msg_mission_request_list)msg.unpack();
		pipe.SendMavlinkData(msg);
	}
	
	
	public void HandleFenceEnable(msg_command_long msg){		
		gfdata.index = (int)msg.param2;
		gfdata.type  = (int)msg.param3;
		gfdata.totalVertices = (int)msg.param4;
		gfdata.floor = msg.param5;
		gfdata.ceiling = msg.param6;
		
		msg_fence_fetch_point fenceFetchPoint = new msg_fence_fetch_point();
		fenceFetchPoint.target_system = 1;
		fenceFetchPoint.target_component = 0;
		fenceFetchPoint.idx = 0;	
		
		SendMavlinkData(fenceFetchPoint);
	}
	
	@Override
	public void HandleFencePoint(MAVLinkPacket msg){
		msg_fence_point msgFencePoint = (msg_fence_point) msg.unpack();
		int id = msgFencePoint.idx;
		int total = msgFencePoint.count;		
		gfdata.vertexIndex = id;
		gfdata.latitude = msgFencePoint.lat;
		gfdata.longitude = msgFencePoint.lng;
		
		msg_Geofence gfdata_cpy = new msg_Geofence();
		gfdata_cpy.ceiling = gfdata.ceiling;
		gfdata_cpy.floor   = gfdata.floor;
		gfdata_cpy.index   = gfdata.index;
		gfdata_cpy.latitude = gfdata.latitude;
		gfdata_cpy.longitude = gfdata.longitude;
		gfdata_cpy.totalVertices = gfdata.totalVertices;
		gfdata_cpy.type = gfdata.type;
		gfdata_cpy.vertexIndex = gfdata.vertexIndex;
		
		IC.InputGeofenceData(gfdata_cpy);
		
		if(id < total - 1){
			msg_fence_fetch_point fenceFetchPoint = new msg_fence_fetch_point();
			fenceFetchPoint.target_system = 1;
			fenceFetchPoint.target_component = 0;
			fenceFetchPoint.idx = (short) (id + 1);	
			
			SendMavlinkData(fenceFetchPoint);
		}else{
			msg_command_ack msgCommandAck = new msg_command_ack();
			msgCommandAck.result = 1;
			SendMavlinkData(msgCommandAck);
		}			
	}
	
	
	public void HandleReset(msg_command_long msg){
		IC.InputResetIcarous();
	}
	
	
	public void HandleTraffic(msg_command_long msg){
		msg_Object traffic = new msg_Object();
		traffic.type = 1;
		traffic.index = (int) msg.param1;
		traffic.latitude = msg.param5;
		traffic.longitude = msg.param6;
		traffic.altiude = msg.param6;
		traffic.vx = msg.param2;
		traffic.vy = msg.param3;
		traffic.vz = msg.param4;
	}
	
	
	public void HandleMissionStart(msg_command_long msg){
		IC.InputStartMission((int) msg.param1);
	}
	
	@Override
	public void HandleMissionRequest(MAVLinkPacket msg){
		pipe.SendMavlinkData(msg);
	}
	
	@Override
	public void HandleParamRequestList(MAVLinkPacket msg){
		pipe.SendMavlinkData(msg);
	}
	
	@Override
	public void HandleParamRequestRead(MAVLinkPacket msg){
		pipe.SendMavlinkData(msg);
	}
	
	@Override
	public void HandleParamValue(MAVLinkPacket msg){
		pipe.SendMavlinkData(msg);
	}
	
	@Override
	public void HandleParamSet(MAVLinkPacket msg){
		pipe.SendMavlinkData(msg);
	}
	
	@Override
	public void HandleCommandLong(MAVLinkPacket msg){
		msg_command_long msgCommandLong = (msg_command_long)msg.unpack();
		
		if(msgCommandLong.command == MAV_CMD.MAV_CMD_MISSION_START){
			HandleMissionStart(msgCommandLong);
		}else if(msgCommandLong.command == MAV_CMD.MAV_CMD_DO_FENCE_ENABLE){
			HandleFenceEnable(msgCommandLong);
		}else if(msgCommandLong.command == MAV_CMD.MAV_CMD_SPATIAL_USER_1){
			HandleTraffic(msgCommandLong);
		}else if(msgCommandLong.command == MAV_CMD.MAV_CMD_USER_1){
			HandleReset(msgCommandLong);
		}else{
			
		}
	}
}
