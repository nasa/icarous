package gov.nasa.larcfm.ICAROUS.Interface;

import java.util.ArrayList;

import com.MAVLink.MAVLinkPacket;
import com.MAVLink.Parser;
import com.MAVLink.Messages.MAVLinkMessage;
import com.MAVLink.ardupilotmega.msg_fence_point;
import com.MAVLink.ardupilotmega.msg_radio;
import com.MAVLink.common.msg_command_ack;
import com.MAVLink.common.msg_command_int;
import com.MAVLink.common.msg_command_long;
import com.MAVLink.common.msg_global_position_int;
import com.MAVLink.common.msg_heartbeat;
import com.MAVLink.common.msg_mission_count;
import com.MAVLink.common.msg_mission_item;
import com.MAVLink.common.msg_mission_item_reached;
import com.MAVLink.common.msg_mission_request;
import com.MAVLink.common.msg_mission_request_list;
import com.MAVLink.common.msg_mission_set_current;
import com.MAVLink.common.msg_param_request_list;
import com.MAVLink.common.msg_param_request_read;
import com.MAVLink.common.msg_param_set;
import com.MAVLink.common.msg_param_value;
import com.MAVLink.common.msg_radio_status;
import com.MAVLink.common.msg_request_data_stream;
import com.MAVLink.common.msg_set_mode;
import com.MAVLink.common.msg_set_position_target_global_int;
import com.MAVLink.common.msg_set_position_target_local_ned;
import com.MAVLink.enums.MAV_AUTOPILOT;
import com.MAVLink.enums.MAV_CMD;
import com.MAVLink.enums.MAV_DATA_STREAM;
import com.MAVLink.enums.MAV_FRAME;
import com.MAVLink.enums.MAV_TYPE;
import com.MAVLink.icarous.msg_kinematic_bands;

import gov.nasa.larcfm.ICAROUS.Icarous;
import gov.nasa.larcfm.ICAROUS.Icarous.IcarousMode;
import gov.nasa.larcfm.ICAROUS.Port;
import gov.nasa.larcfm.ICAROUS.Messages.*;

public class MAVLinkInterface implements Interface,Runnable {

	public class ARDUPILOT_MODES{
		public static final int STABILIZE =     0;  // manual airframe angle with manual throttle
		public static final int ACRO =          1;  // manual body-frame angular rate with manual throttle
		public static final int ALT_HOLD =      2;  // manual airframe angle with automatic throttle
		public static final int AUTO =          3;  // fully automatic waypoint control using mission commands
		public static final int GUIDED =        4;  // fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
		public static final int LOITER =        5;  // automatic horizontal acceleration with automatic throttle
		public static final int RTL =           6;  // automatic return to launching point
		public static final int CIRCLE =        7;  // automatic circular flight with automatic throttle
		public static final int LAND =          9;  // automatic landing with horizontal position control
		public static final int DRIFT =        11;  // semi-automous position, yaw and throttle control
		public static final int SPORT =        13;  // manual earth-frame angular rate control with manual throttle
		public static final int FLIP =         14;  // automatically flip the vehicle on the roll axis
		public static final int AUTOTUNE =     15;  // automatically tune the vehicle's roll and pitch gains
		public static final int POSHOLD =      16;  // automatic position hold with manual override, with automatic throttle
		public static final int BRAKE =        17;  // full-brake using inertial/GPS system, no pilot input
		public static final int THROW =        18;  // throw to launch mode using inertial/GPS system, no pilot input
		public static final int AVOID_ADSB =   19;  // automatic avoidance of obstacles in the macro scale - e.g. full-sized aircraft
		public static final int GUIDED_NOGPS = 20;  // guided mode but only accepts attitude and altitude
	}
	
	protected Port PT;
	protected Icarous IC;
	protected Parser MsgParser      = new Parser();
	public MAVLinkInterface pipe;
	public ArrayList<Integer> waypointType;	
	public float speed;
	
	MAVLinkInterface(Icarous ic){
		IC = ic;
	}

	public void SetPipe(MAVLinkInterface intf){
		pipe = intf;
	}
	
	public void ConfigurePorts(String portname,int baudrate){
		PT = new Port(Port.PortType.SERIAL, portname, baudrate);
	}
	
	public void ConfigurePorts(String hostname,int input,int output){
		PT = new Port(Port.PortType.SOCKET,hostname,input,output);
	}

	@Override
	public void GetData() {					
		ArrayList<MAVLinkPacket> RcvdData = null;
		MAVLinkPacket RcvdPacket = null;
		RcvdData = ReadMavlinkData();		
		for(int i=0;i<RcvdData.size();++i){
			RcvdPacket = RcvdData.get(i);				
			ProcessMessage(RcvdPacket);						
		}
	}
	
	@Override
	public void SendData(IcarousMessages msg) {
		// TODO Auto-generated method stub
		switch(msg.msgid){
		case msg_ArgCmds.ICAROUS_MSG_ID_ARG_CMDS:
			SendCommand((msg_ArgCmds) msg);
		}
	}

	public ArrayList<MAVLinkPacket> ReadMavlinkData(){
		ArrayList<MAVLinkPacket> listRcvdData = new ArrayList<MAVLinkPacket>();
		byte[] buffer = null;
		MAVLinkPacket RcvdPacket = null;
		buffer = PT.Read();
		if(buffer != null){
			for(int i=0;i<buffer.length;++i){
				RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer[i]);
				if(RcvdPacket != null){
					listRcvdData.add(RcvdPacket);
				}			
			}
		}		
		return listRcvdData;
	}

	public void SendMavlinkData(MAVLinkMessage msg2send){
		MAVLinkPacket raw_packet = msg2send.pack();
		raw_packet.sysid  = msg2send.sysid;
		raw_packet.compid = msg2send.compid;	    
		SendMavlinkData(raw_packet);
	}

	public void SendMavlinkData(MAVLinkPacket raw_packet){			    
		byte[] buffer     = raw_packet.encodePacket();	    	    
		PT.Write(buffer);
	}


	public void ProcessMessage(MAVLinkPacket RcvdPacket){
		switch(RcvdPacket.msgid){
		
		case msg_global_position_int.MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
			HandlePosition(RcvdPacket);
			break;
			
		case msg_command_ack.MAVLINK_MSG_ID_COMMAND_ACK:
			HandleCommandAck(RcvdPacket);
			break;
			
		case msg_mission_item_reached.MAVLINK_MSG_ID_MISSION_ITEM_REACHED:
			HandleMissionItemReached(RcvdPacket);
			break;
			
		case msg_heartbeat.MAVLINK_MSG_ID_HEARTBEAT:        
            //HandleHeartbeat(RcvdPacket);
            break;        

        case msg_mission_count.MAVLINK_MSG_ID_MISSION_COUNT:
            HandleMissionCount(RcvdPacket);
            break;

        case msg_mission_item.MAVLINK_MSG_ID_MISSION_ITEM:
            HandleMissionItem(RcvdPacket);
            break;


        case msg_mission_request_list.MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
            HandleMissionRequestList(RcvdPacket);
            break;

        case msg_mission_request.MAVLINK_MSG_ID_MISSION_REQUEST:
            HandleMissionRequest(RcvdPacket);
            break;


        case msg_param_request_list.MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
            HandleParamRequestList(RcvdPacket);
            break;

        case msg_param_request_read.MAVLINK_MSG_ID_PARAM_REQUEST_READ:
            HandleParamRequestRead(RcvdPacket);
            break;

        case msg_param_value.MAVLINK_MSG_ID_PARAM_VALUE:
            HandleParamValue(RcvdPacket);
            break;

        case msg_param_set.MAVLINK_MSG_ID_PARAM_SET:
            HandleParamSet(RcvdPacket);
            break;

        case msg_command_long.MAVLINK_MSG_ID_COMMAND_LONG:
            HandleCommandLong(RcvdPacket);
            break;

        case msg_command_int.MAVLINK_MSG_ID_COMMAND_INT:
            HandleCommandInt(RcvdPacket);
            break;

        case msg_set_mode.MAVLINK_MSG_ID_SET_MODE:
            HandleSetMode(RcvdPacket);
            break;


        case msg_fence_point.MAVLINK_MSG_ID_FENCE_POINT:
            HandleFencePoint(RcvdPacket);
            break;

        case msg_radio.MAVLINK_MSG_ID_RADIO:
            HandleRadio(RcvdPacket);
            break;

        case msg_radio_status.MAVLINK_MSG_ID_RADIO_STATUS:
            HandleRadioStatus(RcvdPacket);
            break;	
		}
	}
	
	
		
	protected void HandleRadioStatus(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleRadio(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleFencePoint(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleSetMode(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleCommandInt(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleCommandLong(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleParamSet(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleParamValue(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleParamRequestList(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleParamRequestRead(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleMissionRequest(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleMissionRequestList(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleMissionItem(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleMissionCount(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleHeartbeat(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleMissionItemReached(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandleCommandAck(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}

	protected void HandlePosition(MAVLinkPacket rcvdPacket) {
		// TODO Auto-generated method stub
		
	}
	

	public void SendCommand(msg_ArgCmds msg){
		switch(msg.name){

		case msg_ArgCmds.command_name._ARM_:
			SendCommand(0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,0,
					1,0,0,0,0,0,0);			
			break;
			
		case msg_ArgCmds.command_name._TAKEOFF_:
			SendCommand(0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,0,
					1,0,0,0,0,0,msg.param1);
			break;
			
		case msg_ArgCmds.command_name._SETMODE_:				
			if(msg.param1 == Icarous.IcarousMode._ACTIVE_){			
				SetMode(ARDUPILOT_MODES.GUIDED);
			}else if(msg.param1 == Icarous.IcarousMode._PASSIVE_){
				SetMode(ARDUPILOT_MODES.AUTO);
			}		
			break;
			
		case msg_ArgCmds.command_name._LAND_:
			SendCommand(0,0,MAV_CMD.MAV_CMD_NAV_LAND,0,
					0,0,0,0,0,0,0);			
			break;
			
		case msg_ArgCmds.command_name._GOTOWP_:
			SetMissionItem((int)msg.param1);
			break;
			
		case msg_ArgCmds.command_name._SETPOS_:
			SetGPSPos(msg.param1, msg.param2, msg.param3);
			break;
			
		case msg_ArgCmds.command_name._SETVEL_:
			SetVelocity(msg.param1, msg.param2, msg.param3);
			break;
			
		case msg_ArgCmds.command_name._SETYAW_:
			SendCommand(0,0,MAV_CMD.MAV_CMD_CONDITION_YAW,0,
					msg.param1,msg.param2,msg.param3,msg.param4,
					0,0,0);
			break;
			
		case msg_ArgCmds.command_name._SETSPEED_:
			SetSpeed(msg.param1);
			break;			
		}
	}

	public void SendBand(msg_Visbands msg){
		msg_kinematic_bands visbands = new msg_kinematic_bands();
		visbands.numBands = (byte)msg.numBands;
		visbands.type1 = (byte) msg.type1;
		visbands.min1 = (float) msg.min1;
		visbands.max1 = (float) msg.max1;
		
		visbands.type2 = (byte) msg.type2;
		visbands.min2 = (float) msg.min2;
		visbands.max2 = (float) msg.max2;
		
		visbands.type3 = (byte) msg.type3;
		visbands.min3 = (float) msg.min3;
		visbands.max3 = (float) msg.max3;
		
		visbands.type4 = (byte) msg.type4;
		visbands.min4 = (float) msg.min4;
		visbands.max4 = (float) msg.max4;
		
		visbands.type5 = (byte) msg.type5;
		visbands.min5 = (float) msg.min5;
		visbands.max5 = (float) msg.max5;
		
		SendMavlinkData(visbands);
	}

	// Function to send mavlink long commands
	protected void SendCommand( int target_system,int target_component,int command,int confirmation,
			float param1,float param2,float param3,float param4,float param5,
			float param6,float param7){

		msg_command_long CommandLong  = new msg_command_long();

		CommandLong.target_system     = (short) target_system;
		CommandLong.target_component  = (short) target_component;
		CommandLong.command           = command;
		CommandLong.confirmation      = (short) confirmation;
		CommandLong.param1            = param1;
		CommandLong.param2            = param2;
		CommandLong.param3            = param3;
		CommandLong.param4            = param4;
		CommandLong.param5            = param5;
		CommandLong.param6            = param6;
		CommandLong.param7            = param7;

		SendMavlinkData(CommandLong);

		try{
			Thread.sleep(100);
		}catch(InterruptedException e){
			System.out.println(e);
		}
	}

	// Yaw command
	protected void SetYaw(boolean relative, double heading){
		int rel = relative?1:0;		
		SendCommand(0,0,MAV_CMD.MAV_CMD_CONDITION_YAW,0,
				(float)heading,0,1,rel,
				0,0,0);
	}
	
	protected void SetSpeed(float speed){
		SendCommand(0,0,MAV_CMD.MAV_CMD_DO_CHANGE_SPEED,0,
				1,speed,0,0,0,0,0);			
	}

	// Position command
	protected void SetGPSPos(double lat,double lon, double alt){

		msg_set_position_target_global_int msg= new msg_set_position_target_global_int();

		msg.time_boot_ms     = 0;
		msg.target_system    = 0;
		msg.target_component = 0;
		msg.coordinate_frame = MAV_FRAME.MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;
		msg.type_mask        = 0b0000111111111000;
		msg.lat_int          = (int) (lat*1E7);
		msg.lon_int          = (int) (lon*1E7);
		msg.alt              = (float) alt;
		msg.vx               = 0;
		msg.vy               = 0;
		msg.vz               = 0;
		msg.afx              = 0;
		msg.afy              = 0;
		msg.afz              = 0;
		msg.yaw              = 0;
		msg.yaw_rate         = 0;

		SendMavlinkData(msg);

		try{
			Thread.sleep(100);
		}catch(InterruptedException e){
			System.out.println(e);
		}
	}

	// Velocity command
	protected void SetVelocity(double Vn,double Ve, double Vu){

		msg_set_position_target_local_ned msg= new msg_set_position_target_local_ned();

		msg.time_boot_ms     = 0;
		msg.target_system    = 0;
		msg.target_component = 0;
		msg.coordinate_frame = MAV_FRAME.MAV_FRAME_LOCAL_NED;	
		msg.type_mask        = 0b0000111111000111;
		msg.x                = 0;
		msg.y                = 0;
		msg.z                = 0;
		msg.vx               = (float)Vn;
		msg.vy               = (float)Ve;
		msg.vz               = (float)Vu;
		msg.afx              = 0;
		msg.afy              = 0;
		msg.afz              = 0;
		msg.yaw              = 0;
		msg.yaw_rate         = 0;

		SendMavlinkData(msg);

		try{
			Thread.sleep(100);
		}
		catch(InterruptedException e){
			System.out.println(e);
		}
	}

	// Function to set mode
	protected int SetMode(int modeid){

		msg_set_mode Mode = new msg_set_mode();
		Mode.target_system = (short) 0;
		Mode.base_mode     = (short) 1;
		Mode.custom_mode   = (long) modeid;

		SendMavlinkData(Mode);

		try{
			Thread.sleep(200);
		}catch(InterruptedException e){
			System.out.println(e);
		}

		return 1;
	}

	protected void SetMissionItem(int nextWP){
		msg_mission_set_current msgMission = new msg_mission_set_current();
		msgMission.target_system = 1;
		msgMission.target_component = 0;        
		msgMission.seq = nextWP;               
		
		//System.out.println("Setting AP mission seq:"+msgMission.seq);
		SendMavlinkData(msgMission);
	}
	
	protected void EnableDataStream(int option){
		
		msg_request_data_stream req = new msg_request_data_stream();
		req.req_message_rate = 4;
		req.req_stream_id    = MAV_DATA_STREAM.MAV_DATA_STREAM_ALL;
		req.start_stop       = (byte) option;
		req.target_system    = 1;
		req.target_component = 0;

		SendMavlinkData(req);
	}

	@Override
	public void run() {		
		while(true){
			GetData();
		}
	}
	
	public void Start(String threadName){
		Thread t = new Thread(this,threadName);
		t.start();
	}

}
