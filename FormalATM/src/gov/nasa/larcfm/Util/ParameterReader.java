/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * Interface class for various readers that can read in parameters
 */
public interface ParameterReader {

	/**
	 * Return a reference (not a copy) to the parameter database
	 */
	public ParameterData getParametersRef();

}
