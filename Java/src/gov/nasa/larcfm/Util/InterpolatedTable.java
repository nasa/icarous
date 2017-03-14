/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * This interface is an abstraction of a general linear interpolation table.
 */
public interface InterpolatedTable {

    /** Returns the value of the dependent variable for this table, based on the 
     *  values loaded into the corresponding {@link BreakpointSchedule}'s.   
     *  Linear interpolation is performed.  If an independent variable is out of 
     *  range (as defined by its {@link BreakpointSchedule}), it will either limited 
     *  to the allowed range or linear extrapolation will be performed, depending 
     *  on the implementation.<p>
     *
     * @return the value of the dependent variable for this table
     */
    public double getDependentValue();

}
