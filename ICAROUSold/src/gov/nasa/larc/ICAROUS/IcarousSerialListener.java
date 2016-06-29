/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.ICAROUS;
//
//import gov.nasa.larc.serial.ByteQueue;
//import gov.nasa.larc.serial.SerialPortManager;
//import gov.nasa.larc.serial.SerialReceiver;
//import gov.nasa.larcfm.Util.Position;
//import gov.nasa.larcfm.Util.Velocity;
//
//import java.nio.ByteBuffer;
//import java.util.ArrayList;
//import java.util.Vector;
//
//
//public class IcarousSerialListener implements SerialReceiver {
//
//  //====================================================================================================================//
//  // DATA Section
//  //====================================================================================================================//
//
//  // Serial port specific stuff
//  public String portName = null;
//  public boolean portOpenStat = false;
//  public boolean enoughForMsg = true;
//  IcarousCodec coder = new IcarousCodec();
//
//  public UAVHeartBeat hb;		// moved to IcarousCodec
//  //public static int headerSize = 4;				// moved to IcarousCodec
//
//  //private static Vector<Byte> serBuff = new Vector<Byte>();
//  private static ByteQueue serBuff = new ByteQueue();
//
//  SynchronizedMsgQue msgQue = new SynchronizedMsgQue();
//  public static double messagesLost = 0.0;
//  public static double messagesReceived = 1.0;
//
//  public static double percentMessagesLost = 0.0;
//  //public static double percentPacketsLostInAir = 0.0;
//  public int packetsDroppedLastSecond;
//  public static int test = 999;
//  public Message m = null;
//
//  public Position Ts = null;
//  public Velocity Tv = null;
//
//  public int count = 0;
//  public int totalSkippedBytes = 0;
//  public static int packetsReceived = 1;
//
//  ByteBuffer buf = ByteBuffer.allocate(200);
//
//  //====================================================================================================================//
//  // FUNCTION Section
//  //====================================================================================================================//
//
//  /**
//   * Constructor to open serial port.
//   * @param pn   Serial port logical name.
//   */
//  public IcarousSerialListener(String pn) {
//    if (pn != null) {
//      portName = pn;
//      //Thread.currentThread().setPriority(4);
//      //Thread.currentThread().setName("Watch");
//      Messenger.msg("IcarousListener:"
//          + "Opening serial port & set listener at ["+ portName +"] "
//          + "with thread priority ["+Thread.currentThread().getPriority()+"]..."
//          );		
//      portOpenStat = SerialPortManager.addSerialReceiver(portName,this); // opens port and add listener
//      //SerialPortManager.
//      if (portOpenStat) {
//        Messenger.msg("IcarousListener:SUCCESS Opening serial port & set listener at ["+ portName +"].");						
//      } else {
//        Messenger.err("IcarousListener:FAILED Opening serial port & set listener at ["+ portName +"].");										
//      }
//    }
//  }
//
//  //=====================================================================================//
//  // Override interface method.
//  //=====================================================================================//
//  public synchronized void serialDataReceived(byte[] data)
//  {
//
//    //		System.out.println(Thread.currentThread().getName()+"-->IcarousListener::serialDataReceived: "
//    //							+"data size["+data.length+"]"
//    //							+"data ["+data+"]"
//    //							);
//
//    boolean newCode = true;
//
//    if (newCode ) {
//      //System.err.println("Edge-->IcarousListener::serialDataReceived: Head : "+ serBuff.getHead() + " Tail: " + serBuff.getTail());
//      packetsReceived += (data.length/72);
//      serBuff.enque(data);
//
//      try {
//        smp();
//
//      } catch (Exception e)	{
//        //STR.err("IcarousListener:serialDataReceived: Exception during parsing!");
//        System.err.println("IcarousListener:serialDataReceived: Exception during parsing!");
//      }
//    } else {	// run old code
//      //			Vector<Byte> v = new Vector<Byte>();
//      //			for (byte b : data)
//      //				v.add(b);
//      //	
//      //			try
//      //			{
//      //				//myParser.byteadder(v);
//      //				byteadder(v);
//      //			} catch (IOException e)
//      //			{
//      //				STR.err("IOException while passing byte vector to IcarousListener!");
//      //			}
//    }
//  }
//
//  //	synchronized public Message getMessage() {		
//  ////		if (m != null) {
//  ////			Message msg = m;
//  ////			m = null;
//  ////			return msg;
//  ////		}
//  ////		return null;
//  //		System.out.println("IcarousListener RECEIVE MSG: "+m);
//  //		return m;
//  //	}
//
//  //=====================================================================================//
//  /**
//   * Concatenates byte stream data as it comes in
//   * @param vec New stuff coming in from port.
//   * @return void
//   */
//  //=====================================================================================//
//  //	public  void byteadder (Vector<Byte> vec) throws IOException
//  //	{
//  ////		serBuff.addAll(vec);
//  ////		
//  ////		System.out.println(Thread.currentThread().getName()+"-->IcarousListener::byteadder: "
//  ////							+"serBuff size["+serBuff.size()+"]"
//  ////							+"serBuff ["+serBuff+"]"
//  ////							);
//  ////		smp();
//  ////		
//  //	}
//
//  //	/**
//  //	 * 
//  //	 * @return Marker index of next valid message
//  //	 * @author Evan Orenstein
//  //	 * 
//  //	 */
//  //	public  int getMarkerIndex (boolean enoughForMsg) 
//  //	{
//  //		
//  //		byte[] hdrBuff = new byte[2];
//  //		ByteBuffer buff = null;
//  //		int msgSize = -1;
//  //		int markerIdx = 0;
//  //		int count = 0;
//  //		int markerShift = 0;
//  //		while(enoughForMsg == true && markerIdx != -1 && (msgSize < 43 || msgSize > 404) )
//  //		{
//  //		markerIdx = serBuff.indexOf(IcarousCodec.msgMarker,markerShift);
//  //		
//  //		
//  //		
//  //		// found a message header.  now get message size
//  //		hdrBuff[0] = serBuff.get(markerIdx+3);//was 2
//  //		hdrBuff[1] = serBuff.get(markerIdx+2);//was 3
//  //		
//  //		buff = ByteBuffer.wrap(hdrBuff);
//  //		buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//  //		msgSize = (int)buff.getShort();
//  //		
//  ////			System.out.println(Thread.currentThread().getName()
//  ////						+"--> IcarousListener::smp: "
//  ////						+" Header["+serBuff.get(markerIdx)+"] at ["+markerIdx+"] "
//  ////						+"msg size raw bytes ["+hdrBuff[0]+","+hdrBuff[1]+"] msg size ["+msgSize+"]"
//  ////						);
//  //		buff.clear();
//  //		markerShift = markerIdx +1;
//  //		if(count > 4){
//  //			System.err.println("Edge-->IcarousListener::getMarkerIndex: Infinite Loop");
//  //
//  //		}
//  //		count++;
//  //		}
//  //		
//  //		return markerIdx;
//  //		
//  //	}
//
//  //=====================================================================================//
//  /**
//   * Hand coded watch message parser
//   * @param Uses static variable "serBuff"
//   * @return void
//   * Per Mathew Peak: Smp stands for Serial Message Parser
//   */
//  //=====================================================================================//
//  /*synchronized*/ public void smp () 
//  {
//    //int byte_idx = 0;
//    int serBuffSize = serBuff.size();
//    byte[] hdrBuff = new byte[2];
//    ByteBuffer buff = null;
//    int msgSize = -1;
//    int msgType = 0;
//    int markerIdx = 0;
//
//
//    //do
//    //{
//    //System.out.println(Thread.currentThread().getName()+"--> IcarousListener::smp: Do parsing.");
//    //byte_idx = 0;
//
//    // find out how much data is available to parse
//    serBuffSize = serBuff.size();
//    if ( (serBuffSize <= 0) || (serBuffSize > serBuff.capacity()) ){
//      System.out.println(Thread.currentThread().getName()
//          +"--> IcarousListener::smp: serBuffSize ["+serBuffSize+"]");
//    }
//    try {
//      //  buffer overflow kludge
//      if(serBuffSize > serBuff.capacity())	{  // this is a cludge need to check why?
//        serBuff.clear();
//        System.err.println(Thread.currentThread().getName()+"--> IcarousListener::smp: Error in Vector size (>300). Cleared.");
//        return;
//      }
//
//      //  has enough for a message header come yet?
//
//
//
//      //			//  is the header char in the buffer?
//      //			if(enoughForMsg)
//      //			{
//      markerIdx = serBuff.indexOf(IcarousCodec.msgMarker);
//      //			}
//
//      if (markerIdx == -1)	{				
//        serBuff.clear();
//        return;
//      } 
//
//      if(serBuffSize < IcarousCodec.headerSize) {
//        System.out.println("Edge-->IcarousListener::smp(): Byte Stream too small. Awaiting more Bytes");
//        return;
//      }
//
//      // found a message header.  now get message size
//      hdrBuff[0] = serBuff.get(markerIdx+3);//was 2
//      hdrBuff[1] = serBuff.get(markerIdx+2);//was 3
//
//      buff = ByteBuffer.wrap(hdrBuff);
//      buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//      msgSize = (int)buff.getShort();
//
//      //				System.out.println(Thread.currentThread().getName()
//      //							+"--> IcarousListener::smp: "
//      //							+" Header["+serBuff.get(markerIdx)+"] at ["+markerIdx+"] "
//      //							+"msg size raw bytes ["+hdrBuff[0]+","+hdrBuff[1]+"] msg size ["+msgSize+"]"
//      //							);
//      buff.clear();
//
//
//      //markerIdx = getMarkerIndex(enoughForMsg);
//
//      //			if (markerIdx == -1)	{				
//      //				serBuff.clear();
//      //				return;
//      //			} 
//
//      //			hdrBuff[0] = serBuff.get(markerIdx+3);//was 2
//      //			hdrBuff[1] = serBuff.get(markerIdx+2);//was 3
//      //			
//      //			buff = ByteBuffer.wrap(hdrBuff);
//      //			buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//      //			msgSize = (int)buff.getShort();
//      //			
//
//      msgType = (int)serBuff.get(markerIdx+1);
//
//      if(msgType > 5 || msgType < 0)
//      {
//        //System.err.println("Edge-->IcarousListener::smp(): Found Invalid msgType in Message with msgSize " +msgSize);
//
//        serBuff.shiftHead(markerIdx+1, 1);
//        return;
//      }
//      if(msgSize < 45 || msgSize > 1000)//Needs to be changed to reflect expected FP size of current plan. should be smallest message sent and biggest message sent in bytes. static 1000 for now while we make changes to data being passed
//      {
//        //System.err.println("Edge-->IcarousListener::smp(): Found 97 Header Marker in Message with msgSize " +msgSize);
//        serBuff.shiftHead(markerIdx+1, 1);
//
//        return;
//      }
//      //  is there enough bytes to process one complete message
//      if (serBuffSize < markerIdx + msgSize) {
//        //				
//        //				System.out.println(Thread.currentThread().getName()
//        //									+ "--> IcarousListener::smp: "
//        //									+ "serBuff size ["+serBuffSize+"] "
//        //									+ "Msg marker at ["+markerIdx+"] "
//        //									+ "looking for Msg size ["+msgSize+"] "
//        //									+ "Waiting for rest of message (buffer needs ["+(markerIdx + msgSize)+"] bytes)..."
//        //									);
//        //				
//
//        // waiting for full message
//        enoughForMsg = false;
//        return;	// not a full message yet.
//      }
//      enoughForMsg = true;
//
//      // if we get here there is a message to extract
//      // Copy full message to a ByteBuffer and remove that from serBuff
//
//      // copy message payload from serial buffer into ByteQue for parsing later
//      //			public static final byte SVmsg = (byte)0;    	// Position message from Edge to GndWatch or GndWatch to XPlane
//      //			public static final byte WPmsg = (byte)1;		// Waypoint message to pass new flight WPlist from Edge to GndWatch
//      //			public static final byte HBmsg = (byte)2;		// HeartBeat message between Edge and GndWatch for Synch with time steeping ghost traffic
//      //			public static final byte FPmsg = (byte)3;		// message to pass new flight WPlist from Edge to GndWatch
//      totalSkippedBytes += markerIdx;
//      if(markerIdx != 0)
//      {
//        System.err.println("Bytes skipped until next message " +markerIdx + " Total bytes skipped " + totalSkippedBytes);
//      }
//      switch ( msgType ) {
//      case ((int)IcarousCodec.SVmsg):	// call SV msg parser
//        //System.out.println(Thread.currentThread().getName()+"--> IcarousListener::smp: enque SVmsg.");
//        //m = coder.decodeSVpacket ( byteCopy(serBuff, markerIdx+headerSize, msgSize-headerSize )); // don't pass header	// code for using Vector
//        //m = coder.decodeSVpacket ( serBuff.deque(markerIdx+headerSize, msgSize-headerSize) ); // don't pass header		// code for byte array immediate decode to message object in serial thread
//        //m.timeStamp = GlobalState.getSimTime();
//        //msgQue.add(m);  // message que change to raw bytes instead of messages
//
//        byte[] checkSV = serBuff.deque(markerIdx, msgSize);
//      //System.err.println("Edge-->IcarousListener::smp(): SV Packet dequed = "+Codec.bytesToInt(checkSV));
//
//      if(Codec.isValidChkSum(checkSV))
//      {
//        messagesReceived++;
//        // [CAM] Originally, the code used GlobalState.getSimTime(). Is this needed here?
//        msgQue.enque ( checkSV, Icarous.getSimTime() ); // do pass header		// code for byte array immediate decode to message object in serial thread
//
//        //					System.err.println(Thread.currentThread().getName()
//        //							+ "-->IcarousListener::Line 273: valid SV packet received : "+Codec.bytesToInt(checkSV));
//      }
//      else
//      {
//        messagesReceived++;
//        packetsDroppedLastSecond++;
//        messagesLost++;
//
//        System.err.println (Thread.currentThread().getName()
//            + "-->IcarousListener::Line 278: Checksum Invalid. Invalid SVPacket Received"
//            );
//        System.err.println(Thread.currentThread().getName()
//            + "-->IcarousListener::Line 278: SVpacket not received length " + checkSV.length+" Invalid Bytes: "+Codec.bytesToInt(checkSV) );
//        serBuff.shiftHead(72,0);
//        //					System.err.println("Serial-->ByteQueue::shiftHead: Thread "+Thread.currentThread().getName()+" Bytes after head is shifted: " +Codec.bytesToInt(serBuff.copy(0, 12)));
//
//
//      }
//      break;
//      case ((int)IcarousCodec.WPmsg):	 // call WP msg parser
//        ;//ac.parseWaypoint ( byteCopy(serBuff, markerIdx+headerSize, msgSize-headerSize ));
//      break;
//      case ((int)Hbeat.HBmsg):	// call heartbeat msg parser
//        //				coder.decodeHBpacket (edgeStat, byteCopy(serBuff, markerIdx+headerSize, msgSize-headerSize ));
//        //				coder.decodeHBpacket (edgeStat, serBuff.deque(markerIdx, msgSize), GlobalState.getSimTime());
//        count++;
//
//      byte[] checkHB = serBuff.deque(markerIdx, msgSize);
//      //				if(count%15 != 1)
//      //				{
//      //					System.err.println("Edge-->IcarousListener::smp(): skipped hb decode "	);
//      //					
//      //					break;
//      //				}
//      //System.err.println("Edge-->IcarousListener::smp(): HB Packet dequed = "+Codec.bytesToInt(checkHB));
//
//      if(Codec.isValidChkSum(checkHB))
//      {
//        messagesReceived++;
//        UAVHeartBeat.inBeat.decode(checkHB);
//      }
//      else
//      {
//        packetsDroppedLastSecond++;
//        messagesReceived++;		
//        messagesLost++;
//        System.err.println (Thread.currentThread().getName()
//            + "-->IcarousListener::Line 295: Checksum Invalid. Invalid Packet Received"
//            );
//        System.err.println(Thread.currentThread().getName()
//            + "-->IcarousListener::Line 295: packet received length " + checkHB.length+" Invalid Bytes: "+Codec.bytesToInt(checkHB));
//
//        //					serBuff.shiftHead(74);
//        //					System.err.println("Serial-->ByteQueue::shiftHead: Thread "+Thread.currentThread().getName()+" Bytes after head is shifted: " +Codec.bytesToInt(serBuff.copy(0, 12)));
//
//      }
//      break;
//      case ((int)IcarousCodec.FPmsg):	// call heartbeat msg parser
//        //m = coder.decodeFPPacket ( byteCopy(serBuff, markerIdx+headerSize, msgSize-headerSize ));//	public Message decodeFPPacket(byte[] input) {
//        //				m = coder.decodeFPPacket ( serBuff.deque(markerIdx+headerSize, msgSize-headerSize) );//	public Message decodeFPPacket(byte[] input) {
//        //				m = coder.decodeFPPacket ( serBuff.deque(markerIdx, msgSize) );//	public Message decodeFPPacket(byte[] input) {
//        //				m.timeStamp = GlobalState.getSimTime();
//        //				msgQue.add(m);
//        //				System.out.println(Thread.currentThread().getName()+"--> IcarousListener::smp: enque FPmsg.");
//
//        byte[] checkFP = serBuff.deque(markerIdx, msgSize);
//      //System.err.println("Edge-->IcarousListener::smp(): FP Packet dequed = "+Codec.bytesToInt(checkFP));
//
//      if(Codec.isValidChkSum(checkFP))
//      {
//        //					System.err.println(Thread.currentThread().getName()
//        //							+ "-->IcarousListener::Line 315: valid FPpacket received : "+Codec.bytesToInt(checkFP));
//        messagesReceived++;
//        // [CAM] Originally, the code used GlobalState.getSimTime(). Is this needed here?
//        msgQue.enque ( checkFP, Icarous.getSimTime() ); // do pass header		// code for byte array immediate decode to message object in serial thread
//      }
//      else
//      {
//        packetsDroppedLastSecond++;
//        messagesReceived++;
//        messagesLost++;
//        System.err.println (Thread.currentThread().getName()
//            + "-->IcarousListener::Line 319: Checksum Invalid. Invalid FPPacket Received"
//            );
//        System.err.println(Thread.currentThread().getName()
//            + "-->IcarousListener::Line 319: FPpacket not received length " + checkFP.length+" Invalid Bytes: "+Codec.bytesToInt(checkFP));
//        serBuff.shiftHead(72,0);
//        //					System.err.println("Serial-->ByteQueue::shiftHead: Thread "+Thread.currentThread().getName()+" Bytes after head is shifted: " +Codec.bytesToInt(serBuff.copy(0, 12)));
//
//
//      }
//      break;
//      case((int)IcarousCodec.ACmsg):
//        byte[] checkAC = serBuff.deque(markerIdx,msgSize);
//
//      if(Codec.isValidChkSum(checkAC))
//      {
//        messagesReceived++;
//        msgQue.enque(checkAC, Icarous.getSimTime())	;
//      }
//      else
//      {
//        packetsDroppedLastSecond++;
//        messagesReceived++;
//        messagesLost++;
//        //					System.err.println (Thread.currentThread().getName()
//        //							+ "-->IcarousListener::Line 319: Checksum Invalid. Invalid ACPacket Received"
//        //							);
//        //					System.err.println(Thread.currentThread().getName()
//        //							+ "-->IcarousListener::Line 319: ACpacket not received : "+Codec.bytesToInt(checkAC));
//      }
//      break;
//      default:
//        break;
//      }
//      percentMessagesLost = (messagesLost/messagesReceived)*100;
//      UAVHeartBeat.outBeat.messagesLostAir = (short)messagesLost;
//      UAVHeartBeat.outBeat.packetsReceivedAir = (short)packetsReceived;
//      //percentPacketsLostInAir = ((EdgeHeartBeat.inBeat.packetsLostAir)/AircraftCommunicationsEdge.packetsSent)*100;
//
//      if(1 == IcarousSerialListener.messagesReceived%200)
//      {
//        System.err.println("Edge-->IcarousListener::smp(): Number of messages to be received: " + IcarousSerialListener.messagesReceived+ "  Number of Messages lost: "+ IcarousSerialListener.messagesLost + "  Percent messages lost: "+ IcarousSerialListener.percentMessagesLost);
//      }
//      //System.out.println(Thread.currentThread().getName()+"--> IcarousListener::smp: Removing ["+ (markerIdx + msgSize-1)+"]");
//      //byteRemove(serBuff, 0, markerIdx + msgSize-1);
//      //serBuff.remove( markerIdx + msgSize );
//
//
//
//      //} while ( byte_idx < serBuffSize );
//
//    } catch (Exception e) {
//
//      //			System.err.println("IcarousListener:smp: Exception during parsing! thrown by thread :	"+Thread.currentThread().getName());
//      //			
//      //			System.err.println("IcarousListener:smp: msgSize  :	"+ msgSize+" Exception : "+ e);
//      //			e.printStackTrace();
//
//    }
//  }		// end smp function
//
//  //===================================================================
//  //===================================================================
//  public synchronized ArrayList<Message> getMsgQue ()
//  {
//    //return msgQue.getQue();
//    //ArrayList<byte[]> msgList = msgQue.getQue();
//    packetsDroppedLastSecond = 0;
//    return SynchronizedMsgQue.convertToWatchMsgs(msgQue.getQue());
//
//  }
//
//  //	//===================================================================
//  //	//===================================================================
//  //	private void parseWaypoint(byte[] byteCopy) {
//  //		// TODO Auto-generated method stub
//  //		
//  //	}
//
//  //===================================================================
//  /**
//   * Quick byte copy.  NO ERROR CHECKING
//   * @param vec Original stuff.
//   * @return copy of original stuff
//   */
//  public byte[] byteCopy (Vector<Byte> vec, int sIdx, int size) 
//  {
//    byte[] copy = new byte[size];
//
//    for (int j=0;  j < size-1;  j++ ) {
//      copy[j] = vec.get(sIdx+j);
//    }
//
//    return copy;
//
//  }
//
//  //===================================================================
//  /**
//   * Quick byte copy.  Inverts byte order.  NO ERROR CHECKING
//   * @param vec Original stuff.
//   * @return copy of original stuff
//   */
//  public byte[] byteCopInv (Vector<Byte> vec, int sIdx, int size) 
//  {
//    byte[] copy = new byte[size];
//    int k = size-1;
//    for (int j=0;  j < size-1;  j++ ) {
//      copy[j] = vec.get(sIdx+k);
//      k--;
//    }
//
//    return copy;
//
//  }
//
//  //===================================================================
//  /**
//   * Quick byte copy.  NO ERROR CHECKING
//   * @param vec Original stuff.
//   * @return copy of original stuff
//   */
//  public byte[] bbyteCopy (byte[] vec, int sIdx, int size) 
//  {
//    byte[] copy = new byte[size];
//
//    for (int j=0;  j < size-1;  j++ ) {
//      copy[j] = vec[sIdx+j];
//    }
//
//    return copy;
//
//  }
//
//
//  //===================================================================
//  /**
//   * Quick byte remove.  NO ERROR CHECKING
//   * @param vec Original stuff.
//   * @return copy of original stuff
//   */
//  public void byteRemove (Vector<Byte> vec, int sIdx, int size) 
//  {
//    //byte[] copy = new byte[size];
//
//    //System.out.println ("IcarousListener::byteRemove: removing ["+size+"] from vector of ["+vec.size()+"]");
//    try {
//      for (int j=sIdx;  j <= size;  j++ ) {
//        vec.removeElementAt(j);		// somthing very strange about vector indexing??
//      }			
//    }  catch (Exception  e) { 
//      return;
//    }
//  }
//
//
//  //===================================================================
//  /**
//   * Parses serialized ID/pos/vel and update our info if the ID matches
//   * @param msg byte array of serialized info
//   * @return true if ID matches, msg is valid, and we update our info
//   */
//
//  //===================================================================
//  /**
//   * Write a byte array into a serial port.
//   * @param msg The byte array to output
//   * @return
//   */
//  //===================================================================
//  public /*synchronized*/ boolean send(byte[] msg) {
//
//    //		System.out.println(Thread.currentThread().getName()+"--> IcarousListener::send: Sending ["+ coder.bytesToInt(msg)+"]");
//    return SerialPortManager.write(portName, msg);  // write function returns boolean status
//  }
//
//  //===================================================================
//  /**
//   * Notify all flying vehicles to start the scenario which should 
//   * trigger vehicles to head to first point of scenario flight plan.
//   * 
//   * @param msg The byte array to output
//   * @return
//   */
//  //===================================================================
//  //	public void notifyStartScenario() {
//  //		this.send(coder.encodeScenarioStart(portName, portName, 0));
//  //		
//  //	}	// end notifyStartScenario() method
//
//
//}	// end IcarousListener class
