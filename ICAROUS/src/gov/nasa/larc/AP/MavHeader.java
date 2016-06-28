/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import java.nio.ByteBuffer;

public class MavHeader
{
	//  {  0,   ,   ,   ,   ,   ,   ,   ,   ,   , 10,   ,   ,   ,   ,   ,   ,   ,   ,   , 20,   ,   ,   ,   ,   ,   ,   ,   ,   , 30,   ,   ,   ,   ,   ,   ,   ,   ,   , 40,   ,   ,   ,   ,   ,   ,   ,   ,   , 50,   ,   ,   ,   ,   ,   ,   ,   ,   , 60,   ,   ,   ,   ,   ,   ,   ,   ,   , 70,   ,   ,   ,   ,   ,   ,   ,   ,   , 80,   ,   ,   ,   ,   ,   ,   ,   ,   , 90,   ,   ,   ,   ,   ,   ,   ,   ,   ,100,   ,   ,   ,   ,   ,   ,   ,   ,   ,110,   ,   ,   ,   ,   ,   ,   ,   ,   ,120,   ,   ,   ,   ,   ,   ,   ,   ,   ,130,   ,   ,   ,   ,   ,   ,   ,   ,   ,140,   ,   ,   ,   ,   ,   ,   ,   ,   ,150,   ,   ,   ,   ,   ,   ,   ,   ,   ,160,   ,   ,   ,   ,   ,   ,   ,   ,   ,170,   ,   ,   ,   ,   ,   ,   ,   ,   ,180,   ,   ,   ,   ,   ,   ,   ,   ,   ,190,   ,   ,   ,   ,   ,   ,   ,   ,   ,200,   ,   ,   ,   ,   ,   ,   ,   ,   ,210,   ,   ,   ,   ,   ,   ,   ,   ,   ,220,   ,   ,   ,   ,   ,   ,   ,   ,   ,230,   ,   ,   ,   ,   ,   ,   ,   ,   ,240,   ,   ,   ,   ,   ,   ,   ,   ,   ,250,   ,   ,   ,   ,   };
	public static final byte[] MsgLengthArray = {  5,  4,  8, 14,  8, 28,  3, 32,  0,  2,  3,  2,  2,  0,  0,  0,  0,  0,  0,  0, 19,  2, 23, 21,  0, 37, 26,101, 26, 16, 32, 32, 37, 32, 11, 17, 17, 16, 18, 36,  4,  4,  2,  2,  4,  2,  2,  3, 14, 12, 18, 16,  8, 27, 25, 18, 18, 24, 24,  0,  0,  0, 26, 16, 36,  5,  6, 56, 26, 21, 18,  0,  0, 18, 20, 20,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40, 72, 12, 33, 11,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0, 42,  8,  4, 12, 15, 13,  6, 15, 14,  0, 12,  3,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30, 14, 14, 51,  5};


	static final short HEADER_SIZE = 6;
	static final short CHKSUM_SIZE = 2;
	static final byte  MAV_MARKER  = (byte) 85;
	static final byte  BEAGLE_ID   = (byte)103;
	static final byte  AP_ID       = (byte)  1;
	static final byte  XBEE_ID     = (byte)255;
	static final byte  COMP_ID     = (byte)  1;

	boolean vHeader;
	byte msgHead;
	byte msgLen;
	byte msgSeq;
	byte msgSid;
	byte msgCid;
	byte msgGid;

	static byte RcvSeq;
	static byte PreRcvSeq;
	static byte SndSeq = 0;

	public MavHeader(byte[] hdr)
	{
		vHeader = isValid(hdr);
		if (!vHeader)	return;

		msgHead = hdr[0];
		msgLen  = hdr[1];
		msgSeq  = hdr[2];
		msgSid  = hdr[3];
		msgCid  = hdr[4];
		msgGid  = hdr[5];
		PreRcvSeq = RcvSeq;
		RcvSeq    = msgSeq;
	}

	public MavHeader(byte mid)
	{
		vHeader = true;
		msgHead = MAV_MARKER;
		msgLen  = MsgLengthArray[getMsgID(mid)];
		msgSeq  = SndSeq++;
		msgSid  = BEAGLE_ID;
		msgCid  = COMP_ID;
		msgGid  = mid;
	}

	public boolean isValid(byte[] hdr)
	{
		if (hdr.length<HEADER_SIZE)	return(false);
		return ((hdr[0]==MAV_MARKER)
		      &&(hdr[1]==MsgLengthArray[getMsgID(hdr)])
		      &&(hdr[3]==AP_ID)||(hdr[3]==XBEE_ID)||(hdr[3]==BEAGLE_ID)
		      &&(hdr[4]==COMP_ID));
	}

	public boolean isValid()
	{
		return(vHeader);
	}

	public byte[] getHeader()
	{
		byte[] hdr = {msgHead,msgLen,msgSeq,msgSid,msgCid,msgGid};
		return(hdr);
	}

	public short getMsgHead(byte[] hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr[0]).getShort(0));
	}

	public short getMsgHead()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,msgHead).getShort(0));
	}

	public short getMsgLength(byte[] hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr[1]).getShort(0));
	}

	public short getMsgLength()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,msgLen).getShort(0));
	}

	public short getMsgLength(short idx)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,MsgLengthArray[idx]).getShort(0));
	}

	public short getFullLength()
	{
		return (short)(getMsgLength()+HEADER_SIZE+CHKSUM_SIZE);
	}

	public short getFullLength(short idx)
	{
		return (short)(getMsgLength(idx)+HEADER_SIZE+CHKSUM_SIZE);
	}

	public short getMsgSeq(byte[] hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr[2]).getShort(0));
	}

	public short getMsgSeq()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,msgSeq).getShort(0));
	}

	public short getMsgSysID(byte[] hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr[3]).getShort(0));
	}

	public short getMsgSysID()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,msgSid).getShort(0));
	}

	public short getMsgCompID(byte[] hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr[4]).getShort(0));
	}

	public short getMsgCompID()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,msgCid).getShort(0));
	}

	public short getMsgID(byte[] hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr[5]).getShort(0));
	}

	public short getMsgID()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,msgGid).getShort(0));
	}

	public short getMsgID(byte hdr)
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,hdr).getShort(0));
	}

	public short getRcvSeq()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,RcvSeq).getShort(0));
	}

	public short getPreRcvSeq()
	{
		ByteBuffer BB = ByteBuffer.allocate(2);
		return (BB.putShort(0,(short)0).put(0,PreRcvSeq).getShort(0));
	}

}
