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
//import java.nio.ByteBuffer;
//import java.util.Arrays;
//import java.util.Vector;
//
///**
// * @author cquach
// *
// */
//public class Codec {
//
////=========================================================================================================//
//// DATA Section
////=========================================================================================================//
//	public static byte msgMarker = (byte)97;
//	public static int msgTypeIdx = 1;		// index location of message type byte.
//	public static int headerSize = 4; 		// full size of header in bytes
//	public static int idStartIdx = 4;		   // fixed length of the aircraft ID for SV messages (msg0)
//	public static int idFixedLimit = 8;		   // fixed length of the aircraft ID for SV messages (msg0)
//	public static int llen = 8;	// 8 bytes to store java long type
//	public static int ilen = 4;	// 4 bytes to store java int type
//	public static int slen = 2;	// 4 bytes to store java short type
//	public static int dlen = 8;	// 8 bytes to store java double type
//
//	
//	int msgBufSize = 1000;
//	ByteBuffer buf = ByteBuffer.allocate(msgBufSize);
//	byte[] placeHolder = new byte[headerSize+idFixedLimit];	// make room for headerSize byte message header
//
//	// Decoded data available for external use.  Are we assuming data is most recent?
//	//public String acIDStr = null;
//	//public String acTypeStr = null;
//	//public double hbTime = 0;	// local system clock time in sec
//	//public double simTime = 0;	// simulator clock in seconds
//	
//	
//	
////=========================================================================================================//
//// Methods Section
////=========================================================================================================//
//
//
//	//=====================================================================================//
//	/**
//	 * Encode a byte array containing data from the buf.  returns null if buff is empty.
//	 * @param input is the lat, lon, alt of waypoint
//	 * @return byte array.
//	 */
//	//=====================================================================================//
//	public byte[] encodePacket (String id, String ACtype, byte msgType)
//	{
//		
//		if (!buf.hasArray()  || buf.position() == 0)
//		{
//			return null;
//		}
//
//		byte[] idRaw = id.getBytes();
//		//byte[] idFixed = byteCopy (idRaw,0,idFixedLimit);	// left justified padded with 0x00 if id less than idFixedLimit
//
//		// mark the ownship and actype flags for XPlane
//		byte ownShip = (byte)0x00;			// Ownship stays in xplane camera focal point
//		byte acType = (byte)0x01;			// actype codes are :: 0=edge, 1=GA, 2=transport
//		if ( id == "N802RE") {
//			ownShip = (byte)0x01;
//		}
//		if ( ACtype == "Edge540T" ) 
//			acType = (byte)0x00;		// actype codes are :: 0=edge, 1=GA, 2=transport
////		else if ( ACtype == "default" ) 
////			acType = (byte)0x01;
//
//
//		byte[] checksum = new byte[2];
//		checksum[0] = (byte)0xFF;  checksum[1] = (byte)0xFF;
//
//		buf.put(ownShip).put(acType).put(checksum);
//		
//		buf.put(0,(byte)IcarousCodec.msgMarker);// msg header marker
//		buf.put(1,(byte)msgType);// msg type 0 is assigned to passing S & V
//		short len = (short) (buf.position()); //gets length of msg and puts in header. includes checksum in length
//		buf.put(2,(byte)(len & 0xff));
//		buf.put(3,(byte)((len >> 8) & 0xff));	
//		
//		
//		
//		
//		//<<<<<<<<<<<<<  end checksum section
//		
//		// Extract byte array of only the filled content not the default 1024 bytes
//		byte[] msg = Arrays.copyOfRange(buf.array(), 0, buf.position());
//		buf.clear();
//
//		// prepend the header
//		//msg[0] = (byte) IcarousCodec.msgMarker;	// msg header marker
//		//msg[1] = (byte) msgType;  // msg type 0 is assigned to passing S & V
//		//short len = (short) msg.length; // length INCLUDES header byte of 97 :)
//		//buf.putShort(len);
//		//msg[2] = buf.get(0);
//		//msg[3] = buf.get(1);
//		
//		// insert the acID
//		byteCopy (idRaw, 0, idRaw.length, msg, IcarousCodec.headerSize, IcarousCodec.idFixedLimit );
//		
//		//>>>>>>>>>>>>  checksum section
//
//		short crc = calcChkSum( Arrays.copyOfRange(msg, 0, msg.length) );
//		msg[msg.length-2] = (byte)(crc & 0xff);
//		msg[msg.length-1] = (byte)((crc >> 8) & 0xff);
//		// ------------------------		
//		
//		return msg;
//		
//	}	// end encodePacket() method
//
//	
//	public String stripTrailing0s (String inStr)
//	{
//		String outStr = inStr;
//		if  ( (inStr != null) && (inStr.length() > 0))		// strip everything after first blank. - no blanks in the callsign!
//		{
//			int k = inStr.indexOf(0);
//			if (k > 0)
//			{
//				outStr = inStr.substring(0, k);
//			}
//		}
//		return outStr;
//		
//	
//	}
//	
//	
//	//=====================================================================================//
//	/**
//	 * Quick byte copy.  NO ERROR CHECKING
//	 * @param vec Original stuff.
//	 * @return copy of original stuff
//	 */
//	//=====================================================================================//
//	public byte[] Vector2Bytes (Vector<Byte> vec, int sIdx, int size) 
//	{
//		byte[] copy = new byte[size];
//	
//		for (int j=0;  j < size-1;  j++ ) {
//			copy[j] = vec.get(sIdx+j);
//		}
//		
//		return copy;
//			
//	}
//	
//	//=====================================================================================//
//	/**
//	 * Byte copy.  If size is bigger than input, pad with 0x00  
//	 * @param vec Original stuff.
//	 * @return copy of original stuff
//	 */
//	//=====================================================================================//
//	public static byte[] byteCopy (byte[] vec, int sIdx, int size) 
//	{
//		byte[] copy = new byte[size];
//		int srcSize = vec.length;
//		
//		for (int j=0;  j < size;  j++ ) {
//			if ( (sIdx+j) < srcSize ) { // length check
//				copy[j] = vec[sIdx+j];
//			} else {	// pad with 0x00
//				copy[j] = (byte)0x00;
//			}
//		}
//		
//		return copy;
//			
//	}
//
//	
//	//=====================================================================================//
//	/**
//	 * Copy bytes from src to dest.  Will only from src up to the dSize number of bytes. 
//	 * @param vec Original stuff.
//	 * @return copy of original stuff
//	 */
//	//=====================================================================================//
//	public void byteCopy (byte[] src, int sidx, int sSize, byte[] dest, int didx, int dSize) 
//	{
//		if (   src.length == 0
//			|| dest.length == 0
//				)
//			{
//				return;	// can't do the copy
//			}
//			
//		for (int j=0;  j < dSize;  j++ ) {
//			if ( j < sSize ) { // length check
//				dest[didx+j] = src[sidx+j];
//			} else {	// pad with 0x00
//				dest[didx+j] = (byte)0x00;
//			}
//		}
//					
//	}
//
//
//	//=====================================================================================//
//	/**
//	 * Quick byte remove.  NO ERROR CHECKING
//	 * @param vec Original stuff.
//	 * @return copy of original stuff
//	 */
//	//=====================================================================================//
//	public void byteRemove (Vector<Byte> vec, int sIdx, int size) 
//	{
//		//byte[] copy = new byte[size];
//		
//		//System.out.println ("IcarousCodec::byteRemove: removing ["+size+"] from vector of ["+vec.size()+"]");
//		try {
//			for (int j=sIdx;  j <= size;  j++ ) {
//					vec.removeElementAt(j);		// somthing very strange about vector indexing??
//			}			
//		}  catch (Exception  e) { 
//			return;
//		}
//	}
//	
//	
//	
//	//=====================================================================================//
//	/**
//	 * convert a byte array into a HEX string
//	 * */
//	//=====================================================================================//
//	public static String bytesToHex (byte[] bytes) 
//	{     
//		final char[] hexArray = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};     
//		char[] hexChars = new char[bytes.length * 2];     
//		int v;     
//		for ( int j = 0; j < bytes.length; j++ ) 
//		{         
//			v = bytes[j] & 0xFF;         
//			hexChars[j * 2] = hexArray[v >>> 4];         
//			hexChars[j * 2 + 1] = hexArray[v & 0x0F];     
//		}     
//		return new String(hexChars); 
//	} 
//	
//	//=====================================================================================//
//	/**
//	 * convert a byte array into a comma  string
//	 * */
//	//=====================================================================================//
//	public static String bytesToInt (byte[] bytes) 
//	{     
//		String out = new String();
//		out += (int)bytes[0];
//		for ( int j = 1; j < bytes.length; j++ ) 
//		{         
//			out += ","+(int)bytes[j];         
//		}     
//		return out; 
//	}
//	
//	/**
//	 * 
//	 * @param p byte array of serialized info
//	 * @return True if valid crc is attatched to message. False if invalid.
//	 */
//	
//	public static boolean isValidChkSum(byte[] p)
//	{
//		ByteBuffer pktb = ByteBuffer.wrap(p);
//		pktb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
//		short pCRC = pktb.getShort(p.length-2);
//		short nCRC = calcChkSum(p);
//		return (boolean)(pCRC==nCRC);
//	}
//	
//	/**
//	 * 
//	 * @param p byte array of serialized info
//	 * @return a crc value represented as a short
//	 */
//	public static short calcChkSum(byte[] p)
//	{
//	    int N = p.length;
//	    byte b  = 0x00;
//	    byte ch = 0x00;
//	    short crc  = (short) 0xffff;
//	    short tempcrc = 0x0000;
//	    for (int i=1; i<N-2; i++)
//	    {
//	        b = p[i];
//	        ch = (byte) (b  ^  ((byte)(crc & 0x00ff) ))  ;
//	        ch = (byte) ((ch ^ (ch << 4) ));
//	        tempcrc = (short) (crc >> 8);
//	        tempcrc = (short) (tempcrc & 0x00ff);
//	        short a1 = (short) ((short)ch << 8);
//	        short a2 = (short) (tempcrc ^ a1);
//	        short a3 = (short) ((short)ch << 3);
//	        short a3a = (short) (a3 & 0x07ff);
//	        short a4 = (short) (a2 ^ a3a);
//	        short a5 = (short) ((short)ch >> 4);
//	        short a6 = (short) ((short) a5 & 0x000f);
//	        crc = (short) (a4 ^ a6);
//	    }
//	    return crc;
//	}
//	
//	
//	
////=========================================================================================================//
//// MAIN for unit testing
////=========================================================================================================//
//	//===================================================================//
//	/**
//	 * @param args
//	 */
//	//===================================================================//
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//
//	}
//
//}
