/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/**
 * 
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.TextWriter;

/**
 * Track AP serial communication statistics.
 * @author cquach
 *
 */
public class APLinkMonitor {

//====================================================================================================================//
// DATA Section
//====================================================================================================================//
	public static int maxMsgPerBlock = (int)256;
	public static int maxMsgTypes = (int)256;

	int blockMsgCnt = 0;
	int blockSize = maxMsgPerBlock;
	int totalMsgs = 0;
		
	int[] MsgTable = new int[maxMsgTypes];
	//int skipCnt = 0;
	//boolean rollOver = false;
	int droppedMsgCnt = 0;
	int prevSeqNum = 0;
	boolean prevSeqValid = false;
	//private int nextBlockSkips = 0;
	String logFileName = null;
	TextWriter logFile = null;
	
//====================================================================================================================//
// FUNCTION Section
//====================================================================================================================//
		
	//=====================================================================================//
	/**
	 * Keep statistics on message received based on sequence numbers.  Message drop rate is
	 * keeped for each block of 255 messages.  MavLink uses a 1 byte message sequence number
	 * to tag each message.  This class watches the sequence number to detect dropped
	 * messages and gives a detail count of which types of messages are received. 
	 */
	//=====================================================================================//
	public APLinkMonitor(String fn) 
	{

		logFileName = fn;
		logFile = new TextWriter(logFileName);
		clearMsgCounts ();
		writeStatsHdr();		// write header to log file.	
	}
	
	
	//=====================================================================================//
	/**
	 *  Clears the message count table for each block
	 */
	//=====================================================================================//
	private void clearMsgCounts() 
	{
		for (int i=0; i<maxMsgTypes; i++)
		{
			MsgTable[i] = 0;
		}
	}


	//=====================================================================================//
	/**
	 * 
	 */
	//=====================================================================================//
//	public void saveLinkInfo (int seqNum, int msgID, int msgLength)
	public void saveLinkInfo (int seqNum, int msgID )
	{
		int skips = 0;

		// increment message counts
		MsgTable[msgID] += 1;
		blockMsgCnt++;;
		totalMsgs++;

		// figure out if there are any skipped messages
		if (!prevSeqValid) 	{  	// this IS first time thru	
			blockSize = maxMsgPerBlock - seqNum + 1;  // inclusive of seqNum
			droppedMsgCnt = 0;
			prevSeqNum = seqNum;  // no skipped messages to check on first time thru
			prevSeqValid = true;
		} else {	// this IS NOT first time thru	
			// check for roll over processing
			if (seqNum > prevSeqNum) {	// rollover has NOT occured
				// check to see if this is the next message in the sequence
				skips = seqNum - prevSeqNum;
				if ( skips > 1) {
					droppedMsgCnt += (skips - 1);
				}
			} else {	// rollover HAS occured
				// figure out if any dropped messages before the sequence rollover
				if (prevSeqNum < (maxMsgPerBlock-1) )  {
					droppedMsgCnt += (maxMsgPerBlock - prevSeqNum) - 1;
				}
				//timeToWrite = true;  	// signal a write to file only on rollover
				// write stats to log file everytime seqNum rolls over
				writeStats ();
				clearMsgCounts ();
				blockSize = maxMsgPerBlock;
				blockMsgCnt = 0;
				prevSeqNum = seqNum;  // save sequence 				
				if (seqNum > 0)		// Reset dropped message count after rollover write
					droppedMsgCnt = seqNum;	// reset skip message count after writing stats about current message block
				else 
					droppedMsgCnt = 0;	
			}
		}
		
	}		// end saveLinkInfo() function


	//=====================================================================================//
	/**
	 * Convenience function to log statistics for one block of messages
	 */
	//=====================================================================================//
	private void writeStats() {
		// File looks like
		// block size (255),blockMsgCnt, MsgRecvRate, msg[0] cnt, msg[1] cnt, ... , msg[max] cnt
		
		String out = blockSize
				+ "," + blockMsgCnt
				+ "," + ((float)blockMsgCnt/(float)blockSize)*(float)100	// percentage drop messages
				+ MsgTableToString()
				;
		// flush after write
		try {
			logFile.write(out);
			System.err.println(Thread.currentThread().getName()
								+"--> APLinkMOnitor::writeStats: "
								+ out
								);

		} catch (Exception e) {
			System.err.println(Thread.currentThread().getName()
					+"--> APLinkMOnitor::writeStats: "
					+ "Exception tryint to write to file."
					);
			e.printStackTrace();
			
		}
	}	// end writeStats() function

	//=====================================================================================//
	/**
	 * Convenience function to convert message type table to a string for logging.
	 */
	//=====================================================================================//
	private String MsgTableToString() {
		String out = new String();
		for (int i=0; i<maxMsgTypes; i++)
		{
			out += "," + MsgTable[i];
		}
		return out;
	}

	//=====================================================================================//
	/**
	 * Convenience function to log header for the message receive statistics.
	 */
	//=====================================================================================//
	private void writeStatsHdr() 
	{
		// File looks like
		// block size (255),blockMsgCnt, MsgLossRate, msg[0] cnt, msg[1] cnt, ... , msg[max] cnt
		
		String out = "blockSize"
				+ ", blockMsgCnt"
				+ ", MsgRecvRate"	// percentage drop messages
				+ MsgTableHdrToString()
				;
		// flush after write
		logFile.write(out);
	
	}

	//=====================================================================================//
	/**
	 * Convenience function to create message type table header.
	 */
	//=====================================================================================//
	private String MsgTableHdrToString() {
		String out = new String();
		for (int i=0; i<maxMsgTypes; i++)
		{
			out += ", MsgType["+i+"]";
		}
		return out;
	}


//====================================================================================================================//
//TEST ENTRY POINT
//====================================================================================================================//

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
