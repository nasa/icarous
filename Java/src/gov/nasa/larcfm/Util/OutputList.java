/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.List;

/**
 * Interface class for various readers that can read in parameters
 */
public interface OutputList {

    /**
     * Return a list of strings representing key values of this object.
	 * @return array of strings
     */
	public List<String> toStringList();

	/**
	 * Return a list of string representing key values of this object.
	 * @param precision the number of digits to display
	 * @return array of strings
	 */
	public List<String> toStringList(int precision);

}
