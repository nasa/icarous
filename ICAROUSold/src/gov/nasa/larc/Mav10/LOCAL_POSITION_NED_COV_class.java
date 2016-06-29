/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//MavLink 1.0

//package gov.nasa.larc.AP;
//import gov.nasa.larc.serial.Loggable;

/**
Message ID: LOCAL_POSITION_NED_COV(64)
--------------------------------------
%%~ The filtered local position (e.g. fused computer vision and accelerometers). Coordinate 
%%~ frame is right-handed, Z-axis down (aeronautical frame, NED / north-east-down 
%%~ convention)
--------------------------------------
*/
public class LOCAL_POSITION_NED_COV_class //implements Loggable
{
	public static final int msgID = 64;
	public long	 time_utc;		 	// Timestamp (microseconds since UNIX epoch) in UTC. 0 for unknown. Commonly filled by the precision time source of a GPS receiver.
	public long	 time_boot_ms;	 	// Timestamp (milliseconds since system boot)
	public float	 x;				 	// X Position
	public float	 y;				 	// Y Position
	public float	 z;				 	// Z Position
	public float	 vx;				 	// X Speed
	public float	 vy;				 	// Y Speed
	public float	 vz;				 	// Z Speed
	public float[]	 covariance = new float[36];			// Covariance matrix (first six entries are the first ROW, next six entries are the second row, etc.)
	public short	 estimator_type;	 	// Class id of the estimator this estimate originated from.

	private packet rcvPacket;
	private packet sndPacket;

	public LOCAL_POSITION_NED_COV_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LOCAL_POSITION_NED_COV_class(LOCAL_POSITION_NED_COV_class o)
	{
		time_utc = o.time_utc;
		time_boot_ms = o.time_boot_ms;
		x = o.x;
		y = o.y;
		z = o.z;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
		covariance = o.covariance;
		estimator_type = o.estimator_type;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 144, 1};
			// int[] javLen = {8, 8, 4, 4, 4, 4, 4, 4, 144, 2};

			time_utc			= rcvPacket.getLong();
			time_boot_ms		= rcvPacket.getLongI();
			x				= rcvPacket.getFloat();
			y				= rcvPacket.getFloat();
			z				= rcvPacket.getFloat();
			vx				= rcvPacket.getFloat();
			vy				= rcvPacket.getFloat();
			vz				= rcvPacket.getFloat();
			rcvPacket.getByte(covariance, 0, 36);
			estimator_type	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_utc
					 ,time_boot_ms
					 ,x
					 ,y
					 ,z
					 ,vx
					 ,vy
					 ,vz
					 ,covariance
					 ,estimator_type
					 );
	}

	public byte[] encode(
						 long v_time_utc
						,long v_time_boot_ms
						,float v_x
						,float v_y
						,float v_z
						,float v_vx
						,float v_vy
						,float v_vz
						,float[] v_covariance
						,short v_estimator_type
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 144, 1};
		// int[] javLen = {8, 8, 4, 4, 4, 4, 4, 4, 144, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_utc);	// Add "time_utc" parameter
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter
		sndPacket.putFloat(v_vx);	// Add "vx" parameter
		sndPacket.putFloat(v_vy);	// Add "vy" parameter
		sndPacket.putFloat(v_vz);	// Add "vz" parameter
		sndPacket.putByte(v_covariance,0,36);	// Add "covariance" parameter
		sndPacket.putByteS(v_estimator_type);	// Add "estimator_type" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LOCAL_POSITION_NED_COV_time_utc"
 				+ ", LOCAL_POSITION_NED_COV_time_boot_ms"
 				+ ", LOCAL_POSITION_NED_COV_x"
 				+ ", LOCAL_POSITION_NED_COV_y"
 				+ ", LOCAL_POSITION_NED_COV_z"
 				+ ", LOCAL_POSITION_NED_COV_vx"
 				+ ", LOCAL_POSITION_NED_COV_vy"
 				+ ", LOCAL_POSITION_NED_COV_vz"
 				+ ", LOCAL_POSITION_NED_COV_covariance"
 				+ ", LOCAL_POSITION_NED_COV_estimator_type"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_utc
 				+ ", " + time_boot_ms
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
 				+ ", " + covariance
 				+ ", " + estimator_type
				);
		return param;
	}
}
