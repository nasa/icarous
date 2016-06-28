/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
//import gov.nasa.larc.ICAROUS.IcarousCodec;

public class APPacket
{
	
	//====================================================================================================================//
	// DATA Section
	//====================================================================================================================//

	//=====================================================================================//
	// These are the Parser constants
	// ------------------------------>>>  MavHeader
	//                                            {  0,   ,   ,   ,   ,   ,   ,   ,   ,   , 10,   ,   ,   ,   ,   ,   ,   ,   ,   , 20,   ,   ,   ,   ,   ,   ,   ,   ,   , 30,   ,   ,   ,   ,   ,   ,   ,   ,   , 40,   ,   ,   ,   ,   ,   ,   ,   ,   , 50,   ,   ,   ,   ,   ,   ,   ,   ,   , 60,   ,   ,   ,   ,   ,   ,   ,   ,   , 70,   ,   ,   ,   ,   ,   ,   ,   ,   , 80,   ,   ,   ,   ,   ,   ,   ,   ,   , 90,   ,   ,   ,   ,   ,   ,   ,   ,   ,100,   ,   ,   ,   ,   ,   ,   ,   ,   ,110,   ,   ,   ,   ,   ,   ,   ,   ,   ,120,   ,   ,   ,   ,   ,   ,   ,   ,   ,130,   ,   ,   ,   ,   ,   ,   ,   ,   ,140,   ,   ,   ,   ,   ,   ,   ,   ,   ,150,   ,   ,   ,   ,   ,   ,   ,   ,   ,160,   ,   ,   ,   ,   ,   ,   ,   ,   ,170,   ,   ,   ,   ,   ,   ,   ,   ,   ,180,   ,   ,   ,   ,   ,   ,   ,   ,   ,190,   ,   ,   ,   ,   ,   ,   ,   ,   ,200,   ,   ,   ,   ,   ,   ,   ,   ,   ,210,   ,   ,   ,   ,   ,   ,   ,   ,   ,220,   ,   ,   ,   ,   ,   ,   ,   ,   ,230,   ,   ,   ,   ,   ,   ,   ,   ,   ,240,   ,   ,   ,   ,   ,   ,   ,   ,   ,250,   ,   ,   ,   ,   };
	//public static final byte[] MsgLengthArray = {  3,  4,  8, 14,  8, 28,  3, 32,  0,  2,  3,  2,  2,  0,  0,  0,  0,  0,  0,  0, 19,  2, 23, 21,  0, 37, 26,101, 26, 16, 32, 32, 37, 32, 11, 17, 17, 16, 18, 36,  4,  4,  2,  2,  4,  2,  2,  3, 14, 12, 18, 16,  8, 27, 25, 18, 18, 24, 24,  0,  0,  0, 26, 16, 36,  5,  6, 56, 26, 21, 18,  0,  0, 18, 20, 20,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40, 72, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0, 42,  8,  4, 12, 15, 13,  6, 15, 14,  0, 12,  3,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30, 14, 14, 51,  5};
	public static final byte[] MsgLengthArray = MavHeader.MsgLengthArray;		//{  3,  4,  8, 14,  8, 28,  3, 32,  0,  2,  3,  2,  2,  0,  0,  0,  0,  0,  0,  0, 19,  2, 23, 21,  0, 37, 26,101, 26, 16, 32, 32, 37, 32, 11, 17, 17, 16, 18, 36,  4,  4,  2,  2,  4,  2,  2,  3, 14, 12, 18, 16,  8, 27, 25, 18, 18, 24, 24,  0,  0,  0, 26, 16, 36,  5,  6, 56, 26, 21, 18,  0,  0, 18, 20, 20,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40, 72, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0, 42,  8,  4, 12, 15, 13,  6, 15, 14,  0, 12,  3,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30, 14, 14, 51,  5};
	public static final int HEADER_OFFSET = 0;
	public static final int HEADER_SIZE = 6;
	public static final int DATA_OFFSET = HEADER_OFFSET+HEADER_SIZE;
	// DATA_SIZE		depends on the type of packet
	// CHKSUM_OFFSET	depends on the DATA_SIZE
	public static final int CHKSUM_SIZE = 2;

	public static final byte  MAV_MARKER  = (byte) 85;
	public static final byte  BEAGLE_ID   = (byte)103;
//	public static final byte  BEAGLE_ID   = (byte)0xFF;
	public static final byte  AP_ID       = (byte)  1;
	public static final byte  XBEE_ID     = (byte)255;
	public static final byte  COMP_ID     = (byte)  1;
	// <<<------------------------------
	// end of Parser constants
	
	// These are the queue global variables
	// ------------------------------>>>
	private static byte rcvSeqNum = -1;
	private static byte preSeqNum = -1;
	private static byte    seqNum = 0;
	private static byte[]   queue = new byte[1024];
	private static ByteBuffer qBB = ByteBuffer.wrap(queue);
	private static int       head = 0;
	private static int       tail = 0;
	private static boolean foundMarker = false;
	private static boolean gotForHeader = false;
	private static boolean gotHeader = false;
	private static boolean gotForPacket = false;
	// <<<------------------------------
	// end of queue global variables
	
	// These are the checksum local variables
	// ------------------------------>>>
	private int DATA_SIZE;
	private int CHKSUM_OFFSET;

	private byte[] temp = {0,0,0,0,0,0,0,0};
	private ByteBuffer BB = ByteBuffer.wrap(temp);
	
	private byte[] pkt;
	private ByteBuffer pBuf;
	// <<<------------------------------
	// end of checksum local variables

	

	//==================================//
	// Serial port specific stuff
	// ------------------------------>>>
	//
	//public static String portName = "COM22";
	public static APListener serial;		// = new SerialPort(portName);
	// <<<------------------------------
	// end of serial port variables

	
	//==================================//
	// AP Flight Plan
	// ------------------------------>>>
	//
	//public static APFlightPlan fp;		// = new APFlightPlan();	
	public static APPlan outPlan;
	public static APPlan inPlan;

	// <<<------------------------------
	// end of AP Flight Plan variables

	public static APLinkMonitor APLink;	// = new APLinkMonitor("APLinkMonitorLog.csv");

	// variables for handling stateful message series (conversations)
	public enum Context {  stateless, gettingAPFlightPlan, sendingAPFlightPlan }
	public static Context conversationContext  = Context.stateless;
	public static Context newConversation = Context.stateless;

	private static int targetMsgID;
	static int wpCount = 0;
	static short maxRetransmissions = 500;		//100;		//30;	// 20;
	static short maxRetries = 5;		// second level remediation not implemented yet.
	private static int reTransmissionCnt;
	public static boolean APplanReceived;
	private static long nowTime;
	private static long lastMsgTime;
	private static long retransmissionTimeout = 500;		// time out in 500 ms if a reply is not received during a conversation.  retransmit last packet
	
	private static long FlightPlanConversationTimeout = 2000;		// time out in 1000 ms if a reply is not received during a conversation.  Restart the conversation.
	
	//====================================================================================================================//
	// FUNCTION Section
	//====================================================================================================================//
//	public packet(APFlightPlan f)
//	{
//		fp = f;
//	}
	
	public APPacket()
	{
		
	}
	
	
	//=====================================================================================//
	/**
	 * Constructor to open serial port.
	 * @param pn   Serial port logical name.
	 */
	//=====================================================================================//
	// THIS SECTION OPERATES ON THE INTERNAL CLASS VARIABLES
	public APPacket(int msgID)
	{
		DATA_SIZE = MsgLengthArray[msgID];
		CHKSUM_OFFSET = DATA_OFFSET+DATA_SIZE;
		pkt = new byte[HEADER_SIZE+DATA_SIZE+CHKSUM_SIZE];
		pBuf = ByteBuffer.wrap(pkt);
		pkt[0] = MAV_MARKER;
		pkt[1] = MsgLengthArray[msgID];
		//pkt[2] = seqNum++;
		pkt[3] = BEAGLE_ID;
		pkt[4] = COMP_ID;
		pkt[5] = (byte) msgID;
		pBuf.position(DATA_OFFSET);
		BB.order(ByteOrder.LITTLE_ENDIAN);
	}


	
	public void load(byte[] b)
	{
		pBuf.position(HEADER_OFFSET);
		pBuf.put(b,HEADER_OFFSET,pkt.length);
		pBuf.position(DATA_OFFSET);
	}
	
	public void setSndSeq()
	{
		pkt[2] = seqNum++;
	}
//	public static boolean enque(byte[] b)
	public static boolean enque(byte[] b)
	{
		try {
			qBB.put(b);
			tail = qBB.position();
		} catch (Exception all) {
			System.out.println(Thread.currentThread().getName()
								+ "--> packet::enque: "
								+" failed inserting byte array ["+ b+"]"
								 );
		}
		boolean status = false;
		try {
			status = processQueue();
		} catch (Exception all) {
			System.out.println(Thread.currentThread().getName()
					+ "--> packet::enque: "
					+"  processQueue threw exception"
					 );
			all.printStackTrace();
	
		}
		return status;
	}
//	public static boolean processQueue()
	public static boolean processQueue()
	{

		if (!foundMarker)
		{
			// Check for empty queue
			if (head==tail) return false;
			
			// Search for a MAV_MARKER
			for (; head<tail && queue[head]!=MAV_MARKER; head++);
			if (queue[head]!=MAV_MARKER)
			{	// No marker found, therefore throw away entire queue
				qBB.clear();
				head = tail = 0;
				return false;
			}
			else
			{	// Marker found, set the marker at the beginning of the queue
				if (head>0)
				{	// Ensures marker is at beginning of queue
					qBB.position(head);
					qBB.limit(tail);
					qBB.compact();
					head = 0;
					tail = qBB.position();
				}
				foundMarker = true;
			}
		}
		
		// Found MAV_MARKER
		if (!gotForHeader)
		{
			// Check to see if you have enough bytes for a full header
			if ((tail-head) < HEADER_SIZE) return false;
			else gotForHeader = true;
		}
		
		// Have enough for a full header
		if (!gotHeader)
		{
			// Check for a valid header
			if (!isValidHeader(queue))
			{	// Header is not valid, skip past marker and start over
				head++;
				foundMarker = false;
				gotForHeader = false;
				return processQueue();
			}
			else gotHeader = true;
		}
		
		// Have a valid header
		if (!gotForPacket)
		{
			// Check to see if you have enough bytes for a full packet
			if ((tail-head) < getPacketSize(queue)) return false;
			else gotForPacket = true;
		}
		
		// Have enough for a full packet, check to see if it is valid
		foundMarker = false;
		gotForHeader = false;
		gotHeader = false;
		gotForPacket = false;

		// Valid packet or not, need to move head to the beginning of next packet
		head += getPacketSize(queue);
		
		// Check for a valid packet
		if (!isPacket(queue))
		{	//	If packet is not valid, throw it away
			qBB.position(head);
			qBB.limit(tail);
			qBB.compact();
			head = 0;
			tail = qBB.position();
			return processQueue();
		}

		// Valid packet found
		if (MavLink_0_9.processPacket(queue, true))  
		{
			int msgID = getMessageID(queue);
//			int msgSeq = getSequence(queue);
			//APLink.saveLinkInfo(msgSeq, msgID);
			if (conversationContext != Context.stateless) {
//				System.out.println(Thread.currentThread().getName()
//						+ "--> packet::conversationHandler: "
//						+" msgSeq ["+ msgSeq+"]"
//						+" transmissions ["+ reTransmissionCnt+"]"
//						 );
//				conversationChangeHandler(msgID);
				conversationHandler(msgID);
			}
		
		}
		return processQueue();
	}

	//=====================================================================================//
	/**
	 * This function handles conversation changes in a thread safe way.
	 * 
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//	
	public static synchronized void changeConversation(Context newCon ) 
	{
		newConversation = newCon;
	}
	
	//=====================================================================================//
	/**
	 * This function handles conversation changes in a thread safe way.
	 * 
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//	
//	private static void conversationChangeHandler(int msgID) 
//	{
//		if (newConversation != conversationContext.stateless) 
//		{
//			switch(conversationContext) // accept newConversation depending on what we are currently doing
//			{
//				case stateless: 
//					reTransmissionCnt = 0;
//					
//					return;
//				case gettingAPFlightPlan:	//
//					handle_GettingAPFlightPlanConversation (msgID);
//					return;
//				case sendingAPFlightPlan:
//					handle_SendingAPFlightPlanConversation (msgID);
//					return;
//				default: return;
//			}	// end switch(conversationContext) 
//
//			
//		}
//	}
	
	//=====================================================================================//
	/**
	 * This function handles state transitions for conversation with the AP.  Use this code
	 * area for fetching data that requires a sequence of message. Such as to get flight WPlist,
	 * or to get a series of control parameters.  The conversation is controlled by the
	 * "conversationContext" variable and the state sequencing is handled by the "msgID" and 
	 * "targetMsgID".
	 * 
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//	
	private static void conversationHandler(int msgID) 
	{
		
			switch(conversationContext) 
			{
				case stateless: 
					reTransmissionCnt = 0;
					return;
				case gettingAPFlightPlan:
					//handle_GettingAPFlightPlanConversation (msgID);
					handle_GettingAPFlightPlanConversation (msgID);
					return;
				case sendingAPFlightPlan:
					handle_SendingAPFlightPlanConversation (msgID);
					return;
				default: return;
			}	// end switch(conversationContext) 

	}


	//=====================================================================================//
	/**
	 * Deprecated!
	 * Converse with AP to get flight WPlist.  Data loaded in APFlightPlan object.  
	 * @param launchPlan 
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//
	public void getAPFlightPlan ()
	{		
		conversationContext = Context.stateless;
		wpCount = 0;

//		fp.cumulativeTime = 0;
//		fp.setUpdateTime();
		
		targetMsgID = WAYPOINT_COUNT_class.msgID;
		APplanReceived = false;
		
		conversationContext = Context.gettingAPFlightPlan;
//		serial.send(  WAYPOINT_REQUEST_LIST.encode((short) 0x01, (short)0x01)  );
		serial.send(  MavLink_0_9.WAYPOINT_REQUEST_LIST.encode()  );
	}
	

	//=====================================================================================//
	/**
	 * converse with AP to get flight WPlist
	 * @param launchPlan 
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//
	public void getAPFlightPlan (APPlan planStore)
	{		
		conversationContext = Context.stateless;
		wpCount = 0;

		if (planStore == null) return;
		inPlan = planStore;
//		inPlan.cumulativeTime = 0;
		inPlan.setRequestTime();
		
		targetMsgID = WAYPOINT_COUNT_class.msgID;
		APplanReceived = false;
		
		conversationContext = Context.gettingAPFlightPlan;
//		serial.send(  WAYPOINT_REQUEST_LIST.encode((short) 0x01, (short)0x01)  );
		serial.send(  MavLink_0_9.WAYPOINT_REQUEST_LIST.encode()  );
	}
	

	//=====================================================================================//
	/**
	 * Deprecate
	 * State machine to handle Getting Flight Plan from AP.(
	 * @param 
	 * @return 
	 */
	//=====================================================================================//
//	private static void handle_GettingAPFlightPlanConversation(int msgID) 
//	{
//
//		short condition = (byte)0x00;
////			System.out.println(Thread.currentThread().getName()
////			+ "--> packet::handle_GettingAPFlightPlanConversation: Got a Packet:"
////			+" MsgSeq ["+ msgSeq+"]"
//// 			);
//		if (msgID == targetMsgID) 
//		{
//			switch(targetMsgID)  
//			{
//				case WAYPOINT_COUNT_class.msgID:	// this is the response from AP on a WAYPOINT_REQUEST_LIST message
//					fp.WaypointsInFP = MavLink_0_9.WAYPOINT_COUNT.count;
//					if (fp.WaypointsInFP > 0)  {
//						targetMsgID = MavLink_0_9.WAYPOINT.msgID;
//						serial.send(MavLink_0_9.WAYPOINT_REQUEST.encode((short) 0x01, (short) 0x01, wpCount)  );
//						reTransmissionCnt = 0;
//					} else {
//						conversationContext = Context.stateless;
//					}
//					return;
//				case WAYPOINT_class.msgID:
//					if (MavLink_0_9.WAYPOINT.seq == wpCount)
//					{
//						fp.addWayPoint(new APWayPoint(MavLink_0_9.WAYPOINT));
//						wpCount ++;
//						if (wpCount < fp.WaypointsInFP)  {
//							targetMsgID = WAYPOINT_class.msgID;
//							serial.send(  MavLink_0_9.WAYPOINT_REQUEST.encode(wpCount) );
//							reTransmissionCnt = 0;
//						} else {	// finished getting last waypoint  Send an ACK
//							targetMsgID = WAYPOINT_COUNT_class.msgID;
//							serial.send(MavLink_0_9.WAYPOINT_ACK.encode(condition)  );
//							conversationContext = Context.stateless;
//							System.out.println(Thread.currentThread().getName()
//												+ "--> packet::handle_GettingAPFlightPlanConversation: Got a Packet:"
//												+" SUCCESS getting flight plan from AP ["+ fp.WaypointsInFP+"] points received."
//									 			);
//							
//							fp.repeatWayPoint1();  // do this when you get a CDnR reference. 
//							APplanReceived = true;
//						}			
//					} else {	// re-ask for waypoint if we don't get the right waypoint msg from AP (ie ask for 3 and got wp 5)
//						serial.send(  MavLink_0_9.WAYPOINT_REQUEST.encode(wpCount)   );
//						reTransmissionCnt = 0;
//					}
//					return;
//			}	// end switch
//		} else {	// Error handling for 'gettingAPFlightPlan' conversation when we didn't get the message we are waiting for
//			// inc transmission
//			// if too many packet go by and no targetID then re-ask 
//			reTransmissionCnt++;
//			if (reTransmissionCnt > maxRetransmissions)
//			{
//				System.out.println(Thread.currentThread().getName()
//						+ "--> packet::handle_GettingAPFlightPlanConversation: waiting for targetMsgID ["+targetMsgID+"] "
//						+" after ["+ reTransmissionCnt+"] transmissions."
//						 );						
//				reTransmissionCnt = 0;
//				
//				switch(targetMsgID)  
//				{
//					case WAYPOINT_COUNT_class.msgID:	// Ask again for waypoint count
//						System.out.println(Thread.currentThread().getName()
//											+ "--> packet::handle_GettingAPFlightPlanConversation: resending [WAYPOINT_REQUEST_LIST] "
//											+" reset transmissions  ["+ reTransmissionCnt+"]."
//											 );
//						serial.send(  MavLink_0_9.WAYPOINT_REQUEST_LIST.encode((short) 0x01, (short)0x01)  );
//						return;
//					case WAYPOINT_class.msgID:	// ask for the waypoint again
////						System.out.println(Thread.currentThread().getName()
////								+ "--> packet::handle_GettingAPFlightPlanConversation: resending [WAYPOINT_REQUEST] "
////								+ "for WP ["+wpCount+"] "
////								+" reset transmissions ["+ reTransmissionCnt+"]."
////								 );
//						serial.send(  MavLink_0_9.WAYPOINT_REQUEST.encode( (short) 0x01, (short) 0x01, wpCount)  );								
//						return;									
//				} // end  switch(msgID)  on error condition				
//				
//				return;
//			}
//			
//			
//		}	// end if (msgID == targetMsgID
//		return;
//		
//	}	// end method handle_GettingAPFlightPlanConversation(int msgID) 

	//=====================================================================================//
	/**
	 * State machine to handle Getting Flight Plan from AP.  Updates to APPlan object.
	 * @param 
	 * @return 
	 */
	//=====================================================================================//
	private static void handle_GettingAPFlightPlanConversation(int msgID) 
	{

		short condition = (byte)0x00;
//			System.out.println(Thread.currentThread().getName()
//			+ "--> packet::handle_GettingAPFlightPlanConversation: Got a Packet:"
//			+" MsgSeq ["+ msgSeq+"]"
// 			);
		if (msgID == targetMsgID) 
		{
			switch(targetMsgID)  
			{
				case WAYPOINT_COUNT_class.msgID:	// this is the response from AP on a WAYPOINT_REQUEST_LIST message
					inPlan.WaypointsInFP = MavLink_0_9.WAYPOINT_COUNT.count;
					if (inPlan.WaypointsInFP > 0)  {
						targetMsgID = WAYPOINT_class.msgID;
						serial.send(MavLink_0_9.WAYPOINT_REQUEST.encode((short) 0x01, (short) 0x01, wpCount)  );
						reTransmissionCnt = 0;
					} else {
						conversationContext = Context.stateless;
					}
					return;
				case WAYPOINT_class.msgID:
					if (MavLink_0_9.WAYPOINT.seq == wpCount)
					{
						inPlan.addWayPoint(new APWayPoint(MavLink_0_9.WAYPOINT));
						wpCount ++;
						if (wpCount < inPlan.WaypointsInFP)  {
							targetMsgID = WAYPOINT_class.msgID;
							serial.send(  MavLink_0_9.WAYPOINT_REQUEST.encode(wpCount) );
							reTransmissionCnt = 0;
						} else {	// finished getting last waypoint  Send an ACK
							targetMsgID = WAYPOINT_COUNT_class.msgID;
							serial.send(MavLink_0_9.WAYPOINT_ACK.encode(condition)  );
							conversationContext = Context.stateless;
							System.out.println(Thread.currentThread().getName()
												+ "--> packet::handle_GettingAPFlightPlanConversation: Got a Packet:"
												+" SUCCESS getting flight plan from AP ["+ inPlan.WaypointsInFP+"] points received."
									 			);
							
							//fp.repeatWayPoint1();  // do this when you get a CDnR reference. 
							//APplanReceived = true;
							inPlan.setReadyForUse();
						}			
					} else {	// re-ask for waypoint if we don't get the right waypoint msg from AP (ie ask for 3 and got wp 5)
						serial.send(  MavLink_0_9.WAYPOINT_REQUEST.encode(wpCount)   );
						reTransmissionCnt = 0;
					}
					return;
			}	// end switch
		} else {	// Error handling for 'gettingAPFlightPlan' conversation when we didn't get the message we are waiting for
			// inc transmission
			// if too many packet go by and no targetID then re-ask 
			reTransmissionCnt++;
			if (reTransmissionCnt > maxRetransmissions)
			{
				System.out.println(Thread.currentThread().getName()
						+ "--> packet::handle_GettingAPFlightPlanConversation: waiting for targetMsgID ["+targetMsgID+"] "
						+" after ["+ reTransmissionCnt+"] transmissions."
						 );						
				reTransmissionCnt = 0;
				
				switch(targetMsgID)  
				{
					case WAYPOINT_COUNT_class.msgID:	// Ask again for waypoint count
						System.out.println(Thread.currentThread().getName()
											+ "--> packet::handle_GettingAPFlightPlanConversation: resending [WAYPOINT_REQUEST_LIST] "
											+" reset transmissions  ["+ reTransmissionCnt+"]."
											 );
						serial.send(  MavLink_0_9.WAYPOINT_REQUEST_LIST.encode((short) 0x01, (short)0x01)  );
						return;
					case WAYPOINT_class.msgID:	// ask for the waypoint again
//						System.out.println(Thread.currentThread().getName()
//								+ "--> packet::handle_GettingAPFlightPlanConversation: resending [WAYPOINT_REQUEST] "
//								+ "for WP ["+wpCount+"] "
//								+" reset transmissions ["+ reTransmissionCnt+"]."
//								 );
						serial.send(  MavLink_0_9.WAYPOINT_REQUEST.encode( (short) 0x01, (short) 0x01, wpCount)  );								
						return;									
				} // end  switch(msgID)  on error condition				
				
				return;
			}
			
			
		}	// end if (msgID == targetMsgID
		return;
		
	}


	
	//=====================================================================================//
	/**
	 * Given an APPlan, start the converse with AP hardware to send WPlist if there is not 
	 * another conversation in progress.  If there is a conversation in progress, don't send
	 * the new plan.  Return true if able to send and false otherwise.  
	 * @param applan 
	 * @param Uses static variable "serBuff"
	 * @return 
	 * @return void
	 */
	//=====================================================================================//
	public boolean sendAPFlightPlan (APPlan applan)
	{		
		boolean status = false;
		if (conversationContext == Context.stateless)
		{
			
//			Packet.sendAPFlightPlan(fp.PlanCoreToAPPlan(newPlanCore, simTime));
			System.out.println(Thread.currentThread().getName()
					+ "--> packet::sendAPFlightPlan: Start sending plan to AP "
					+ " WP count ["+applan.size()+"]... "
					 );
			System.out.println(applan.toString());
			//conversationContext = conversationContext.stateless;
			outPlan = applan;
			//fp.currentAPPlan = applan;
			//targetMsgID = WAYPOINT_REQUEST_class.msgID;
			//wpCount = 0;
			lastMsgTime = 0;
			conversationContext = Context.sendingAPFlightPlan;
	//		serial.send(  MavLink_0_9.WAYPOINT_COUNT.encode(  applan.size())  );
			status = true;
		}
		return status;
	}
	
	//=====================================================================================//
	/**
	 * converse with AP to send flight WPlist
	 * @param applan 
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//
//	public void sendAPFlightPlan ()
//	{		
//		System.out.println(Thread.currentThread().getName()
//				+ "--> packet::sendAPFlightPlan: Start sending plan to AP "
//				+ " WP count ["+fp.currentAPPlan.size()+"]... "
//				 );
//		targetMsgID = WAYPOINT_REQUEST_class.msgID;
//		wpCount = 0;
//		fp.WaypointsInFP = fp.currentAPPlan.size();
//		conversationContext = conversationContext.sendingAPFlightPlan;
//		serial.send(  MavLink_0_9.WAYPOINT_COUNT.encode((short)1, (short)1, fp.currentAPPlan.size())  );
//	}
	
	//=====================================================================================//
	/**
	 * State machine to handle Sending Flight Plan to AP.
	 * @param Uses static variable "serBuff"
	 * @return void
	 */
	//=====================================================================================//
	public static void handle_SendingAPFlightPlanConversation(int msgID, int dummy) 
	{

		nowTime  = System.currentTimeMillis();
		long lapseTime = nowTime - lastMsgTime;
		if (msgID == targetMsgID) 
		{
			System.err.println(Thread.currentThread().getName()
					+ "--> packet::handle_SendingAPFlightPlanConversation: "
					+" waiting for target msg ["+targetMsgID+"]"
					+" got msg ["+msgID+"]"
					+" waiting to send WP ["+wpCount+"]"
					+"after ["+reTransmissionCnt+"] messages."
					);
			if (lapseTime > FlightPlanConversationTimeout ) 
			{
				// restart conversation
				
			}
			switch(targetMsgID)  
			{
				case WAYPOINT_REQUEST_class.msgID:
					if (MavLink_0_9.WAYPOINT_REQUEST.seq == wpCount) 
					{
						lastMsgTime = nowTime;
						if (wpCount == 2) 
						{
							System.err.println(Thread.currentThread().getName()
									+ "--> packet::handle_SendingAPFlightPlanConversation: "
//									+" setting WP(1) as current waypoint.  Msg: ["+ IcarousCodec.bytesToInt(MavLink_0_9.WAYPOINT_SET_CURRENT.encode(1))+"]"
									);
							serial.send(MavLink_0_9.WAYPOINT_SET_CURRENT.encode(1));
						}
						System.err.println(Thread.currentThread().getName()
								+ "--> packet::handle_SendingAPFlightPlanConversation: "
//								+" sending ["+ IcarousCodec.bytesToInt(outPlan.get(wpCount).encode())+"]"
								);
						serial.send(outPlan.get(wpCount).encode());						
						wpCount++;						
						if (wpCount < outPlan.WaypointsInFP) {
							targetMsgID = WAYPOINT_REQUEST_class.msgID;
						} else {
							targetMsgID = WAYPOINT_ACK_class.msgID;
							System.err.println(Thread.currentThread().getName()
												+ "--> packet::handle_SendingAPFlightPlanConversation: "
												+" Success sending ["+ outPlan.WaypointsInFP+"] points to AP."
												);

						}
					} else {	// AP asking for wrong waypoint for now send AP the waypoint that it wants?
//						System.err.println(Thread.currentThread().getName()
//											+ "--> packet::handle_SendingAPFlightPlanConversation: "
//											+" sending ["+ WatchCodec.bytesToInt(outPlan.get(MavLink_0_9.WAYPOINT_REQUEST.seq).encode())+"]"
//											);
//						serial.send(outPlan.get(MavLink_0_9.WAYPOINT_REQUEST.seq).encode());						
//						reTransmissionCnt = 0;
					}
					return;
				case WAYPOINT_ACK_class.msgID:
					
					//if (WAYPOINT_ACK.type == GOOD) {	// need to check WAYPOINT_ACK.type in AP code to see how its used?
					serial.send(MavLink_0_9.WAYPOINT_ACK.encode(MavLink_0_9.WAYPOINT_ACK.type)  );
					conversationContext = Context.stateless;
					//reTransmissionCnt = 0;
					//lastMsgTime = 0;
					return;
			}	// end switch on target state
		} else {	// (msgID != targetMsgID )
			// handle error conditions 
			//		1) if request to send not received, 
			//		2) no or not enough request for wp after request to send 
			//		3) no ack received after series of wp request
			//		4) got ack from AP before before all wp sent to AP
			if ((nowTime - lastMsgTime)> FlightPlanConversationTimeout) {
				// restart conversation
				//sendAPFlightPlan(outPlan);
			}
			if ((nowTime - serial.lastSendTime()) < retransmissionTimeout) 
			{
				return;
			} 
//			reTransmissionCnt++;
//			if (reTransmissionCnt > maxRetransmissions) {
			switch(targetMsgID)  	// targetMsgID is the state variable
			{
				case WAYPOINT_REQUEST_class.msgID:
					// resend the waypoint_count message to prompt AP to ask for waypoints 
					if (wpCount == 0) {
						// resend WayPoint_Request_List msg
						//targetMsgID = WAYPOINT_REQUEST_class.msgID;
						//wpCount = 0;
						outPlan.WaypointsInFP = outPlan.size();
						serial.send(  MavLink_0_9.WAYPOINT_COUNT.encode( outPlan.size())  );
					} else {	// resend previous WayPoint(wpCount)
						wpCount --;
						serial.send(outPlan.get(wpCount).encode());						
						wpCount++;						
						if (wpCount < outPlan.WaypointsInFP) {
							targetMsgID = WAYPOINT_REQUEST_class.msgID;
						} else {
							targetMsgID = WAYPOINT_ACK_class.msgID;
						}
						
					}
					return;
				case WAYPOINT_ACK_class.msgID:
					// resend last waypoint
					wpCount --;
					serial.send(outPlan.get(wpCount).encode());						
					wpCount++;						
				return;
				default:
				return;
						
			}	// end switch
		}
	}	// end handle_SendingAPFlightPlanConversation() function

	private static void handle_SendingAPFlightPlanConversation(int msgID) 
	{

		nowTime  = System.currentTimeMillis();
		long lapseTime = nowTime - lastMsgTime;
		if (lapseTime > FlightPlanConversationTimeout ) 
		{
			// restart conversation
			targetMsgID = WAYPOINT_REQUEST_class.msgID;
			wpCount = 0;
			//conversationContext = conversationContext.sendingAPFlightPlan;
			serial.send(  MavLink_0_9.WAYPOINT_COUNT.encode(  outPlan.size())  );
			lastMsgTime = nowTime;
			return;
		}
		
//		if (msgID == targetMsgID) 
//		{
//		System.err.println(Thread.currentThread().getName()
//				+ "--> packet::handle_SendingAPFlightPlanConversation: "
//				+" waiting for target msg ["+targetMsgID+"]"
//				+" got msg ["+msgID+"]"
//				+" waiting to send WP ["+wpCount+"]"
//				+"after ["+reTransmissionCnt+"] messages."
//				);
		switch(targetMsgID)  
		{
			case WAYPOINT_REQUEST_class.msgID:
				if (  (msgID == targetMsgID) 
					&&  (MavLink_0_9.WAYPOINT_REQUEST.seq == wpCount) )
				{
					lastMsgTime = nowTime;
					if (wpCount == 2) 
					{
						System.err.println(Thread.currentThread().getName()
								+ "--> packet::handle_SendingAPFlightPlanConversation: "
//								+" setting WP(1) as current waypoint.  Msg: ["+ IcarousCodec.bytesToInt(MavLink_0_9.WAYPOINT_SET_CURRENT.encode(1))+"]"
								);
						serial.send(MavLink_0_9.WAYPOINT_SET_CURRENT.encode(1));
					}
					System.err.println(Thread.currentThread().getName()
							+ "--> packet::handle_SendingAPFlightPlanConversation: "
//							+" sending ["+ IcarousCodec.bytesToInt(outPlan.get(wpCount).encode())+"]"
							);
					serial.send(outPlan.get(wpCount).encode());						
					wpCount++;						
					if (wpCount >= outPlan.size()) 
					{
						targetMsgID = WAYPOINT_ACK_class.msgID;
						System.err.println(Thread.currentThread().getName()
											+ "--> packet::handle_SendingAPFlightPlanConversation: "
											+" Success sending ["+ outPlan.WaypointsInFP+"] points to AP."
											);

					}
				} else {	// AP asking for wrong waypoint for now send AP the waypoint that it wants?
					if ((nowTime - serial.lastSendTime()) > retransmissionTimeout) 
					{
						// repeat last message
						serial.resendLastMsg();
//						System.err.println(Thread.currentThread().getName()
//											+ "--> packet::handle_SendingAPFlightPlanConversation: "
//											+" sending ["+ WatchCodec.bytesToInt(outPlan.get(MavLink_0_9.WAYPOINT_REQUEST.seq).encode())+"]"
//											);
//						serial.send(outPlan.get(MavLink_0_9.WAYPOINT_REQUEST.seq).encode());						
//						reTransmissionCnt = 0;
					}
				}
				return;
			case WAYPOINT_ACK_class.msgID:
				if ( msgID == targetMsgID) 
				{				
					//if (WAYPOINT_ACK.type == GOOD) {	// need to check WAYPOINT_ACK.type in AP code to see how its used?
					serial.send(MavLink_0_9.WAYPOINT_ACK.encode(MavLink_0_9.WAYPOINT_ACK.type)  );
					conversationContext = Context.stateless;
					//reTransmissionCnt = 0;
					//lastMsgTime = 0;
				} else {
					if ((nowTime - serial.lastSendTime()) > retransmissionTimeout) 
					{
						// repeat last message
						serial.resendLastMsg();
//						System.err.println(Thread.currentThread().getName()
//											+ "--> packet::handle_SendingAPFlightPlanConversation: "
//											+" sending ["+ WatchCodec.bytesToInt(outPlan.get(MavLink_0_9.WAYPOINT_REQUEST.seq).encode())+"]"
//											);
//						serial.send(outPlan.get(MavLink_0_9.WAYPOINT_REQUEST.seq).encode());						
//						reTransmissionCnt = 0;
					}
					
				}
				return;
		}	// end switch on target state
	}	// end of function
	
	
	//=====================================================================================//
	/**
	 * @param 
	 * @return
	 */
	//=====================================================================================//
	public static byte[] getQueue() 
	{
		return queue;
	}
	public void updateSeqNum()
	{
		updateSeqNum(pkt);
	}
	public static int getRcvSeq()
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,rcvSeqNum).getInt(0);
	}
	public byte[] getPacket()
	{
		return pkt;
	}
	public byte[] getHeader()
	{
		byte[] dst = new byte[HEADER_SIZE];
		pBuf.position(HEADER_OFFSET);
		pBuf.get(dst, 0, HEADER_SIZE);
		return dst;
	}
	public byte[] getData()
	{
		byte[] dst = new byte[DATA_SIZE];
		pBuf.position(DATA_OFFSET);
		pBuf.get(dst, 0, DATA_SIZE);
		return dst;
	}
	public byte[] getChkSum()
	{
		byte[] dst = new byte[CHKSUM_SIZE];
		pBuf.position(CHKSUM_OFFSET);
		pBuf.get(dst, 0, CHKSUM_SIZE);
		return dst;
	}
	public void putChkSum()
	{
		pBuf.order(ByteOrder.LITTLE_ENDIAN);
		pBuf.putShort(CHKSUM_OFFSET,calcChkSum(pkt));
		pBuf.order(ByteOrder.BIG_ENDIAN);
	}
//	private short calcChkSum()
//	{
//		return calcChkSum(pkt);
//	}
	public int getChkSumVal()
	{
		return BB.putLong(0,0L).putShort(0,pBuf.getShort(CHKSUM_OFFSET)).getInt(0);
	}
	public int getMarker()
	{
		return BB.putLong(0,0L).put(0,pBuf.get(0)).getInt(0);
	}
	public int getDataLength()
	{
		return BB.putLong(0,0L).put(0,pBuf.get(1)).getInt(0);
	}
	public int getSequence()
	{
		return BB.putLong(0,0L).put(0,pBuf.get(2)).getInt(0);
	}
	public int getSystemID()
	{
		return BB.putLong(0,0L).put(0,pBuf.get(3)).getInt(0);
	}
	public int getComponenID()
	{
		return BB.putLong(0,0L).put(0,pBuf.get(4)).getInt(0);
	}
	public int getMessageID()
	{
		return BB.putLong(0,0L).put(0,pBuf.get(5)).getInt(0);
	}
	public boolean isPacket()
	{
		return isPacket(pkt);
	}
//	public boolean isValidHeader()
//	{
//		return isValidHeader(pkt);
//	}
//	public boolean isValidChkSum()
//	{
//		return isValidChkSum(pkt);
//	}
	
	//Get Section
	public byte getByte()
	{
		return pBuf.get();
	}
	public short getShortB()
	{
		return BB.putLong(0,0L).put(0,pBuf.get()).getShort(0);
	}
	public short getShort()
	{
		return pBuf.getShort();
	}
	public int getIntS()
	{
		return BB.putLong(0,0L).putShort(0,pBuf.getShort()).getInt(0);
	}
	public int getInt()
	{
		return pBuf.getInt();
	}
	public long getLongI()
	{
		return BB.putLong(0,0L).putInt(0,pBuf.getInt()).getLong(0);
	}
	public long getLong()
	{
		return pBuf.getLong();
	}
	public float getFloat()
	{
		return pBuf.getFloat();
	}
	public void getByte(byte[] p, int off, int len)
	{
		pBuf.get(p, off, len);
	}
	
	//Put Section
	public void putByte(byte b)
	{
		pBuf.put(b);
	}
	public void putByteS(short b)
	{
		pBuf.put(BB.putShort(0,b).get(0));
	}
	public void putShort(short b)
	{
		pBuf.putShort(b);
	}
	public void putShortI(int b)
	{
		pBuf.putShort(BB.putInt(0,b).getShort(0));
	}
	public void putInt(int b)
	{
		pBuf.putInt(b);
	}
	public void putIntL(long b)
	{
		pBuf.putInt(BB.putLong(0,b).getInt(0));
	}
	public void putLong(long b)
	{
		pBuf.putLong(b);
	}
	public void putFloat(float b)
	{
		pBuf.putFloat(b);
	}
	public void putByte(byte[] p, int off, int len)
	{
		pBuf.put(p, off, len);
	}
	
	
	// THIS SECTION OPERATES ON A GIVEN BYTE ARRAY
	public APPacket(byte[] p)
	{
		DATA_SIZE = p.length-HEADER_SIZE-CHKSUM_SIZE;
		CHKSUM_OFFSET = DATA_OFFSET+DATA_SIZE;
		pkt = new byte[HEADER_SIZE+DATA_SIZE+CHKSUM_SIZE];
		pBuf = ByteBuffer.wrap(pkt);
		pBuf.put(p,HEADER_OFFSET,pkt.length);
		//pkt = p;	// This may not be thread safe
		//pBuf = ByteBuffer.wrap(pkt);
		pBuf.position(DATA_OFFSET);
		BB.order(ByteOrder.LITTLE_ENDIAN);
		//valid = isValid();
	}


	public void resetDataIdx() {
		pBuf.position(DATA_OFFSET);
		
	}

	
	private static void updateSeqNum(byte[] p)
	{
		preSeqNum = rcvSeqNum;
		rcvSeqNum = p[2];
		if (preSeqNum == -1) preSeqNum = -1;	// dummy line to remove warning about preSeqNum not being used
												// It will be used in the future to determine missing-packet statistics
	}
	public static int getMarker(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,p[0]).getInt(0);
	}
	public static int getDataLength(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,p[1]).getInt(0);
	}
	public static int getSequence(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,p[2]).getInt(0);
	}
	public static int getSystemID(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,p[3]).getInt(0);
	}
	public static int getComponenID(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,p[4]).getInt(0);
	}
	public static int getMessageID(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		return jB.putLong(0,0L).put(0,p[5]).getInt(0);
	}
	public static int getPacketSize(byte[] p)
	{
		return (HEADER_SIZE+getDataLength(p)+CHKSUM_SIZE);
	}
/*	public static int getPacketSize(byte b) 
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		int dataLen = jB.putLong(0,0L).put(0,b).getInt(0);
		return (HEADER_SIZE+dataLen+CHKSUM_SIZE);
	}*/
	public static boolean enoughForHeader(byte[] p)
	{
		if (p.length<HEADER_SIZE) return false;
		else					  return true;
	}
	public static boolean isValidHeader(byte[] p)
	{
		byte[] junk = {0,0,0,0,0,0,0,0};
		ByteBuffer jB = ByteBuffer.wrap(junk);
		jB.order(ByteOrder.LITTLE_ENDIAN);
		int msgID = jB.putLong(0,0L).put(0,p[5]).getInt(0);
		return (p[0]==MAV_MARKER)
			&& (p[1]==MsgLengthArray[msgID])
			&& (p[3]==BEAGLE_ID)||(p[3]==AP_ID)||(p[3]==XBEE_ID)
			&& (p[4]==COMP_ID);
	}
	public static boolean enoughForPacket(byte[] p)
	{
		return enoughForHeader(p) && isValidHeader(p) && (p.length>=getPacketSize(p));
	}
	public static boolean isValidChkSum(byte[] p)
	{
		ByteBuffer pktb = ByteBuffer.wrap(p);
		pktb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
		short pCRC = pktb.getShort(getPacketSize(p)-2);
		short nCRC = calcChkSum(p);
		return (boolean)(pCRC==nCRC);
	}
	public static short calcChkSum(byte[] p)
	{
	    int N = getPacketSize(p);
	    byte b  = 0x00;
	    byte ch = 0x00;
	    short crc  = (short) 0xffff;
	    short tempcrc = 0x0000;
	    for (int i=1; i<N-2; i++)
	    {
	        b = p[i];
	        ch = (byte) (b  ^  ((byte)(crc & 0x00ff) ))  ;
	        ch = (byte) ((ch ^ (ch << 4) ));
	        tempcrc = (short) (crc >> 8);
	        tempcrc = (short) (tempcrc & 0x00ff);
	        short a1 = (short) ((short)ch << 8);
	        short a2 = (short) (tempcrc ^ a1);
	        short a3 = (short) ((short)ch << 3);
	        short a3a = (short) (a3 & 0x07ff);
	        short a4 = (short) (a2 ^ a3a);
	        short a5 = (short) ((short)ch >> 4);
	        short a6 = (short) ((short) a5 & 0x000f);
	        crc = (short) (a4 ^ a6);
	    }
	    return crc;
	}
	public static boolean isPacket(byte[] p)
	{
		return (enoughForPacket(p) && isValidChkSum(p));
	}
	
	//=====================================================================================//
	/**
	 * Entry point to test AP communication and data storage.
	 * @param pn   Serial port logical name.
	 */
	//=====================================================================================//
	public static void main(String[] args) 
	{
		//AP ap;		// = new AP(0);
		// TODO Auto-generated method stub
		byte[] p1 = {(byte)85,(byte)51,(byte)236,(byte)1,(byte)1,(byte)254,(byte)1,(byte)102,(byte)108,(byte )105,(byte)103,(byte)104,(byte)116,(byte)32,(byte)112,(byte)108,(byte)97,(byte)110,(byte)32,(byte)114,(byte)101,(byte)99,(byte)101,(byte)105,(byte)118,(byte)101,(byte)100,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)71,(byte)85};
		byte[] p2 = {(byte)85,(byte)37,(byte)123,(byte)1,(byte)1,(byte)32,(byte)0,(byte)0,(byte)0,(byte)0,(byte)69,(byte)239,(byte)176,(byte)164,(byte)2,(byte)66,(byte)20,(byte)17,(byte)14,(byte)194,(byte)153,(byte)47,(byte)45,(byte)67,(byte)66,(byte)30,(byte)184,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)65,(byte)177,(byte)174,(byte)20,(byte)67,(byte)129,(byte)115,(byte)51,(byte)46,(byte)156};
		byte[] p3 = {(byte)85,(byte)36,(byte)42,(byte)255,(byte)1,(byte)39,(byte)1,(byte)1,(byte)0,(byte)7,(byte)0,(byte)177,(byte)0,(byte)0,(byte)63,(byte)128,(byte)0,(byte)0,(byte)191,(byte)128,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)191,(byte)128,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)131,(byte)200};
		byte[] p4 = {(byte)85,(byte)36,(byte)39,(byte)255,(byte)1,(byte)39,(byte)1,(byte)1,(byte)0,(byte)4,(byte)0,(byte)16,(byte)0,(byte)0,(byte)65,(byte)136,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)66,(byte)20,(byte)26,(byte)41,(byte)194,(byte)153,(byte)46,(byte)243,(byte)67,(byte)72,(byte)0,(byte)0,(byte)119,(byte)162};

		byte[] q2 = {(byte)37,(byte)123,(byte)1,(byte)1,(byte)32,(byte)0,(byte)0,(byte)0,(byte)0,(byte)69,(byte)239,(byte)176,(byte)164,(byte)2,(byte)66,(byte)20,(byte)17,(byte)14,(byte)194,(byte)153,(byte)47,(byte)45,(byte)67,(byte)66,(byte)30,(byte)184,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)65,(byte)177,(byte)174,(byte)20,(byte)67,(byte)129,(byte)115,(byte)51,(byte)46,(byte)156};
		byte[] q3 = {(byte)37,(byte)123,(byte)1,(byte)1,(byte)32,(byte)0,(byte)0,(byte)85,(byte)36,(byte)42,(byte)255};
		byte[] q4 = {(byte)1,(byte)39,(byte)1,(byte)1,(byte)0,(byte)7,(byte)0,(byte)177};
		byte[] q5 = {(byte)0,(byte)0,(byte)63,(byte)128,(byte)0,(byte)0,(byte)191,(byte)128,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)191,(byte)128,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)131,(byte)200,(byte)85,(byte)36,(byte)39,(byte)255,(byte)1,(byte)39,(byte)1,(byte)1,(byte)0,(byte)4,(byte)0,(byte)16,(byte)0};
		byte[] q6 = {(byte)0,(byte)65,(byte)136,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)66,(byte)20,(byte)26,(byte)41,(byte)194,(byte)153,(byte)46,(byte)243,(byte)67,(byte)72,(byte)0,(byte)0,(byte)119,(byte)162,(byte)85,(byte)36,(byte)42,(byte)255};
		byte[] q7 = {(byte)0,(byte)65,(byte)136,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)66,(byte)20,(byte)26,(byte)41,(byte)194,(byte)153,(byte)46,(byte)243,(byte)67,(byte)72,(byte)0,(byte)0,(byte)119,(byte)162,(byte)85,(byte)36,(byte)42,(byte)255};
		byte[] q8 = {(byte)85,(byte)37,(byte)123,(byte)1,(byte)1,(byte)32,(byte)0,(byte)0,(byte)0,(byte)0,(byte)69,(byte)239,(byte)176,(byte)164,(byte)2,(byte)66,(byte)20,(byte)17,(byte)14,(byte)194,(byte)153,(byte)47,(byte)45,(byte)67,(byte)66,(byte)30,(byte)184,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)65,(byte)177,(byte)174,(byte)20,(byte)67,(byte)129,(byte)115,(byte)51,(byte)35,(byte)156};

		//ap = new AP(0);
		enque(p1);	// full valid packet
		enque(q2);	// no marker
		enque(q3);	// marker, no full header
		enque(q4);	// marker, full header, no full packet
		enque(q5);	// marker, full header, full packet, and a partial packet
		enque(q6);	// rest of partial packet
		enque(q7);	// invalid header situation
		enque(p1);	// full valid packet
		enque(q8);	// bad checksum
		enque(p3);	// full valid packet
				
		int[] seq = {0,0,0,0};
		MavLink_0_9.processPacket(p1, false);
		seq[0] = getRcvSeq();
		MavLink_0_9.processPacket(p2, false);
		seq[1] = getRcvSeq();
		MavLink_0_9.processPacket(p3, false);
		seq[2] = getRcvSeq();
		MavLink_0_9.processPacket(p4, false);
		seq[3] = getRcvSeq();
		
		
		
	}	// end main()


	
	
}		// end AP class
