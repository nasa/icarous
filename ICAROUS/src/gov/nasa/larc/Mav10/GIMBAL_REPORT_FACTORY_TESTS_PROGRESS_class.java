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
Message ID: GIMBAL_REPORT_FACTORY_TESTS_PROGRESS(210)
--------------------------------------
%%~              Reports the current status of a section of a running factory test  
%%~        
--------------------------------------
*/
public class GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_class //implements Loggable
{
	public static final int msgID = 210;
	public short	 test;					 	// Which factory test is currently running
	public short	 test_section;			 	// Which section of the test is currently running.  The meaning of this is test-dependent
	public short	 test_section_progress;	 	// The progress of the current test section, 0x64=100%
	public short	 test_status;				 	// The status of the currently executing test section.  The meaning of this is test and section-dependent

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_class(GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_class o)
	{
		test = o.test;
		test_section = o.test_section;
		test_section_progress = o.test_section_progress;
		test_status = o.test_status;
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

			// int[] mavLen = {1, 1, 1, 1};
			// int[] javLen = {2, 2, 2, 2};

			test						= rcvPacket.getShortB();
			test_section				= rcvPacket.getShortB();
			test_section_progress	= rcvPacket.getShortB();
			test_status				= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  test
					 ,test_section
					 ,test_section_progress
					 ,test_status
					 );
	}

	public byte[] encode(
						 short v_test
						,short v_test_section
						,short v_test_section_progress
						,short v_test_status
						)
	{
		// int[] mavLen = {1, 1, 1, 1};
		// int[] javLen = {2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_test);	// Add "test" parameter
		sndPacket.putByteS(v_test_section);	// Add "test_section" parameter
		sndPacket.putByteS(v_test_section_progress);	// Add "test_section_progress" parameter
		sndPacket.putByteS(v_test_status);	// Add "test_status" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_test"
 				+ ", GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_test_section"
 				+ ", GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_test_section_progress"
 				+ ", GIMBAL_REPORT_FACTORY_TESTS_PROGRESS_test_status"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + test
 				+ ", " + test_section
 				+ ", " + test_section_progress
 				+ ", " + test_status
				);
		return param;
	}
}
