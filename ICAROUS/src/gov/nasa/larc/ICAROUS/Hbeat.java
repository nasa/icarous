/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
///**
// * 
// */
//package gov.nasa.larc.ICAROUS;
//
//import gov.nasa.larc.AP.AP;
//import gov.nasa.larc.AP.AP.APModeEnum;
//import gov.nasa.larc.ICAROUS.Alert.AlertEnum;
//
//import java.nio.ByteBuffer;
//
//
//
///**
// * @author cquach
// *
// */
//public class Hbeat extends Codec
//{
//
////=========================================================================================================//
//// DATA Section
////=========================================================================================================//
//	public static final byte HBmsg = (byte)2;		// HeartBeat message between Edge and GndWatch for Synch with time steeping simulated traffic
//
//	/**
//	  <field type="int8_t[8]" name="acid"> 8 char Aircraft call sign</field> 
//	  <field type="float" name="time">local system time</field> 
//	  <field type="float" name="scenarioTime">local GlobalState time (scenario time)</field> 
//	  <field type="uint8_t" name="wpIdx">waypoint index if HB is from aircraft, not used if HB is from GndWatch</field> 
//	  <field type="uint8_t" name="control">enum {acManual:0, acAuto}</field>
//	  <field type="uint8_t" name="state">enum{acGnd:0, acTakeoff, acGoingToLoiter, acAtLoiter, acScenario, acReload, acHoming, simStandby, simReady, simScenario, simPaused}</field> 
//	  <field type="uint8_t" name="alert">enum {acNone:0, acTraffic, acBHM, acCom, acSW, acMotorTemp, acMotorCurrent, acGeofenceViolation, acNoGPS}</field>
//	  <field type="uint8_t" name="alertTypeCnt">number of times the given alert has occured
//	  <field type="uint8_t" name="alertTotalCnt">total count of all alerts
//	  <field type="uint8_t" name="seq">sequence number of heartbeat message
//
//	*/
//	
//	/*---- valid values for state enumerations and their designated meaning -----*/
//	// AC_Gnd = AC on the ground - either in maintenance area or at flight line.
//	// AC_TakeOff = RPM is up, AC rolling down run way or air born climbing
//	// AC_GoingToStartGate = flying in manual mode or auto mode
//	// AC_AtStartGate = AP signals that its has reached the loiter point
//	// AC_Scenario = Airwatch is engaged in scenario with GndWatch
//	// AC_Reload, AC_Homing, 
//	// SIM_Standby = Simulator running but no scenario loaded
//	// SIM_Ready = Simulator running AND scenario loaded
//	// SIM_Scenario = simulator time stepping traffic
//	// SIM_Paused = simulator paused. 
//	// SIM_Down = no heart beat from simulator.  state unknown
//	public enum OpState { AC_Gnd, AC_TakeOff, AC_GoingToStartGate, AC_AtStartGate, AC_ReadyToStart, AC_Scenario, AC_Reload, AC_Homing, SIM_Standby, SIM_Ready, SIM_Scenario, SIM_Paused, SIM_Down };
//
//	
//	/*---- data in a heartbeat message -----*/
//	public String acid;
//	String actype;
//	public static long hbTime = 0;
//	public static double scenarioTime;
//	public int wpidx;
//	public byte controlMode;
//	public byte state;
//	public byte alert;			// Alert name
//	public byte alertTypeCnt;	// number of times "Alert name" has occurred
//	public byte alertTotalCnt;	// total count of all alerts from the vehicle
//	public byte seq;			// sequence number of HB message
//	public byte resolutionType;	//Type of vectoring avoidance being performed. Heading, Speed, or Altitude
//	public short messagesLostAir;
//	public short packetsReceivedAir;
//	
//	
//	/*---- used for encode/decode functions -----*/
////	int msgBufSize = 1000;
//	//ByteBuffer buff = ByteBuffer.allocate(msgBufSize);
////	byte[] placeHolder = new byte[headerSize+idFixedLimit];	// make room for headerSize byte message header
//
////=========================================================================================================//
//// METHOD Section
////=========================================================================================================//
//	
//	//=====================================================================================//
//	/**
//	 * Heart beat message constructor.
//	 * @param name name of airplane to encode (limit to 8 characters
//	 * @return
//	 */
//	//=====================================================================================//
//	public Hbeat(String acName, String acTypeName) {
//		acid = acName;
//		actype = acTypeName;
//		wpidx = -1;
//		controlMode = AP.toAPModeByte(APModeEnum.MANUAL);
//		state = ToOpStateByte(OpState.AC_Gnd);
//		alert = Alert.toByte(AlertEnum.None);
//		resolutionType = (byte)0x00;
//		messagesLostAir = 0;
//		packetsReceivedAir = 0;
//		
//	}
//
//	//=====================================================================================//
//	/**
//	 * Encode new heart beat message.  Follow XML HeartBeat message definition
//	 * 		- <message id="2" name="HeartBeat">
//				  <description>This is the heartbeat message sent from the flying unit in the test scenario
//				  </description> 
//				  <field type="int8_t[8]" name="acid"> 8 char Aircraft call sign</field> 
//				  <field type="float" name="time">local system time</field> 
//				  <field type="float" name="scenarioTime">local GlobalState time (scenario time)</field> 
//				  <field type="uint8_t" name="wpIdx">waypoint index if HB is from aircraft, not used if HB is from GndWatch</field> 
//				  <field type="uint8_t" name="control">enum {acManual:0, acAuto}</field>
//				  <field type="uint8_t" name="state">enum{acGnd:0, acTakeoff, acGoingToLoiter, acAtLoiter, acScenario, acReload, acHoming, simStandby, simReady, simScenario, simPaused}</field> 
//				  <field type="uint8_t" name="alert">enum {acNone:0, acTraffic, acBHM, acCom, acSW, acMotorTemp, acMotorCurrent, acGeofenceViolation, acNoGPS}</field>
//				  <field type="uint8_t" name="alertTypeCnt">number of times the given alert has occured
//				  <field type="uint8_t" name="alertTotalCnt">total count of all alerts
//				  <field type="uint8_t" name="seq">sequence number of heartbeat message
//		  </message>
//	 * @param name name of airplane to encode (limit to 8 characters
//	 * @return
//	 */
//	//=====================================================================================//
//	public byte[] encode (  ) 
//	{
//			return encode (  acid
//							, actype
//							, System.currentTimeMillis()
//							, Icarous.getSimTime()
//							, wpidx
//							, controlMode
//							, state
//							, alert
//							, alertTypeCnt	// number of times "Alert name" has occurred
//							, alertTotalCnt	// total count of all alerts from the vehicle
//							, seq			// HB message sequence number
//							,resolutionType //Type of resolution
//							,messagesLostAir
//							,packetsReceivedAir
//					) ;
//	}
//
//	public byte[] encode (  String id
//							, String ACtype
//							, long sysTime
//							, double simTime
//							, int   currentWP
//							, byte  control
//							, byte state
//							, byte alert
//							, byte alertTypeCnt	// number of times "Alert name" has occurred
//							, byte alertTotalCnt	// total count of all alerts from the vehicle
//							, byte seq
//							, byte resolutionType
//							,short packetsLostAir
//							,short packetsReceivedAir
//						) 
//	{
//	
//
//		if (!buf.hasArray())  
//			buf = ByteBuffer.allocate(msgBufSize);
//			
//		buf.clear();
//		buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//		
//		
//		buf.put(placeHolder).putLong(sysTime).putDouble(simTime).putInt(currentWP).put(control).put(state)
//		   .put(alert).put(alertTypeCnt).put(alertTotalCnt).put(seq).put(resolutionType).putShort(packetsLostAir);
//
//		byte[] packet = encodePacket(id, ACtype, HBmsg);
//		
//
//		System.out.println(Thread.currentThread().getName()
//							+ "-->Hbeat::encode: "
//							+ "AC["+id+"] "
//							+ "Seq["+seq+"] "
//							+ "hbTime["+sysTime+"] "
//							+ "simTime["+simTime+"] "
//							+ "currentWP["+currentWP+"] "
//							+ "controlMode["+AP.getModeText(controlMode)+"] "
//							+ "state["+ToOpText(state)+"] "
//							+ "alert["+Alert.toText(alert)+"] "
//							+ "alertTypeCount["+alertTypeCnt+"] "
//							+ "alertTotalCnt["+alertTotalCnt+"] "
//							+ "seq["+seq+"] "
//							+ "out msg len[" + packet.length + "] "
//							+ "out bytes["+IcarousCodec.bytesToInt(packet)+"] "
//							);
//		return packet;	
//	
//	}	// end encodeHBpacket function
//
//	//=====================================================================================//
//	/**
//	 * Decodes a heartbeat packet and put data into class variables.
//	 * @param packet
//	 */
//	//=====================================================================================//
//	public void decode (byte[] packet) 
//	{
//		
////		System.out.println (Thread.currentThread().getName()
////							+ "-->HBeat::decode: "
////							+ "got [" + packet.length + "] byte msg "
////							+ "raw msg[" + IcarousCodec.bytesToInt(packet) + "] "
////							);
//		//if(isValidChkSum(packet)) //Moved to be done before calling decode in IcarousListener::smp()
//		//	{
//			buf.clear();
//	
//			int s = IcarousCodec.headerSize;
//	
//			acid = new String(packet,s,IcarousCodec.idFixedLimit-1);		// extract calsign
//			acid = this.stripTrailing0s(acid);
//			
//			s += IcarousCodec.idFixedLimit;
//			buf = ByteBuffer.wrap( IcarousCodec.byteCopy(packet, s, IcarousCodec.llen) );
//			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// standardize on little endian!
//			hbTime   = buf.getLong();
//	
//			s += IcarousCodec.llen;
//			buf = ByteBuffer.wrap( IcarousCodec.byteCopy(packet, s, IcarousCodec.llen) );
//			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// standardize on little endian!
//			scenarioTime   = buf.getDouble();
//	
//			s += IcarousCodec.llen;
//			buf = ByteBuffer.wrap( IcarousCodec.byteCopy( packet, s, IcarousCodec.ilen) );
//			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// standardize on little endian!
//			this.wpidx = buf.getInt();
//	
//			s += IcarousCodec.ilen;
//			buf = ByteBuffer.wrap( IcarousCodec.byteCopy( packet, s, IcarousCodec.ilen) );
//			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// standardize on little endian!
//			this.controlMode = buf.get();
//			//controlMode = packet[s];	// get one byte modes
//			
//			s++;
//			state = packet[s];	// get one byte state
//			
//			s += 1;
//			alert = packet[s];	// get one byte alert
//	
//			s += 1;
//			alertTypeCnt = packet[s];	// get one byte alertTypeCnt
//	
//			s += 1;
//			alertTotalCnt = packet[s];	// get one byte alertTotalCnt
//	
//			s += 1;
//			seq = packet[s];	// get one byte alertTotalCnt
//			
//			s += 1;
//			resolutionType = packet[s];
//			
//			s += 1;
//			buf = ByteBuffer.wrap( IcarousCodec.byteCopy( packet, s, IcarousCodec.slen) );
//			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// standardize on little endian!
//			messagesLostAir = buf.getShort();
//			
//			s += IcarousCodec.slen;
//			buf = ByteBuffer.wrap( IcarousCodec.byteCopy( packet, s, IcarousCodec.slen) );
//			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// standardize on little endian!
//			packetsReceivedAir = buf.getShort();
//	
//			//System.err.println("Edge-->Hbeat::decode() hbTime : "+ hbTime+ " System Time: "+ System.currentTimeMillis()+ " Scenario Time: "+ scenarioTime);
//			System.out.println (Thread.currentThread().getName()
//								+ "-->HBeat::decode: "
//								+ "ID["+acid+"] "
//								+ "Seq["+seq+"] "
//								+ "hbTime["+hbTime+"] "
//								+ "simTime["+scenarioTime+"] "
//								+ "wpidx["+wpidx+"] "
//								+ "controlMode["+AP.getModeText(controlMode)+"] "
//								+ "state["+ToOpText(state)+"] "
//								+ "alert["+Alert.toText(alert)+"] "
//								+ "alertTypeCount["+alertTypeCnt+"] "
//								+ "alertTotalCnt["+alertTotalCnt+"] "
//								+ "msgLength["+packet.length+"] "
//								+ "seq["+seq+"] "
//								+ "raw msg[" + IcarousCodec.bytesToInt(packet) + "] "
//								);
//	
//	}  // end decode function
//
//	//===================================================================//
//	/**
//	 * @return  Returns the state enum  equivalent of the state uint.  
//	 */
//	//===================================================================//
//	public OpState getOpState ()
//	{
//		return ToOpState (state);
//	}
//	
//	public String getOpStateText ()
//	{
//		return ToOpText(state);
//	}
//	
//	//===================================================================//
//	/**
//	 * @return  Returns the state enum  equivalent of the state uint.  
//	 */
//	//===================================================================//
//	public void setOpState (OpState s)
//	{
//		state = ToOpStateByte ( s );
//	}
//	
//	//===================================================================//
//	/**
//	 * Convert state enum to its uint equivalent.
//	 * 
//	 * @return  Returns the state information received over the serial link.  
//	 * 
//	 */
//	//===================================================================//
//	public static byte ToOpStateByte (OpState s) 
//	{
//		byte o = (byte)0x00;
//		switch (s)
//		{
//		case AC_Gnd:  			o =  (byte)0x00; 		break;
//		case AC_TakeOff:		o =  (byte)0x01; 		break;
//		case AC_GoingToStartGate:	o =  (byte)0x02; 		break;
//		case AC_AtStartGate: 		o =  (byte)0x03; 		break;
//		case AC_ReadyToStart: 	o =  (byte)0x04; 		break;
//		case AC_Scenario:		o =  (byte)0x05; 		break;
//		case AC_Reload: 		o =  (byte)0x06; 		break;
//		case AC_Homing: 		o =  (byte)0x07; 		break;
//		case SIM_Standby: 		o =  (byte)0x08; 		break;
//		case SIM_Ready: 		o =  (byte)0x09; 		break;
//		case SIM_Scenario: 		o =  (byte)0x10; 		break;
//		case SIM_Paused: 		o =  (byte)0x11; 		break;
//		case SIM_Down: 			o =  (byte)0x12; 		break;
//		default:				o =  (byte)0x12; 		break;
//		}
//		return o;
//	}
//	
//	
//	
//	//===================================================================//
//	/**
//	 * Convert state enum to its uint equivalent.
//	 * 
//	 * @return  Returns the state information received over the serial link.  
//	 * 
//	 */
//	//===================================================================//
//	public static OpState ToOpState (byte s) 
//	{
//		OpState o = OpState.AC_Gnd;
//		switch (s)
//		{
//		case 0x00: o = OpState.AC_Gnd;   		break;
//		case 0x01: o = OpState.AC_TakeOff;   	break;
//		case 0x02: o = OpState.AC_GoingToStartGate;   break;
//		case 0x03: o = OpState.AC_AtStartGate;   	break;
//		case 0x04: o = OpState.AC_ReadyToStart;  	break;
//		case 0x05: o = OpState.AC_Scenario;  	break;
//		case 0x06: o = OpState.AC_Reload;   	break;
//		case 0x07: o = OpState.AC_Homing;   	break;
//		case 0x08: o = OpState.SIM_Standby;   	break;
//		case 0x09: o = OpState.SIM_Ready;   	break;
//		case 0x10: o = OpState.SIM_Scenario;   	break;
//		case 0x11: o = OpState.SIM_Paused;   	break;
//		case 0x12: o = OpState.SIM_Down;   		break;
//		default:   o = OpState.SIM_Down;   		break;
//		}
//		return o;
//	}
//	
//	//===================================================================//
//	/**
//	 * Convert state enum to its uint equivalent.
//	 * 
//	 * @return  Returns the state information received over the serial link.  
//	 * 
//	 */
//	//===================================================================//
//	public static String ToOpText (byte s) 
//	{
//		
//		return ToOpState(s).toString();
//	}
//	
//	
//	
//	
////=========================================================================================================//
//// MAIN Test Entry point
////=========================================================================================================//
//		
//	
//	/**
//	 * @param args
//	 */
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//
//	}
//
//	public void setCurrentAlert(Alert alert2) {
//		// TODO Auto-generated method stub
//		
//	}
//
//}
