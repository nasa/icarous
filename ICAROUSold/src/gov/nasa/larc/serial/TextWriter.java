/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/**
 * 
 */
package gov.nasa.larc.serial;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

/**
 * @author cquach
 *
 */
public class TextWriter {

//====================================================================================================================//
// DATA Section
//====================================================================================================================//
	static final String  DEFAULT_LOG_FILENAME = "textlog.txt";	
	
	//float logFreq = DEFAULT_LOG_FREQUENCY;		// 1 hz default
	long sleepTics = 0;							// equivalent sleep period for the log frequency
	
	String logFileName = DEFAULT_LOG_FILENAME;	
	BufferedWriter logFile = null;			// file pointer
	static boolean APPEND_TRUE = true;
	
//====================================================================================================================//
// FUNCTION Section
//====================================================================================================================//
	/**
	 * 
	 */
	public TextWriter() {
		this( DEFAULT_LOG_FILENAME );
	}

	
	//=====================================================================================//
	/**
	 *   Constructor allows specifying filename and frequency.
	 */
	//=====================================================================================//
	public TextWriter  (String fn) 
	{

		// open file
		if (fn != null)  {
			logFileName = fn;
		} 
		openFile (logFileName, APPEND_TRUE);		// file will open with given name or default name given by DEFAULT_LOG_FILENAME
		
		//setName(logFileName+"-loggerThread");		// set thread name
	}
	public TextWriter  (String fn, boolean appendFlag) 
	{

		// open file
		if (fn != null)  {
			logFileName = fn;
		} 
		openFile (logFileName, appendFlag);		// file will open with given name or default name given by DEFAULT_LOG_FILENAME
		
		//setName(logFileName+"-loggerThread");		// set thread name
	}

	
	//=====================================================================================//
	/**
	 *   Open TextWriter output file.
	 *   
			String content = "This is the content to write into file";
 
			File file = new File("/users/mkyong/filename.txt");
 
			// if file doesnt exists, then create it
			if (!file.exists()) {
				file.createNewFile();
			}
 
			FileWriter fw = new FileWriter(file.getAbsoluteFile());
			BufferedWriter bw = new BufferedWriter(fw);
			bw.write(content);

	 *   
	 */
	//=====================================================================================//
	public boolean openFile ( String logFileName, boolean append)
	{
		boolean openStatus = false;
		
		try {
			
//			logFile = new BufferedWriter(new FileWriter(logFileName, append)); 

			File file = new File(logFileName);
			// if file doesnt exists, then create it
			if (!file.exists()) {
				file.createNewFile();
			}
			logFile = new BufferedWriter(new FileWriter(file.getAbsoluteFile())); 
			openStatus = true;

		} catch (IOException e) {
			System.err.println( Thread.currentThread().getName()
		  						+ " --> TextWriter::openFile: "
		  						+ "Error encountered trying to OPEN file [" + logFileName + "] "
							);
			e.printStackTrace();
		}
		return openStatus;
	}
	
	
	
	
	//=====================================================================================//
	/**
	 *   Close TextWriter output file.
	 */
	//=====================================================================================//
	public void closeFile ()
	{
		if (logFile != null) {
			try {
				logFile.close();
			} catch (IOException e) {
				System.err.println( Thread.currentThread().getName()
  						+ " --> TextWriter::closeFile: "
  						+ "Error encountered trying to CLOSE file [" + logFileName + "] "
					);
				e.printStackTrace();	
			}
		}
	}
	
	
	//=====================================================================================//
	/**
	 *   Assign a data source for this TextWriter.  data source has to implement Loggable.  
	 *   new source may be assigned mid stream.  Same log file used.
	 */
	//=====================================================================================//
//	public void setDataSource (Loggable src)
//	{
//		newSrc = true;
//		newLogSource = src;		
//	}

	//=====================================================================================//
	/**
	 *   Do checks and start log.
	 */
	//=====================================================================================//
	public void write (String logText) {
		try {
			logFile.write(logText + "\n");
			logFile.flush();
		} catch (IOException e) {
			System.err.println( Thread.currentThread().getName()
						+ " --> TextWriter::write: "
						+ "Error encountered trying to WRITE to file [" + logFileName + "] "
				);
			e.printStackTrace();
		}
	}
	
	
	//=====================================================================================//
	/**
	 *   Constructor allows specifying filename and frequency.
	 */
	//=====================================================================================//
	protected void finalize () throws Throwable 
	{
		if (logFile != null) {
			logFile.flush();
			logFile.close();
		}
	}
	
//====================================================================================================================//
// TEST ENTRY POINT
//====================================================================================================================//

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
