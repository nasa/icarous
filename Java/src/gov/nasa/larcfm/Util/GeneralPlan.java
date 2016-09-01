/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.io.IOException;
import java.io.ObjectStreamException;
import java.io.Serializable;


/**
 * This class represents either an aircraft Plan or a polygon Path
 */
public class GeneralPlan implements ErrorReporter {
	private final Plan fp; // plan
	private final PolyPath pp; // path
	private final boolean containment;  // path containment? (always false for plans)
//	private final double t; // activation time

	public GeneralPlan() {
		fp = null;
		pp = null;
		containment = false;
//		t = 0.0;
	}

	public GeneralPlan(Plan p) {
		fp = p;
		pp = null;
		containment = false;
//		t = 0;
	}

	public GeneralPlan(PolyPath p) {
		fp = null;
		pp = p;
		containment = false;
//		t = 0;
	}

	
//	public GeneralPlan(Plan p, double time) {
//		fp = p;
//		pp = null;
//		containment = false;
//		t = time;
//	}
//
//	public GeneralPlan(PolyPath p, double time) {
//		fp = null;
//		pp = p;
//		containment = false;
//		t = time;
//	}

	/**
	 * Allows for containment polygons
	 * @param p
	 * @param cont true = containment poly, false = avoidance poly
	 * @param time
	 */
	public GeneralPlan(PolyPath p, boolean cont) {
		fp = null;
		pp = p;
		containment = cont;
//		t = time;
	}

	public GeneralPlan(GeneralPlan g) {
		fp = g.fp;
		pp = g.pp;
		containment = g.containment;
//		t = g.t;
	}

	public GeneralState state(double time) {
//		if (time >= t) {
			if (fp != null) {
				if (time >= fp.getFirstTime() && time <= fp.getLastTime()) {
					return new GeneralState(fp.getName(), fp.position(time), fp.velocity(time), time);
				}
			} else if (pp != null) {
				if (time >= pp.getFirstTime() && time <= pp.getLastTime()) {
					return new GeneralState(pp.getName(), pp.getSimpleMovingPoly(time), time, containment);
				}
			}
//		}
		return GeneralState.INVALID;
	}

	public double getFirstTime() {
		if (fp != null) {
			return fp.getFirstTime();
		} else if (pp != null) {
			return pp.getFirstTime();
		}
		return -1.0;
	}

	public double getLastTime() {
		if (fp != null) {
			return fp.getLastTime();
		} else if (pp != null) {
			return pp.getLastTime();
		}
		return -1.0;
	}

	public int size() {
		if (fp != null) {
			return fp.size();
		} else if (pp != null) {
			return pp.size();
		}
		return 0;
	}

	public String getName() {
		if (fp != null) {
			return fp.getName();
		} else if (pp != null) {
			return pp.getName();
		}
		return "";
	}

	public boolean isLatLon() {
		if (fp != null) {
			return fp.isLatLon();
		} else if (pp != null) {
			return pp.isLatLon();
		}
		return false;
	}

	public boolean hasPlan() {
		return fp != null;
	}
	
	public boolean hasPolyPath() {
		return pp != null;
	}
	
	public Plan getPlan() {
		return fp;
	}
	
	public PolyPath getPolyPath() {
		return pp;
	}
	
//	public double getActiveTime() {
//		return t;
//	}
	
	public boolean isContainment() {
		return (pp != null) && containment;
	}
	
	public boolean isInvalid() {
		return (pp == null) && (fp == null);
	}
	
	public GeneralState point(int i) {
		if (i >= 0) {
			if (fp != null) {
				if (i < fp.size()) {
					double time = fp.getTime(i);
					return new GeneralState(fp.getName(), fp.point(i).position(), fp.initialVelocity(i), time);
				}
			} else if (pp != null) {
				if (i < pp.size()) {
					double time = pp.getTime(i);
					return new GeneralState(pp.getName(), pp.getSimpleMovingPoly(i), time, containment);
				}
			}
		}
		return GeneralState.INVALID;
	}
	
	public int getSegment(double t) {
		if (fp != null) {
			return fp.getSegment(t);
		} else if (pp != null) {
			return pp.getSegment(t);
		}
		return -1;
	}
	
	public boolean validate() {
		if (fp != null) {
			return fp.isWeakConsistent(true);     // **RWB** use weak Consistent rather than is consistent
		} else if (pp != null) {
			return pp.validate();
		}
		return false;		
	}
	
	public String toString() {
		if (fp != null) {
			return fp.toString();
		} else if (pp != null) {
			return pp.toString();
		}
		return "INVALID";
	}

	public String toOutput(int precision) {
		if (fp != null) {
			return fp.toOutput();
		} else if (pp != null) {
			return pp.toOutput();
		} else {
			return "";
		}
	}


	// ErrorReporter Interface Methods

	public boolean hasError() {
		if (fp != null) {
			return fp.hasError();
		} else if (pp != null) {
			return pp.hasError();
		}
		return false;
	}
	public boolean hasMessage() {
		if (fp != null) {
			return fp.hasMessage();
		} else if (pp != null) {
			return pp.hasMessage();
		}
		return false;
	}
	public String getMessage() {
		if (fp != null) {
			return fp.getMessage();
		} else if (pp != null) {
			return pp.getMessage();
		}
		return "";
	}
	public String getMessageNoClear() {
		if (fp != null) {
			return fp.getMessageNoClear();
		} else if (pp != null) {
			return pp.getMessageNoClear();
		}
		return "";
	}

	
}
