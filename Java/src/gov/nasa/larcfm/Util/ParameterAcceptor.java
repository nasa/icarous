/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * Minimal interface for object that accept ParameterData objects
 */
public interface ParameterAcceptor extends ParameterProvider {

	/**
	 * Modify this object's parameters to match the given ParameterData object.  Unrecognized keys are ignored.
	 */
	public void setParameters(ParameterData p);
	
}
