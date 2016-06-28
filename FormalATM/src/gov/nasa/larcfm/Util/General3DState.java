/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;


/**
 * A "general state" object that holds Euclidean information about position and velocity for an object.
 * Currently this can either be a (point-mass) aircraft or a (possible morphing) polygon.
 * This is intended to be traffic information for a GeneralDetector object.  
 */
public class General3DState {
	private final Vect3 si;
	private final Velocity vi;
	private final MovingPolygon3D mp;
	
	public General3DState() {
		si = null;
		vi = null;
		mp = null;
	}
	
	public final static General3DState INVALID = new General3DState();
	public final static General3DState ZERO = new General3DState(Vect3.ZERO,Velocity.ZERO);
	
	public General3DState(Vect3 s, Velocity v) {
		si = s;
		vi = v;
		mp = null;
	}
	
	public General3DState(MovingPolygon3D p) {
		si = null;
		vi = null;
		mp = p;
	}
	
	public General3DState(General3DState g) {
		si = g.si;
		vi = g.vi;
		mp = g.mp.copy();
	}
	
	public boolean hasPointMass() {
		return si != null && vi != null;
	}
	
	public boolean hasPolygon() {
		return mp != null;
	}
	
	public Vect3 getVect3() {
		return si;
	}
	
	public Velocity getVelocity() {
		return vi;
	}
	
	public MovingPolygon3D getPolygon() {
		return mp;
	}
	
	public Vect3 representativePosition() {
		if (hasPointMass()) {
			return si;
		} else if (hasPolygon()) {
			return mp.position(0).averagePoint();
		} else {
			return Vect3.INVALID;
		}				
	}
	
	public Velocity representativeVelocity() {
		if (hasPointMass()) {
			return vi;
		} else if (hasPolygon()) {
			return mp.averageVelocity();
		} else {
			return Velocity.INVALID;
		}						
	}

	
	/**
	 * Advance this state by the indicated amount of (relative) time
	 */
	public General3DState linear(double dt) {
		if (hasPointMass()) {
			return new General3DState(si.AddScal(dt, vi), vi);
		} else if (hasPolygon()) {
			return new General3DState(mp.linear(dt));
		} else {
			return INVALID;
		}
	}

}
