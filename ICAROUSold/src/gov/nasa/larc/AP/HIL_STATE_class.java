/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: HIL_STATE(67)
--------------------------------------
%%~ This packet is useful for high throughput                 applications such as hardware 
%%~ in the loop simulations.             
--------------------------------------
*/
public class HIL_STATE_class implements Loggable
{
	public static final int msgID = 67;
	public long	 usec;		 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public float	 roll;		 	// Roll angle (rad)
	public float	 pitch;		 	// Pitch angle (rad)
	public float	 yaw;			 	// Yaw angle (rad)
	public float	 rollspeed;	 	// Roll angular speed (rad/s)
	public float	 pitchspeed;	 	// Pitch angular speed (rad/s)
	public float	 yawspeed;	 	// Yaw angular speed (rad/s)
	public int		 lat;			 	// Latitude, expressed as * 1E7
	public int		 lon;			 	// Longitude, expressed as * 1E7
	public int		 alt;			 	// Altitude in meters, expressed as * 1000 (millimeters)
	public short	 vx;			 	// Ground X Speed (Latitude), expressed as m/s * 100
	public short	 vy;			 	// Ground Y Speed (Longitude), expressed as m/s * 100
	public short	 vz;			 	// Ground Z Speed (Altitude), expressed as m/s * 100
	public short	 xacc;		 	// X acceleration (mg)
	public short	 yacc;		 	// Y acceleration (mg)
	public short	 zacc;		 	// Z acceleration (mg)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public HIL_STATE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public HIL_STATE_class(HIL_STATE_class o)
	{
		usec = o.usec;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		rollspeed = o.rollspeed;
		pitchspeed = o.pitchspeed;
		yawspeed = o.yawspeed;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
		xacc = o.xacc;
		yacc = o.yacc;
		zacc = o.zacc;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2};

			usec			= rcvPacket.getLong();
			roll			= rcvPacket.getFloat();
			pitch		= rcvPacket.getFloat();
			yaw			= rcvPacket.getFloat();
			rollspeed	= rcvPacket.getFloat();
			pitchspeed	= rcvPacket.getFloat();
			yawspeed		= rcvPacket.getFloat();
			lat			= rcvPacket.getInt();
			lon			= rcvPacket.getInt();
			alt			= rcvPacket.getInt();
			vx			= rcvPacket.getShort();
			vy			= rcvPacket.getShort();
			vz			= rcvPacket.getShort();
			xacc			= rcvPacket.getShort();
			yacc			= rcvPacket.getShort();
			zacc			= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  usec
					 ,roll
					 ,pitch
					 ,yaw
					 ,rollspeed
					 ,pitchspeed
					 ,yawspeed
					 ,lat
					 ,lon
					 ,alt
					 ,vx
					 ,vy
					 ,vz
					 ,xacc
					 ,yacc
					 ,zacc
					 );
	}

	public byte[] encode(
						 long v_usec
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_rollspeed
						,float v_pitchspeed
						,float v_yawspeed
						,int v_lat
						,int v_lon
						,int v_alt
						,short v_vx
						,short v_vy
						,short v_vz
						,short v_xacc
						,short v_yacc
						,short v_zacc
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_rollspeed);	// Add "rollspeed" parameter
		sndPacket.putFloat(v_pitchspeed);	// Add "pitchspeed" parameter
		sndPacket.putFloat(v_yawspeed);	// Add "yawspeed" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
		sndPacket.putShort(v_vx);	// Add "vx" parameter
		sndPacket.putShort(v_vy);	// Add "vy" parameter
		sndPacket.putShort(v_vz);	// Add "vz" parameter
		sndPacket.putShort(v_xacc);	// Add "xacc" parameter
		sndPacket.putShort(v_yacc);	// Add "yacc" parameter
		sndPacket.putShort(v_zacc);	// Add "zacc" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HIL_STATE_usec"
 				+ ", HIL_STATE_roll"
 				+ ", HIL_STATE_pitch"
 				+ ", HIL_STATE_yaw"
 				+ ", HIL_STATE_rollspeed"
 				+ ", HIL_STATE_pitchspeed"
 				+ ", HIL_STATE_yawspeed"
 				+ ", HIL_STATE_lat"
 				+ ", HIL_STATE_lon"
 				+ ", HIL_STATE_alt"
 				+ ", HIL_STATE_vx"
 				+ ", HIL_STATE_vy"
 				+ ", HIL_STATE_vz"
 				+ ", HIL_STATE_xacc"
 				+ ", HIL_STATE_yacc"
 				+ ", HIL_STATE_zacc"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + usec
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + rollspeed
 				+ ", " + pitchspeed
 				+ ", " + yawspeed
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
 				+ ", " + xacc
 				+ ", " + yacc
 				+ ", " + zacc
				);
		return param;
	}
}
