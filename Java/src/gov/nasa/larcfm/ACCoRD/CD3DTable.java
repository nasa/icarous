/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.HashMap;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;

public class CD3DTable implements ParameterTable {
	double D;
	double H;

	private HashMap<String,String> units_;

	public CD3DTable() {
		D = Units.from("nmi",5);
		H = Units.from("ft",1000);
		units_ = new HashMap<String,String>();
		units_.put("D","nmi");
		units_.put("H","ft");
	}

	public CD3DTable(double d, double h) {
		D = Math.abs(d);
		H = Math.abs(h);
		units_ = new HashMap<String,String>();
		units_.put("D","m");
		units_.put("H","m");
	}

	public CD3DTable(double d, String dunit, double h, String hunit) {
		D = Units.from(dunit, Math.abs(d));
		H = Units.from(hunit, Math.abs(h));
		units_ = new HashMap<String,String>();
		units_.put("D",dunit);
		units_.put("H",hunit);
	}

	/**
	 * Copy constructor -- returns a fresh copy.
	 */
	public CD3DTable(CD3DTable tab) {
		units_ = new HashMap<String,String>();
		copyValues(tab);
	}

	public CD3DTable copy() {
		return new CD3DTable(this);
	}

	public void copyValues(CD3DTable tab) {
		D = tab.D;
		H = tab.H;
		units_.clear();
		units_.putAll(tab.units_);
	}

	public double getHorizontalSeparation() {
		return D;
	}

	public void setHorizontalSeparation(double d) {
		D = Math.abs(d);
	}

	public double getVerticalSeparation() {
		return H;
	}

	public void setVerticalSeparation(double h) {
		H = Math.abs(h);
	}

	public double getHorizontalSeparation(String u) {
		return Units.to(u, D);
	}

	public void setHorizontalSeparation(double d, String u) {
		setHorizontalSeparation(Units.from(u,Math.abs(d)));
		units_.put("D",u);
	}

	public double getVerticalSeparation(String u) {
		return Units.to(u, H);
	}

	public void setVerticalSeparation(double h, String u) {
		setVerticalSeparation(Units.from(u,Math.abs(h)));
		units_.put("H",u);
	}

	public ParameterData getParameters() {
		ParameterData p = new ParameterData();
		updateParameterData(p);
		return p;
	}

	public void updateParameterData(ParameterData p) {
	  p.setInternal("D",D,getUnits("D"));
	  p.setInternal("H",H,getUnits("H"));
	}

	public void setParameters(ParameterData p) {
		if (p.contains("D")) {
			setHorizontalSeparation(p.getValue("D"));
			units_.put("D",p.getUnit("D"));
		}
		if (p.contains("H")) {
			setVerticalSeparation(p.getValue("H"));
			units_.put("H",p.getUnit("H"));
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
		return "D = "+Units.str(getUnits("D"),D)+", H = "+Units.str(getUnits("H"),H);
	}

	public String toPVS(int prec) {
		return "(# D:= "+f.FmPrecision(D,prec)+", H:= "+f.FmPrecision(H,prec)+" #)";
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(D);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(H);
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
		CD3DTable other = (CD3DTable) obj;
		if (Double.doubleToLongBits(D) != Double.doubleToLongBits(other.D))
			return false;
		if (Double.doubleToLongBits(H) != Double.doubleToLongBits(other.H))
			return false;
		return true;
	}

}

