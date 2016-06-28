/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

/**
 * @author svazquez
 *
 */
public class MavLink_0_9
{
	public static ANALOG_RAW_class							ANALOG_RAW							= new ANALOG_RAW_class();							// (111)
	public static ANALOG_EU_class							ANALOG_EU							= new ANALOG_EU_class();							// (112)
	public static CDNR_CONTROLLER_class						CDNR_CONTROLLER						= new CDNR_CONTROLLER_class();						// (113)
	public static TRAFFIC_DATA_class						TRAFFIC_DATA						= new TRAFFIC_DATA_class();						// (114)
	public static STATE_DATA_class							STATE_DATA							= new STATE_DATA_class();							// (115)
	public static TRAFFIC_SIM_TIMING_class					TRAFFIC_SIM_TIMING					= new TRAFFIC_SIM_TIMING_class();					// (116)
	public static HEARTBEAT_class							HEARTBEAT							= new HEARTBEAT_class();							// (0)
	public static BOOT_class								BOOT								= new BOOT_class();								// (1)
	public static SYSTEM_TIME_class							SYSTEM_TIME							= new SYSTEM_TIME_class();							// (2)
	public static PING_class								PING								= new PING_class();								// (3)
	public static SYSTEM_TIME_UTC_class						SYSTEM_TIME_UTC						= new SYSTEM_TIME_UTC_class();						// (4)
	public static CHANGE_OPERATOR_CONTROL_class				CHANGE_OPERATOR_CONTROL				= new CHANGE_OPERATOR_CONTROL_class();				// (5)
	public static CHANGE_OPERATOR_CONTROL_ACK_class			CHANGE_OPERATOR_CONTROL_ACK			= new CHANGE_OPERATOR_CONTROL_ACK_class();			// (6)
	public static AUTH_KEY_class							AUTH_KEY							= new AUTH_KEY_class();							// (7)
	public static ACTION_ACK_class							ACTION_ACK							= new ACTION_ACK_class();							// (9)
	public static ACTION_class								ACTION								= new ACTION_class();								// (10)
	public static SET_MODE_class							SET_MODE							= new SET_MODE_class();							// (11)
	public static SET_NAV_MODE_class						SET_NAV_MODE						= new SET_NAV_MODE_class();						// (12)
	public static PARAM_REQUEST_READ_class					PARAM_REQUEST_READ					= new PARAM_REQUEST_READ_class();					// (20)
	public static PARAM_REQUEST_LIST_class					PARAM_REQUEST_LIST					= new PARAM_REQUEST_LIST_class();					// (21)
	public static PARAM_VALUE_class							PARAM_VALUE							= new PARAM_VALUE_class();							// (22)
	public static PARAM_SET_class							PARAM_SET							= new PARAM_SET_class();							// (23)
	public static GPS_RAW_INT_class							GPS_RAW_INT							= new GPS_RAW_INT_class();							// (25)
	public static SCALED_IMU_class							SCALED_IMU							= new SCALED_IMU_class();							// (26)
	public static GPS_STATUS_class							GPS_STATUS							= new GPS_STATUS_class();							// (27)
	public static RAW_IMU_class								RAW_IMU								= new RAW_IMU_class();								// (28)
	public static RAW_PRESSURE_class						RAW_PRESSURE						= new RAW_PRESSURE_class();						// (29)
	public static SCALED_PRESSURE_class						SCALED_PRESSURE						= new SCALED_PRESSURE_class();						// (38)
	public static ATTITUDE_class							ATTITUDE							= new ATTITUDE_class();							// (30)
	public static LOCAL_POSITION_class						LOCAL_POSITION						= new LOCAL_POSITION_class();						// (31)
	public static GLOBAL_POSITION_class						GLOBAL_POSITION						= new GLOBAL_POSITION_class();						// (33)
	public static GPS_RAW_class								GPS_RAW								= new GPS_RAW_class();								// (32)
	public static SYS_STATUS_class							SYS_STATUS							= new SYS_STATUS_class();							// (34)
	public static RC_CHANNELS_RAW_class						RC_CHANNELS_RAW						= new RC_CHANNELS_RAW_class();						// (35)
	public static RC_CHANNELS_SCALED_class					RC_CHANNELS_SCALED					= new RC_CHANNELS_SCALED_class();					// (36)
	public static SERVO_OUTPUT_RAW_class					SERVO_OUTPUT_RAW					= new SERVO_OUTPUT_RAW_class();					// (37)
	public static WAYPOINT_class							WAYPOINT							= new WAYPOINT_class();							// (39)
	public static WAYPOINT_REQUEST_class					WAYPOINT_REQUEST					= new WAYPOINT_REQUEST_class();					// (40)
	public static WAYPOINT_SET_CURRENT_class				WAYPOINT_SET_CURRENT				= new WAYPOINT_SET_CURRENT_class();				// (41)
	public static WAYPOINT_CURRENT_class					WAYPOINT_CURRENT					= new WAYPOINT_CURRENT_class();					// (42)
	public static WAYPOINT_REQUEST_LIST_class				WAYPOINT_REQUEST_LIST				= new WAYPOINT_REQUEST_LIST_class();				// (43)
	public static WAYPOINT_COUNT_class						WAYPOINT_COUNT						= new WAYPOINT_COUNT_class();						// (44)
	public static WAYPOINT_CLEAR_ALL_class					WAYPOINT_CLEAR_ALL					= new WAYPOINT_CLEAR_ALL_class();					// (45)
	public static WAYPOINT_REACHED_class					WAYPOINT_REACHED					= new WAYPOINT_REACHED_class();					// (46)
	public static WAYPOINT_ACK_class						WAYPOINT_ACK						= new WAYPOINT_ACK_class();						// (47)
	public static GPS_SET_GLOBAL_ORIGIN_class				GPS_SET_GLOBAL_ORIGIN				= new GPS_SET_GLOBAL_ORIGIN_class();				// (48)
	public static GPS_LOCAL_ORIGIN_SET_class				GPS_LOCAL_ORIGIN_SET				= new GPS_LOCAL_ORIGIN_SET_class();				// (49)
	public static LOCAL_POSITION_SETPOINT_SET_class			LOCAL_POSITION_SETPOINT_SET			= new LOCAL_POSITION_SETPOINT_SET_class();			// (50)
	public static LOCAL_POSITION_SETPOINT_class				LOCAL_POSITION_SETPOINT				= new LOCAL_POSITION_SETPOINT_class();				// (51)
	public static CONTROL_STATUS_class						CONTROL_STATUS						= new CONTROL_STATUS_class();						// (52)
	public static SAFETY_SET_ALLOWED_AREA_class				SAFETY_SET_ALLOWED_AREA				= new SAFETY_SET_ALLOWED_AREA_class();				// (53)
	public static SAFETY_ALLOWED_AREA_class					SAFETY_ALLOWED_AREA					= new SAFETY_ALLOWED_AREA_class();					// (54)
	public static SET_ROLL_PITCH_YAW_THRUST_class			SET_ROLL_PITCH_YAW_THRUST			= new SET_ROLL_PITCH_YAW_THRUST_class();			// (55)
	public static SET_ROLL_PITCH_YAW_SPEED_THRUST_class		SET_ROLL_PITCH_YAW_SPEED_THRUST		= new SET_ROLL_PITCH_YAW_SPEED_THRUST_class();		// (56)
	public static ROLL_PITCH_YAW_THRUST_SETPOINT_class		ROLL_PITCH_YAW_THRUST_SETPOINT		= new ROLL_PITCH_YAW_THRUST_SETPOINT_class();		// (57)
	public static ROLL_PITCH_YAW_SPEED_THRUST_SETPOINT_class	ROLL_PITCH_YAW_SPEED_THRUST_SETPOINT	= new ROLL_PITCH_YAW_SPEED_THRUST_SETPOINT_class();	// (58)
	public static NAV_CONTROLLER_OUTPUT_class				NAV_CONTROLLER_OUTPUT				= new NAV_CONTROLLER_OUTPUT_class();				// (62)
	public static POSITION_TARGET_class						POSITION_TARGET						= new POSITION_TARGET_class();						// (63)
	public static STATE_CORRECTION_class					STATE_CORRECTION					= new STATE_CORRECTION_class();					// (64)
	public static SET_ALTITUDE_class						SET_ALTITUDE						= new SET_ALTITUDE_class();						// (65)
	public static REQUEST_DATA_STREAM_class					REQUEST_DATA_STREAM					= new REQUEST_DATA_STREAM_class();					// (66)
	public static HIL_STATE_class							HIL_STATE							= new HIL_STATE_class();							// (67)
	public static HIL_CONTROLS_class						HIL_CONTROLS						= new HIL_CONTROLS_class();						// (68)
	public static MANUAL_CONTROL_class						MANUAL_CONTROL						= new MANUAL_CONTROL_class();						// (69)
	public static RC_CHANNELS_OVERRIDE_class				RC_CHANNELS_OVERRIDE				= new RC_CHANNELS_OVERRIDE_class();				// (70)
	public static GLOBAL_POSITION_INT_class					GLOBAL_POSITION_INT					= new GLOBAL_POSITION_INT_class();					// (73)
	public static VFR_HUD_class								VFR_HUD								= new VFR_HUD_class();								// (74)
	public static COMMAND_class								COMMAND								= new COMMAND_class();								// (75)
	public static COMMAND_ACK_class							COMMAND_ACK							= new COMMAND_ACK_class();							// (76)
	public static OPTICAL_FLOW_class						OPTICAL_FLOW						= new OPTICAL_FLOW_class();						// (100)
	public static OBJECT_DETECTION_EVENT_class				OBJECT_DETECTION_EVENT				= new OBJECT_DETECTION_EVENT_class();				// (140)
	public static DEBUG_VECT_class							DEBUG_VECT							= new DEBUG_VECT_class();							// (251)
	public static NAMED_VALUE_FLOAT_class					NAMED_VALUE_FLOAT					= new NAMED_VALUE_FLOAT_class();					// (252)
	public static NAMED_VALUE_INT_class						NAMED_VALUE_INT						= new NAMED_VALUE_INT_class();						// (253)
	public static STATUSTEXT_class							STATUSTEXT							= new STATUSTEXT_class();							// (254)
	public static DEBUG_class								DEBUG								= new DEBUG_class();								// (255)
	public static SENSOR_OFFSETS_class						SENSOR_OFFSETS						= new SENSOR_OFFSETS_class();						// (150)
	public static SET_MAG_OFFSETS_class						SET_MAG_OFFSETS						= new SET_MAG_OFFSETS_class();						// (151)
	public static MEMINFO_class								MEMINFO								= new MEMINFO_class();								// (152)
	public static AP_ADC_class								AP_ADC								= new AP_ADC_class();								// (153)
	public static DIGICAM_CONFIGURE_class					DIGICAM_CONFIGURE					= new DIGICAM_CONFIGURE_class();					// (154)
	public static DIGICAM_CONTROL_class						DIGICAM_CONTROL						= new DIGICAM_CONTROL_class();						// (155)
	public static MOUNT_CONFIGURE_class						MOUNT_CONFIGURE						= new MOUNT_CONFIGURE_class();						// (156)
	public static MOUNT_CONTROL_class						MOUNT_CONTROL						= new MOUNT_CONTROL_class();						// (157)
	public static MOUNT_STATUS_class						MOUNT_STATUS						= new MOUNT_STATUS_class();						// (158)
	public static FENCE_POINT_class							FENCE_POINT							= new FENCE_POINT_class();							// (160)
	public static FENCE_FETCH_POINT_class					FENCE_FETCH_POINT					= new FENCE_FETCH_POINT_class();					// (161)
	public static FENCE_STATUS_class						FENCE_STATUS						= new FENCE_STATUS_class();						// (162)

	
	//=====================================================================================//
	/**
	 * parser muxing based on message type.
	 * @param pn   Serial port logical name.
	 */
	//=====================================================================================//
	public static boolean processPacket(byte[] pkt, boolean valid)
	{
		switch(pkt[5])
		{
			case (byte)111:
				return(ANALOG_RAW.parse(pkt, valid));
			case (byte)112:
				return(ANALOG_EU.parse(pkt, valid));
			case (byte)113:
				return(CDNR_CONTROLLER.parse(pkt, valid));
			case (byte)114:
				return(TRAFFIC_DATA.parse(pkt, valid));
			case (byte)115:
				return(STATE_DATA.parse(pkt, valid));
			case (byte)116:	
				return(TRAFFIC_SIM_TIMING.parse(pkt, valid));
			case (byte)0:
				return(HEARTBEAT.parse(pkt, valid));
			case (byte)1:
				return(BOOT.parse(pkt, valid));
			case (byte)2:
				return(SYSTEM_TIME.parse(pkt, valid));
			case (byte)3:
				return(PING.parse(pkt, valid));
			case (byte)4:
				return(SYSTEM_TIME_UTC.parse(pkt, valid));
			case (byte)5:
				return(CHANGE_OPERATOR_CONTROL.parse(pkt, valid));
			case (byte)6:
				return(CHANGE_OPERATOR_CONTROL_ACK.parse(pkt, valid));
			case (byte)7:
				return(AUTH_KEY.parse(pkt, valid));
			case (byte)9:
				return(ACTION_ACK.parse(pkt, valid));
			case (byte)10:
				return(ACTION.parse(pkt, valid));
			case (byte)11:
				return(SET_MODE.parse(pkt, valid));
			case (byte)12:
				return(SET_NAV_MODE.parse(pkt, valid));
			case (byte)20:
				return(PARAM_REQUEST_READ.parse(pkt, valid));
			case (byte)21:
				return(PARAM_REQUEST_LIST.parse(pkt, valid));
			case (byte)22:
				return(PARAM_VALUE.parse(pkt, valid));
			case (byte)23:
				return(PARAM_SET.parse(pkt, valid));
			case (byte)25:
				return(GPS_RAW_INT.parse(pkt, valid));
			case (byte)26:
				return(SCALED_IMU.parse(pkt, valid));
			case (byte)27:
				return(GPS_STATUS.parse(pkt, valid));
			case (byte)28:
				return(RAW_IMU.parse(pkt, valid));
			case (byte)29:
				return(RAW_PRESSURE.parse(pkt, valid));
			case (byte)38:
				return(SCALED_PRESSURE.parse(pkt, valid));
			case (byte)30:
				return(ATTITUDE.parse(pkt, valid));
			case (byte)31:
				return(LOCAL_POSITION.parse(pkt, valid));
			case (byte)33:
				return(GLOBAL_POSITION.parse(pkt, valid));
			case (byte)32:
				return(GPS_RAW.parse(pkt, valid));
			case (byte)34:
				return(SYS_STATUS.parse(pkt, valid));
			case (byte)35:
				return(RC_CHANNELS_RAW.parse(pkt, valid));
			case (byte)36:
				return(RC_CHANNELS_SCALED.parse(pkt, valid));
			case (byte)37:
				return(SERVO_OUTPUT_RAW.parse(pkt, valid));
			case (byte)39:
				return(WAYPOINT.parse(pkt, valid));
			case (byte)40:
				return(WAYPOINT_REQUEST.parse(pkt, valid));
			case (byte)41:
				return(WAYPOINT_SET_CURRENT.parse(pkt, valid));
			case (byte)42:
				return(WAYPOINT_CURRENT.parse(pkt, valid));
			case (byte)43:
				return(WAYPOINT_REQUEST_LIST.parse(pkt, valid));
			case (byte)44:
				return(WAYPOINT_COUNT.parse(pkt, valid));
			case (byte)45:
				return(WAYPOINT_CLEAR_ALL.parse(pkt, valid));
			case (byte)46:
				return(WAYPOINT_REACHED.parse(pkt, valid));
			case (byte)47:
				return(WAYPOINT_ACK.parse(pkt, valid));
			case (byte)48:
				return(GPS_SET_GLOBAL_ORIGIN.parse(pkt, valid));
			case (byte)49:
				return(GPS_LOCAL_ORIGIN_SET.parse(pkt, valid));
			case (byte)50:
				return(LOCAL_POSITION_SETPOINT_SET.parse(pkt, valid));
			case (byte)51:
				return(LOCAL_POSITION_SETPOINT.parse(pkt, valid));
			case (byte)52:
				return(CONTROL_STATUS.parse(pkt, valid));
			case (byte)53:
				return(SAFETY_SET_ALLOWED_AREA.parse(pkt, valid));
			case (byte)54:
				return(SAFETY_ALLOWED_AREA.parse(pkt, valid));
			case (byte)55:
				return(SET_ROLL_PITCH_YAW_THRUST.parse(pkt, valid));
			case (byte)56:
				return(SET_ROLL_PITCH_YAW_SPEED_THRUST.parse(pkt, valid));
			case (byte)57:
				return(ROLL_PITCH_YAW_THRUST_SETPOINT.parse(pkt, valid));
			case (byte)58:
				return(ROLL_PITCH_YAW_SPEED_THRUST_SETPOINT.parse(pkt, valid));
			case (byte)62:
				return(NAV_CONTROLLER_OUTPUT.parse(pkt, valid));
			case (byte)63:
				return(POSITION_TARGET.parse(pkt, valid));
			case (byte)64:
				return(STATE_CORRECTION.parse(pkt, valid));
			case (byte)65:
				return(SET_ALTITUDE.parse(pkt, valid));
			case (byte)66:
				return(REQUEST_DATA_STREAM.parse(pkt, valid));
			case (byte)67:
				return(HIL_STATE.parse(pkt, valid));
			case (byte)68:
				return(HIL_CONTROLS.parse(pkt, valid));
			case (byte)69:
				return(MANUAL_CONTROL.parse(pkt, valid));
			case (byte)70:
				return(RC_CHANNELS_OVERRIDE.parse(pkt, valid));
			case (byte)73:
				return(GLOBAL_POSITION_INT.parse(pkt, valid));
			case (byte)74:
				return(VFR_HUD.parse(pkt, valid));
			case (byte)75:
				return(COMMAND.parse(pkt, valid));
			case (byte)76:
				return(COMMAND_ACK.parse(pkt, valid));
			case (byte)100:
				return(OPTICAL_FLOW.parse(pkt, valid));
			case (byte)140:
				return(OBJECT_DETECTION_EVENT.parse(pkt, valid));
			case (byte)251:
				return(DEBUG_VECT.parse(pkt, valid));
			case (byte)252:
				return(NAMED_VALUE_FLOAT.parse(pkt, valid));
			case (byte)253:
				return(NAMED_VALUE_INT.parse(pkt, valid));
			case (byte)254:
				return(STATUSTEXT.parse(pkt, valid));
			case (byte)255:
				return(DEBUG.parse(pkt, valid));
			case (byte)150:
				return(SENSOR_OFFSETS.parse(pkt, valid));
			case (byte)151:
				return(SET_MAG_OFFSETS.parse(pkt, valid));
			case (byte)152:
				return(MEMINFO.parse(pkt, valid));
			case (byte)153:
				return(AP_ADC.parse(pkt, valid));
			case (byte)154:
				return(DIGICAM_CONFIGURE.parse(pkt, valid));
			case (byte)155:
				return(DIGICAM_CONTROL.parse(pkt, valid));
			case (byte)156:
				return(MOUNT_CONFIGURE.parse(pkt, valid));
			case (byte)157:
				return(MOUNT_CONTROL.parse(pkt, valid));
			case (byte)158:
				return(MOUNT_STATUS.parse(pkt, valid));
			case (byte)160:
				return(FENCE_POINT.parse(pkt, valid));
			case (byte)161:
				return(FENCE_FETCH_POINT.parse(pkt, valid));
			case (byte)162:
				return(FENCE_STATUS.parse(pkt, valid));
			default:
				return(false);
		}
	}
	// <<<------------------------------
	// end of message definitions

	
	
	
	public static boolean processPacket(byte[] pkt)
	{
		switch(pkt[5])
		{
			case (byte)111:
				return(ANALOG_RAW.parse(pkt));
			case (byte)112:
				return(ANALOG_EU.parse(pkt));
			case (byte)113:
				return(CDNR_CONTROLLER.parse(pkt));
			case (byte)114:
				return(TRAFFIC_DATA.parse(pkt));
			case (byte)115:
				return(STATE_DATA.parse(pkt));
			case (byte)116:	
				return(TRAFFIC_SIM_TIMING.parse(pkt));
			case (byte)0:
				return(HEARTBEAT.parse(pkt));
			case (byte)1:
				return(BOOT.parse(pkt));
			case (byte)2:
				return(SYSTEM_TIME.parse(pkt));
			case (byte)3:
				return(PING.parse(pkt));
			case (byte)4:
				return(SYSTEM_TIME_UTC.parse(pkt));
			case (byte)5:
				return(CHANGE_OPERATOR_CONTROL.parse(pkt));
			case (byte)6:
				return(CHANGE_OPERATOR_CONTROL_ACK.parse(pkt));
			case (byte)7:
				return(AUTH_KEY.parse(pkt));
			case (byte)9:
				return(ACTION_ACK.parse(pkt));
			case (byte)10:
				return(ACTION.parse(pkt));
			case (byte)11:
				return(SET_MODE.parse(pkt));
			case (byte)12:
				return(SET_NAV_MODE.parse(pkt));
			case (byte)20:
				return(PARAM_REQUEST_READ.parse(pkt));
			case (byte)21:
				return(PARAM_REQUEST_LIST.parse(pkt));
			case (byte)22:
				return(PARAM_VALUE.parse(pkt));
			case (byte)23:
				return(PARAM_SET.parse(pkt));
			case (byte)25:
				return(GPS_RAW_INT.parse(pkt));
			case (byte)26:
				return(SCALED_IMU.parse(pkt));
			case (byte)27:
				return(GPS_STATUS.parse(pkt));
			case (byte)28:
				return(RAW_IMU.parse(pkt));
			case (byte)29:
				return(RAW_PRESSURE.parse(pkt));
			case (byte)38:
				return(SCALED_PRESSURE.parse(pkt));
			case (byte)30:
				return(ATTITUDE.parse(pkt));
			case (byte)31:
				return(LOCAL_POSITION.parse(pkt));
			case (byte)33:
				return(GLOBAL_POSITION.parse(pkt));
			case (byte)32:
				return(GPS_RAW.parse(pkt));
			case (byte)34:
				return(SYS_STATUS.parse(pkt));
			case (byte)35:
				return(RC_CHANNELS_RAW.parse(pkt));
			case (byte)36:
				return(RC_CHANNELS_SCALED.parse(pkt));
			case (byte)37:
				return(SERVO_OUTPUT_RAW.parse(pkt));
			case (byte)39:
				return(WAYPOINT.parse(pkt));
			case (byte)40:
				return(WAYPOINT_REQUEST.parse(pkt));
			case (byte)41:
				return(WAYPOINT_SET_CURRENT.parse(pkt));
			case (byte)42:
				return(WAYPOINT_CURRENT.parse(pkt));
			case (byte)43:
				return(WAYPOINT_REQUEST_LIST.parse(pkt));
			case (byte)44:
				return(WAYPOINT_COUNT.parse(pkt));
			case (byte)45:
				return(WAYPOINT_CLEAR_ALL.parse(pkt));
			case (byte)46:
				return(WAYPOINT_REACHED.parse(pkt));
			case (byte)47:
				return(WAYPOINT_ACK.parse(pkt));
			case (byte)48:
				return(GPS_SET_GLOBAL_ORIGIN.parse(pkt));
			case (byte)49:
				return(GPS_LOCAL_ORIGIN_SET.parse(pkt));
			case (byte)50:
				return(LOCAL_POSITION_SETPOINT_SET.parse(pkt));
			case (byte)51:
				return(LOCAL_POSITION_SETPOINT.parse(pkt));
			case (byte)52:
				return(CONTROL_STATUS.parse(pkt));
			case (byte)53:
				return(SAFETY_SET_ALLOWED_AREA.parse(pkt));
			case (byte)54:
				return(SAFETY_ALLOWED_AREA.parse(pkt));
			case (byte)55:
				return(SET_ROLL_PITCH_YAW_THRUST.parse(pkt));
			case (byte)56:
				return(SET_ROLL_PITCH_YAW_SPEED_THRUST.parse(pkt));
			case (byte)57:
				return(ROLL_PITCH_YAW_THRUST_SETPOINT.parse(pkt));
			case (byte)58:
				return(ROLL_PITCH_YAW_SPEED_THRUST_SETPOINT.parse(pkt));
			case (byte)62:
				return(NAV_CONTROLLER_OUTPUT.parse(pkt));
			case (byte)63:
				return(POSITION_TARGET.parse(pkt));
			case (byte)64:
				return(STATE_CORRECTION.parse(pkt));
			case (byte)65:
				return(SET_ALTITUDE.parse(pkt));
			case (byte)66:
				return(REQUEST_DATA_STREAM.parse(pkt));
			case (byte)67:
				return(HIL_STATE.parse(pkt));
			case (byte)68:
				return(HIL_CONTROLS.parse(pkt));
			case (byte)69:
				return(MANUAL_CONTROL.parse(pkt));
			case (byte)70:
				return(RC_CHANNELS_OVERRIDE.parse(pkt));
			case (byte)73:
				return(GLOBAL_POSITION_INT.parse(pkt));
			case (byte)74:
				return(VFR_HUD.parse(pkt));
			case (byte)75:
				return(COMMAND.parse(pkt));
			case (byte)76:
				return(COMMAND_ACK.parse(pkt));
			case (byte)100:
				return(OPTICAL_FLOW.parse(pkt));
			case (byte)140:
				return(OBJECT_DETECTION_EVENT.parse(pkt));
			case (byte)251:
				return(DEBUG_VECT.parse(pkt));
			case (byte)252:
				return(NAMED_VALUE_FLOAT.parse(pkt));
			case (byte)253:
				return(NAMED_VALUE_INT.parse(pkt));
			case (byte)254:
				return(STATUSTEXT.parse(pkt));
			case (byte)255:
				return(DEBUG.parse(pkt));
			case (byte)150:
				return(SENSOR_OFFSETS.parse(pkt));
			case (byte)151:
				return(SET_MAG_OFFSETS.parse(pkt));
			case (byte)152:
				return(MEMINFO.parse(pkt));
			case (byte)153:
				return(AP_ADC.parse(pkt));
			case (byte)154:
				return(DIGICAM_CONFIGURE.parse(pkt));
			case (byte)155:
				return(DIGICAM_CONTROL.parse(pkt));
			case (byte)156:
				return(MOUNT_CONFIGURE.parse(pkt));
			case (byte)157:
				return(MOUNT_CONTROL.parse(pkt));
			case (byte)158:
				return(MOUNT_STATUS.parse(pkt));
			case (byte)160:
				return(FENCE_POINT.parse(pkt));
			case (byte)161:
				return(FENCE_FETCH_POINT.parse(pkt));
			case (byte)162:
				return(FENCE_STATUS.parse(pkt));
			default:
				return(false);
		}
	}

	public static void initMessages()
	{
		PING.target_system = 1;
		PING.target_component = 1;
		CHANGE_OPERATOR_CONTROL.target_system = 1;
		ACTION.target_component = 1;
		PARAM_REQUEST_READ.target_system = 1;
		PARAM_REQUEST_READ.target_component = 1;
		PARAM_REQUEST_LIST.target_system = 1;
		PARAM_REQUEST_LIST.target_component = 1;
		PARAM_SET.target_system = 1;
		PARAM_SET.target_component = 1;
		WAYPOINT.target_system = 1;
		WAYPOINT.target_component = 1;
		WAYPOINT_REQUEST.target_system = 1;
		WAYPOINT_REQUEST.target_component = 1;
		WAYPOINT_SET_CURRENT.target_system = 1;
		WAYPOINT_SET_CURRENT.target_component = 1;
		WAYPOINT_REQUEST_LIST.target_system = 1;
		WAYPOINT_REQUEST_LIST.target_component = 1;
		WAYPOINT_COUNT.target_system = 1;
		WAYPOINT_COUNT.target_component = 1;
		WAYPOINT_CLEAR_ALL.target_system = 1;
		WAYPOINT_CLEAR_ALL.target_component = 1;
		WAYPOINT_ACK.target_system = 1;
		WAYPOINT_ACK.target_component = 1;
		GPS_SET_GLOBAL_ORIGIN.target_system = 1;
		GPS_SET_GLOBAL_ORIGIN.target_component = 1;
		LOCAL_POSITION_SETPOINT_SET.target_system = 1;
		LOCAL_POSITION_SETPOINT_SET.target_component = 1;
		SAFETY_SET_ALLOWED_AREA.target_system = 1;
		SAFETY_SET_ALLOWED_AREA.target_component = 1;
		SET_ROLL_PITCH_YAW_THRUST.target_system = 1;
		SET_ROLL_PITCH_YAW_THRUST.target_component = 1;
		SET_ROLL_PITCH_YAW_SPEED_THRUST.target_system = 1;
		SET_ROLL_PITCH_YAW_SPEED_THRUST.target_component = 1;
		REQUEST_DATA_STREAM.target_system = 1;
		REQUEST_DATA_STREAM.target_component = 1;
		RC_CHANNELS_OVERRIDE.target_system = 1;
		RC_CHANNELS_OVERRIDE.target_component = 1;
		COMMAND.target_system = 1;
		COMMAND.target_component = 1;
		SET_MAG_OFFSETS.target_system = 1;
		SET_MAG_OFFSETS.target_component = 1;
		DIGICAM_CONFIGURE.target_system = 1;
		DIGICAM_CONFIGURE.target_component = 1;
		DIGICAM_CONTROL.target_system = 1;
		DIGICAM_CONTROL.target_component = 1;
		MOUNT_CONFIGURE.target_system = 1;
		MOUNT_CONFIGURE.target_component = 1;
		MOUNT_CONTROL.target_system = 1;
		MOUNT_CONTROL.target_component = 1;
		MOUNT_STATUS.target_system = 1;
		MOUNT_STATUS.target_component = 1;
		FENCE_POINT.target_system = 1;
		FENCE_POINT.target_component = 1;
		FENCE_FETCH_POINT.target_system = 1;
		FENCE_FETCH_POINT.target_component = 1;
	}
/*
	ANALOG_RAW	%%%%~ analog channels raw (counts from ADC 0-1024)\n
		chan01			adc channel_01
		chan02			adc channel_02
		chan03			adc channel_03
		chan04			adc channel_04
		chan05			adc channel_05
		chan06			adc channel_06
		chan07			adc channel_07
		chan08			adc channel_08
		chan09			adc channel_09
		chan10			adc channel_10
		chan11			adc channel_11
		chan12			adc channel_12
		chan13			adc channel_13
		chan14			adc channel_14
		chan15			adc channel_15
		chan16			adc channel_16
		usec			time
	ANALOG_EU	%%%%~ analog channels EU (Raw counts converted to engineering units, e.g. feet, meters, \n%%%%~ Amps, degrees, etc)\n
		chan01			adc channel_01
		chan02			adc channel_02
		chan03			adc channel_03
		chan04			adc channel_04
		chan05			adc channel_05
		chan06			adc channel_06
		chan07			adc channel_07
		chan08			adc channel_08
		chan09			adc channel_09
		chan10			adc channel_10
		chan11			adc channel_11
		chan12			adc channel_12
		chan13			adc channel_13
		chan14			adc channel_14
		chan15			adc channel_15
		chan16			adc channel_16
		usec			time
	CDNR_CONTROLLER	%%%%~ Message from Conflict Detection and Resolution monitor to aircraft. This is a command \n%%%%~ to resolve conflicts and includes flags and values for heading,altitude,and \n%%%%~ speed changes as well as a max time duration\n
		h_flag				enables/disables new heading command
		s_flag				enables/disables new airspeed command
		a_flag				enables/disables new altitude command
		t_flag				enables/disables max time command
		new_heading			value for new heading
		new_airspeed		value for new airspeed
		new_altitude		value for new altitude
		max_time			maximum time to hold this command
	TRAFFIC_DATA	%%%%~ Message that provides 6 dof position data about other traffic. This includes: Vehicle \n%%%%~ number, latitud (deg), longitud (deg), altitud(meters), roll (deg), pitch \n%%%%~ (deg), heading(deg)\n
		num				vehicle number
		lat				latitude (deg)
		lon				longitude (deg)
		alt				altitude (meters)
		roll			roll (deg)
		pitch			pitch (deg)
		heading			heading (deg)
	STATE_DATA	%%%%~ Message that information about the state of the aircraft\n
		num				Vehicle number, e.g. 1 for R1, 2 for R2, 3 for R3
		atloiter		if 0, vehicle is not currently loitering, if 1, vehicle is loitering
		mode			mode: MANUAL=0, CIRCLE=1, STABILIZE=2, FLY_BY_WIRE_A=5, FLY_BY_WIRE_B=6, FLY_BY_WIRE_C=7, AUTO=10, RTL=11, LOITER=12, GUIDED=15, INITIALISING=16
		usec			time
	HEARTBEAT	%%%%~ The heartbeat message shows that a system is present and responding. The type of \n%%%%~ the MAV and Autopilot hardware allow the receiving system to treat further messages \n%%%%~ from this system appropriate (e.g. by laying out the user interface based on \n%%%%~ the autopilot).\n
		type					Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
		autopilot				Type of the Autopilot: 0: Generic, 1: PIXHAWK, 2: SLUGS, 3: Ardupilot (up to 15 types), defined in MAV_AUTOPILOT_TYPE ENUM
		mavlink_version			MAVLink version
	BOOT	%%%%~ The boot message indicates that a system is starting. The onboard software version \n%%%%~ allows to keep track of onboard soft/firmware revisions.\n
		version			The onboard software version
	SYSTEM_TIME	%%%%~ The system time is the time of the master clock, typically the computer clock of \n%%%%~ the main onboard computer.\n
		time_usec			Timestamp of the master clock in microseconds since UNIX epoch.
	PING	%%%%~ A ping message either requesting or responding to a ping. This allows to measure \n%%%%~ the system latencies, including serial port, radio modem and UDP connections.\n
		seq						PING sequence
		target_system			0: request ping from all receiving systems, if greater than 0: message is a ping response and number is the system id of the requesting system
		target_component		0: request ping from all receiving components, if greater than 0: message is a ping response and number is the system id of the requesting system
		time					Unix timestamp in microseconds
	SYSTEM_TIME_UTC	%%%%~ UTC date and time from GPS module\n
		utc_date		GPS UTC date ddmmyy
		utc_time		GPS UTC time hhmmss
	CHANGE_OPERATOR_CONTROL	%%%%~ Request to control this MAV\n
		target_system			System the GCS requests control for
		control_request			0: request control of this MAV, 1: Release control of this MAV
		version					0: key as plaintext, 1-255: future, different hashing/encryption variants. The GCS should in general use the safest mode possible initially and then gradually move down the encryption level if it gets a NACK message indicating an encryption mismatch.
		passkey					Password / Key, depending on version plaintext or encrypted. 25 or less characters, NULL terminated. The characters may involve A-Z, a-z, 0-9, and "!?,.-"
	CHANGE_OPERATOR_CONTROL_ACK	%%%%~ Accept / deny control of this MAV\n
		gcs_system_id			ID of the GCS this message 
		control_request			0: request control of this MAV, 1: Release control of this MAV
		ack						0: ACK, 1: NACK: Wrong passkey, 2: NACK: Unsupported passkey encryption method, 3: NACK: Already under control
	AUTH_KEY	%%%%~ Emit an encrypted signature / key identifying this system. PLEASE NOTE: This protocol \n%%%%~ has been kept simple, so transmitting the key requires an encrypted channel \n%%%%~ for true safety.\n
		key			key
	ACTION_ACK	%%%%~ This message acknowledges an action. IMPORTANT: The acknowledgement can be also \n%%%%~ negative, e.g. the MAV rejects a reset message because it is in-flight. The action \n%%%%~ ids are defined in ENUM MAV_ACTION in mavlink/include/mavlink_types.h\n
		action			The action id
		result			0: Action DENIED, 1: Action executed
	ACTION	%%%%~ An action message allows to execute a certain onboard action. These include liftoff, \n%%%%~ land, storing parameters too EEPROM, shutddown, etc. The action ids are defined \n%%%%~ in ENUM MAV_ACTION in mavlink/include/mavlink_types.h\n
		target					The system executing the action
		target_component		The component executing the action
		action					The action id
	SET_MODE	%%%%~ Set the system mode, as defined by enum MAV_MODE in mavlink/include/mavlink_types.h. \n%%%%~ There is no target component id as the mode is by definition for the overall \n%%%%~ aircraft, not only for one component.\n
		target			The system setting the mode
		mode			The new mode
	SET_NAV_MODE	%%%%~ Set the system navigation mode, as defined by enum MAV_NAV_MODE in mavlink/include/mavlink_types.h. \n%%%%~ The navigation mode applies to the whole aircraft and thus all \n%%%%~ components.\n
		target			The system setting the mode
		nav_mode		The new navigation mode
	PARAM_REQUEST_READ	%%%%~ Request to read the onboard parameter with the param_id string id. Onboard parameters \n%%%%~ are stored as key[const char*] -> value[float]. This allows to send a parameter \n%%%%~ to any other component (such as the GCS) without the need of previous knowledge \n%%%%~ of possible parameter names. Thus the same GCS can store different parameters \n%%%%~ for different autopilots. See also http://qgroundcontrol.org/parameter_interface \n%%%%~ for a full documentation of QGroundControl and IMU code.\n
		target_system			System ID
		target_component		Component ID
		param_id				Onboard parameter id
		param_index				Parameter index. Send -1 to use the param ID field as identifier
	PARAM_REQUEST_LIST	%%%%~ Request all parameters of this component. After his request, all parameters are \n%%%%~ emitted.\n
		target_system			System ID
		target_component		Component ID
	PARAM_VALUE	%%%%~ Emit the value of a onboard parameter. The inclusion of param_count and param_index \n%%%%~ in the message allows the recipient to keep track of received parameters and \n%%%%~ allows him to re-request missing parameters after a loss or timeout.\n
		param_id			Onboard parameter id
		param_value			Onboard parameter value
		param_count			Total number of onboard parameters
		param_index			Index of this onboard parameter
	PARAM_SET	%%%%~ Set a parameter value TEMPORARILY to RAM. It will be reset to default on system \n%%%%~ reboot. Send the ACTION MAV_ACTION_STORAGE_WRITE to PERMANENTLY write the RAM contents \n%%%%~ to EEPROM. IMPORTANT: The receiving component should acknowledge the new \n%%%%~ parameter value by sending a param_value message to all communication partners. \n%%%%~ This will also ensure that multiple GCS all have an up-to-date list of all parameters. \n%%%%~ If the sending GCS did not receive a PARAM_VALUE message within its timeout \n%%%%~ time, it should re-send the PARAM_SET message.\n
		target_system			System ID
		target_component		Component ID
		param_id				Onboard parameter id
		param_value				Onboard parameter value
	GPS_RAW_INT	%%%%~ The global position, as returned by the Global Positioning System (GPS). This is \n%%%%~ NOT the global position estimate of the sytem, but rather a RAW sensor value. See \n%%%%~ message GLOBAL_POSITION for the global position estimate. Coordinate frame is \n%%%%~ right-handed, Z-axis up (GPS frame)\n
		usec			Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		fix_type		0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
		lat				Latitude in 1E7 degrees
		lon				Longitude in 1E7 degrees
		alt				Altitude in 1E3 meters (millimeters)
		eph				GPS HDOP
		epv				GPS VDOP
		v				GPS ground speed (m/s)
		hdg				Compass heading in degrees, 0..360 degrees
	SCALED_IMU	%%%%~ The RAW IMU readings for the usual 9DOF sensor setup. This message should contain \n%%%%~ the scaled values to the described units\n
		usec			Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		xacc			X acceleration (mg)
		yacc			Y acceleration (mg)
		zacc			Z acceleration (mg)
		xgyro			Angular speed around X axis (millirad /sec)
		ygyro			Angular speed around Y axis (millirad /sec)
		zgyro			Angular speed around Z axis (millirad /sec)
		xmag			X Magnetic field (milli tesla)
		ymag			Y Magnetic field (milli tesla)
		zmag			Z Magnetic field (milli tesla)
	GPS_STATUS	%%%%~ The positioning status, as reported by GPS. This message is intended to display \n%%%%~ status information about each satellite visible to the receiver. See message GLOBAL_POSITION \n%%%%~ for the global position estimate. This message can contain information \n%%%%~ for up to 20 satellites.\n
		satellites_visible			Number of satellites visible
		satellite_prn				Global satellite ID
		satellite_used				0: Satellite not used, 1: used for localization
		satellite_elevation			Elevation (0: right on top of receiver, 90: on the horizon) of satellite
		satellite_azimuth			Direction of satellite, 0: 0 deg, 255: 360 deg.
		satellite_snr				Signal to noise ratio of satellite
	RAW_IMU	%%%%~ The RAW IMU readings for the usual 9DOF sensor setup. This message should always \n%%%%~ contain the true raw values without any scaling to allow data capture and system \n%%%%~ debugging.\n
		usec			Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		xacc			X acceleration (raw)
		yacc			Y acceleration (raw)
		zacc			Z acceleration (raw)
		xgyro			Angular speed around X axis (raw)
		ygyro			Angular speed around Y axis (raw)
		zgyro			Angular speed around Z axis (raw)
		xmag			X Magnetic field (raw)
		ymag			Y Magnetic field (raw)
		zmag			Z Magnetic field (raw)
	RAW_PRESSURE	%%%%~ The RAW pressure readings for the typical setup of one absolute pressure and one \n%%%%~ differential pressure sensor. The sensor values should be the raw, UNSCALED ADC \n%%%%~ values.\n
		usec				Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		press_abs			Absolute pressure (raw)
		press_diff1			Differential pressure 1 (raw)
		press_diff2			Differential pressure 2 (raw)
		temperature			Raw Temperature measurement (raw)
	SCALED_PRESSURE	%%%%~ The pressure readings for the typical setup of one absolute and differential pressure \n%%%%~ sensor. The units are as specified in each field.\n
		usec				Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		press_abs			Absolute pressure (hectopascal)
		press_diff			Differential pressure 1 (hectopascal)
		temperature			Temperature measurement (0.01 degrees celsius)
	ATTITUDE	%%%%~ The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right).\n
		usec				Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		roll				Roll angle (rad)
		pitch				Pitch angle (rad)
		yaw					Yaw angle (rad)
		rollspeed			Roll angular speed (rad/s)
		pitchspeed			Pitch angular speed (rad/s)
		yawspeed			Yaw angular speed (rad/s)
	LOCAL_POSITION	%%%%~ The filtered local position (e.g. fused computer vision and accelerometers). Coordinate \n%%%%~ frame is right-handed, Z-axis down (aeronautical frame)\n
		usec		Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		x			X Position
		y			Y Position
		z			Z Position
		vx			X Speed
		vy			Y Speed
		vz			Z Speed
	GLOBAL_POSITION	%%%%~ The filtered global position (e.g. fused GPS and accelerometers). Coordinate frame \n%%%%~ is right-handed, Z-axis up (GPS frame)\n
		usec		Timestamp (microseconds since unix epoch)
		lat			Latitude, in degrees
		lon			Longitude, in degrees
		alt			Absolute altitude, in meters
		vx			X Speed (in Latitude direction, positive: going north)
		vy			Y Speed (in Longitude direction, positive: going east)
		vz			Z Speed (in Altitude direction, positive: going up)
	GPS_RAW	%%%%~ The global position, as returned by the Global Positioning System (GPS). This is \n%%%%~ NOT the global position estimate of the sytem, but rather a RAW sensor value. See \n%%%%~ message GLOBAL_POSITION for the global position estimate. Coordinate frame is \n%%%%~ right-handed, Z-axis up (GPS frame)\n
		usec			Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		fix_type		0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
		lat				Latitude in degrees
		lon				Longitude in degrees
		alt				Altitude in meters
		eph				GPS HDOP
		epv				GPS VDOP
		v				GPS ground speed
		hdg				Compass heading in degrees, 0..360 degrees
	SYS_STATUS	%%%%~ The general system state. If the system is following the MAVLink standard, the system \n%%%%~ state is mainly defined by three orthogonal states/modes: The system mode, \n%%%%~ which is either LOCKED (motors shut down and locked), MANUAL (system under RC control), \n%%%%~ GUIDED (system with autonomous position control, position setpoint controlled \n%%%%~ manually) or AUTO (system guided by path/waypoint planner). The NAV_MODE \n%%%%~ defined the current flight state: LIFTOFF (often an open-loop maneuver), LANDING, \n%%%%~ WAYPOINTS or VECTOR. This represents the internal navigation state machine. The \n%%%%~ system status shows wether the system is currently active or not and if an emergency \n%%%%~ occured. During the CRITICAL and EMERGENCY states the MAV is still considered \n%%%%~ to be active, but should start emergency procedures autonomously. After a failure \n%%%%~ occured it should first move from active to critical to allow manual intervention \n%%%%~ and then move to emergency after a certain timeout.\n
		mode						System mode, see MAV_MODE ENUM in mavlink/include/mavlink_types.h
		nav_mode					Navigation mode, see MAV_NAV_MODE ENUM
		status						System status flag, see MAV_STATUS ENUM
		load						Maximum usage in percent of the mainloop time, (0%%: 0, 100%%: 1000) should be always below 1000
		vbat						Battery voltage, in millivolts (1 = 1 millivolt)
		battery_remaining			Remaining battery energy: (0%%: 0, 100%%: 1000)
		packet_drop					Dropped packets (packets that were corrupted on reception on the MAV)
	RC_CHANNELS_RAW	%%%%~ The RAW values of the RC channels received. The standard PPM modulation is as follows: \n%%%%~ 1000 microseconds: 0%%, 2000 microseconds: 100%%. Individual receivers/transmitters \n%%%%~ might violate this specification.\n
		chan1_raw			RC channel 1 value, in microseconds
		chan2_raw			RC channel 2 value, in microseconds
		chan3_raw			RC channel 3 value, in microseconds
		chan4_raw			RC channel 4 value, in microseconds
		chan5_raw			RC channel 5 value, in microseconds
		chan6_raw			RC channel 6 value, in microseconds
		chan7_raw			RC channel 7 value, in microseconds
		chan8_raw			RC channel 8 value, in microseconds
		rssi				Receive signal strength indicator, 0: 0%%, 255: 100%%
	RC_CHANNELS_SCALED	%%%%~ The scaled values of the RC channels received. (-100%%) -10000, (0%%) 0, (100%%) \n%%%%~ 10000\n
		chan1_scaled		RC channel 1 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan2_scaled		RC channel 2 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan3_scaled		RC channel 3 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan4_scaled		RC channel 4 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan5_scaled		RC channel 5 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan6_scaled		RC channel 6 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan7_scaled		RC channel 7 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		chan8_scaled		RC channel 8 value scaled, (-100%%) -10000, (0%%) 0, (100%%) 10000
		rssi				Receive signal strength indicator, 0: 0%%, 255: 100%%
	SERVO_OUTPUT_RAW	%%%%~ The RAW values of the servo outputs (for RC input from the remote, use the RC_CHANNELS \n%%%%~ messages). The standard PPM modulation is as follows: 1000 microseconds: \n%%%%~ 0%%, 2000 microseconds: 100%%.\n
		servo1_raw			Servo output 1 value, in microseconds
		servo2_raw			Servo output 2 value, in microseconds
		servo3_raw			Servo output 3 value, in microseconds
		servo4_raw			Servo output 4 value, in microseconds
		servo5_raw			Servo output 5 value, in microseconds
		servo6_raw			Servo output 6 value, in microseconds
		servo7_raw			Servo output 7 value, in microseconds
		servo8_raw			Servo output 8 value, in microseconds
	WAYPOINT	%%%%~ Message encoding a waypoint. This message is emitted to announce      the presence \n%%%%~ of a waypoint and to set a waypoint on the system. The waypoint can be either \n%%%%~ in x, y, z meters (type: LOCAL) or x:lat, y:lon, z:altitude. Local frame is Z-down, \n%%%%~ right handed, global frame is Z-up, right handed\n
		target_system			System ID
		target_component		Component ID
		seq						Sequence
		frame					The coordinate system of the waypoint. see MAV_FRAME in mavlink_types.h
		command					The scheduled action for the waypoint. see MAV_COMMAND in common.xml MAVLink specs
		current					false:0, true:1
		autocontinue			autocontinue to next wp
		param1					PARAM1 / For NAV command waypoints: Radius in which the waypoint is accepted as reached, in meters
		param2					PARAM2 / For NAV command waypoints: Time that the MAV should stay inside the PARAM1 radius before advancing, in milliseconds
		param3					PARAM3 / For LOITER command waypoints: Orbit to circle around the waypoint, in meters. If positive the orbit direction should be clockwise, if negative the orbit direction should be counter-clockwise.
		param4					PARAM4 / For NAV and LOITER command waypoints: Yaw orientation in degrees, [0..360] 0 = NORTH
		x						PARAM5 / local: x position, global: latitude
		y						PARAM6 / y position: global: longitude
		z						PARAM7 / z position: global: altitude
	WAYPOINT_REQUEST	%%%%~ Request the information of the waypoint with the sequence number seq. The response \n%%%%~ of the system to this message should be a WAYPOINT message.\n
		target_system			System ID
		target_component		Component ID
		seq						Sequence
	WAYPOINT_SET_CURRENT	%%%%~ Set the waypoint with sequence number seq as current waypoint. This means that the \n%%%%~ MAV will continue to this waypoint on the shortest path (not following the waypoints \n%%%%~ in-between).\n
		target_system			System ID
		target_component		Component ID
		seq						Sequence
	WAYPOINT_CURRENT	%%%%~ Message that announces the sequence number of the current active waypoint. The MAV \n%%%%~ will fly towards this waypoint.\n
		seq			Sequence
	WAYPOINT_REQUEST_LIST	%%%%~ Request the overall list of waypoints from the system/component.\n
		target_system			System ID
		target_component		Component ID
	WAYPOINT_COUNT	%%%%~ This message is emitted as response to WAYPOINT_REQUEST_LIST by the MAV. The GCS \n%%%%~ can then request the individual waypoints based on the knowledge of the total number \n%%%%~ of waypoints.\n
		target_system			System ID
		target_component		Component ID
		count					Number of Waypoints in the Sequence
	WAYPOINT_CLEAR_ALL	%%%%~ Delete all waypoints at once.\n
		target_system			System ID
		target_component		Component ID
	WAYPOINT_REACHED	%%%%~ A certain waypoint has been reached. The system will either hold this position (or \n%%%%~ circle on the orbit) or (if the autocontinue on the WP was set) continue to the \n%%%%~ next waypoint.\n
		seq			Sequence
	WAYPOINT_ACK	%%%%~ Ack message during waypoint handling. The type field states if this message is a \n%%%%~ positive ack (type=0) or if an error happened (type=non-zero).\n
		target_system			System ID
		target_component		Component ID
		type					0: OK, 1: Error
	GPS_SET_GLOBAL_ORIGIN	%%%%~ As local waypoints exist, the global waypoint reference allows to transform between \n%%%%~ the local coordinate frame and the global (GPS) coordinate frame. This can be \n%%%%~ necessary when e.g. in- and outdoor settings are connected and the MAV should \n%%%%~ move from in- to outdoor.\n
		target_system			System ID
		target_component		Component ID
		latitude				global position * 1E7
		longitude				global position * 1E7
		altitude				global position * 1000
	GPS_LOCAL_ORIGIN_SET	%%%%~ Once the MAV sets a new GPS-Local correspondence, this message announces the origin \n%%%%~ (0,0,0) position\n
		latitude			Latitude (WGS84), expressed as * 1E7
		longitude			Longitude (WGS84), expressed as * 1E7
		altitude			Altitude(WGS84), expressed as * 1000
	LOCAL_POSITION_SETPOINT_SET	%%%%~ Set the setpoint for a local position controller. This is the position in local \n%%%%~ coordinates the MAV should fly to. This message is sent by the path/waypoint planner \n%%%%~ to the onboard position controller. As some MAVs have a degree of freedom in \n%%%%~ yaw (e.g. all helicopters/quadrotors), the desired yaw angle is part of the message.\n
		target_system			System ID
		target_component		Component ID
		x						x position
		y						y position
		z						z position
		yaw						Desired yaw angle
	LOCAL_POSITION_SETPOINT	%%%%~ Transmit the current local setpoint of the controller to other MAVs (collision avoidance) \n%%%%~ and to the GCS.\n
		x			x position
		y			y position
		z			z position
		yaw			Desired yaw angle
	CONTROL_STATUS	% No Description provided\n
		position_fix			Position fix: 0: lost, 2: 2D position fix, 3: 3D position fix 
		vision_fix				Vision position fix: 0: lost, 1: 2D local position hold, 2: 2D global position fix, 3: 3D global position fix 
		gps_fix					GPS position fix: 0: no reception, 1: Minimum 1 satellite, but no position fix, 2: 2D position fix, 3: 3D position fix 
		ahrs_health				Attitude estimation health: 0: poor, 255: excellent
		control_att				0: Attitude control disabled, 1: enabled
		control_pos_xy			0: X, Y position control disabled, 1: enabled
		control_pos_z			0: Z position control disabled, 1: enabled
		control_pos_yaw			0: Yaw angle control disabled, 1: enabled
	SAFETY_SET_ALLOWED_AREA	%%%%~ Set a safety zone (volume), which is defined by two corners of a cube. This message \n%%%%~ can be used to tell the MAV which setpoints/waypoints to accept and which to \n%%%%~ reject. Safety areas are often enforced by national or competition regulations.\n
		target_system			System ID
		target_component		Component ID
		frame					Coordinate frame, as defined by MAV_FRAME enum in mavlink_types.h. Can be either global, GPS, right-handed with Z axis up or local, right handed, Z axis down.
		p1x						x position 1 / Latitude 1
		p1y						y position 1 / Longitude 1
		p1z						z position 1 / Altitude 1
		p2x						x position 2 / Latitude 2
		p2y						y position 2 / Longitude 2
		p2z						z position 2 / Altitude 2
	SAFETY_ALLOWED_AREA	%%%%~ Read out the safety zone the MAV currently assumes.\n
		frame			Coordinate frame, as defined by MAV_FRAME enum in mavlink_types.h. Can be either global, GPS, right-handed with Z axis up or local, right handed, Z axis down.
		p1x				x position 1 / Latitude 1
		p1y				y position 1 / Longitude 1
		p1z				z position 1 / Altitude 1
		p2x				x position 2 / Latitude 2
		p2y				y position 2 / Longitude 2
		p2z				z position 2 / Altitude 2
	SET_ROLL_PITCH_YAW_THRUST	%%%%~ Set roll, pitch and yaw.\n
		target_system			System ID
		target_component		Component ID
		roll					Desired roll angle in radians
		pitch					Desired pitch angle in radians
		yaw						Desired yaw angle in radians
		thrust					Collective thrust, normalized to 0 .. 1
	SET_ROLL_PITCH_YAW_SPEED_THRUST	%%%%~ Set roll, pitch and yaw.\n
		target_system			System ID
		target_component		Component ID
		roll_speed				Desired roll angular speed in rad/s
		pitch_speed				Desired pitch angular speed in rad/s
		yaw_speed				Desired yaw angular speed in rad/s
		thrust					Collective thrust, normalized to 0 .. 1
	ROLL_PITCH_YAW_THRUST_SETPOINT	%%%%~ Setpoint in roll, pitch, yaw currently active on the system.\n
		time_us			Timestamp in micro seconds since unix epoch
		roll			Desired roll angle in radians
		pitch			Desired pitch angle in radians
		yaw				Desired yaw angle in radians
		thrust			Collective thrust, normalized to 0 .. 1
	ROLL_PITCH_YAW_SPEED_THRUST_SETPOINT	%%%%~ Setpoint in rollspeed, pitchspeed, yawspeed currently active on the system.\n
		time_us				Timestamp in micro seconds since unix epoch
		roll_speed			Desired roll angular speed in rad/s
		pitch_speed			Desired pitch angular speed in rad/s
		yaw_speed			Desired yaw angular speed in rad/s
		thrust				Collective thrust, normalized to 0 .. 1
	NAV_CONTROLLER_OUTPUT	%%%%~ Outputs of the APM navigation controller. The primary use of this message is to \n%%%%~ check the response and signs of the controller before actual flight and to assist \n%%%%~ with tuning controller parameters \n
		nav_roll				Current desired roll in degrees
		nav_pitch				Current desired pitch in degrees
		nav_bearing				Current desired heading in degrees
		target_bearing			Bearing to current waypoint/target in degrees
		wp_dist					Distance to active waypoint in meters
		alt_error				Current altitude error in meters
		aspd_error				Current airspeed error in meters/second
		xtrack_error			Current crosstrack error on x-y plane in meters
	POSITION_TARGET	%%%%~ The goal position of the system. This position is the input to any navigation or \n%%%%~ path planning algorithm and does NOT represent the current controller setpoint.\n
		x			x position
		y			y position
		z			z position
		yaw			yaw orientation in radians, 0 = NORTH
	STATE_CORRECTION	%%%%~ Corrects the systems state by adding an error correction term to the position and \n%%%%~ velocity, and by rotating the attitude by a correction angle.\n
		xErr			x position error
		yErr			y position error
		zErr			z position error
		rollErr			roll error (radians)
		pitchErr		pitch error (radians)
		yawErr			yaw error (radians)
		vxErr			x velocity
		vyErr			y velocity
		vzErr			z velocity
	SET_ALTITUDE	% No Description provided\n
		target			The system setting the altitude
		mode			The new altitude in meters
	REQUEST_DATA_STREAM	% No Description provided\n
		target_system			The target requested to send the message stream.
		target_component		The target requested to send the message stream.
		req_stream_id			The ID of the requested message type
		req_message_rate		Update rate in Hertz
		start_stop				1 to start sending, 0 to stop sending.
	HIL_STATE	%%%%~ This packet is useful for high throughput                 applications such as hardware \n%%%%~ in the loop simulations.             \n
		usec				Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		roll				Roll angle (rad)
		pitch				Pitch angle (rad)
		yaw					Yaw angle (rad)
		rollspeed			Roll angular speed (rad/s)
		pitchspeed			Pitch angular speed (rad/s)
		yawspeed			Yaw angular speed (rad/s)
		lat					Latitude, expressed as * 1E7
		lon					Longitude, expressed as * 1E7
		alt					Altitude in meters, expressed as * 1000 (millimeters)
		vx					Ground X Speed (Latitude), expressed as m/s * 100
		vy					Ground Y Speed (Longitude), expressed as m/s * 100
		vz					Ground Z Speed (Altitude), expressed as m/s * 100
		xacc				X acceleration (mg)
		yacc				Y acceleration (mg)
		zacc				Z acceleration (mg)
	HIL_CONTROLS	%%%%~ Hardware in the loop control outputs\n
		time_us					Timestamp (microseconds since UNIX epoch or microseconds since system boot)
		roll_ailerons			Control output -3 .. 1
		pitch_elevator			Control output -1 .. 1
		yaw_rudder				Control output -1 .. 1
		throttle				Throttle 0 .. 1
		mode					System mode (MAV_MODE)
		nav_mode				Navigation mode (MAV_NAV_MODE)
	MANUAL_CONTROL	% No Description provided\n
		target					The system to be controlled
		roll					roll
		pitch					pitch
		yaw						yaw
		thrust					thrust
		roll_manual				roll control enabled auto:0, manual:1
		pitch_manual			pitch auto:0, manual:1
		yaw_manual				yaw auto:0, manual:1
		thrust_manual			thrust auto:0, manual:1
	RC_CHANNELS_OVERRIDE	%%%%~ The RAW values of the RC channels sent to the MAV to override info received from \n%%%%~ the RC radio. A value of -1 means no change to that channel. A value of 0 means \n%%%%~ control of that channel should be released back to the RC radio. The standard PPM \n%%%%~ modulation is as follows: 1000 microseconds: 0%%, 2000 microseconds: 100%%. Individual \n%%%%~ receivers/transmitters might violate this specification.\n
		target_system			System ID
		target_component		Component ID
		chan1_raw				RC channel 1 value, in microseconds
		chan2_raw				RC channel 2 value, in microseconds
		chan3_raw				RC channel 3 value, in microseconds
		chan4_raw				RC channel 4 value, in microseconds
		chan5_raw				RC channel 5 value, in microseconds
		chan6_raw				RC channel 6 value, in microseconds
		chan7_raw				RC channel 7 value, in microseconds
		chan8_raw				RC channel 8 value, in microseconds
	GLOBAL_POSITION_INT	%%%%~ The filtered global position (e.g. fused GPS and accelerometers). The position is \n%%%%~ in GPS-frame (right-handed, Z-up)\n
		lat			Latitude, expressed as * 1E7
		lon			Longitude, expressed as * 1E7
		alt			Altitude in meters, expressed as * 1000 (millimeters)
		vx			Ground X Speed (Latitude), expressed as m/s * 100
		vy			Ground Y Speed (Longitude), expressed as m/s * 100
		vz			Ground Z Speed (Altitude), expressed as m/s * 100
	VFR_HUD	%%%%~ Metrics typically displayed on a HUD for fixed wing aircraft\n
		airspeed			Current airspeed in m/s
		groundspeed			Current ground speed in m/s
		heading				Current heading in degrees, in compass units (0..360, 0=north)
		throttle			Current throttle setting in integer percent, 0 to 100
		alt					Current altitude (MSL), in meters
		climb				Current climb rate in meters/second
	COMMAND	%%%%~ Send a command with up to four parameters to the MAV\n
		target_system			System which should execute the command
		target_component		Component which should execute the command, 0 for all components
		command					Command ID, as defined by MAV_CMD enum.
		confirmation			0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
		param1					Parameter 1, as defined by MAV_CMD enum.
		param2					Parameter 2, as defined by MAV_CMD enum.
		param3					Parameter 3, as defined by MAV_CMD enum.
		param4					Parameter 4, as defined by MAV_CMD enum.
	COMMAND_ACK	%%%%~ Report status of a command. Includes feedback wether the command was executed\n
		command			Current airspeed in m/s
		result			1: Action ACCEPTED and EXECUTED, 1: Action TEMPORARY REJECTED/DENIED, 2: Action PERMANENTLY DENIED, 3: Action UNKNOWN/UNSUPPORTED, 4: Requesting CONFIRMATION
	OPTICAL_FLOW	%%%%~ Optical flow from a flow sensor (e.g. optical mouse sensor)\n
		time					Timestamp (UNIX)
		sensor_id				Sensor ID
		flow_x					Flow in pixels in x-sensor direction
		flow_y					Flow in pixels in y-sensor direction
		quality					Optical flow quality / confidence. 0: bad, 255: maximum quality
		ground_distance			Ground distance in meters
	OBJECT_DETECTION_EVENT	%%%%~ Object has been detected\n
		time				Timestamp in milliseconds since system boot
		object_id			Object ID
		type				Object type: 0: image, 1: letter, 2: ground vehicle, 3: air vehicle, 4: surface vehicle, 5: sub-surface vehicle, 6: human, 7: animal
		name				Name of the object as defined by the detector
		quality				Detection quality / confidence. 0: bad, 255: maximum confidence
		bearing				Angle of the object with respect to the body frame in NED coordinates in radians. 0: front
		distance			Ground distance in meters
	DEBUG_VECT	% No Description provided\n
		name		Name
		usec		Timestamp
		x			x
		y			y
		z			z
	NAMED_VALUE_FLOAT	%%%%~ Send a key-value pair as float. The use of this message is discouraged for normal \n%%%%~ packets, but a quite efficient way for testing new messages and getting experimental \n%%%%~ debug output.\n
		name			Name of the debug variable
		value			Floating point value
	NAMED_VALUE_INT	%%%%~ Send a key-value pair as integer. The use of this message is discouraged for normal \n%%%%~ packets, but a quite efficient way for testing new messages and getting experimental \n%%%%~ debug output.\n
		name			Name of the debug variable
		value			Signed integer value
	STATUSTEXT	%%%%~ Status text message. These messages are printed in yellow in the COMM console of \n%%%%~ QGroundControl. WARNING: They consume quite some bandwidth, so use only for important \n%%%%~ status and error messages. If implemented wisely, these messages are buffered \n%%%%~ on the MCU and sent only at a limited rate (e.g. 10 Hz).\n
		severity		Severity of status, 0 = info message, 255 = critical fault
		text			Status text message, without null termination character
	DEBUG	%%%%~ Send a debug value. The index is used to discriminate between values. These values \n%%%%~ show up in the plot of QGroundControl as DEBUG N.\n
		ind				index of debug variable
		value			DEBUG value
	SENSOR_OFFSETS	%%%%~ Offsets and calibrations values for hardware         sensors. This makes it easier \n%%%%~ to debug the calibration process.\n
		mag_ofs_x				magnetometer X offset
		mag_ofs_y				magnetometer Y offset
		mag_ofs_z				magnetometer Z offset
		mag_declination			magnetic declination (radians)
		raw_press				raw pressure from barometer
		raw_temp				raw temperature from barometer
		gyro_cal_x				gyro X calibration
		gyro_cal_y				gyro Y calibration
		gyro_cal_z				gyro Z calibration
		accel_cal_x				accel X calibration
		accel_cal_y				accel Y calibration
		accel_cal_z				accel Z calibration
	SET_MAG_OFFSETS	%%%%~ set the magnetometer offsets\n
		target_system			System ID
		target_component		Component ID
		mag_ofs_x				magnetometer X offset
		mag_ofs_y				magnetometer Y offset
		mag_ofs_z				magnetometer Z offset
	MEMINFO	%%%%~ state of APM memory\n
		brkval			heap top
		freemem			free memory
	AP_ADC	%%%%~ raw ADC output\n
		adc1		ADC output 1
		adc2		ADC output 2
		adc3		ADC output 3
		adc4		ADC output 4
		adc5		ADC output 5
		adc6		ADC output 6
	DIGICAM_CONFIGURE	%%%%~ Configure on-board Camera Control System.\n
		target_system			System ID
		target_component		Component ID
		mode					Mode enumeration from 1 to N //P, TV, AV, M, Etc (0 means ignore)
		shutter_speed			Divisor number //e.g. 1000 means 1/1000 (0 means ignore)
		aperture				F stop number x 10 //e.g. 28 means 2.8 (0 means ignore)
		iso						ISO enumeration from 1 to N //e.g. 80, 100, 200, Etc (0 means ignore)
		exposure_type			Exposure type enumeration from 1 to N (0 means ignore)
		command_id				Command Identity (incremental loop: 0 to 255)//A command sent multiple times will be executed or pooled just once
		engine_cut_off			Main engine cut-off time before camera trigger in seconds/10 (0 means no cut-off)
		extra_param				Extra parameters enumeration (0 means ignore)
		extra_value				Correspondent value to given extra_param
	DIGICAM_CONTROL	%%%%~ Control on-board Camera Control System to take shots.\n
		target_system			System ID
		target_component		Component ID
		session					0: stop, 1: start or keep it up //Session control e.g. show/hide lens
		zoom_pos				1 to N //Zoom's absolute position (0 means ignore)
		zoom_step				-100 to 100 //Zooming step value to offset zoom from the current position
		focus_lock				0: unlock focus or keep unlocked, 1: lock focus or keep locked, 3: re-lock focus
		shot					0: ignore, 1: shot or start filming
		command_id				Command Identity (incremental loop: 0 to 255)//A command sent multiple times will be executed or pooled just once
		extra_param				Extra parameters enumeration (0 means ignore)
		extra_value				Correspondent value to given extra_param
	MOUNT_CONFIGURE	%%%%~ Message to configure a camera mount, directional antenna, etc.\n
		target_system			System ID
		target_component		Component ID
		mount_mode				mount operating mode (see MAV_MOUNT_MODE enum)
		stab_roll				(1 = yes, 0 = no)
		stab_pitch				(1 = yes, 0 = no)
		stab_yaw				(1 = yes, 0 = no)
	MOUNT_CONTROL	%%%%~ Message to control a camera mount, directional antenna, etc.\n
		target_system			System ID
		target_component		Component ID
		input_a					pitch(deg*100) or lat, depending on mount mode
		input_b					roll(deg*100) or lon depending on mount mode
		input_c					yaw(deg*100) or alt (in cm) depending on mount mode
		save_position			if "1" it will save current trimmed position on EEPROM (just valid for NEUTRAL and LANDING)
	MOUNT_STATUS	%%%%~ Message with some status from APM to GCS about camera or antenna mount\n
		target_system			System ID
		target_component		Component ID
		pointing_a				pitch(deg*100) or lat, depending on mount mode
		pointing_b				roll(deg*100) or lon depending on mount mode
		pointing_c				yaw(deg*100) or alt (in cm) depending on mount mode
	FENCE_POINT	%%%%~ A fence point. Used to set a point when from        GCS -> MAV. Also used to return \n%%%%~ a point from MAV -> GCS\n
		target_system			System ID
		target_component		Component ID
		idx						point index (first point is 1, 0 is for return point)
		count					total number of points (for sanity checking)
		lat						Latitude of point
		lng						Longitude of point
	FENCE_FETCH_POINT	%%%%~ Request a current fence point from MAV\n
		target_system			System ID
		target_component		Component ID
		idx						point index (first point is 1, 0 is for return point)
	FENCE_STATUS	%%%%~ Status of geo-fencing. Sent in extended      status stream when fencing enabled\n
		breach_status			0 if currently inside fence, 1 if outside
		breach_count			number of fence breaches
		breach_type				last breach type (see FENCE_BREACH_* enum)
		breach_time				time of last breach in milliseconds since boot
*/
}
