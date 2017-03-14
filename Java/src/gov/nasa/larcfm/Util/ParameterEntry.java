/*
 * Copyright (c) 2013-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

public class ParameterEntry {
	public String  sval; // String value
	public double  dval; // Double value (internal units)
	public String  units; // Unit
	public boolean bval; // Boolean value
	public String  comment; // Comment for printing
	public int precision; // Precision for printing double value. Negative precision means undefined

	public ParameterEntry(String s, double d, String u, boolean b, String msg, int p) {
		sval = s;
		dval = d;
		units = u;
		bval = b;
		comment = msg;
		precision = p;
	}

	public ParameterEntry(ParameterEntry entry) {
			sval = entry.sval;
			dval = entry.dval;
			units = entry.units;
			bval = entry.bval;
			comment = entry.comment;
			precision = entry.precision;
	}

	// Make boolean entry
	public static ParameterEntry makeBoolEntry(boolean b) {
		return new ParameterEntry(b ? "true" : "false",0,"unitless", b, "", -1);
	}

	// New double entry
	public static ParameterEntry makeDoubleEntry(double d, String u, int p) {
		return new ParameterEntry(format(u,d,p),d,u,false,"",p);	
	}

	// New integer entry
	public static ParameterEntry makeIntEntry(int i) {
		return new ParameterEntry(Integer.toString(i),i,"unitless",false,"",0);	
	}

	private static String format(String u, double d, int p) {
		if (!u.equals("unitless") && !u.equals("unspecified")) {
			if (p < 0) {
				return Units.strX(u,d);
			} 	else {
				return Units.str(u,d,p);
			}
		} else {
			if (p < 0) {
				return Double.toString(d);
			} else {
				return f.FmPrecision(d,p);
			}
		}
	}
	
	public void set_sval() {
		sval = format(units,dval,precision);
	}

}
