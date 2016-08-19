/* AircraftState
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * manages a history of aircraft state information
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.Arrays;


/**
 * This class captures a limited history of an aircraft's position and velocity.  
 * This class essentially operates as an array, where the time field determines
 * the order.  It is intended that this method is called frequently and the 
 * oldest values are dropped.  The maximum number of values stored is available
 * through maxBufferSize().  The oldest value (that is, the smallest t) is always 
 * in element 0, the latest element (that is, the largest time) is always at index
 * size() - 1.<p>
 *
 * The primary accessors are position() and velocity() which are indexed from 0 to size()-1.
 * The latest values are located at size()-1.<p>
 * 
 * The method get() will return a Euclidean projection of the position and
 * velocity given the projection function in setProjection() or updateProjection().  Be careful:
 * if you do not specifically indicate which projection to use, this class will make
 * a choice for you, that is probably not what you expect.<p>
 * 
 * In addition, this method can predict future points (currently this uses a quadratic 
 * regression approximation).  These predictions are available through the methods pred(t), and predLinear(t).
 * These are based on the Euclidean projected points.
 */
public final class AircraftState implements ErrorReporter {

	/** The default maximum number of history points stored in this object */
	public static final int DEFAULT_BUFFER_SIZE = 10;

	public static final boolean projectVelocity = true;  // get rid of this eventually!!


    private int bufferSize;    // the number of possible elements
    private int oldest;        // the internal index to the oldest element
    private int size;          // the number of elements currently in list
    
    /** The percent difference in altitude that is assumed to be acceptable, used in prune() */
	private static final double MAX_RELATIVE_DIFF = 0.10;

    private String id;         // Store Aircraft id (name)
	
    // internal data structure, users use s(), v()
    private Position[] s_list;
    private Velocity[] v_list;
    private double[] t_list;
    
    // Euclidean positions and velocities created by a projection function  
    private EuclideanProjection sp;
    private Vect2 [] projS2;
    private double [] projH;
    private Vect2 [] projV2;
    private double [] projVZ;
    private double [] projT;
    private boolean projection_initialized;
    private boolean projection_done;
    private boolean regression_done;
    
    // Regression parameters
	private int    recentInd;               // Index of most recent time from the aircraft
	private double horizvelintercept;
	private double horizvelslope;
	private double vertvelintercept;
	private double vertvelslope;
	
	private ErrorLog error;

	// optional storage for protection zone information
	private double d_d;
	private double d_h;
	private double r_d;
	private double r_h;
	
	// experimental information about the time that a turn was initiated
	private double ls_t = -1000000;
	private double ls_trk;
	
	double lastZeroTrackRateThreshold = Units.from("deg/s",0.1);
	
    /**
     * Construct a new object.
     * @param name the string name for this object
     * @param buffer_size the maximum number of data values that are stored in this object.
     */
    public AircraftState(String name, int buffer_size) {
    	bufferSize = buffer_size < 1 ? DEFAULT_BUFFER_SIZE : buffer_size;
    	s_list = new Position[bufferSize];
    	v_list = new Velocity[bufferSize];
    	t_list = new double[bufferSize];
    	oldest = 0;
    	size = 0;
    	id = name;
    	projS2 = new Vect2[bufferSize];
    	projH = new double[bufferSize];
    	projV2 = new Vect2[bufferSize];
    	projVZ = new double[bufferSize];
    	projT = new double[bufferSize];
    	sp = Projection.createProjection(0,0,0);
    	projection_initialized = false;
    	projection_done = false;
    	regression_done = false;
    	d_d = -1;
    	d_h = -1;
    	r_d = -1;
    	r_h = -1;
    	error = new ErrorLog("AircraftState");
    	//fc = new FitCurve(); 
    }

    /** Construct a new object with the default buffer size */
    public AircraftState() {
    	this("Aircraft", DEFAULT_BUFFER_SIZE);
    }
    
    /** Construct a new object with the default buffer size */
    public AircraftState(String id) {
    	this(id, DEFAULT_BUFFER_SIZE);
    }
    
    /** Return the name of this object 
     * 
     * @return name
     */
    public String name() {
    	return id;
    }
    
	/**  
	 * If the parameter ll matches the current latlon (true for lat/lon/alt, false 
	 * for Euclidean) value, then return true.  If there are
	 * no positions in this object, then true is always returned. 
	 */
	public boolean checkLatLon(boolean ll) {
		if (size() == 0) {
			return true;
		}
		return ll == s_list[oldest].isLatLon();		
	}
	
	public boolean isLatLon() {
		return checkLatLon(true);
	}
	
    /** return a deep copy of this object */
    public AircraftState copy() {
    	AircraftState a = new AircraftState(id, bufferSize);
    	System.arraycopy(s_list, 0, a.s_list, 0, bufferSize);
    	System.arraycopy(v_list, 0, a.v_list, 0, bufferSize);
    	System.arraycopy(t_list, 0, a.t_list, 0, bufferSize);
    	a.oldest = oldest;
    	a.size = size;
    	a.sp = sp;
    	if (projection_done) {
    		System.arraycopy(projS2, 0, a.projS2, 0, a.size);
    		System.arraycopy(projH, 0, a.projH, 0, a.size);
    		System.arraycopy(projV2, 0, a.projV2, 0, a.size);
    		System.arraycopy(projVZ, 0, a.projVZ, 0, a.size);
    		System.arraycopy(projT, 0, a.projT, 0, a.size);
    	}
    	a.projection_initialized = projection_initialized;
    	a.projection_done = projection_done;
    	a.regression_done = regression_done;
    	a.d_d = d_d;
    	a.d_h = d_h;
    	a.r_d = r_d;
    	a.r_h = r_h;
    	
    	a.ls_t = ls_t;  // EXPERIMENTAL
    	a.ls_trk = ls_trk;
    	return a;
    }

    
    /** 
     * Return the maximum number of elements that can be stored.  
     * This value is always greater or equal to size().  
     * There is deliberately no method setBufferSize() 
     * */
    public int getBufferSize() {
    	return bufferSize;
    }

    /** Clear all data */
    public void clear() {
    	size = 0;
    	oldest = 0;
    	ls_t = -1000000;
    }
    
    /** 
     * Return the index of the given time.  A negative index
     * is returned if the time doesn't exist.
     */
    public int find(double time) {
    	for (int i = 0; i < size; i++) {
    		double tm = time(i);
    		if (time == tm) {
    			return i;
    		}
    		if (time < tm) {
    			return -i-1;
    		}
    	}
    	return -size-1;
    }
    
    /** Return the number of data elements */
    public int size() {
    	return size;
    }

    /** 
     * Add a new position and velocity vector for the given time. If the given
     * time is greater than any other time in the list, then this method will
     * operate very fast.  On the other hand, if the time is between some elements that already 
     * exist then the addition will be in the correct order, but may be fairly slow.  If 
     * an element is added that matches another time, then this point overwrites
     * the existing point.
     * 
     * @param ss the position at the given time
     * @param vv the velocity at the given time
     * @param tm the time of the position and velocity
     */
	public void add(Position ss, Velocity vv, double tm) {
//if (id.equals("own")) f.pln("AircraftSate.add "+ss+" "+vv+" "+tm+" "+Arrays.toString(Thread.currentThread().getStackTrace()));
		int i;
		if (size >= 1 && tm <= timeLast()) {
			i = find(tm);
			if (i >= 0 ) {
				s_list[ext2int(i)] = ss;
				v_list[ext2int(i)] = vv;
				//t_list[ext2int(i)] = tm;     // unneeded, times must be the same.
			} else {
				if (i < -1) { 
					i = -i - 2;
					// invariant: i /= size()
					insertAt(i, ss, vv, tm);
			   	} else if (size < bufferSize) { // note i == -1 (insert before beginning)
			   		oldest = ext2int(-1);
					s_list[oldest] = ss;
					v_list[oldest] = vv;
					t_list[oldest] = tm;
					size++;
				} // note on this empty else clause: at this point an attempt is 
				  // being made to insert something before any other time into
				  // a list that is already full, this does nothing, so that
				  // is precisely what I do, nothing.
			}
		} else { // the list is empty or we are adding to the end.
			s_list[ext2int(size)] = ss;
			v_list[ext2int(size)] = vv;
			t_list[ext2int(size)] = tm;
			size++;
			if (size > bufferSize) {
				size = bufferSize;
				oldest = ext2int(1);
			}
		}
		projection_done = false;
	
		// EXPERIMENTAL
		if (size == 1) {
			ls_t = tm;
			ls_trk = vv.trk();
			//f.pln(id+" INIT: ls_t = "+ls_t+" ls_trk = "+Units.str("deg",ls_trk) );
		} else {
			if (tm > ls_t && Util.turnDelta(ls_trk,vv.trk())/(tm - ls_t) < lastZeroTrackRateThreshold) {
				ls_t = tm;
				ls_trk = vv.trk();
				//f.pln(id+ " at tm = "+tm+" REVISE: ls_t = "+ls_t+" ls_trk = "+Units.str("deg",ls_trk) );
			}
		}
	}
	
	
	/**
	 * Return a Euclidean position and velocity for the given index.
	 * The position and velocity are projected into a Euclidean frame by the
	 * projection set by the setProjection() method.
	 * If the index is out of range (less than zero or greater than size()),
	 * then a zero position and velocity are returned.
	 */
	public StateVector get(int i) {
		if (i >= size || i < 0) return new StateVector(Vect3.ZERO, Velocity.ZERO,0.0);//null;
		updateProjection();
		return new StateVector(new Vect3(projS2[i], projH[i]), Velocity.mkVxyz(projV2[i].x(), projV2[i].y(), projVZ[i]), time(i));
	}
	
	/**
	 * Return a Euclidean position and velocity for the latest time in this object.
	 * The position and velocity are projected into a Euclidean frame by the
	 * projection set by the setProjection() method.
	 */
	public StateVector getLast() {
	    return get(size() - 1);
	}


	// This inserts the given data at point i--no questions asked.  Everything
	// from index 0..i is shifted down one place. i is an external index
	//
	// Assumes:
	// 1. 0 &lt;= i &lt; size
	// 2. i is the correct place to insert the data; time is correctly ordered: t(i) < tm < t(i+1)
	// 3. the projection_done flag is set by somewhere else
	private void insertAt(int i, Position ss, Velocity vv, double tm) {
		if (size < bufferSize) {
			for(int j = 0; j <= i; j++) {
				int first = ext2int(j);
				int second = ext2int(j-1);
				s_list[second] = s_list[first];
				v_list[second] = v_list[first];
				t_list[second] = t_list[first];
			}
			s_list[ext2int(i)] = ss;
			v_list[ext2int(i)] = vv;
			t_list[ext2int(i)] = tm;
			oldest = ext2int(-1);
			size++;
		} else {
			for(int j = 1; j <= i; j++) {
				int first = ext2int(j);
				int second = ext2int(j-1);
				s_list[second] = s_list[first];
				v_list[second] = v_list[first];
				t_list[second] = t_list[first];
			}
			s_list[ext2int(i)] = ss;
			v_list[ext2int(i)] = vv;
			t_list[ext2int(i)] = tm;			
		}
	}

	/** 
	 * The position for the given index.  An index of 0 corresponds to the
	 * earliest time in this object; and index of size()-1 corresponds to 
	 * the latest time.  This method returns a "zero" position for an out of bounds index 
	 */
	public Position position(int i) {
		if (i >= size || i < 0) return checkLatLon(true) ? Position.ZERO_LL : Position.ZERO_XYZ; //{throw new RuntimeException("why?");} //return null;
		return s_list[ext2int(i)];
	}
	
	/** 
	 * The velocity for the given index.  An index of 0 corresponds to the
	 * earliest time in this object; and index of size()-1 corresponds to 
	 * the latest time.  This method returns a zero velocity for an out of bounds index 
	 */
	public Velocity velocity(int i) {
		if (i >= size || i < 0) return Velocity.ZERO; //null;
		return v_list[ext2int(i)];
	}

	/** 
	 * The time of the given index.  An index of 0 corresponds to the
	 * earliest time in this object; and index of size()-1 corresponds to 
	 * the latest time.  This method returns a negative time for an out of bounds index 
	 */
	public double time(int i) {
		if (i >= size || i < 0) return -1.0;
		return t_list[ext2int(i)];
	}

	/** 
	 * The latest (i.e., last) position in this object.
	 * Note that this may not be synchronized with data from other aircraft!
	 * It is generally better to use positionLinear(tm) for traffic aircraft. 
	 */
	public Position positionLast() {
		return position(size-1);
	}

	/** 
	 * The latest (i.e., last) velocity in this object.
	 * Note that this may not be synchronized with data from other aircraft!
	 * It is generally better to use velocityBefore(tm) for traffic aircraft. 
	 */
	public Velocity velocityLast() {
		return velocity(size-1);
	}

	/** 
	 * The latest (i.e., last) time in this object.
	 */
	public double timeLast() {
		return time(size-1);
	}
	
    
    /** remove oldest n entries */
    public void remove(int n) {
    	if (n <= 0) return;
    	if (n > size) n = size;
    	oldest = ext2int(n);
    	size = size - n;
    	projection_done = false;
    }
    
    /** remove any data older than the given time */
    public void removeUpToTime(double time) {
    	while (size() > 0) {
    		if (time(0) < time) {
    			remove(1);
    		} else {
    			break;
    		}
    	}    	
    }
 
    
    /** remove the latest (i.e. newest) data point */
    public void removeLast() {
    	if (size > 0) size = size - 1;
    	// the projection_done flag does not need to be set to false, 
    	//   because we are removing from the end of the list.  Thus, 
    	//   the arrays that hold the projected values will not be
    	//   corrupted when "size" shrinks. ????????????????????????????????????? RWB ????????????????????????????????????????????????
    }
    
//    /** remove entry for time tm */   DOES NOT WORK
//    public void remove(double tm) {
//    	int ix = find(tm);
//    	if (ix < 0) return;
//    	if (ix == 0) { // oldest element
//    		remove(1);
//    	} else {
//    		for (int j = ix; j+1 < size; j++) {
//    			s_list[ext2int(j)] = s_list[ext2int(j+1)];
//    			v_list[ext2int(j)] = v_list[ext2int(j+1)];
//    			t_list[ext2int(j)] = t_list[ext2int(j+1)];
//    		}
//    		size = size - 1;
//    	}
//    		
//    }

	/**
	 * Set the new projection, but do not do any calculations 
	 * Note that altitudes will be preserved when using this projection -- it should have a projection reference point with a zero altitude.
	 * @param p the new projection object
	 */
    public void setProjection(EuclideanProjection p) {
    	sp = p;
    	projection_initialized = true;
    	projection_done = false;
    }

//    /**
//     * Set the new projection and reproject all the points
//	 * @param sp the new projection object
//     */
//	private void updateProjection(EuclideanProjection sp) {
//		setProjection(sp);
//		updateProjection();
//	}
	
	/**
	 * Reproject all the points
	 */
	private void updateProjection() {
		// idea: have a dirty flag.  set it to true when setProjection is called.
		// Only call this if the flag is true, and then make it false
		if (projection_done || size == 0) {
			return; // no need to do any work.
		}
		if (position(0).isLatLon()) {
			if ( ! projection_initialized) {
				error.addWarning("No projection defined in updateProjection()");
				sp = Projection.createProjection(LatLonAlt.ZERO);
			}
			//f.pln(" $$$ AircraftState.updateProjection RECOMPUTE! sp = "+sp);
			for (int j = 0; j < size(); j++){                     // project all the points for aircraft id
				Position s = position(j);
				projS2[j] = sp.project2(s.lla());
				projH[j] = s.alt();
				if (projectVelocity) {
					Velocity v = sp.projectVelocity(s, velocity(j));
					projV2[j] = v.vect2(); 
					projVZ[j] = v.z();

				} else {
					Velocity v = velocity(j);
					projV2[j] = v.vect2();  
					projVZ[j] = v.z();
				}
				projT[j] = time(j);
			}
		} else {
			for (int j = 0; j < size(); j++){                     // Euclidean coordinates, no projection done
				Position s = position(j);
				projS2[j] = s.vect2();
				projH[j] = s.alt();
				Velocity v = velocity(j);
				projV2[j] = v.vect2();
				projVZ[j] = v.z();
				projT[j] = time(j);
			}
		}
        projection_done = true;
        regression_done = false;
	}

	/**
	 * Return the current projection used in this AircraftState
	 * Be sure to note if the returned projection point has a zero altitude or not.
	 * @return the current projection object
	 */
	public EuclideanProjection getProjection() {
		return sp;
	}
    
//	/**
//	 * Return a Euclidean position and velocity for the the given index.  
//	 * The position and velocity are projected into a Euclidean frame by the 
//	 * projection set by the setProjection() method.
//	 * If the index is out of range (less than zero or greater than size()), 
//	 * then a zero position and velocity are returned.
//	 */
//	public Pair<Vect3,Velocity> get(int i) {
//		if (i >= size || i < 0) return new Pair<Vect3,Velocity>(Vect3.ZERO, Velocity.ZERO);//null;
//		updateProjection();
//		return new Pair<Vect3,Velocity>(new Vect3(projS2[i], projH[i]), Velocity.mkVxyz(projV2[i].x(), projV2[i].y(), projVZ[i]));
//	}
	
	// assumes that all arrays are the same length and have at least "length" elements
	// assumes the arrays are sorted in increasing time order.
	private void calc(Vect2 pos2[], double posH[], Vect2 vel2[], double velZ[], double timevar[], int length) {
		if (regression_done) {
			return;
		}
		regression_done = true;
		if (length == 1) {
			horizvelintercept = vel2[0].norm();
			vertvelintercept  = velZ[0];
			horizvelslope     = 0;
			vertvelslope      = 0;
			recentInd		  = 0;
			return;
		}

		double sumv = 0;
		double sumt = 0;
		double sumtsq = 0;
		double sumvt = 0;
		double hsumv = 0;
		double hsumvt = 0;
		double timediff = 0;
		double vnorm = 0;
		double vertvelz = 0;
		double regdenom = 0;

		recentInd = length - 1;
// This search is not needed, currently, may be needed if data is out of order.
//		for (int point = 0; point < length; point++) {
//			if (timevar[point] > timevar[recentInd]) {
//				recentInd = point;
//			}
//		}

		for (int point = 0; point < length; point++) {
			timediff = timevar[point] - timevar[recentInd];
			vnorm = vel2[point].norm();
			vertvelz = velZ[point];

			sumv = sumv + vnorm;
			sumt = sumt + timediff;
			sumtsq = sumtsq + Math.pow(timediff, 2.0);
			sumvt = sumvt + vnorm * timediff;
			hsumv = hsumv + vertvelz;
			hsumvt = hsumvt + vertvelz * timediff;
			//f.pln("Inside Regression Loop: "+point+": sumv is "+sumv+" and sumt is "+sumt+" and sumtsq is "
			// +sumtsq+" and sumvt is "+sumvt+" and timediff is "+timediff);
		}
		regdenom = length * sumtsq - sumt * sumt;
		if (regdenom != 0) {
			horizvelintercept = (sumv * sumtsq - sumt * sumvt) / regdenom;
			horizvelslope = (length * sumvt - sumt * sumv) / regdenom;
			vertvelintercept = (hsumv * sumtsq - sumt * hsumvt) / regdenom;
			vertvelslope = (length * hsumvt - sumt * hsumv) / regdenom;
		}
		//f.pln(":: horizvelintercept = "+horizvelintercept);
		//f.pln(":: horizvelslope = "+horizvelslope);
		//f.pln(":: vertvelintercept = "+vertvelintercept);
		//f.pln(":: vertvelslope  = "+vertvelslope);
		return;
	}

	private Vect3 predS(double t) {
		//f.pln("calling predS with time = "+t);
		double trel = t - projT[size - 1];
		Vect2 predSxy = projS2[recentInd].AddScal(trel * (horizvelintercept + trel * horizvelslope / 2)
								* (1 / projV2[recentInd].norm()),projV2[recentInd]);
		double predAlt = projH[recentInd] + trel * (vertvelintercept + trel * vertvelslope / 2);
		return new Vect3(predSxy, predAlt);
	}
	
	private Velocity predV(double t) {
		//f.pln("Calling PredV with horizvelintercept = "+horizvelintercept+" and horizvelslope = "+horizvelslope);
		double trel = t - projT[recentInd];
		Vect2 predVxy = projV2[recentInd].Scal((horizvelintercept + trel * horizvelslope)
						* (1 / projV2[recentInd].norm()));
		double predVz = vertvelintercept + trel * vertvelslope;
		return Velocity.mkVxyz(predVxy.x(), predVxy.y(), predVz);
	}

	
	/**
	 * Return a predicted Euclidean position and velocity for the given time, based on 
	 * an analysis of the history data.  For
	 * accuracy, the time, t, should be close the latest time added to this class 
	 * (accessed through the method timeLast()).
	 * The positions and velocities used are projected into a Euclidean frame by the 
	 * projection set by the setProjection() method.
	 */
	public StateVector pred(double t) {
		updateProjection();
		calc(projS2, projH, projV2, projVZ, projT, size());
		return new StateVector(predS(t), predV(t),t);
	}	

	/**
	 * Return a predicted Euclidean position and velocity for the given time, based on 
	 * a linear extrapolation from the most recent (relative to t) data point.
	 * For accuracy, the time, t, should be close the latest time added to this class 
	 * (accessed through the method timeLast()).
	 * The positions and velocities used are projected into a Euclidean frame by the 
	 * projection set by the setProjection() method.
	 * 
	 * A warning is generated if all data points are at times greater than t.
	 */
	public StateVector predLinear(double t) {
		int i = size-1;
		while (i > 0 && Util.almost_greater(time(i),t,Util.PRECISION13)) {
			i--;
		}
		double dt = t - time(i);
		if (time(i) > t) {
			error.addWarning("predLinear: no data points before time");
		}
		Pair<Vect3,Velocity> sv = get(i).pair();
//if (id.equals("own")) f.pln(" ## predLinear: dt = "+dt+" sv.first = "+f.sStr(sv.first)+" sv.second = "+sv.second +" baseV="+velocity(i));
		return new StateVector(sv.first.AddScal(dt, sv.second), sv.second,t);
	}

	/*
	 * Return a Position for a given time, linearly projected, based on the most recent data at or before time t.
	 * This does not perform any explicit projection into the Euclidean frame if the original Position was geodetic.
	 * 
	 * A warning is generated if all data points are at times greater than t and an INVALID position is returned.
	 */
	public Position positionLinear(double t) {
		int i = size-1;
		while (i > 0 && Util.almost_greater(time(i),t,Util.PRECISION13)) {
			i--;
		}
		double dt = t - time(i);
		if (time(i) > t) {
			error.addWarning("predLinear: no data points before time");
			return Position.INVALID;
		}
		return position(i).linear(velocity(i), dt);
	}
	
	/*
	 * Return a Velocity for a given time, based on the most recent data at or before time t.
	 * 
	 * A warning is generated if all data points are at times greater than t and an INVALID velocity is returned.
	 */
	/**
	 * Returns the velocity data from the most recent entry at or before time t.
	 *
	 * Generates a warning and returns INVALID if there is no data.
	 */
	public Velocity velocityAt(double t) {
		int i = size-1;
		while (i > 0 && Util.almost_greater(time(i),t,Util.PRECISION13)) {
			i--;
		}
		if (time(i) > t) {
			error.addWarning("velocity: no data points before time");
			return Velocity.INVALID;
		}
		return velocity(i);
	}
	
	//
	// Methods to check the "integrity" of the data
	//

	public static final double minClimbVelocity = Units.from("fpm",150);    // used to determine when a climb/descent occurs

	/**
	 * Determines if the aircraft is almost in level flight
	 */
	public boolean inLevelFlight(){
        int point = size()-1;
        Velocity vol,vonl;
        if (point <= 0) {
        	return false;
        } else {
	  	   vol = get(point).v();
        }
        
        if (point == 1) {
        	vonl = vol;
        } else {
		    vonl = get(point-1).v();
        }
		return Math.abs(vol.z) < minClimbVelocity && Math.abs(vonl.z) < minClimbVelocity;
	}
	
	/**
	 * Determines if these two velocities are "close" to each other
	 */
	public static boolean closeEnough(Velocity v1, Velocity v2) {
  	     double delTrk = Math.abs(v1.trk() - v2.trk());
		 double delGs = Math.abs(v1.gs() - v2.gs()) ;
		 double delVs = Math.abs(v1.vs() - v2.vs()) ;
		 boolean trkOk = delTrk < Units.from("deg",10);
		 boolean gsOk = delGs < Units.from("kn",10);
		 boolean vsOk = delVs < Units.from("fpm",10);
         if (!trkOk)
        	 System.out.println(" $$>> delTrk = "+delTrk);
         if (!gsOk)
        	 System.out.println(" $$>> delGs = "+delGs);
         if (!trkOk)
        	 System.out.println(" $$>> delVs = "+delVs);
		 return trkOk && gsOk && vsOk;
	}

	/** For debugging, do not use */
	public boolean checkIntegrity() {  // for debug purposes only
		boolean chk = true;
		for (int j = 0; j < size()-1; j++ ) {
			//Triple<Vect3,Velocity,Double> trip1 = get(j);
			//Triple<Vect3,Velocity,Double> trip2 = get(j+1);
            Position s1 = s_list[j];
            Position s2 = s_list[j+1];
            Velocity v1 = v_list[j];
            //Velocity v2 = v_list[j+1];
            double   t1 = t_list[j];
            double   t2 = t_list[j+1];          
            NavPoint np1 = new NavPoint(s1,t1);
            NavPoint np2 = new NavPoint(s2,t2);
            Velocity calcVel = np1.initialVelocity(np2);
            if (!closeEnough(v1,calcVel)) {
            	System.out.println(" $$>> velocity from "+j+" to "+(j+1)+" not consistent!");
            	System.out.println(" $$>> v = "+v1+" calculated v = "+calcVel);
            	chk = false;
             }
		}
		return chk;
	}
	

	/** For debugging, do not use */
	public boolean repairVelocities() {  // for debug purposes only
		boolean chk = true;
		for (int j = 0; j < size()-1; j++ ) {
            Position s1 = s_list[j];
            Position s2 = s_list[j+1];
            //Velocity v1 = v_list[j];
            //Velocity v2 = v_list[j+1];
            double   t1 = t_list[j];
            double   t2 = t_list[j+1];          
            NavPoint np1 = new NavPoint(s1,t1);
            NavPoint np2 = new NavPoint(s2,t2);
            Velocity calcVel = np1.initialVelocity(np2);
            v_list[j] = calcVel;
		}
		System.out.println("AircraftHistory for "+id+"REPAIRED");
		return chk;
	}

	public boolean velocitiesAboutEqual(Velocity v1, Velocity v2) {
		double dTrk = Util.turnDelta(v1.trk(),v2.trk());
		if (dTrk > Units.from("deg",1.0)) return false;
		double dGs = Math.abs(v1.gs() - v2.gs());
		if (dGs > Units.from("kn",10.0)) return false;
		double dVs = Math.abs(v1.vs() - v2.vs());
		if (dVs > Units.from("fpm",100.0)) return false;		
		return true;
	}
	
	public boolean velocityChanging() {
        if (size > 0) {
		   Velocity v1 = velocity(size-1);
	        if (size > 1) {
	        	Velocity v2 = velocity(size-2);
	        	if (!velocitiesAboutEqual(v1,v2)) return true;
		        if (size > 2) {
		        	Velocity v3 = velocity(size-3);
		        	if (!velocitiesAboutEqual(v2,v3)) return true;
		        }
	        }
        }
        return false;
	}
	



	/** Last time when track rate was near zero
	 * 
	 * @return 
	 */
	public double lastStraightTime() {
		return ls_t;
	}

	/** Track Rate calculation
	 * @param i      index (0..size()-1)
	 * @return track rate at ith data location
	 */
	public double trackRate(int i) {
		if (i >= size || i < 0 || size < 2) return 0.0;
		StateVector svt = get(i);
		StateVector svtm1 = get(i-1);
		//f.pln(" $$$ svt = "+svt+" svtm1 = "+svtm1);
		return (svt.v().trk() - svtm1.v().trk())/(svt.t() - svtm1.t());
	}


	/** EXPERIMENTAL
	 * returns time when track rate was near zero.  This method cannot return a value older than bufferSize. The companion
	 * method lastStraightTime is not as limited.
	 * @return
	 */
	public double timeLastZeroTrackRate() {
		//f.pln(" $$$ timeLastZeroTrackRate: "+id+" size = "+size);
		if (size < 2) return -1;
        for (int j = size-1; j > 0; j--) {
        	double trkRate = trackRate(j);
        	//f.pln(get(j).t()+" $$$ timeLastZeroTrackRate: trkRate = "+Units.str("deg/s",trkRate));
        	if (Math.abs(trkRate) < lastZeroTrackRateThreshold) {
        		StateVector svt = get(j);
        		//f.pln(get(j).t()+" $$$ timeLastZeroTrackRate: svt.t() = "+Units.str("s",svt.t()));
                return svt.t();
        	}
        }
   		StateVector svt = get(0);
        return svt.t();
	}
	
	/** EXPERIMENTAL
	 * returns time when track rate was near zero.  This method cannot return a value older than bufferSize. The companion
	 * method lastStraightTime is not as limited.
	 * @return
	 */
	public boolean turnFinishing() {
		//f.pln(" $$$ timeLastZeroTrackRate: "+id+" size = "+size);
		ArrayList<Double> times = new ArrayList<Double>(10);
		ArrayList<Double> trackRates = new ArrayList<Double>(10);
		
		if (size < 2) return false;
		double trkLast = 0;
		double tmLast = 0;
		int n = size();
		int numPts = 5;    // typical rollin time
		int startI = Math.max(0,n - numPts);
		for (int i = startI; i < n && i >= 0; i++){                      // i = 0 is oldest, i = size() -1 is newest
			StateVector svt = get(i);
            //f.pln(" $$$ svt = "+svt);
			double track = svt.v().compassAngle();
			double tmTr = time(i);
			if (i > startI) {  // make sure trackLast is defined
				double tR = trackRate(i);
				double trackRate = Math.abs(Util.signedTurnDelta(trkLast,track)/(tmTr-tmLast));
				//f.pln(i+" >>>> finishingTurn: tmTr = "+tmTr+" tR = "+Units.str("deg/s",tR)+" trackRate = "+Units.str("deg/s",trackRate));
				//f.pln(" >>>> finishingTurn: tmTr = "+tmTr+" trackRate = "+trackRate+" trackRate = "+Units.str("deg/s",trackRate));
				//		+" trackRateSum = "+Units.str("deg/s",trackRateSum));
				times.add(tmTr);
				trackRates.add(trackRate);
			}
			trkLast = track;
			tmLast = tmTr;
		}
		Pair<Double,Double> lsq = LinearRegression.calculate(times,trackRates);
		//f.pln(" #### turnFinishing:  lsq.first = "+Units.str("deg/s",lsq.first));
		return lsq.first < Units.from("deg/s",-0.075);
	}
	
	
	/** EXPERIMENTAL
	 * Estimate track rate from sequence of velocity vectors stored in this object.  The sign of the track rate indicates
	 * the direction of the turn
	 * @param numPtsTrkRateCalc   number of data points used in the average, must be at least 2
	 * @return signed track rate (if insufficient number of points, return 0);
	 */
	public double avgTrackRate(int numPtsTrkRateCalc) {
		int n = size();
		if (numPtsTrkRateCalc < 2) numPtsTrkRateCalc = 2;
		int numPts = Math.min(numPtsTrkRateCalc,n);
		double trkLast = 0;
		double tmLast = 0;
		double trackRateSum = 0.0;
 		for (int i = n - 1; i > n - numPts - 1 && i >= 0; i--){                      // i = 0 is oldest, i = size() -1 is newest
			StateVector svt = get(i);
            //f.pln(" $$$ svt = "+svt);
			double track = svt.v().compassAngle();
			double tmTr = time(i);
			if (i < n-1) {  // make sure trackLast is defined
				double trackRate = Util.signedTurnDelta(trkLast,track)/(tmTr-tmLast);
				trackRateSum = trackRateSum + trackRate;
				//f.pln(" >>>> avgTrackRate: i = "+i+" track = "+Units.str("deg",track)+" trackRate = "+Units.str("deg/s",trackRate));
				//		+" trackRateSum = "+Units.str("deg/s",trackRateSum));
			}
			trkLast = track;
			tmLast = tmTr;
		}
		//f.pln(" >>>> avgTrackRate: numPts = "+numPts+" trackRateSum = "+Units.str("deg/s",trackRateSum));
		//return new Pair<Double,Boolean>(trackRateSum/(numPts-1),turnRight);
		if (numPts < 2) return 0;
		else return trackRateSum/(numPts-1);
	}

	
	/** EXPERIMENTAL
	 * Estimate rate of change of vertical speed from sequence of velocity vectors stored in this object.  
	 * The sign of the vertical speed rate indicates the direction of the acceleration
	 * @param numPtsVsRateCalc   number of data points used in the average, must be at least 2
	 * @return signed vertical speed rate (if insufficient number of points, return 0);
	 */
	public double avgVsRate(int numPtsVsRateCalc) {
		int n = size();
		if (numPtsVsRateCalc < 2) numPtsVsRateCalc = 2;
		int numPts = Math.min(numPtsVsRateCalc,n);
		double vsLast = 0;
		double tmLast = 0;
		double vsRateSum = 0.0;
 		for (int i = n - 1; i > n - numPts - 1 && i >= 0; i--){                      // i = 0 is oldest, i = size() -1 is newest
			StateVector svt = get(i);
            //f.pln(" $$$ svt = "+svt);
			double vs = svt.v().vs();
			double tmTr = time(i);
			if (i < n-1) {  // make sure trackLast is defined
				double vsRate = (vs-vsLast)/(tmTr-tmLast);
				vsRateSum = vsRateSum + vsRate;
				//f.pln(" >>>> avgvsRate: i = "+i+" track = "+Units.str("deg",track)+" vsRate = "+Units.str("deg/s",vsRate));
				//		+" vsRateSum = "+Units.str("deg/s",vsRateSum));
			}
			vsLast = vs;
			tmLast = tmTr;
		}
		//f.pln(" >>>> avgvsRate: numPts = "+numPts+" vsRateSum = "+Units.str("deg/s",vsRateSum));
		//return new Pair<Double,Boolean>(vsRateSum/(numPts-1),turnRight);
		if (numPts < 2) return 0;
		else return vsRateSum/(numPts-1);
	}

	
	
	/**
	 * Estimate track rate from sequence of velocity vectors.  Always returns a non-negative number
	 */
	public void printAccels() {
		int n = size();
		double trkLast = 0;
		double gsLast = 0;
		double vsLast = 0;
		double tmLast = 0;
 		f.pln("");
		f.pln(" time     trkRate    BankAngle    gsAccel    vsAccel");
		f.pln(" ----   ---------   ----------   ---------   -------");
		for (int i = 0; i < n; i++){                      // i = 0 is oldest, i = size() -1 is newest
			Pair<Vect3,Velocity> muPair = get(i).pair();
			Velocity v = muPair.getSecond();
			double track = v.compassAngle();
			double gs = v.gs();
			double vs = v.vs();
			double tmTr = time(i);
			//f.pln(" >>>> estimateOmega: i = "+i+" track= "+Units.str("deg",track));
			if (i > 0) {
				double trackRate = Math.abs(Util.turnDelta(trkLast,track)/(tmTr-tmLast));
				double gsAccel = (gs - gsLast)/(tmTr-tmLast);
				double vsAccel = (vs - vsLast)/(tmTr-tmLast);
				double bank = Kinematics.bankAngle(v.gs(),trackRate);
				if (i == n-1) f.p("T");
				f.pln("  "+(i-n+1)+"   "+f.Fm2(Units.to("deg",trackRate))+" [deg/s]  "+Units.str("deg",bank)+"       "+f.Fm2(gsAccel)+"      "+f.Fm2(vsAccel));
			}
			trkLast = track;
			gsLast = gs;
			tmLast = tmTr;
		}
	}

	
	
	/** EXPERIMENTAL
	 * @return a triple containing 3-components of acceleration: track-rate, ground speed acceleration, and vertical speed acceleration 
	 */
	public Triple<Double,Double,Double> calcAccels() {
		int n = size();
		//f.p(" >>>> calcAccels: name = "+name()+": n = "+n);
		if (n < 2) return new Triple<Double,Double,Double>(0.0,0.0,0.0);
		Velocity v = get(n-1).v();
		double track = v.compassAngle();
		double gs = v.gs();
		double vs = v.vs();
		double tmTr = time(n-1);
		Velocity vLast = get(n-2).v();
		double trkLast = vLast.compassAngle();
		double gsLast = vLast.gs();
		double vsLast = vLast.vs();
		double tmLast = time(n-2);
		//f.pln("  v = "+v+"  vLast = "+vLast); 
		double trackRate = Util.signedTurnDelta(trkLast,track)/(tmTr-tmLast);
		double gsAccel = (gs - gsLast)/(tmTr-tmLast);
		double vsAccel = (vs - vsLast)/(tmTr-tmLast);
		//double bank = Kinematics.calculatedBankAngle(v.groundSpeed(),trackRate);
		return new Triple<Double,Double,Double>(trackRate,gsAccel,vsAccel);
	}

	
	/**
	 * Throw away old points that have accelerations "significantly" different from the latest
	 */
	public void prune() {
		//dump();
		Velocity lastV = Velocity.ZERO;
		double lastT = 0.0;
		//f.pln("prune: initially, start = "+oldest+" size = " + size);
		if (size < 2)
			return;
		double baseAccel = (velocity(size-1).z - velocity(size - 2).z) / (time(size-1) - time(size - 2));
		//f.pln(" baseAccel = " + baseAccel);
		int largestPruned = -1;  // relative index
		lastV = velocity(0);
		lastT = time(0);
		for (int i = 1; i < size; i++) {
			//f.pln("!! s = " + s(i) + "  v =" + v(i) + " t = " + t(i));
			double delT = time(i) - lastT;
			double accel = (velocity(i).z - lastV.z) / delT;
			lastV = velocity(i);
			lastT = time(i);
			//f.pln("!! for i = "+i+" accel = " + accel);
			if (Math.abs((accel - baseAccel)/baseAccel) > MAX_RELATIVE_DIFF) {
				//f.pln(" prune "+i+" value "+accel);
				largestPruned = i;
			}
		}// for
		//f.pln(" largestPruned = "+largestPruned);
		if (largestPruned > 0) {
		   oldest = ext2int(largestPruned); 
		   size = size - largestPruned;
		   System.out.println("prune: Deleted "+largestPruned+" from aircraft "+id+"; start = "+oldest+" size = " + size);
		}
        //dump();
	}

	
    
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public double getDetDistance() {
    	return d_d;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public double getDetHeight() {
    	return d_h;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public double getResDistance() {
    	return r_d;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public double getResHeight() {
    	return r_h;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public void setDetDistance(double d) {
    	d_d = d;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public void setDetHeight(double h) {
    	d_h = h;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public void setResDistance(double d) {
    	r_d = d;
    }
    /** Storing protection zone data.  A value &lt;= 0 indicates no specific information.  Experimental. */
    public void setResHeight(double h) {
    	r_h = h;
    }

    /** Returns true if separation data has been defined for this Aircraft.  Experimantal */
    public boolean hasSeparationData() {
    	return d_d >0 && d_h > 0 && r_d > 0 && r_h >0;
    }
    
	//
	// Utility methods
	//
	
	private int ext2int(int i) {
		int j = (oldest+i) % bufferSize;
		if (j < 0) {
			return bufferSize + j;
		} else {
			return j;
		}
	}

	/** Return a debug string */
	public String dump() {
		String rtn = "";
		for (int i = 0; i < size; i++){
			rtn = rtn + id + "     "+position(i).toStringNP(12)+"    "+velocity(i).toStringNP(12)+"    "+time(i)+"\n";
		}
		return rtn;
	}
	
	/** Return debug string for last position */
	public String dumpLast() {
		String rtn = "";
		int last = size - 1;
		if (last > 0)
			rtn = id + ":  s= "+position(last).toStringNP(8)+" v = "+velocity(last).toStringNP(8)+" t = "+time(last)+"\n";
		return rtn;
	}
	
	/** Return a string representaiton of this object */
	public String toString() {
		String rtn = "AircraftState("+id+"): size = "+size+" start = "+oldest+"sepData="+hasSeparationData()+"(d_d="+d_d+" d_h="+d_h+" r_d="+r_d+" r_h="+r_h+")\n";
		rtn += " projection_initialized = "+projection_initialized+" projection_done = "+projection_done+"\n";
		for (int i = 0; i < size; i++){
			rtn = rtn + "  ---   s = "+position(i)+"  v = "+velocity(i)+" t = "+time(i)+"\n";
		}
		return rtn;
	}
	
	   /** string representation of this object showing Euclidean data*/
	public String toStringEucl() {
		String rtn = "AircraftState.toStringEucl -----------------------------------------------EE\n";
		rtn = rtn + "sp = "+sp+"\n";
		StateVector svt = getLast();
		Vect3 s = svt.s();
		Velocity v = svt.v();
		double t = svt.t();
		rtn = rtn +"  ["+name()+"] s = "+f.sStr(s)+" v = "+v.toStringXYZUnits()+" t = "+t+"\n";
	    rtn = rtn + "----------------------------------------------------------------------EE\n";	
		return rtn;
	}
	
	
	   /** string representation of this object showing Euclidean data*/
	public String dumpEucl() {
		String rtn = "Euclidean State of Aircraft id: "+id+" -----------------------------------------------EE\n";
		rtn = rtn + "sp = "+sp+"\n";
		rtn = rtn + " NAME     sx     sy       sz         trk      gs     vs        time\n";
		for (int i = 0; i < size; i++){
			StateVector svt = get(i);
			Vect3 s = svt.s();
			Velocity v = svt.v();
			double t = svt.t();
			//rtn = rtn+name()+" "+s.toString8NP()+" "+v.toString8NP()+" "+t+"\n";
			rtn = rtn+name()+" "+f.list2str(svt, 8, " ")+"\n";
		}
		rtn = rtn + "----------------------------------------------------------------------EE\n";	
		return rtn;
	}

	
	/**
	 *  Returns a string representation of this object compatible with StateReader
	 */
	public String toOutput() {
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < size; i++){
		  sb.append(id+" "+position(i).toStringNP(8)+" "+velocity(i).toStringNP(6)+" "+time(i)+"\n");
		}
		return sb.toString();
	}
	
	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError();
	}
	public boolean hasMessage() {
		return error.hasMessage();
	}
	public String getMessage() {
		return error.getMessage();
	}
	public String getMessageNoClear() {
		return error.getMessageNoClear();
	}

	
}
