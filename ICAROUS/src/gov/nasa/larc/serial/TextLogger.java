/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
///**
// * 
// */
//package gov.nasa.larc.serial;
//
////import gov.nasa.larc.mavlinkparser.data;
//
//import java.io.BufferedWriter;
//import java.io.File;
//import java.io.FileWriter;
//import java.io.IOException;
//
///**
// * @author cquach
// *
// */
//public class TextLogger extends Thread {
//
//	
////=========================================================================================================//
//// Data Section
////=========================================================================================================//
//	static final float DEFAULT_LOG_FREQUENCY = (float)1.0;		// 1 hz default
//	static final long MILLSEC_PER_SEC = 1000;		// 1 hz default
//	static final String  DEFAULT_LOG_FILENAME = "textlog.txt";	
//	
//	float logFreq = DEFAULT_LOG_FREQUENCY;		// 1 hz default
//	long sleepTics = 0;							// equivalent sleep period for the log frequency
//	
//	String logFileName = DEFAULT_LOG_FILENAME;	
//	BufferedWriter logFile = null;			// file pointer
//	static boolean APPEND_TRUE = true;
//	Loggable logSource = null;				// pointer to data object
//	Loggable newLogSource = null;				// pointer to new data object if a setDataSource() is called in middle of logging
//	
//	boolean newSrc = false;			// flag for change source logic.  lets caller change source of data mid-stream.
//	
////=========================================================================================================//
//// Methods Section
////=========================================================================================================//
//
//	//=====================================================================================//
//	/**
//	 *   Default constructor logs to file named "textlog.txt" at 1 hz.
//	 */
//	//=====================================================================================//
//	public TextLogger  (Loggable src) {
//		this(DEFAULT_LOG_FREQUENCY, DEFAULT_LOG_FILENAME, src);
//	}
//
//
//	//=====================================================================================//
//	/**
//	 *   Constructor allows specifying filename and frequency.
//	 */
//	//=====================================================================================//
//	public TextLogger  (float f, String fn, Loggable src) 
//	{
//
//		// save caller settings
//		logFreq = f;		// 1 hz default
//		if (fn != null) logFileName = fn;	
//		logSource = src;
//		newLogSource = src;		// initialize to same as logSource
//		newSrc = true;		// this causes a header to be written to file.  Also allows for changing sources mid-stream
//
//		// open file
//		if (  (logFileName != null) & ( openFile (logFileName, APPEND_TRUE) )   ) {
//			sleepTics = (long) ((float) MILLSEC_PER_SEC / (float)logFreq);
//		}  else {		// don't run logger
//			
//		}
//		
//		setName(logFileName+"-loggerThread");
//	}
//
//	//=====================================================================================//
//	/**
//	 *   Open TextLogger output file.
//	 *   
//			String content = "This is the content to write into file";
// 
//			File file = new File("/users/mkyong/filename.txt");
// 
//			// if file doesnt exists, then create it
//			if (!file.exists()) {
//				file.createNewFile();
//			}
// 
//			FileWriter fw = new FileWriter(file.getAbsoluteFile());
//			BufferedWriter bw = new BufferedWriter(fw);
//			bw.write(content);
//
//	 *   
//	 */
//	//=====================================================================================//
//	public boolean openFile ( String logFileName, boolean append)
//	{
//		boolean openStatus = false;
//		
//		try {
//			
////			logFile = new BufferedWriter(new FileWriter(logFileName, append)); 
//
//			File file = new File(logFileName);
//			// if file doesnt exists, then create it
//			if (!file.exists()) {
//				file.createNewFile();
//			}
//			logFile = new BufferedWriter(new FileWriter(file.getAbsoluteFile())); 
//			openStatus = true;
//
//		} catch (IOException e) {
//			System.err.println( Thread.currentThread().getName()
//		  						+ " --> TextLogger::openFile: "
//		  						+ "Error encountered trying to OPEN file [" + logFileName + "] "
//							);
//			e.printStackTrace();
//		}
//		return openStatus;
//	}
//	
//	
//	
//	
//	//=====================================================================================//
//	/**
//	 *   Close TextLogger output file.
//	 */
//	//=====================================================================================//
//	public void closeFile ()
//	{
//		if (logFile != null) {
//			try {
//				logFile.close();
//			} catch (IOException e) {
//				System.err.println( Thread.currentThread().getName()
//  						+ " --> TextLogger::closeFile: "
//  						+ "Error encountered trying to CLOSE file [" + logFileName + "] "
//					);
//				e.printStackTrace();	
//			}
//		}
//	}
//	
//	
//	//=====================================================================================//
//	/**
//	 *   Assign a data source for this TextLogger.  data source has to implement Loggable.  
//	 *   new source may be assigned mid stream.  Same log file used.
//	 */
//	//=====================================================================================//
//	public void setDataSource (Loggable src)
//	{
//		newSrc = true;
//		newLogSource = src;		
//	}
//
//	//=====================================================================================//
//	/**
//	 *   Do checks and start log.
//	 */
//	//=====================================================================================//
//	private void write (String logText) {
//		try {
//			logFile.write(logText + "\n");
//			logFile.flush();
//		} catch (IOException e) {
//			System.err.println( Thread.currentThread().getName()
//						+ " --> TextLogger::write: "
//						+ "Error encountered trying to WRITE to file [" + logFileName + "] "
//				);
//			e.printStackTrace();
//		}
//	}
//
//
//	//=====================================================================================//
//	/**
//	 *   This is the TextLogger thread.  Will not do anything until a logSource is assigned.
//	 */
//	//=====================================================================================//
//	public void run ()
//	{
//
//		while (true) {
//			if (newSrc) {
//				logSource = newLogSource;		// switch data source
//				if (  (logSource != null) || (logFile != null)	) {
//					write(logSource.getLogHeader());	// write new header
//				}
//				newSrc = false;		// set flag to prevent header rewrite
//			}
//			if (  (logSource != null) || (logFile != null)	) {
//				write(logSource.getLogData());	// write data
//			}
//
//			try { //sleep for 1 second (1000 millis)
//				Thread.sleep(sleepTics);
//			} catch (InterruptedException e) {
//				//do something
//			}
//
//		}	// end infinite loop
//	}	// end run() function for the thread
//	
//	//=====================================================================================//
//	/**
//	 *   Constructor allows specifying filename and frequency.
//	 */
//	//=====================================================================================//
//	protected void finalize () throws Throwable 
//	{
//		if (logFile != null) logFile.close();
//	}
//	
//	
//}	// END TextLogger Class
