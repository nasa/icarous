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
Message ID: GPS2_RTK(128)
--------------------------------------
%%~ RTK GPS data. Gives information on the relative baseline calculation the GPS is 
%%~ reporting
--------------------------------------
*/
public class GPS2_RTK_class //implements Loggable
{
	public static final int msgID = 128;
	public long	 time_last_baseline_ms;	 	// Time since boot of last baseline message received in ms.
	public long	 tow;						 	// GPS Time of Week of last baseline
	public int		 baseline_a_mm;			 	// Current baseline in ECEF x or NED north component in mm.
	public int		 baseline_b_mm;			 	// Current baseline in ECEF y or NED east component in mm.
	public int		 baseline_c_mm;			 	// Current baseline in ECEF z or NED down component in mm.
	public long	 accuracy;				 	// Current estimate of baseline accuracy.
	public int		 iar_num_hypotheses;		 	// Current number of integer ambiguity hypotheses.
	public int		 wn;						 	// GPS Week Number of last baseline
	public short	 rtk_receiver_id;			 	// Identification of connected RTK receiver.
	public short	 rtk_health;				 	// GPS-specific health report for RTK data.
	public short	 rtk_rate;				 	// Rate of baseline messages being received by GPS, in HZ
	public short	 nsats;					 	// Current number of sats used for RTK calculation.
	public short	 baseline_coords_type;	 	// Coordinate system of baseline. 0 == ECEF, 1 == NED

	private packet rcvPacket;
	private packet sndPacket;

	public GPS2_RTK_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GPS2_RTK_class(GPS2_RTK_class o)
	{
		time_last_baseline_ms = o.time_last_baseline_ms;
		tow = o.tow;
		baseline_a_mm = o.baseline_a_mm;
		baseline_b_mm = o.baseline_b_mm;
		baseline_c_mm = o.baseline_c_mm;
		accuracy = o.accuracy;
		iar_num_hypotheses = o.iar_num_hypotheses;
		wn = o.wn;
		rtk_receiver_id = o.rtk_receiver_id;
		rtk_health = o.rtk_health;
		rtk_rate = o.rtk_rate;
		nsats = o.nsats;
		baseline_coords_type = o.baseline_coords_type;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 1, 1};
			// int[] javLen = {8, 8, 4, 4, 4, 8, 4, 4, 2, 2, 2, 2, 2};

			time_last_baseline_ms	= rcvPacket.getLongI();
			tow						= rcvPacket.getLongI();
			baseline_a_mm			= rcvPacket.getInt();
			baseline_b_mm			= rcvPacket.getInt();
			baseline_c_mm			= rcvPacket.getInt();
			accuracy					= rcvPacket.getLongI();
			iar_num_hypotheses		= rcvPacket.getInt();
			wn						= rcvPacket.getIntS();
			rtk_receiver_id			= rcvPacket.getShortB();
			rtk_health				= rcvPacket.getShortB();
			rtk_rate					= rcvPacket.getShortB();
			nsats					= rcvPacket.getShortB();
			baseline_coords_type		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_last_baseline_ms
					 ,tow
					 ,baseline_a_mm
					 ,baseline_b_mm
					 ,baseline_c_mm
					 ,accuracy
					 ,iar_num_hypotheses
					 ,wn
					 ,rtk_receiver_id
					 ,rtk_health
					 ,rtk_rate
					 ,nsats
					 ,baseline_coords_type
					 );
	}

	public byte[] encode(
						 long v_time_last_baseline_ms
						,long v_tow
						,int v_baseline_a_mm
						,int v_baseline_b_mm
						,int v_baseline_c_mm
						,long v_accuracy
						,int v_iar_num_hypotheses
						,int v_wn
						,short v_rtk_receiver_id
						,short v_rtk_health
						,short v_rtk_rate
						,short v_nsats
						,short v_baseline_coords_type
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 1, 1};
		// int[] javLen = {8, 8, 4, 4, 4, 8, 4, 4, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_last_baseline_ms);	// Add "time_last_baseline_ms" parameter
		sndPacket.putIntL(v_tow);	// Add "tow" parameter
		sndPacket.putInt(v_baseline_a_mm);	// Add "baseline_a_mm" parameter
		sndPacket.putInt(v_baseline_b_mm);	// Add "baseline_b_mm" parameter
		sndPacket.putInt(v_baseline_c_mm);	// Add "baseline_c_mm" parameter
		sndPacket.putIntL(v_accuracy);	// Add "accuracy" parameter
		sndPacket.putInt(v_iar_num_hypotheses);	// Add "iar_num_hypotheses" parameter
		sndPacket.putShortI(v_wn);	// Add "wn" parameter
		sndPacket.putByteS(v_rtk_receiver_id);	// Add "rtk_receiver_id" parameter
		sndPacket.putByteS(v_rtk_health);	// Add "rtk_health" parameter
		sndPacket.putByteS(v_rtk_rate);	// Add "rtk_rate" parameter
		sndPacket.putByteS(v_nsats);	// Add "nsats" parameter
		sndPacket.putByteS(v_baseline_coords_type);	// Add "baseline_coords_type" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS2_RTK_time_last_baseline_ms"
 				+ ", GPS2_RTK_tow"
 				+ ", GPS2_RTK_baseline_a_mm"
 				+ ", GPS2_RTK_baseline_b_mm"
 				+ ", GPS2_RTK_baseline_c_mm"
 				+ ", GPS2_RTK_accuracy"
 				+ ", GPS2_RTK_iar_num_hypotheses"
 				+ ", GPS2_RTK_wn"
 				+ ", GPS2_RTK_rtk_receiver_id"
 				+ ", GPS2_RTK_rtk_health"
 				+ ", GPS2_RTK_rtk_rate"
 				+ ", GPS2_RTK_nsats"
 				+ ", GPS2_RTK_baseline_coords_type"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_last_baseline_ms
 				+ ", " + tow
 				+ ", " + baseline_a_mm
 				+ ", " + baseline_b_mm
 				+ ", " + baseline_c_mm
 				+ ", " + accuracy
 				+ ", " + iar_num_hypotheses
 				+ ", " + wn
 				+ ", " + rtk_receiver_id
 				+ ", " + rtk_health
 				+ ", " + rtk_rate
 				+ ", " + nsats
 				+ ", " + baseline_coords_type
				);
		return param;
	}
}
