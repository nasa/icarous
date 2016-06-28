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

/**
 * Standard functions for any class which has data that can be logged by a text logger.  The text
 * logger is running in a thread to log at some given frequency.  The loggable object needs to supply
 * the header and the data.
 * @author cquach
 *
 */
public interface Loggable {
	
	
	//=====================================================================================//
	/**
	 * @return  the header string to write to file
	 */
	//=====================================================================================//
	public String getLogHeader ();
	
	//=====================================================================================//
	/**
	 * @return  the data string to write to file
	 */
	//=====================================================================================//
	public String getLogData ();

}
