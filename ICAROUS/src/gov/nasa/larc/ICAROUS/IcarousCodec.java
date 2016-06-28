/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//package gov.nasa.larc.ICAROUS;
//
//import gov.nasa.larcfm.Util.*;
//
//import java.nio.ByteBuffer;
//import java.util.Arrays;
//
//
//public class IcarousCodec extends Codec
//{
//
//  //====================================================================================================================//
//  // DATA Section
//  //====================================================================================================================//
//  //	boolean PQDEBUGMSG = false;
//
//  public static final byte SVmsg = (byte)0;    	// Position message from Edge to GndWatch or GndWatch to XPlane
//  public static final byte WPmsg = (byte)1;		// Waypoint message to pass new flight WPlist from Edge to GndWatch
//  public static final byte FPmsg = (byte)3;		// message to pass new flight WPlist from Edge to GndWatch
//  public static final byte ACmsg = (byte)4;
//  public static final byte SSmsg = (byte)5;		// message from GndWatch to Edge to "START scenario"
//
//  public static double svSimTime = -1;
//
//  int msgBufSize = 1000;
//  ByteBuffer buf = ByteBuffer.allocate(msgBufSize);
//  byte[] placeHolder = new byte[headerSize+idFixedLimit];	// make room for headerSize byte message header
//
//  // Decoded data available for external use.  Are we assuming data is most recent?
//  public long hbTime = 0;
//  public int wpIndex = 0;
//  public String acID = null;
//  public Position Ts = null;
//  public Velocity Tv = null;
//
//  //====================================================================================================================//
//  // FUNCTION Section
//  //====================================================================================================================//
//
//  //=====================================================================================//
//  /**
//   * Encode a flight WPlist into a byte array.
//   * @param input is the byte array to decode
//   * @return Watch.Message object containing the Waypoint list from the flight WPlist.
//   */
//  //=====================================================================================//
//  public byte[] encodeFPPacket(String id, String ACtype, Plan fp, double simTime) 
//  {
//    byte[] packet = null;
//
//    try {
//      int fpSize = fp.size();
//      NavPoint wp = null;
//
//      if (!buf.hasArray())  
//        buf = ByteBuffer.allocate(msgBufSize);
//      buf.clear();
//      buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//      //		System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodeFPPacket: "
//      //							+"message byteOrder[" + buf.order().toString() + "]  "
//      //						  );
//      buf.put(placeHolder).putInt(fpSize).putDouble(simTime);
//      for (int i=0; i<fpSize; i++)
//      {
//        wp = fp.point(i);
//
//        buf.putDouble( Units.to("deg", wp.lat()) );  // convert lat from radians to degree before packing 
//        buf.putDouble( Units.to("deg", wp.lon()) );
//        buf.putDouble( Units.to("ft", wp.alt()) );
//        buf.putDouble( wp.time() );
//
//      }
//
//      packet = encodePacket(id, ACtype, FPmsg);
//      buf.clear();
//      System.out.println(Thread.currentThread().getName()
//          + "-->IcarousCodec::encodeFPPacket: "
//          + "AC["+id+"] "
//          + "out msg len[" + packet.length + "] "
//          + "raw msg[ " + packet+"] "
//          + "bytes["+bytesToInt(packet)+"] "
//          );
//      // add byte array size check here
//      //		if (msg.length != 64)  {
//      //			System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodePacket: Message is ["+msg.length+"] bytes.  Suppose to be [64] bytes!!!");
//      //		}
//    } catch (Exception e) {
//      System.err.println(Thread.currentThread().getName()
//          +"-->IcarousCodec::encodeFPPacket: "
//          +"Exception during flightplan encoding. "
//          //+"wp[" + wp.toString() + "]  "
//          );
//
//    }
//
//    return packet;
//  }
//
//  //=====================================================================================//
//  /**
//   * Decode a byte array containing a flight WPlist.
//   * @param input is the byte array to decode
//   * @return Watch.Message object containing the Waypoint list from the flight WPlist.
//   */
//  //=====================================================================================//
//  public Message decodeFPPacket(byte[] input) {
//
//    buf.clear();
//
//    // get acID
//    int s = headerSize;
//    acID = new String(input,s,idFixedLimit);
//    acID = stripTrailing0s(acID);
//
//    // get the number of waypoints in the list
//    s += idFixedLimit;
//    buf = ByteBuffer.wrap( byteCopy(input, s, ilen) );
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    int fpSize = buf.getInt();
//
//    s += ilen;
//    buf = ByteBuffer.wrap( byteCopy(input, s, dlen) );
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    //double simTime = -1;
//    //simTime = buf.getDouble();
//
//    // set start index at first wp in the byte array
//    s += dlen;
//    int wpLen = (3*dlen) + llen;
//
//    // build the flight WPlist 
//    Plan fp = new Plan(acID);
//    for (int i=0; i<fpSize; i++)
//    {
//      fp.add( decodeWatchWPpacket(s, input) );
//      s += wpLen;
//    }
//    buf.clear();
//
//    // build the message to contain the flight WPlist
//    Message m = new Message();
//    /** [CAM]
//    m.setType(Message.Type.PLAN);		// see comments in Watch.Message 
//    m.id = acID;
//    m.trajectory = fp;
//    //m.hbTime = System.currentTimeMillis();
//    m.timeStamp = simTime;
//    //		Position newPos = Position.makeLatLonAlt( (double)MavLink_0_9.GLOBAL_POSITION_INT.lat/1.0E7
//    //												, (double)MavLink_0_9.GLOBAL_POSITION_INT.lon/1.0E7
//    //												, (double)(Units.to("ft", MavLink_0_9.GLOBAL_POSITION_INT.alt/1000.0))
//    //												);
//    //		Velocity newVel = Velocity.mkVxyz((double)MavLink_0_9.GLOBAL_POSITION_INT.vy/100.0
//    //										, (double)MavLink_0_9.GLOBAL_POSITION_INT.vx/100.0
//    //										, (double)MavLink_0_9.GLOBAL_POSITION_INT.vz/100.0
//    //										);		// euclidian space for Stratway is lon = x  lat=y
//    //		m.s = newPos;   // should be current position
//    //		m.v = newVel;	// should be current velocity
//    m.s = fp.point(0).position();   // should be current position
//    m.v = Velocity.INVALID;		//fp.initialVelocity(0);	// should be current velocity
//    */
//    return m;
//  }
//
//
//  //=====================================================================================//
//  /**
//   * Decode a byte array containing one waypoint.
//   * @param input is the byte array to decode
//   * @return gov.nasa.larcfm.Util.NavPoint object containing the 4D Waypoint coordinates.
//   */
//  //=====================================================================================//
//  public NavPoint decodeWatchWPpacket(int offset, byte[] input) {
//
//    double lat, lon, alt;
//    //int wpIdx;
//    double time;
//    //byte[] bb = new byte[8];
//    //boolean latlon = true;
//    buf.clear();
//
//    int s = offset;
//    buf = ByteBuffer.wrap( byteCopy(input, s, dlen) );
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    lat   = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap( byteCopy(input, s, dlen) );
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    lon   = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap( byteCopy(input, s, dlen) );
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    alt   = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap( byteCopy(input, s, llen) );
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    time   = buf.getDouble();
//    buf.clear();
//    NavPoint np = new NavPoint(new Position(LatLonAlt.make(lat, lon, alt)), time);
//    //NavPoint np = new NavPoint(lat, lon, alt, time, latlon);
//    return np;
//  }
//
//
//  //=====================================================================================//
//  /**
//   * Encode a byte array containing one waypoint. Uses the SV message format for 
//   * XPlane consumption.  Does NOT includes time field.
//   * @param input is the lat, lon, alt of waypoint
//   * @return byte array.
//   */
//  //=====================================================================================//
//  public byte[] encodeXPWPpacket (String id, double lat, double lon, double alt)
//  {
//    // [CAM] Originally, the code used GlobalState.getSimTime(). Is this needed here?
//    return encodeSVpacket ("WAYPOINT", "Edge540T", lat, lon, alt, (double)0.0, (double)0.0, (double)0.0, 0 ,Icarous.getSimTime());
//  }
//
//  //=====================================================================================//
//  /**
//   * Encode a byte array containing one waypoint. Uses the Watch WP message format for 
//   * AirWatch/GndWatch consumption.  Includes time field.
//   * @param input is the lat, lon, alt of waypoint
//   * @return byte array.
//   */
//  //=====================================================================================//
//  public byte[] encodeWatchWPpacket (String id, String ACtype, double lat, double lon, double alt, long time)
//  {
//    //ByteBuffer msg = ByteBuffer.allocate(msgBufSize);
//    if (!buf.hasArray())  
//      buf = ByteBuffer.allocate(msgBufSize);
//    buf.clear();
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodeWPPacket: "
//        +"message byteOrder[" + buf.order().toString() + "]  "
//        );
//
//
//    buf.put(placeHolder).putDouble(lat).putDouble(lon).putDouble(alt).putLong(time);
//
//    byte[] packet = encodePacket(id, ACtype, WPmsg);
//
//    // add byte array size check here
//    //		if (msg.length != 64)  {
//    //			System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodePacket: Message is ["+msg.length+"] bytes.  Suppose to be [64] bytes!!!");
//    //		}
//
//    return packet;
//  }
//
//
//  //=====================================================================================//
//  /**
//   * Encode a byte array containing data from the buf.  returns null if buff is empty.
//   * @param input is the lat, lon, alt of waypoint
//   * @return byte array.
//   */
//  //=====================================================================================//
//  public byte[] encodePacket (String id, String ACtype, byte msgType)
//  {
//
//    if (!buf.hasArray()  || buf.position() == 0)
//    {
//      return null;
//    }
//
//    byte[] idRaw = id.getBytes();
//    //byte[] idFixed = byteCopy (idRaw,0,idFixedLimit);	// left justified padded with 0x00 if id less than idFixedLimit
//
//    // mark the ownship and actype flags for XPlane
//    byte ownShip = (byte)0x00;			// Ownship stays in xplane camera focal point
//    byte acType = (byte)0x01;			// actype codes are :: 0=edge, 1=GA, 2=transport
//    //if ( id.startsWith(Icarous.getCallSign()) ) [CAM]
//    {
//      ownShip = (byte)0x01;
//    }
//    if ( ACtype == "Edge540T" ) 
//    {
//      acType = (byte)0x00;		// actype codes are :: 0=edge, 1=GA, 2=transport
//    }
//    buf.put(ownShip).put(acType);
//
//    byte[] checksum = new byte[2];
//    checksum[0] = (byte)0xFF;  checksum[1] = (byte)0xFF;
//    buf.put(checksum);
//
//    buf.put(0,(byte)msgMarker);// msg header marker
//    buf.put(1,(byte)msgType);// msg type 0 is assigned to passing S & V
//    short len = (short) (buf.position()); //gets length of msg and puts in header. includes checksum in length
//    buf.put(2,(byte)(len & 0xff));
//    buf.put(3,(byte)((len >> 8) & 0xff));	
//
//
//
//    //<<<<<<<<<<<<<  end checksum section
//
//
//    // Extract byte array of only the filled content not the default 1024 bytes
//    byte[] msg = Arrays.copyOfRange(buf.array(), 0, buf.position());
//    buf.clear();
//
//    // prepend the header  //Moved to above checksum calculation to be a part of the checksum.
//    //msg[0] = (byte) msgMarker;	
//    //msg[1] = (byte) msgType;  
//    //short len = (short) msg.length; // length INCLUDES header byte of 97 :)
//    //buf.putShort(len);
//    //msg[2] = buf.get(0);
//    //msg[3] = buf.get(1);
//
//    // insert the acID
//    byteCopy (idRaw, 0, idRaw.length, msg, headerSize, idFixedLimit );
//
//    //>>>>>>>>>>>>  checksum section
//
//    short crc = calcChkSum( Arrays.copyOfRange(msg, 0, msg.length) );
//    msg[msg.length-2] = (byte)(crc & 0xff);
//    msg[msg.length-1] = (byte)((crc >> 8) & 0xff);
//    // ------------------------		
//
//    //		System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodePacket: "
//    //				+"msg len[" + msg.length + "]  "
//    //				+"msg type ["+msgType+"] "
//    //				+"ac["+id+"] "
//    //				//+"s["+lat+","+lon+","+alt+"]  "
//    //				+"bytes["+bytesToHex(msg)+"]"
//    //				);
//
//    return msg;
//
//  }
//
//
//  //=====================================================================================//
//  /**
//   * Parses serialized ID/pos/vel and update our info if the ID matches
//   * @param msg byte array of serialized info
//   * @return true if ID matches, msg is valid, and we update our info
//   */
//  //=====================================================================================//
//  public Message decodeSVpacket(byte[] msg)
//  {
//
//    double lat = 0, lon, alt, x, y,z = (double)0.0;
//    int data = 0;
//    //int type = -1;
//    double simTime = -1;
//    buf.clear();
//    int s = headerSize;
//    acID = new String(msg,s,idFixedLimit);
//    acID = stripTrailing0s(acID);
//
//    s += idFixedLimit;
//    buf = ByteBuffer.wrap(byteCopy(msg, s ,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    lat = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap(byteCopy(msg, s ,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    lon = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap(byteCopy(msg, s ,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    alt =  buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap(byteCopy(msg, s ,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    x   = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap(byteCopy(msg, s ,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    y   = buf.getDouble();
//
//    s += dlen;
//    buf = ByteBuffer.wrap(byteCopy(msg, s ,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    z   = buf.getDouble();
//
//    s+= dlen;
//    buf= ByteBuffer.wrap(byteCopy(msg,s,ilen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);
//    data = buf.getInt();
//
//    s+=ilen;
//    buf= ByteBuffer.wrap(byteCopy(msg,s,1));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);
//    //type = buf.get();// Byte.valueOf(buf.get()).intValue();
//
//    s+=1;
//    buf= ByteBuffer.wrap(byteCopy(msg,s,dlen));
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);
//    simTime = buf.getDouble();
//    svSimTime = simTime;
//
//    buf.clear();
//
//    // recreate the S & V
//    Message m = new Message();
//    /* [CAM] 
//    m.setType(Message.Type.STATE);		// Watch.AircraftCommunicationBasicIterative 
//    m.id = acID;
//    m.s = Position.makeLatLonAlt(lat, lon, alt);
//    m.v = Velocity.mkVxyz(x, y, z); // x y z are in m/s
//    //m.timeStamp = GlobalState.getSimTime();
//    m.timeStamp = simTime;
//    m.nacp = 11;
//    m.nacv = 4;
//    m.nic = 11;
//    m.data = data;
//    m.acTypeClass = type;
//    m.hbTime = Hbeat.hbTime;
//   */
//    //System.err.println("Edge-->IcarousCodec::decodeSVpacket: message "+ m);
//
//    System.out.println (Thread.currentThread().getName()
//        + "-->IcarousCodec::decodeSVpacket: "
//        + "raw len[" + msg.length+"] "
//        + "raw msg[" + msg + "] "
//        + "AC["+acID+"] "
//        + "Position[lat:"+lat+" lon:"+lon+" alt:"+alt+"] "
//        + "Velocity[ X:" + x +" Y:"+y  +" z:"+z  +"] "
//        + "Data["+data+"]" 
//        + "bytes["+bytesToInt(msg)+"] "
//
//        );
//    //		System.out.println (Thread.currentThread().getName()
//    //							+ "-->IcarousCodec::decodeSVpacket: Position message ["+acID+"] "
//    //							+ "Position["+Ts.toString15()+"] "
//    //							+ "Velocity["+Tv.toString15NP()+"]"
//    //							);
//
//    return m;
//
//  } 	// end parseSV function
//
//
//
//
//  //=====================================================================================//
//  /**
//   * Forms an SV message byte array of the input parameters and add the message header
//   * @param id Name of aircraft
//   * @param lat position latitude (deg)
//   * @param lon position longitude (deg)
//   * @param alt position altitude (ft)
//   * @param roll Orientation roll (for XPlane) or Velocity.x (for GndWatch simulator)
//   * @param pitch Orientation pitch (for XPlane) or Velocity.y (for GndWatch simulator)
//   * @param yaw Orientation yaw (for XPlane) or Velocity.z (for GndWatch simulator)
//   * @return byte array [header,id,lat,lon,alt,roll,pitch,yaw]  always 60 bytes
//   */
//  //=====================================================================================//
//  public byte[] encodeSVpacket (String id, String ACtype, double lat, double lon, double alt, double roll, double pitch, double yaw,int data, double simTime)
//  {
//
//    if (!buf.hasArray())  
//      buf = ByteBuffer.allocate(msgBufSize);
//    buf.clear();
//    buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//    //		System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodeSVPacket: asemble message"
//    //							+"byteOrder[" + buf.order().toString() + "]  "
//    //						  );
//    byte[] idRaw = id.getBytes();
//    byte[] idFixed = byteCopy (idRaw,0,idFixedLimit);	// left justified padded with 0x00 if id less than idFixedLimit
//    byte[] placeHolder = new byte[headerSize];	// make room for headerSize byte message header
//
//
//    buf.put(placeHolder).put(idFixed)
//    .putDouble(lat).putDouble(lon).putDouble( alt ) // encode S
//    .putDouble(roll).putDouble(pitch).putDouble(yaw).putInt(data).put((byte)Icarous.getTypeClass()).putDouble(simTime);		// encode V
//
//    //System.err.println("Edge-->IcarousCodec::encodeSVpacket acType "+ (byte)AcType.getAcType(ACtype).getTypeClass());
//
//    byte ownShip = (byte)0x00;
//    //if ( id.startsWith(Icarous.getCallSign())) [CAM]
//      ownShip = (byte)0x01;
//    byte acType = (byte)0x01;
//    if ( ACtype == "Edge540T" ) 
//      acType = (byte)0x00;		// actype codes are :: 0=edge, 1=GA, 2=transport
//    //		else if ( ACtype == "default" ) 
//    //			acType = (byte)0x01;
//
//
//    byte[] checksum = new byte[2];
//    checksum[0] = (byte)0xFF;  checksum[1] = (byte)0xFF;
//
//    buf.put(ownShip).put(acType).put(checksum);
//
//    buf.put(0,(byte)msgMarker);// msg header marker
//    buf.put(1,(byte)SVmsg); // msg type 0 is assigned to passing S & V
//    short len = (short) (buf.position()); //gets length of msg and puts in header. includes checksum in length
//    buf.put(2,(byte)(len & 0xff));
//    buf.put(3,(byte)((len >> 8) & 0xff));
//
//
//
//    //>>>>>>>>>>>>  checksum section
//    short crc = calcChkSum( Arrays.copyOfRange(buf.array(), 0, buf.position()) );
//    buf.putShort(buf.position()-2,crc);
//    //<<<<<<<<<<<<<  end checksum section		
//
//    // Extract byte array of only the filled content not the default 1024 bytes
//    byte[] msg = Arrays.copyOfRange(buf.array(), 0, buf.position());
//    buf.clear();
//
//    // prepend the header
//    //msg[0] = (byte) msgMarker;	
//    //msg[1] = (byte) SVmsg; 
//    //short len = (short) msg.length; // length INCLUDES header byte of 97 :)
//    //buf.putShort(len);
//    //msg[2] = buf.get(0);
//    //msg[3] = buf.get(1);
//
//
//    if (msg.length != 77)  {
//      System.out.println(Thread.currentThread().getName()+"-->IcarousCodec::encodeSVPacket: Message is ["+msg.length+"] bytes.  Suppose to be [69] bytes!!!");
//    }
//    System.out.println(Thread.currentThread().getName()
//        + "-->IcarousCodec::encodeSVPacket: "
//        + "AC["+id+"] "
//        + "s["+lat+","+lon+","+alt+"] "
//        + "v["+roll+","+pitch+","+yaw+"] "
//        + "out msg len[" + msg.length + "] "
//        + "raw msg[ " + msg+"] "
//        + "bytes["+bytesToInt(msg)+"] "
//        );
//
//
//    return msg;
//
//  }	// end encodeSVPacket function
//
//
//  //=====================================================================================//
//  /**
//   * Encode a message to notify all parties to start their flight plans.
//   * */
//  //=====================================================================================//
//  //	public byte[] encodeScenarioStart(String id, String ACtype, double time) {
//  //		//ByteBuffer msg = ByteBuffer.allocate(msgBufSize);
//  //		if (!buf.hasArray())  
//  //			buf = ByteBuffer.allocate(msgBufSize);
//  //		buf.clear();
//  //		buf.order(java.nio.ByteOrder.LITTLE_ENDIAN);		// XPlane speaks little endian!
//  //		
//  //		
//  //		buf.put(placeHolder).putDouble(time);
//  //
//  //		byte[] packet = encodePacket(id, ACtype, SSmsg);
//  //		
//  //		return packet;
//  //	} 
//  //
//  //	
//  //	//=====================================================================================//
//  //	/**
//  //	 * Decode a notice from GndWatch to start flying to the flight plans.
//  //	 * */
//  //	//=====================================================================================//
//  //	public byte[] decodeScenarioStart() {
//  //		// TODO Auto-generated method stub
//  //		return null;
//  //	} 
//  //
//
//  //=====================================================================================//
//  /**
//   * Quick byte copy.  NO ERROR CHECKING
//   * @param vec Original stuff.
//   * @return copy of original stuff
//   */
//  //=====================================================================================//
//  public Message packetToWatchMsg (byte[] packet) 
//  {
//    Message outMsg;
//    if (packet == null) return null;
//    int msgType = (int)packet[msgTypeIdx];	// msg type is assumed to be second byte
//    switch ( msgType ) {
//    case ((int)SVmsg):	// call SV msg parser
//      //System.out.println(Thread.currentThread().getName()+"--> IcarousCodec::packetToWatchMsg: Convert SV packet to msg ...");
//      outMsg = decodeSVpacket ( packet ); // don't pass header		// code for byte array immediate decode to message object in serial thread
//    //m.timeStamp = GlobalState.getSimTime();
//    return outMsg;
//    case ((int)WPmsg):	 // call WP msg parser
//      return null;
//    //			case ((int)HBmsg):	// call heartbeat msg parser
//    //				//decodeHBpacket (edgeStat, byteCopy(serBuff, markerIdx+headerSize, msgSize-headerSize ));
//    //				return null;
//    case ((int)FPmsg):	// call heartbeat msg parser
//      outMsg = decodeFPPacket ( packet );
//    return outMsg;
//
//    default:
//      break;
//    }
//
//    return null;
//
//  }
//
//
//
//
//
//}    // END IcarousCodec class
