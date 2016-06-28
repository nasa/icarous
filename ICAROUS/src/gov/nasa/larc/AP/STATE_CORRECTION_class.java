/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: STATE_CORRECTION(64)
--------------------------------------
%%~ Corrects the systems state by adding an error correction term to the position and 
%%~ velocity, and by rotating the attitude by a correction angle.
--------------------------------------
*/
public class STATE_CORRECTION_class implements Loggable
{
	public static final int msgID = 64;
	public float	 xErr;	 	// x position error
	public float	 yErr;	 	// y position error
	public float	 zErr;	 	// z position error
	public float	 rollErr;	 	// roll error (radians)
	public float	 pitchErr; 	// pitch error (radians)
	public float	 yawErr;	 	// yaw error (radians)
	public float	 vxErr;	 	// x velocity
	public float	 vyErr;	 	// y velocity
	public float	 vzErr;	 	// z velocity

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public STATE_CORRECTION_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public STATE_CORRECTION_class(STATE_CORRECTION_class o)
	{
		xErr = o.xErr;
		yErr = o.yErr;
		zErr = o.zErr;
		rollErr = o.rollErr;
		pitchErr = o.pitchErr;
		yawErr = o.yawErr;
		vxErr = o.vxErr;
		vyErr = o.vyErr;
		vzErr = o.vzErr;
	}

	public boolean parse(byte[] b)
	{
		return parse(b, false);
	}

	public boolean parse(byte[] b, boolean valid)
	{
		rcvPacket.load(b);

		boolean pstatus = valid || rcvPacket.isPacket();
		if (pstatus)
		{
			rcvPacket.updateSeqNum();

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4};

			xErr		= rcvPacket.getFloat();
			yErr		= rcvPacket.getFloat();
			zErr		= rcvPacket.getFloat();
			rollErr	= rcvPacket.getFloat();
			pitchErr	= rcvPacket.getFloat();
			yawErr	= rcvPacket.getFloat();
			vxErr	= rcvPacket.getFloat();
			vyErr	= rcvPacket.getFloat();
			vzErr	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  xErr
					 ,yErr
					 ,zErr
					 ,rollErr
					 ,pitchErr
					 ,yawErr
					 ,vxErr
					 ,vyErr
					 ,vzErr
					 );
	}

	public byte[] encode(
						 float v_xErr
						,float v_yErr
						,float v_zErr
						,float v_rollErr
						,float v_pitchErr
						,float v_yawErr
						,float v_vxErr
						,float v_vyErr
						,float v_vzErr
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_xErr);	// Add "xErr" parameter
		sndPacket.putFloat(v_yErr);	// Add "yErr" parameter
		sndPacket.putFloat(v_zErr);	// Add "zErr" parameter
		sndPacket.putFloat(v_rollErr);	// Add "rollErr" parameter
		sndPacket.putFloat(v_pitchErr);	// Add "pitchErr" parameter
		sndPacket.putFloat(v_yawErr);	// Add "yawErr" parameter
		sndPacket.putFloat(v_vxErr);	// Add "vxErr" parameter
		sndPacket.putFloat(v_vyErr);	// Add "vyErr" parameter
		sndPacket.putFloat(v_vzErr);	// Add "vzErr" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", STATE_CORRECTION_xErr"
 				+ ", STATE_CORRECTION_yErr"
 				+ ", STATE_CORRECTION_zErr"
 				+ ", STATE_CORRECTION_rollErr"
 				+ ", STATE_CORRECTION_pitchErr"
 				+ ", STATE_CORRECTION_yawErr"
 				+ ", STATE_CORRECTION_vxErr"
 				+ ", STATE_CORRECTION_vyErr"
 				+ ", STATE_CORRECTION_vzErr"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + xErr
 				+ ", " + yErr
 				+ ", " + zErr
 				+ ", " + rollErr
 				+ ", " + pitchErr
 				+ ", " + yawErr
 				+ ", " + vxErr
 				+ ", " + vyErr
 				+ ", " + vzErr
				);
		return param;
	}
}
