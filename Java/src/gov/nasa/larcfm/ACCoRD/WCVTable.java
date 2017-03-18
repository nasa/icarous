/*
 * Copyright (c) 2012-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.HashMap;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;

public class WCVTable implements ParameterTable {
	protected double DTHR; // Distance threshold
	protected double ZTHR; // Vertical threshold
	protected double TTHR; // Time threshold
	protected double TCOA; // Time to co-altitude threshold

	private HashMap<String,String> units_;

	/**
	 * WCV table SARP concept
	 */
	public WCVTable() {
		DTHR = Units.from("nmi",0.66);   
		ZTHR = Units.from("ft",450);
		TTHR = 35; // [s]
		TCOA = 0;  // [s]
		units_ = new HashMap<String,String>();
		units_.put("WCV_DTHR","nmi");
		units_.put("WCV_ZTHR","ft");
		units_.put("WCV_TTHR","s");
		units_.put("WCV_TCOA","s");
	}

	/**
	 * Copy constructor -- returns a fresh copy
	 * @param tab
	 */
	public WCVTable(WCVTable tab) {
		units_ = new HashMap<String,String>();
		copyValues(tab);
	}

	/**
	 * Table containing specified values (internal units)
	 */

	public WCVTable(double dthr, double zthr, double tthr, double tcoa) {
		DTHR = Math.abs(dthr);
		ZTHR = Math.abs(zthr);
		TTHR = Math.abs(tthr);
		TCOA = Math.abs(tcoa);
		units_ = new HashMap<String,String>();
		units_.put("WCV_DTHR","m");
		units_.put("WCV_ZTHR","m");
		units_.put("WCV_TTHR","s");
		units_.put("WCV_TCOA","s");
	}

	/**
	 * Table containing specified values (specified units units)
	 */

	public WCVTable(double dthr, String udthr, double zthr, String uzthr, 
			double tthr, String utthr, double tcoa, String utcoa) {
		DTHR = Units.from(udthr,dthr);
		ZTHR = Units.from(uzthr,zthr);
		TTHR = Units.from(utthr, tthr);
		TCOA = Units.from(utcoa,tcoa);
		units_ = new HashMap<String,String>();
		units_.put("WCV_DTHR",udthr);
		units_.put("WCV_ZTHR",uzthr);
		units_.put("WCV_TTHR",utthr);
		units_.put("WCV_TCOA",utcoa);
	}

	public WCVTable copy() {
		return new WCVTable(this);
	}

	/**
	 * Initialize this table's values with those of another table
	 */
	public void copyValues(WCVTable tab) {
		DTHR = tab.DTHR;
		ZTHR = tab.ZTHR;
		TTHR = tab.TTHR;
		TCOA = tab.TCOA;
		units_.clear();
		units_.putAll(tab.units_);
	}

	/**
	 * Return horizontal distance threshold DTHR in internal units
	 */

	public double getDTHR()  {
		return DTHR;
	}

	/**
	 * Return horizontal distance threshold DTHR in u units
	 */

	public double getDTHR(String u)  {
		return Units.to(u,DTHR);
	}

	/**
	 * Return vertical distance threshold ZTHR in internal units
	 */

	public double getZTHR()  {
		return ZTHR;
	}

	/**
	 * Return vertical distance threshold ZTHR in u units
	 */

	public double getZTHR(String u)  {
		return Units.to(u,ZTHR);
	}

	/**
	 * Return horizontal time threshold TTHR in seconds
	 */

	public double getTTHR()  {
		return TTHR;
	}

	public double getTTHR(String u)  {
		return Units.to(u, TTHR);
	}

	/**
	 * Return vertical time threshold TCOA in seconds
	 */

	public double getTCOA()  {
		return TCOA;
	}
	public double getTCOA(String u)  {
		return Units.to(u,TCOA);
	}

	/* 
	 * Set horizontal distance threshold DTHR to val, which is specified in internal units
	 */

	public void setDTHR(double val) {
		DTHR = Math.abs(val);
	}

	/*
	 *  Set horizontal distance threshold DTHR to val, which is specified in u units
	 */

	public void setDTHR(double val, String u) {
		setDTHR(Units.from(u,val));
		units_.put("WCV_DTHR",u);
	}

	/* 
	 * Set vertical distance threshold ZTHR to val, which is specified in u units
	 */

	public void setZTHR(double val) {
		ZTHR = Math.abs(val);
	}

	/*
	 *  Set vertical distance threshold ZTHR to val, which is specified in internal units
	 */

	public void setZTHR(double val, String u) {
		setZTHR(Units.from(u,val));
		units_.put("WCV_ZTHR",u);
	}

	/*
	 *  Set horizontal time threshold TTHR to val, which is specified in seconds
	 */

	public void setTTHR(double val) {
		TTHR = Math.abs(val);
	}

	public void setTTHR(double val, String u) {
		setTTHR(Units.from(u,val));
		units_.put("WCV_TTHR",u);
	}

	/* 
	 * Set vertical time threshold TCOA to val, which is specified in seconds
	 */

	public void setTCOA(double val) {
		TCOA = Math.abs(val);
	}

	public void setTCOA(double val, String u) {
		setTCOA(Units.from(u,val));
		units_.put("WCV_TCOA",u);
	}

	public ParameterData getParameters() {
		ParameterData p = new ParameterData();
		updateParameterData(p);
		return p;
	}

	public void updateParameterData(ParameterData p) {
		p.setInternal("WCV_DTHR",DTHR,getUnits("WCV_DTHR"));
		p.setInternal("WCV_ZTHR",ZTHR,getUnits("WCV_ZTHR"));
		p.setInternal("WCV_TTHR",TTHR,getUnits("WCV_TTHR"));
		p.setInternal("WCV_TCOA",TCOA,getUnits("WCV_TCOA"));
	}

	public void setParameters(ParameterData p) {
		if (p.contains("WCV_DTHR")) {
			setDTHR(p.getValue("WCV_DTHR"));
			units_.put("WCV_DTHR",p.getUnit("WCV_DTHR"));
		}
		if (p.contains("WCV_ZTHR")) {
			setZTHR(p.getValue("WCV_ZTHR"));
			units_.put("WCV_ZTHR",p.getUnit("WCV_ZTHR"));
		}
		if (p.contains("WCV_TTHR")) {
			setTTHR(p.getValue("WCV_TTHR"));
			units_.put("WCV_TTHR",p.getUnit("WCV_TTHR"));
		}
		if (p.contains("WCV_TCOA")) {
			setTCOA(p.getValue("WCV_TCOA"));
			units_.put("WCV_TCOA",p.getUnit("WCV_TCOA"));
		}
	}

	public String getUnits(String key) {
		String u = units_.get(key);
		if (u == null) {
			return "unspecified";
		}
		return u;
	}

	public String toString() {
		return "WCV_DTHR = "+Units.str(getUnits("WCV_DTHR"),DTHR)+
				", WCV_ZTHR = "+Units.str(getUnits("WCV_ZTHR"),ZTHR)+
				", WCV_TTHR = "+Units.str(getUnits("WCV_TTHR"),TTHR)+
				", WCV_TCOA = "+Units.str(getUnits("WCV_TCOA"),TCOA);
	}

	public String toPVS(int prec) {
		return "(# DTHR := "+f.FmPrecision(DTHR,prec)+", ZTHR := "+f.FmPrecision(ZTHR,prec)+
				", TTHR := "+f.Fm1(TTHR)+", TCOA := "+f.Fm1(TCOA)+" #)";
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(DTHR);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(TCOA);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(TTHR);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(ZTHR);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		WCVTable other = (WCVTable) obj;
		if (Double.doubleToLongBits(DTHR) != Double.doubleToLongBits(other.DTHR))
			return false;
		if (Double.doubleToLongBits(TCOA) != Double.doubleToLongBits(other.TCOA))
			return false;
		if (Double.doubleToLongBits(TTHR) != Double.doubleToLongBits(other.TTHR))
			return false;
		if (Double.doubleToLongBits(ZTHR) != Double.doubleToLongBits(other.ZTHR))
			return false;
		return true;
	}

	public boolean contains(WCVTable tab) {
		return DTHR >= tab.DTHR && TCOA >= tab.TCOA && TTHR >= tab.TTHR && ZTHR >= tab.ZTHR;
	}

}
