/* 
 * ErrorReporter
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *  
 */

package gov.nasa.larcfm.Util;

public interface ErrorReporter {
	/** 
	 * Does this object have an error?
	 * @return true if there is an error.
	 */
	public boolean hasError();
	/** 
	 * Does this object have an error or a warning?
	 * @return true if there is an error or warning.
	 */
	public boolean hasMessage();
	/**
	 * Return a string representation of any errors or warnings.  
	 * Calling this 
	 * method will clear any messages and reset both the error 
	 * and warning status to none.
	 * 
	 * @return error and warning messages. If there are no messages, an empty string is returned. 
	 */
	public String getMessage();
	/**
	 * Return a string representation of any errors or warnings. Calling this
	 * method will not clear the error or warning status (i.e., hasError() will
	 * return the same value before and after this call.)
	 * 
	 * @return error and warning messages.  If there are 
	 * no messages, an empty string is returned. 
	 */
	public String getMessageNoClear();
}
