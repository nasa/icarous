/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.ICAROUS;
//
//import java.util.ArrayList;
//
///**
// * SynchronizedMsgQue manages arraylist of raw byte messages.  Messages assumed to be either SV (state) or FlightPlan (intent)
// * messages.
// * @author cquach
// *
// */
//public class SynchronizedMsgQue {
//
//	//====================================================================================================================//
//	// DATA Section
//	//====================================================================================================================//
////	class rawMsg { 
////		public byte[] byts; 
////		public double t;
////	
////		public rawMsg{byte[] b, double ti) 
////		{
////			byts = b; 
////			t = ti; 
////		}
////		
////	}
//		
//	ArrayList<byte[]> msgQue = new ArrayList<byte[]>();
//////		ArrayList< rawMsg > msgQue = new ArrayList<rawMsg>();
////	ArrayList< Pair<byte[], double> > msgQue = new ArrayList<rawMsg>();
//	int front = 0;
//	int rear = 0;
//	IcarousCodec codec = new IcarousCodec();
//
//
//	//====================================================================================================================//
//	// METHOD Section
//	//====================================================================================================================//
//
//	
//	//=====================================================================================//
//	/**
//	 * Synchronized add will block other threads from accessing msgQue until add completes
//	 * @param newMsg   The new message to add.
//	 */
//	//=====================================================================================//
//	public synchronized void enque (byte[] newMsg, double time) 
//	{
////		System.out.println(Thread.currentThread().getName()+"--> SynchronizedMsgQue::smp: "
////				+ "enque msg ["+codec.bytesToInt(newMsg)+"]");
//		msgQue.add(newMsg);
//		//rear += 1;
//	}
//	
//	//=====================================================================================//
//	/**
//	 * Synchronized dequeue one message and remove it from the list.  Converts raw bytes to Watch.Message object
//	 * @return a message if available.  Returns null if que is empty. 
//	 */
//	//=====================================================================================//
//	public synchronized Message getNextMsg() 
//	{
//		if (msgQue.isEmpty())
//			return null;
//		else {
//			Message m = null;
//			//byte[] temp = msgQue.get(0);
//			msgQue.remove(0);
//			
//			//front += 1;
//			return m;
//		}
//	}
//	
//
//	//=====================================================================================//
//	/**
//	 */
//	//=====================================================================================//
//	public synchronized ArrayList<byte[]> getQue()
//	{
//		
//		ArrayList<byte[]> originalQue = msgQue;
//		msgQue = new ArrayList<byte[]>();
////		System.err.println(Thread.currentThread().getName()
////							+"--> IcarousListener::getQue: "
////							+ "transfering ["+originalQue.size()+"] messages from MsgQue to StratWay msg que.");
//
//		return originalQue;
//		
//	}		// end getQue() function
//	
//	
//	//=====================================================================================//
//	/**
//	 * Convert a packet queue to a message queue.
//	 */
//	//=====================================================================================//
//	public static ArrayList<Message> convertToWatchMsgs (ArrayList<byte[]> originalQue)
//	{
//		IcarousCodec coder = new IcarousCodec();
//		int len = originalQue.size();
//		if (len <1) new ArrayList<Message>();
//		ArrayList<Message> msgList = new ArrayList<Message>();
//		
//		for (byte[] packet: originalQue) {
//			Message m =  coder.packetToWatchMsg(packet);
//			if (m != null) msgList.add( m );
//		}
//		return msgList;
//		
//	}	// end toWatchMessages() function
//	
//	
//}	// end SynchronizedMsgQue class 
