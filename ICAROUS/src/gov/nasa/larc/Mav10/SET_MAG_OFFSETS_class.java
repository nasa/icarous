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
Message ID: SET_MAG_OFFSETS(151)
--------------------------------------
%%~ Deprecated. Use MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS instead. Set the magnetometer 
%%~ offsets
--------------------------------------
*/
public class SET_MAG_OFFSETS_class //implements Loggable
{
	public static final int msgID = 151;
	public short	 mag_ofs_x;		 	// magnetometer X offset
	public short	 mag_ofs_y;		 	// magnetometer Y offset
	public short	 mag_ofs_z;		 	// magnetometer Z offset
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public SET_MAG_OFFSETS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public SET_MAG_OFFSETS_class(SET_MAG_OFFSETS_class o)
	{
		mag_ofs_x = o.mag_ofs_x;
		mag_ofs_y = o.mag_ofs_y;
		mag_ofs_z = o.mag_ofs_z;
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {2, 2, 2, 1, 1};
			// int[] javLen = {2, 2, 2, 2, 2};

			mag_ofs_x		= rcvPacket.getShort();
			mag_ofs_y		= rcvPacket.getShort();
			mag_ofs_z		= rcvPacket.getShort();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  mag_ofs_x
					 ,mag_ofs_y
					 ,mag_ofs_z
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 short v_mag_ofs_x
						,short v_mag_ofs_y
						,short v_mag_ofs_z
						)
	{
		return encode(
					  v_mag_ofs_x
					 ,v_mag_ofs_y
					 ,v_mag_ofs_z
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 short v_mag_ofs_x
						,short v_mag_ofs_y
						,short v_mag_ofs_z
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {2, 2, 2, 1, 1};
		// int[] javLen = {2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShort(v_mag_ofs_x);	// Add "mag_ofs_x" parameter
		sndPacket.putShort(v_mag_ofs_y);	// Add "mag_ofs_y" parameter
		sndPacket.putShort(v_mag_ofs_z);	// Add "mag_ofs_z" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_MAG_OFFSETS_mag_ofs_x"
 				+ ", SET_MAG_OFFSETS_mag_ofs_y"
 				+ ", SET_MAG_OFFSETS_mag_ofs_z"
 				+ ", SET_MAG_OFFSETS_target_system"
 				+ ", SET_MAG_OFFSETS_target_component"
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
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
