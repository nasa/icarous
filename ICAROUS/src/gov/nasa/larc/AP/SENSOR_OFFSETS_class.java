/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SENSOR_OFFSETS(150)
--------------------------------------
%%~ Offsets and calibrations values for hardware         sensors. This makes it easier 
%%~ to debug the calibration process.
--------------------------------------
*/
public class SENSOR_OFFSETS_class implements Loggable
{
	public static final int msgID = 150;
	public short	 mag_ofs_x;		 	// magnetometer X offset
	public short	 mag_ofs_y;		 	// magnetometer Y offset
	public short	 mag_ofs_z;		 	// magnetometer Z offset
	public float	 mag_declination;	 	// magnetic declination (radians)
	public int		 raw_press;		 	// raw pressure from barometer
	public int		 raw_temp;		 	// raw temperature from barometer
	public float	 gyro_cal_x;		 	// gyro X calibration
	public float	 gyro_cal_y;		 	// gyro Y calibration
	public float	 gyro_cal_z;		 	// gyro Z calibration
	public float	 accel_cal_x;		 	// accel X calibration
	public float	 accel_cal_y;		 	// accel Y calibration
	public float	 accel_cal_z;		 	// accel Z calibration

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SENSOR_OFFSETS_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SENSOR_OFFSETS_class(SENSOR_OFFSETS_class o)
	{
		mag_ofs_x = o.mag_ofs_x;
		mag_ofs_y = o.mag_ofs_y;
		mag_ofs_z = o.mag_ofs_z;
		mag_declination = o.mag_declination;
		raw_press = o.raw_press;
		raw_temp = o.raw_temp;
		gyro_cal_x = o.gyro_cal_x;
		gyro_cal_y = o.gyro_cal_y;
		gyro_cal_z = o.gyro_cal_z;
		accel_cal_x = o.accel_cal_x;
		accel_cal_y = o.accel_cal_y;
		accel_cal_z = o.accel_cal_z;
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

			// int[] mavLen = {2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4};

			mag_ofs_x		= rcvPacket.getShort();
			mag_ofs_y		= rcvPacket.getShort();
			mag_ofs_z		= rcvPacket.getShort();
			mag_declination	= rcvPacket.getFloat();
			raw_press		= rcvPacket.getInt();
			raw_temp			= rcvPacket.getInt();
			gyro_cal_x		= rcvPacket.getFloat();
			gyro_cal_y		= rcvPacket.getFloat();
			gyro_cal_z		= rcvPacket.getFloat();
			accel_cal_x		= rcvPacket.getFloat();
			accel_cal_y		= rcvPacket.getFloat();
			accel_cal_z		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  mag_ofs_x
					 ,mag_ofs_y
					 ,mag_ofs_z
					 ,mag_declination
					 ,raw_press
					 ,raw_temp
					 ,gyro_cal_x
					 ,gyro_cal_y
					 ,gyro_cal_z
					 ,accel_cal_x
					 ,accel_cal_y
					 ,accel_cal_z
					 );
	}

	public byte[] encode(
						 short v_mag_ofs_x
						,short v_mag_ofs_y
						,short v_mag_ofs_z
						,float v_mag_declination
						,int v_raw_press
						,int v_raw_temp
						,float v_gyro_cal_x
						,float v_gyro_cal_y
						,float v_gyro_cal_z
						,float v_accel_cal_x
						,float v_accel_cal_y
						,float v_accel_cal_z
						)
	{
		// int[] mavLen = {2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShort(v_mag_ofs_x);	// Add "mag_ofs_x" parameter
		sndPacket.putShort(v_mag_ofs_y);	// Add "mag_ofs_y" parameter
		sndPacket.putShort(v_mag_ofs_z);	// Add "mag_ofs_z" parameter
		sndPacket.putFloat(v_mag_declination);	// Add "mag_declination" parameter
		sndPacket.putInt(v_raw_press);	// Add "raw_press" parameter
		sndPacket.putInt(v_raw_temp);	// Add "raw_temp" parameter
		sndPacket.putFloat(v_gyro_cal_x);	// Add "gyro_cal_x" parameter
		sndPacket.putFloat(v_gyro_cal_y);	// Add "gyro_cal_y" parameter
		sndPacket.putFloat(v_gyro_cal_z);	// Add "gyro_cal_z" parameter
		sndPacket.putFloat(v_accel_cal_x);	// Add "accel_cal_x" parameter
		sndPacket.putFloat(v_accel_cal_y);	// Add "accel_cal_y" parameter
		sndPacket.putFloat(v_accel_cal_z);	// Add "accel_cal_z" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SENSOR_OFFSETS_mag_ofs_x"
 				+ ", SENSOR_OFFSETS_mag_ofs_y"
 				+ ", SENSOR_OFFSETS_mag_ofs_z"
 				+ ", SENSOR_OFFSETS_mag_declination"
 				+ ", SENSOR_OFFSETS_raw_press"
 				+ ", SENSOR_OFFSETS_raw_temp"
 				+ ", SENSOR_OFFSETS_gyro_cal_x"
 				+ ", SENSOR_OFFSETS_gyro_cal_y"
 				+ ", SENSOR_OFFSETS_gyro_cal_z"
 				+ ", SENSOR_OFFSETS_accel_cal_x"
 				+ ", SENSOR_OFFSETS_accel_cal_y"
 				+ ", SENSOR_OFFSETS_accel_cal_z"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + mag_ofs_x
 				+ ", " + mag_ofs_y
 				+ ", " + mag_ofs_z
 				+ ", " + mag_declination
 				+ ", " + raw_press
 				+ ", " + raw_temp
 				+ ", " + gyro_cal_x
 				+ ", " + gyro_cal_y
 				+ ", " + gyro_cal_z
 				+ ", " + accel_cal_x
 				+ ", " + accel_cal_y
 				+ ", " + accel_cal_z
				);
		return param;
	}
}
