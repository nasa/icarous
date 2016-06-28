/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * Minimal interface for object that accept ParameterData objects
 */
public interface ParameterAcceptor extends ParameterProvider {
//	/**
//	 * Return a fresh ParameterData object populated with this object's parameters. 
//	 */
//	public ParameterData getParameters();

	/**
	 * Modify an existing ParameterData object to include this object's parameters.  Duplicate keys are assumed to be overwritten.
	 */
	public void updateParameterData(ParameterData p);
	
	/**
	 * Modify this object's parameters to match the given ParameterData object.  Unrecognized keys are ignored.
	 */
	public void setParameters(ParameterData p);
	
}
