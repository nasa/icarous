package gov.nasa.larcfm.ICAROUS.Interface;

import com.MAVLink.MAVLinkPacket;
import com.MAVLink.Messages.MAVLinkMessage;
import com.MAVLink.common.msg_command_ack;
import com.MAVLink.common.msg_global_position_int;
import com.MAVLink.common.msg_heartbeat;
import com.MAVLink.common.msg_mission_count;
import com.MAVLink.common.msg_mission_item;
import com.MAVLink.common.msg_mission_item_reached;
import com.MAVLink.common.msg_mission_request;
import com.MAVLink.common.msg_mission_request_list;
import com.MAVLink.common.msg_param_request_list;
import com.MAVLink.common.msg_param_request_read;
import com.MAVLink.common.msg_param_value;
import com.MAVLink.enums.MAV_CMD;

import gov.nasa.larcfm.ICAROUS.Icarous;
import gov.nasa.larcfm.ICAROUS.Messages.msg_ArgCmds;
import gov.nasa.larcfm.ICAROUS.Messages.msg_CmdAck;
import gov.nasa.larcfm.ICAROUS.Messages.msg_MissionItemReached;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Position;

public class ArduPilot extends MAVLinkInterface {
	
	double lastPing,newPing;
	double elapsedTime;
	boolean foundVehicle;
	
	public ArduPilot(Icarous IC){
		super(IC);
		lastPing = (double)System.nanoTime()/1E9;
		newPing  = (double)System.nanoTime()/1E9;
		foundVehicle = false;
	}
	
	@Override
	public void GetData(){
		byte[] buffer = null;
		MAVLinkPacket RcvdPacket = null;
		buffer = PT.Read();
		if(buffer != null){
			for(int i=0;i<buffer.length;++i){
				RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer[i]);
				if(RcvdPacket != null){
					ProcessMessage(RcvdPacket);
					pipe.SendMavlinkData(RcvdPacket);
				}			
			}
		}
	}
	
	@Override
	public void HandleHeartbeat(MAVLinkPacket msg){
		msg_heartbeat hbeat = (msg_heartbeat) msg.unpack();
		lastPing = newPing;
		newPing  = (double)System.nanoTime()/1E9;
		elapsedTime = newPing - lastPing;
		
		if(elapsedTime > 15){
			foundVehicle = false;
			System.out.println("Reconnecting with vehicle");
		}
		
		if(!foundVehicle){
			EnableDataStream(1);
			foundVehicle = true;
		}
	}
		
	@Override
	public void HandleMissionItemReached(MAVLinkPacket msg){
		msg_mission_item_reached msgMissionItemReached = (msg_mission_item_reached) msg.unpack();		
		if(pipe.waypointType.get(msgMissionItemReached.seq) == MAV_CMD.MAV_CMD_NAV_WAYPOINT ||
				pipe.waypointType.get(msgMissionItemReached.seq) == MAV_CMD.MAV_CMD_NAV_SPLINE_WAYPOINT){
			msg_MissionItemReached wpreached = new msg_MissionItemReached(msgMissionItemReached.seq);
			IC.InputMissionItemReached(wpreached);
		}	
	}
	
	@Override
	public void HandleCommandAck(MAVLinkPacket msg){
		msg_command_ack ack = (msg_command_ack) msg.unpack();
		msg_CmdAck icAck = new msg_CmdAck();		
		switch(ack.command){
		case MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM:
			icAck.name = msg_ArgCmds.command_name._ARM_;
			icAck.result = ack.result;
			IC.InputAck(icAck);
			break;
			
		case MAV_CMD.MAV_CMD_NAV_TAKEOFF:
			icAck.name = msg_ArgCmds.command_name._TAKEOFF_;
			icAck.result = ack.result;
			IC.InputAck(icAck);
			break;
		}
	}

	@Override
	public void HandlePosition(MAVLinkPacket msg){
		msg_global_position_int globalPositionInt = (msg_global_position_int) msg.unpack();
		
		msg_Position position = new msg_Position();
		position.time_gps  = (double)globalPositionInt.time_boot_ms/1E3;
		position.latitude  = (double)globalPositionInt.lat/1E7;
		position.longitude = (double)globalPositionInt.lon/1E7;
		position.altitude_abs  = (double)globalPositionInt.alt/1E3;
		position.altitude_rel  = (double)globalPositionInt.relative_alt/1E3;
		position.vx = (double)globalPositionInt.vx/100;
		position.vy = (double)globalPositionInt.vy/100;
		position.vz = (double)globalPositionInt.vz/100;
		
		IC.InputPosition(position);		
	}
}
