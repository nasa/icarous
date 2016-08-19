/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

public interface Configurable {
	/** This allows the object to be configured through some object. At the minimum, this should parse the string returned below and interpret the object returned below. */
	public void setConfiguration(Object cfg);
	/** This returns an object reflecting the current instance's configuration. setConfiguration should interpret this object. It may or may not be the same as the string returned below. */
	public Object getConfiguration();
	/** This returns an string reflecting the current instance's configuration. It should be readable by setConfiguration. it may or may not be the same as the object returned above. */
	public String getConfigurationString();
}
