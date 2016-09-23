/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.MovingPolygon2D;
import gov.nasa.larcfm.Util.MovingPolygon3D;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Poly2D;
import gov.nasa.larcfm.Util.Poly3D;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class CDPolycarp implements DetectionPolygon {

	ArrayList<Double> tin = new ArrayList<Double>();
	ArrayList<Double> tout = new ArrayList<Double>();
	ArrayList<Double> tca = new ArrayList<Double>();
	ArrayList<Double> tdist = new ArrayList<Double>();

	private double buff;
	private double fac;
	private boolean checkNice;
	private String id = "";

	public CDPolycarp() {
		buff = 0.1;
		fac = 0.001;
		checkNice = true;
	}

	public CDPolycarp(double b, double f, boolean check) {
		buff = b;
		fac = f;
		checkNice = check;
	}

	public double getBuff() {
		return buff;
	}

	public void setBuff(double x) {
		buff = x;
	}

	public double getFac() {
		return fac;
	}

	public void setFac(double x) {
		fac = x;
	}

	/**
	 * Set the flag for Polycarp to check that polygons are "nice" as part of the detection step.
	 * A non-nice polygon is considered bad input to the algorithm.
	 * This check is really only needed once for each polygon, so it may be redundant if detection
	 * is called repeatedly on an unchanging polygon. 
	 * If on, this check will also attempt to automatically reverse the order of clockwise polygons.
	 * @param b
	 */
	public void setCheckNice(boolean b) {
		checkNice = b;
	}

	/**
	 * Returns true if Polycarp will explicitly check niceness of polygons on each detection.
	 * A non-nice polygon is considered bad input to the algorithm.
	 * This check is really only needed once for each polygon, so it may be redundant if detection
	 * is called repeatedly on an unchanging polygon. 
	 * If on, this check will also attempt to automatically reverse the order of clockwise polygons.
	 */
	public boolean isNiceCheck() {
		return checkNice;
	}

	/**
	 * Polycarp only produces valid answers for "nice" polygons.
	 * These are simple polygons that have vertices in a counterclockwise ordering.
	 * If all polygons are assured to be "nice" before input, setCheckNice() can be set to FALSE, 
	 * increasing performance.
	 * @param mp static polygon to check
	 * @return true if the polygon is "nice", false otherwise.
	 */
	public boolean isNicePolygon(Poly3D mp) {
		return mp.getBottom() <= mp.getTop() && PolycarpContain.nice_polygon_2D(mp.poly2D().getVertices(), buff);
	}


	/**
	 * Polycarp only produces valid answers for "nice" polygons.
	 * These are simple polygons that have vertices in a counterclockwise ordering.
	 * If all polygons are assured to be "nice" before input, setCheckNice() can be set to FALSE, 
	 * increasing performance.
	 * @param mp moving polygon to check
	 * @param B start time
	 * @param T end time
	 * @return true if the polygon is "nice" for the time range of B to T, false otherwise.
	 */
	public boolean isNicePolygon(MovingPolygon3D mp, double B, double T) {
		return mp.minalt <= mp.maxalt && PolycarpDetection.nice_moving_polygon_2D(B, T, mp.horizpoly, buff, fac);
	}

	/**
	 * Returns a "nice" version of this poly3D, or null if unable.
	 * @param poly input poly, may be either clockwise or counterclockwise
	 * @return counterclockwise version, or null if there is something deeper that makes it not-nice.
	 */
	public Poly3D makeNicePolygon(Poly3D poly) {
		Poly3D ret = poly;
		if (!isNicePolygon(poly)) {			
			ret = poly.reverseOrder();
			if (!isNicePolygon(ret)) {
				return null;				
			}
		}
		return ret;
	}


	/**
	 * Return true if ownship is near an edge
	 * @param so ownship position
	 * @param si polygon
	 * @param h approximate horizontal buffer distance from edge
	 * @param v approximate vertical buffer distance from edge
	 * @return true if ownship is within d of an edge (including top and bottom)
	 */
	public boolean nearEdge(Vect3 so, Poly3D si, double h, double v) {
		return Polycarp3D.nearEdge(so, si, h, v, checkNice);
	}

	/**
	 * Return true if ownship is definitely inside a polygon (fence)
	 * @param so ownship position
	 * @param si polygon
	 * @return true if definitely inside
	 */
	public boolean definitelyInside(Vect3 so, Poly3D si) {
		return Polycarp3D.definitely_inside(so, si, buff, checkNice);
	}

	public boolean violation(Vect3 so, Velocity vo, Poly3D si) {
		return !Polycarp3D.definitely_outside(so, si, buff, checkNice);
	}


	// note: T cannot be "near" Double.MAX_VALUE (or infinity)
	public boolean conflict(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
		return Polycarp3D.entranceDetection(so, vo, si, B, T, buff, fac, checkNice);
	}

	// note: T cannot be "near" Double.MAX_VALUE (or infinity)
	public boolean conflictDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
		tin.clear();
		tout.clear();
		tca.clear();
		tdist.clear();
		IntervalSet times = Polycarp3D.conflictTimes(so, vo, si, B, T, buff, fac, true, checkNice);
		for (int i = 0; i < times.size(); i++) {
			tin.add(times.getInterval(i).low);
			tout.add(times.getInterval(i).up);
			double mid = (times.getInterval(i).low + times.getInterval(i).up)/2.0;
			tca.add(mid);
			Vect3 so3 = so.linear(vo, mid);
			Vect3 cent = si.position(mid).centroid();
			tdist.add(so3.distanceH(cent));
		}
		return times.size() > 0;
	}


	
//	@Override
//	public boolean outside(Vect3 so, Velocity vo, Poly3D si) {
//		return Polycarp3D.definitely_outside(so, si, buff);
//	}
//
//	@Override
//	public boolean inside(Vect3 so, Velocity vo, Poly3D si) {
//		return Polycarp3D.definitely_inside(so, si, buff);
//	}
//
//	@Override
//	public boolean willEnter(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		return Polycarp3D.detection(so, vo, si, B, T, buff, fac, true);
//	}
//
//	@Override
//	public boolean entranceDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		tin.clear();
//		tout.clear();
//		tca.clear();
//		tdist.clear();
//		IntervalSet times = Polycarp3D.conflictTimes(so, vo, si, B, T, buff, fac, true);
//		for (int i = 0; i < times.size(); i++) {
//			tin.add(times.getInterval(i).low);
//			tout.add(times.getInterval(i).up);
//			double mid = (times.getInterval(i).low + times.getInterval(i).up)/2.0;
//			tca.add(mid);
//			Vect3 so3 = so.linear(vo, mid);
//			Vect3 cent = si.position(mid).centroid();
//			tdist.add(so3.distanceH(cent));
//		}
//		return times.size() > 0;
//	}
//
//	@Override
//	public boolean willExit(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		return Polycarp3D.detection(so, vo, si, B, T, buff, fac, false);
//	}
//
//	@Override
//	public boolean exitDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		tin.clear();
//		tout.clear();
//		tca.clear();
//		tdist.clear();
//		IntervalSet times = Polycarp3D.conflictTimes(so, vo, si, B, T, buff, fac, false);
//		for (int i = 0; i < times.size(); i++) {
//			tin.add(times.getInterval(i).up);
//			tout.add(times.getInterval(i).low);
//			double mid = (times.getInterval(i).low + times.getInterval(i).up)/2.0;
//			tca.add(mid);
//			Vect3 so3 = so.linear(vo, mid);
//			Vect3 cent = si.position(mid).centroid();
//			tdist.add(so3.distanceH(cent));
//		}
//		return times.size() > 0;
//	}

	public List<Double> getTimesIn() {
		return tin;
	}

	public List<Double> getTimesOut() {
		return tout;
	}

	public List<Double> getCriticalTimesOfConflict() {
		return tca;
	}

	public List<Double> getDistancesAtCriticalTimes() {
		return tdist;
	}

	public ParameterData getParameters() {
		ParameterData p = new ParameterData();
		updateParameterData(p);
		return p;
	}

	public void updateParameterData(ParameterData p) {
		p.set("id", id);
		p.setInternal("buff", buff, "m");
		p.setInternal("fac",fac, "unitless");
		p.set("checkNice", checkNice);
	}

	public void setParameters(ParameterData p) {
		if (p.contains("id")) {
			id = p.getString("id");
		}
		if (p.contains("buff")) {
			buff = p.getValue("buff");
		}
		if (p.contains("fac")) {
			fac = p.getValue("fac");
		}
		if (p.contains("checkNice")) {
			checkNice = p.getBool("checkNice");
		}
	}

	public DetectionPolygon make() {
		return new CDPolycarp(buff,fac,checkNice);
	}

	public DetectionPolygon copy() {
		CDPolycarp cd = new CDPolycarp(buff,fac,checkNice);
		cd.id = id;
		return cd;
	}

	public String getClassName() {
		return getClass().getCanonicalName(); // "gov.nasa.larcfm.ACCoRD.CDPolyIter"
	}

	public String getSimpleClassName() {
		return getClass().getSimpleName();
	}

	public String getIdentifier() {
		return id;
	}

	public void setIdentifier(String s) {
		id = s;
	}

}
