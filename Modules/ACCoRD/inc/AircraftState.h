/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef AIRCRAFTSTATE_H_
#define AIRCRAFTSTATE_H_

#include "Position.h"
#include "Velocity.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Projection.h"
#include "Units.h"
#include "ErrorReporter.h"
#include "ErrorLog.h"
#include "StateVector.h"
#include <string>

namespace larcfm {


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
class AircraftState : public ErrorReporter {

  private:

  
    /** The percent difference in altitude that is assumed to be acceptable, used in prune() */
    static double MAX_RELATIVE_DIFF;
  
    static double minClimbVelocity;    // used to determine when a climb/descent occurs
  
    int bufferSize;    // the number of possible elements
    int oldest;        // the internal index to the oldest element
    int sz;          // the number of elements currently in list

    mutable ErrorLog error;
    

    std::string id;         // Store Aircraft id (name)
	
    // internal data structure, users use s(), v()
    Position* s_list;
    Velocity* v_list;
    double* t_list;
    
    // Euclidean positions and velocities created by a projection function  
    EuclideanProjection sp;
    Vect2* projS2;
    double* projH;
    Vect2* projV2;
    double* projVZ;
    double* projT;
    bool projection_initialized;
    bool projection_done;
    bool regression_done;
    
    // Regression parameters
	int    recentInd;               // Index of most recent time from the aircraft
	double horizvelintercept;
	double horizvelslope;
	double vertvelintercept;
	double vertvelslope;

	double ls_t;
	double ls_trk;
	double lastZeroTrackRateThreshold;


    void init(std::string name, int buffer_size);

  // This inserts the given data at point i--no questions asked.  Everything
  // from index 0..i is shifted down one place. i is an external index
  //
  // Assumes:
  // 1. 0 <= i < size
  // 2. i is the correct place to insert the data; time is correctly ordered: t(i) < tm < t(i+1)
  // 3. the projection_done flag is set by somewhere else
  void insertAt(int i, const Position& ss, const Velocity& vv, double tm); 

  // assumes that all arrays are the same length and have at least "length" elements
  // assumes the arrays are sorted in increasing time order.
  void calc(Vect2* vel2, double* velZ, double* timevar, int length);
	
  Vect3 predS(double t) const;
  
  Velocity predV(double t) const;
  
  int ext2int(int i) const;
  
  public:

    /** The maximum number of history points this detector will allow */
    static const int DEFAULT_BUFFER_SIZE = 10;

    static bool projectVelocity;                                        // DEBUG --> will eventually be removed

    /**
     * Construct a new object.
     * @param name the string name for this object
     * @param buffer_size the maximum number of data values that are stored in this object.
     */
    AircraftState(const std::string& name, int buffer_size);

    /** Construct a new object with the default buffer size */
    AircraftState();
    
    /** Construct a new object with the default buffer size 
     * @param id Aircraft identifier*/
    explicit AircraftState(const std::string& id);
    
    // copy constructor, needed because of explicit destructor
    AircraftState(const AircraftState &orig);

    // assignment operator, needed because of explicit destructor
    AircraftState& operator=(const AircraftState & rhs);

    ~AircraftState();
    
    /** Return the name of this object 
     * 
     * @return name
     */
    std::string name() const;
    
	/**  
	 * If the parameter ll matches the current latlon (true for lat/lon/alt, false 
	 * for Euclidean) value, then return true.  If there are
	 * no positions in this object, then true is always returned. 
     * 
     * @param ll value to check lat/lon status against
     * @return if ll agrees with the lat/lon status of this object 
     */
	bool checkLatLon(bool ll) const;

	/**
	 * 
	 * @return is lat/lon
	 */
	bool isLatLon() const;

    /** return a deep copy of this object 
     * @return the copy
     * */
    AircraftState copy() const;
    
            
    /** 
     * Return the maximum number of elements that can be stored.  
     * This value is always greater or equal to size().  
     * There is deliberately no method setBufferSize() 
     * @return maximum number of elements
     * */
    int getBufferSize() const;

    /** Clear all data */
    void clear();
    
    /** 
     * Return the index of the given time.  A negative index
     * is returned if the time doesn't exist.
     * 
     * @param time time
     * @return an index
     */
    int find(double time) const;
    
    /** Return the number of data elements 
     * @return size
     * */
    int size() const;


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
	void add(const Position& ss, const Velocity& vv, double tm);
	

	/**
	 * Return a Euclidean position and velocity for the given index.
	 * The position and velocity are projected into a Euclidean frame by the
	 * projection set by the setProjection() method.
	 * If the index is out of range (less than zero or greater than size()),
	 * then a zero position and velocity are returned.
	 * 
	 * @param i index
	 * @return position and velocity at index i
	 */
	StateVector get(int i);

	/**
	 * Return a Euclidean position and velocity for the latest time in this object.
	 * The position and velocity are projected into a Euclidean frame by the
	 * projection set by the setProjection() method.
	 * @return last position and velocity
	 */
	StateVector getLast();



	/** 
	 * The position for the given index.  An index of 0 corresponds to the
	 * earliest time in this object; and index of size()-1 corresponds to 
	 * the latest time.  This method returns a "zero" position for an out of bounds index
	 */
	const Position& position(int i) const;
	
	/** 
	 * The velocity for the given index.  An index of 0 corresponds to the
	 * earliest time in this object; and index of size()-1 corresponds to 
	 * the latest time.  This method returns a zero velocity for an out of bounds index
	 */
	const Velocity& velocity(int i) const;

	/** 
	 * The time of the given index.  An index of 0 corresponds to the
	 * earliest time in this object; and index of size()-1 corresponds to 
	 * the latest time.  This method returns a negative time for an out of bounds index 
	 * 
	 * @param i index
	 * @return time
	 */
	double time(int i) const;

	/** 
	 * The latest (i.e., last) position in this object.
	 * Note that this may not be synchronized with data from other aircraft!
	 * It is generally better to use positionLinear(tm) for traffic aircraft. 
	 * 
	 * @return last position
	 */
	Position positionLast() const;

	/** 
	 * The latest (i.e., last) velocity in this object.
	 * Note that this may not be synchronized with data from other aircraft!
	 * It is generally better to use velocityBefore(tm) for traffic aircraft. 
	 * 
	 * @return last velocity
	 */
	Velocity velocityLast() const;

	/** 
	 * The latest (i.e., last) time in this object.
	 * @return last time
	 * 
	 */
	double timeLast() const;
	


    /** remove oldest n entries 
     * @param n number of entries
     * */
    void remove(int n);

    /** remove any data older than the given time 
     * @param time value of time */
    void removeUpToTime(double time);

    /** remove the latest (i.e. newest) data point */
    void removeLast();


	/**
	 * Set the new projection, but do not do any calculations 
	 * Note that altitudes will be preserved when using this projection -- it should have a projection reference point with a zero altitude.
	 * @param p the new projection object
	 */
    void setProjection(const EuclideanProjection& p);

//    /**
//     * Set the new projection and reproject all the points
//	 * @param sp the new projection object
//     */
//	void updateProjection(const EuclideanProjection sp);

	/**
	 * Reproject all the points
	 */
	void updateProjection();

	/**
	 * Return the current projection used in this AircraftState
	 * Be sure to note if the returned projection point has a zero altitude or not.
	 * @return the current projection object
	 */
    EuclideanProjection getProjection() const;
    


	
	/**
	 * Return a predicted Euclidean position and velocity for the given time, based on 
	 * an analysis of the history data.  For
	 * accuracy, the time, t, should be close the latest time added to this class 
	 * (accessed through the method timeLast()).
	 * The positions and velocities used are projected into a Euclidean frame by the 
	 * projection set by the setProjection() method.
	 * @param t time
	 * @return position and velocity vector
	 */
	StateVector pred(double t);


	/**
	 * Return a predicted Euclidean position and velocity for the given time, based on 
	 * a linear extrapolation from the most recent (relative to t) data point.
	 * For accuracy, the time, t, should be close the latest time added to this class 
	 * (accessed through the method timeLast()).
	 * The positions and velocities used are projected into a Euclidean frame by the 
	 * projection set by the setProjection() method.
	 * 
	 * A warning is generated if all data points are at times greater than t.
	 * @param t time
	 * @return position and velocity vector
	 */
   StateVector predLinear(double t);

	/*
	 * Return a Position for a given time, linearly projected, based on the most recent data at or before time t.
	 * This does not perform any explicit projection into the Euclidean frame if the original Position was geodetic.
	 *
	 * A warning is generated if all data points are at times greater than t and an INVALID position is returned.
	 */
	Position positionLinear(double t) const;

	/**
	 * Returns the velocity data from the most recent entry at or before time t.
	 *
	 * Generates a warning and returns INVALID if there is no data.
	 * @param t time
	 * @return velocity
	 */
	Velocity velocityAt(double t) const;

	//
	// Methods to check the "integrity" of the data
	//

	/**
	 * Determines if the aircraft is almost in level flight
	 * @return true, if in level flight
	 */
	bool inLevelFlight();
	
	/**
	 * Determines if these two velocities are "close" to each other
	 * @param v1 velocity of one aircraft
	 * @param v2 velocity of another aircraft
	 * @return true, if close
	 */
	static bool closeEnough(Velocity v1, Velocity v2);

	/** Last time when track rate was near zero
	 * 
	 * @return time
	 */
	double lastStraightTime() const;


	/** Track Rate calculation
	 * @param i      index (0..size()-1)
	 * @return track rate at ith data location
	 */
	double trackRate(int i);

	/** EXPERIMENTAL
	 * returns time when track rate was near zero.  This method cannot return a value older than bufferSize. The companion
	 * method lastStraightTime is not as limited.
	 * @return time
	 */
	double timeLastZeroTrackRate();


	/** EXPERIMENTAL
	 * Estimate track rate from sequence of velocity vectors stored in this object.  The sign of the track rate indicates
	 * the direction of the turn
	 * @param numPtsTrkRateCalc   number of data points used in the average, must be at least 2
	 * @return signed track rate (if insufficient number of points, return 0);
	 */
	double avgTrackRate(int numPtsTrkRateCalc);

	/** EXPERIMENTAL
	 * Estimate rate of change of vertical speed from sequence of velocity vectors stored in this object.  
	 * The sign of the vertical speed rate indicates the direction of the acceleration
	 * @param numPtsVsRateCalc   number of data points used in the average, must be at least 2
	 * @return signed vertical speed rate (if insufficient number of points, return 0);
	 */
	double avgVsRate(int numPtsVsRateCalc);

	/**
	 * Throw away old points that have accelerations "significantly" different from the latest
	 */
	void prune();

	//
	// Utility methods
	//
	
	/** Return a debug string 
	 * @return string */
	std::string dump() const;

	/** Return a string representation of this object 
	 * @return string
	 */
	std::string toString() const;
   
	/**
	 *  Returns a string representation of this object compatible with StateReader
	 * @return string
	 */
    std::string toOutput() const;
    
  // ErrorReporter Interface Methods
  
  bool hasError() const {
    return error.hasError();
  }
  bool hasMessage() const {
    return error.hasMessage();
  }
  std::string getMessage() {
    return error.getMessage();
  }
  std::string getMessageNoClear() const {
    return error.getMessageNoClear();
  }    
  
	
};

}
#endif
