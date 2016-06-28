/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.ICAROUS;
//
//import gov.nasa.larc.ICAROUS.Hbeat.OpState;
//
//public class UAVHeartBeat extends Thread {
//
//	
//	//=========================================================================================================//
//	// Data Section
//	//=========================================================================================================//
//	boolean PQDEBUGMSG = false;
//	//public static float ticPerSec = 1000;  // millis in 1 sec for using system millisec function
//	
//	String portName = "";
//	public IcarousSerialListener watchComLink = null;
//	IcarousCodec codec = new IcarousCodec();
//
//	
//	//float hbRate = (float) 1.0;	                // hz (must be more than 30 ms)
//	static long hbTics = (long) 1000;	// 1000
//	long hbTicAccum = 0;
////	long sleepTics = Math.min(hbTics,posBroadcastTics) / 4;     // milliSec
//	public static long sleepTics = hbTics / 2 ;     // 250 milliSec, 0.25 sec
//	
//	long lastTime = 0;
//	long nowTime = 0;
//	long lapsTics = 0;
//	
//	long firstStartTime = System.currentTimeMillis();
//	//long hbSendTime = -1;
//	
//	//public boolean started = false;
//
//	public static Hbeat inBeat = new Hbeat(null, null);
//	public static Hbeat outBeat = new Hbeat(null, null);
//	
//	public static Alert currentAlert;
//	
//	
////=========================================================================================================//
//// Methods Section
////=========================================================================================================//
//
////	//===================================================================//
////	//===================================================================//
//	public UAVHeartBeat() {
//		//TODO save serial port handle for writing heartbeat message.
//	}
////	//===================================================================//
////	//===================================================================//
////	public EdgeHeartBeat(String pn) {
////		portName = pn;
////	}
////	
//	//===================================================================//
//	//===================================================================//
//	public UAVHeartBeat(IcarousSerialListener pn, String id, String typeId) 
//	{
//		watchComLink = pn;
//		
//		outBeat.acid = id;		// = new Hbeat(name, typeName);
//		outBeat.actype = typeId;
//		
//		// initialize minimum data for incoming heartbeat object which is the traffic simulator heartbeat
//		inBeat = new Hbeat(null, null);  // this get filled by the serial thread
//		inBeat.setOpState(OpState.SIM_Down);
//		this.setPriority(6);
//		this.setName("HB-thread");
//		System.out.println(Thread.currentThread().getName()
//				+"--> EdgeHeartBeat::Constructor: "
//				+ "Creating HeartBeat for ["+id+"]. "
//				);
//
//	}
//	
//	//===================================================================//
//	//===================================================================//
//	public void setCurrentAlert (Alert a) {
//		currentAlert = a;
//	}
//	
//	
//	//===================================================================//
//	/**
//	 * User of this class is either AircraftEdge or GndWatch. For AircraftEdge
//	 * this method returns the state of GndWatch.  For GneWatch, this call returns
//	 * the state of the AircraftEdge.  In both cases this call returns the 
//	 * state information that was last received through the serial link.  
//	 * 
//	 * @return  Returns the state information received over the serial link.  
//	 * 
//	 */
//	//===================================================================//
//	public OpState getOpState () {
//		return inBeat.getOpState();
//	}
//	
//
//	//===================================================================//
//	/**
//	 * User of this class is either AircraftEdge or GndWatch. For AircraftEdge
//	 * this method sets the state info to SEND TO GndWatch.  For GneWatch, 
//	 * this call sets the state info to SEND TO AircraftEdge.
//	 * 
//	 */
//	//===================================================================//
//	public void setOpState (OpState outS) {
//		outBeat.setOpState(outS);
//	}
//
////	//===================================================================//
////	/**
////	 * Convert state enum to its uint equivalent.
////	 * 
////	 * @return  Returns the state information received over the serial link.  
////	 * 
////	 */
////	//===================================================================//
////	public static byte opStateToByte (OpState s) 
////	{
////		byte o = (byte)0x00;
////		switch (s)
////		{
////		case AC_Gnd:  			o =  (byte)0x00;
////		case AC_TakeOff:		o =  (byte)0x01;
////		case AC_GoingToStartGate:	o =  (byte)0x02;
////		case AC_AtStartGate: 		o =  (byte)0x03;
////		case AC_Scenario:		o =  (byte)0x04;
////		case AC_Reload: 		o =  (byte)0x05;
////		case AC_Homing: 		o =  (byte)0x06;
////		case SIM_Standby: 		o =  (byte)0x07;
////		case SIM_Ready: 		o =  (byte)0x08;
////		case SIM_Scenario: 		o =  (byte)0x09;
////		case SIM_Paused: 		o =  (byte)0x10;
////		}
////		return o;
////	}
////
//	
//	//===================================================================//
//	//===================================================================//
//	public long min (long v1, long v2, long v3)  {
//		if ((v1<v2) && (v1<v3)) {
//			return v1;
//		} else if ((v2<v1) && (v2<v3)) {
//			return v2;
//		} else {
//			return v3;
//		}
//	}
//	
//
//	
//	//===================================================================//
//	//===================================================================//
//	public void run() {
////		System.out.println(Thread.currentThread().getName()
////							+"-->EdgeHeartBeat::run: "
////							+ " hbRate["+ hbRate +"]Hz = every [" +hbTics+"]ms"
////							);	
//		lastTime = System.currentTimeMillis() - firstStartTime;
//		//int tempCnter = 0;
//		outBeat.seq = 0;
//		
//		
//		//infinite loop
//		while(true) {
//			nowTime = System.currentTimeMillis() - firstStartTime;
//			lapsTics = nowTime - lastTime;
//			lastTime = nowTime;
//			
//			//double dt = lapsTics/1000.0;
//					
//			if (lastTime > 0 ) {	// 
//				//myState.t += lapsTics/1000.0; // convert millis to seconds
//			} else {
//				//myState.t = 0;
//			}
////			if (PQDEBUGMSG) System.out.println("EdgeHeartBeat::run: Wakeup after["+lapsTics+"]ms"
////												//+ " myState.t ["+myState.t+"]"
////												+ " hbTicAccum["+ hbTicAccum +"/"+hbTics+"]ms"
////												+ " trafficTicAccum["+trafficTicAccum +"/"+trafficTics+"]ms"
////												+ " cdrTicsAccum["+cdrTicsAccum +"/"+cdrTics+"]ms"
////												+ " posBroadCastTicsAccum["+posBroadCastTicsAccum +"/"+posBroadcastTics+"]ms"
////											);
//
//			// ----------------------------------------------------------------//
//			// Handle sending heart beat messages
//			// ----------------------------------------------------------------//
//		//	if (hbTicAccum >= hbTics) { // # milliseconds >= 250
////				System.out.println(Thread.currentThread().getName()
////						+"--> EdgeHeartBeat::run: BH Wakeup after["+lapsTics+"]ms"
////						+ " hbTicAccum["+ hbTicAccum +"/"+hbTics+"]ms"
////					);
////				System.out.println("EdgeHeartBeat::run: Send HeartBeat Msg every"
////									+"["+hbTics+"]ms actually["+hbTicAccum+"]ms");
//				outBeat.seq += 1;
//				//AircraftCommunicationsEdge.packetsSent++;
//				//SendBuffer.packetsSent++;
//				watchComLink.send ( outBeat.encode() );   // encode and send the heartbeat message
//				//long currentSysTime = System.currentTimeMillis();
//				
//				//System.err.println("Edge-->EdgeHeartBeat::run(): HBTime difference " + (currentSysTime - hbSendTime));
//				
//				//hbSendTime = currentSysTime;
//				
//				hbTicAccum = 0;
////			} else {
////				hbTicAccum += lapsTics;  // accumulate milliseconds
////			}
//
//			
//			try { //sleep for 1 second (1000 millis)
//				Thread.sleep(sleepTics);
//			} catch (InterruptedException e) {
//				//do something
//			}
//		}
//	}
//
//}		// end EdgeHeartBeat class
//
