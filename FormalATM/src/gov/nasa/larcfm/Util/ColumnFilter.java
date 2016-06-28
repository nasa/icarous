/* 
 * ColumnFilter
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

import java.io.Reader;
import java.util.ArrayList;

public final class ColumnFilter {
	
    private SeparatedInput si;
    private ArrayList<Constraint> constraints;
    private boolean eof;

	private static final int ALL = 1;
	private static final int VALUE = 2;
	private static final int RANGE = 3;
	
	private class Constraint {
		int index;
		int ty;
		ArrayList<String> values;
		double lower;
		double upper;
		
		Constraint() {
			values = new ArrayList<String>(5);
		}
		
		// true if it passed the filter
		boolean filter(String str, double v) {
			if (ty == ALL) return true;
			if (ty == VALUE && values.indexOf(str) >= 0) return true;
			if (ty == RANGE && lower <= v && v <= upper) return true;
			return false;
		}
	}
	
    public ColumnFilter(Reader r) {
    	if (r == null) {
    		eof = true;
    		return;
    	}
    	si = new SeparatedInput(r);
    	eof = si.readLine();
    	constraints = new ArrayList<Constraint>(20);
    }

    /**
     * This constraint is always satisfied.  It means include the column
     * indicated by heading in the output.
     * 
     * @param heading the heading to include in the output
     */
    public void allHeading(String heading) {
    	Constraint c = new Constraint();
    	c.index = si.findHeading(heading);
    	c.ty = ALL;
    	
    	if (c.index >= 0) {
    		constraints.add(c);
    	}
    }

    private Constraint findConstraint(int index) {
    	for (Constraint i : constraints) {
    		if (i.index == index) {
    			return i;
    		}
    	}
    	return null;
    }
    
    /**
     * Given <code>v<\code> is the value for a line under the given 
     * heading, this constraint will be satisfied if <code>value == v</code>. 
     * 
     * @param heading the heading of the relevant column
     * @param value the required value
     */
    public void oneHeading(String heading, String value) {
    	int index = si.findHeading(heading);
    	if (index < 0) return;
    	
    	Constraint c = findConstraint(index);
    	if (c == null) {
    		c = new Constraint();
        	c.index = index;
        	c.ty = VALUE;
    		constraints.add(c);
    	}
    	c.values.add(value);
    }

    /**
     * Given <code>v<\code> is the value for a line under the given 
     * heading, this constraint will be satisfied if <code>lower <= v <= upper</code>. 
     * 
     * @param heading the heading of the relevant column
     * @param lower the lower bound
     * @param upper the upper bound
     */
    public void rangeHeading(String heading, double lower, double upper) {
    	int index = si.findHeading(heading);
    	if (index < 0) return;
    	
    	Constraint c = findConstraint(index);
    	if (c == null) {
    		c = new Constraint();
    		c.index = index;
    		c.ty = RANGE;
    		constraints.add(c);
    	}
    	
    	c.lower = Units.from(si.getUnit(c.index),lower);
    	c.upper = Units.from(si.getUnit(c.index),upper);
    }
    
    public int size() {
    	return constraints.size();
    }

    /**
     * Reads a line from the Reader (provided in the constructor) which satisfies 
     * the constraints.  The string representation of the line is provided by str_out
     * and a floating point double representation of the line is provided by double_out.<p>
     * 
     * The arrays str_out and double_out should have a length of at least ColumnFilter.size().<p>
     * 
     * @param str_out the output line
     * @param double_out the output line (in internal units)
     * @return true if an end of file has been reached.
     */
    public boolean readLine(String[] str_out, double[] double_out) {
    	while ( ! eof) {
    		int index = 0;
    	
    		// Get appropriate headings
    		for (Constraint c : constraints) {
    	    	if (str_out != null && str_out.length > index) {
        			str_out[index] = si.getColumnString(c.index);
    	    	}
    	    	if (double_out != null && double_out.length > index) {
    				double_out[index] = si.getColumn(c.index);
    			}
    	    	index++;
    		}
    		
    		// Filter by the constraints
    		boolean pass = true;
    		for (Constraint c : constraints) {
    			if ( ! pass ) break;
    			if ( c.filter(si.getColumnString(c.index), si.getColumn(c.index))) {
    				pass = pass && true;
    			} else {
    				pass = false;
    			}
    		}
    		eof = si.readLine();
    		if (pass) {
    			return false;
    		} 
    	}
    	return true;
    }
    
    public String[] getHeadings() {
    	String[] rtn = new String[constraints.size()];
    	
    	int index = 0;
    	for (Constraint c : constraints) {
    		rtn[index++] = si.getHeading(c.index);
    	}
    	
    	return rtn;
    }

    public String[] getUnits() {
    	String[] rtn = new String[constraints.size()];
    	
    	int index = 0;
    	for (Constraint c : constraints) {
    		rtn[index++] = si.getUnit(c.index);
    	}
    	
    	return rtn;
    }
}
