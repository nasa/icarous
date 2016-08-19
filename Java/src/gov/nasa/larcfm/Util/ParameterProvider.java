/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * Minimal interface for object that produce ParameterData objects
 */
public interface ParameterProvider {
	/**
	 * Return a fresh ParameterData object populated with this object's parameters. 
	 */
	public ParameterData getParameters();
	
	/**
	 * Modify an existing ParameterData object to include this object's parameters.  Duplicate keys are assumed to be overwritten.
	 */
	public void updateParameterData(ParameterData p);

}
