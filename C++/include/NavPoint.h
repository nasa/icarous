/*
 * NavPoint.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef NAVPOINT_H_
#define NAVPOINT_H_

#include "Position.h"
#include "Point.h"
#include "Velocity.h"

namespace larcfm {
  
/** A position at a time.  This class is immutable.
 * Possible types of points.  
 * "Orig" are from the original plan with no change
 * "Virtual" are temporary points, generally interpolated points (long legs are broken into segments).  Virtual points will be silently over-written, and never register as overlapping with other points.  Virtual points may be deleted unexpectedly.  Not for general use.
 * "Added" are new points added by strategies.
 * "Modified" are original points that have been moved.
 * "AltPreserve" are marked points used in trajectory generation
 * 	Virtual points are internal to Stratway and not
 * expected to be returned to the pilot or plane's automation.
 */
class NavPoint {
public:

	/** Major types of NavPoints, UNKNOWN_WT only used when parsing a string representation */
	enum WayType {Orig, Virtual, AltPreserve, UNKNOWN_WT};

	/** UNKNOWN values should ONLY be used when parsing a string representation fails */
	enum Trk_TCPType {NONE, BOT, EOT, EOTBOT, UNKNOWN_TRK};
	enum Gs_TCPType {NONEg, BGS, EGS, EGSBGS, UNKNOWN_GS};
	enum Vs_TCPType {NONEv, BVS, EVS, EVSBVS, UNKNOWN_VS};

	static const int TCP_OUTPUT_COLUMNS = 20;
	static const int MIN_OUTPUT_COLUMNS = 5;

private:
	Position p;
	double t;
	WayType ty;
	std::string label_s;
	//mutable std::string tmp_label;
	//TCPType tcp;
	Trk_TCPType      tcp_trk;		        //
	Gs_TCPType       tcp_gs;		        //
	Vs_TCPType       tcp_vs;		        //
	Position sourcePosition_p;
	double sourceTime_d;
	//double   accel_trk;		    // omega (signed turn rate)
	double   sgnRadius;
	double   accel_gs;            // signed gs-acceleration value
	double   accel_vs;            // signed vs-acceleration value
	Velocity velocityIn_v;


public:


    NavPoint(const Position& pp, double tt, WayType tty,
   		     const std::string& llabel, 	Trk_TCPType tcp_t, Gs_TCPType tcp_g, Vs_TCPType tcp_v,
  			 double sRadius, double a_gs, double a_vs,
  			 const Velocity& v_velocityIn, const Position& sourcePos, double sourceTime
   		     ) ;

    /** Construct a zero NavPoint */
    NavPoint();

	/**
	 * Construct a new NavPoint from a position and a time.
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 */
    NavPoint(const Position& p, double t);

	/**
	 * Construction a non-TCP NavPoint piecemeal
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 */
    NavPoint(const Position& p, double t, const std::string& label);


    static NavPoint makeFull(const Position& p, double t, WayType ty, const std::string& label,
    	      Trk_TCPType tcp_trk, Gs_TCPType tcp_gs, Vs_TCPType tcp_vs,
    	      double sgnRadius, double accel_gs, double accel_vs,
    	      const Velocity& velocityIn, const Position& sourcePosition, double sourceTime);


    /**
     * Creates a new lat/lon NavPoint with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
     *
     * @param lat latitude [deg north latitude]
     * @param lon longitude [deg east longitude]
     * @param alt altitude [ft]
     * @param t   time
     */
    static NavPoint makeLatLonAlt(double lat, double lon, double alt, double t);


    /**
     * Creates a new Euclidean NavPoint with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
     *
     * @param x coordinate [nmi]
     * @param y coordinate [nmi]
     * @param z altitude [ft]
     * @param t time
     */
    static NavPoint makeXYZ(double x, double y, double z, double t);

    /**
     * Returns true if any components of this NavPoint are marked as Invalid
     */
    bool isInvalid() const;

    bool mergeable(const NavPoint& point) const;

    /**
     * Creates a new point that is the merger of the this point and the given
     * point.  Assumes that mergeable() on the two points would return true, if
     * not the orignal point is returned.
     *
     * @param point the other point
     * @return a new point that is the merger of the two points.
     */
    NavPoint mergeTCPInfo(const NavPoint& point) const;

    
    
//	/** Construct a new NavPoint from a position and a time */
//    NavPoint(const LatLonAlt& p, double t);
    
    /** A zero NavPoint, in lat/lon coordinates */
    static const NavPoint& ZERO_LL();
    /** A zero NavPoint, in xyz coordinates */
    static const NavPoint& ZERO_XYZ();

    static const NavPoint& INVALID();

    /**
     * Checks if two NavPoint are almost the same.
     * 
     * @param v NavPoint for comparison
     * 
     * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
     * to <code>v</code>.
     */
    bool almostEquals(const NavPoint& v) const;
    
	/**
	 * Checks if two NavPoint are almost the same in position.
	 * 
	 * @param v NavPoint for comparison
	 * 
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 */
    bool almostEqualsPosition(const NavPoint& v) const;

    /** Equality */
    bool operator == (const NavPoint& v) const;
    
    /** Equality */
    bool equals(const NavPoint& v) const;

    /** Inequality */
    bool operator != (const NavPoint& v) const;
    
    /** Return the horizontal position vector */
    Vect2  vect2() const;
    
    /** Return the three dimensional position vector */
    Point  point() const;
    
    /** Return the LatLonAlt object */
    const LatLonAlt& lla() const;
    
    /** Return the Position object */
    const Position& position() const;
    
    /** Return the x coordinate */
    double x() const;
    /** Return the y coordinate */
    double y() const;
    /** Return the z coordinate */
    double z() const;
    /** Return the latitude */
    double lat() const;
    /** Return the longitude */
    double lon() const;
    /** Return the altitude */
    double alt() const;
    /** Return the latitude in degrees north */
    double latitude() const;
    /** Return the longitude in degrees east */
    double longitude() const;
    /** Return the altitude in [ft] */
    double altitude() const;
    /** Return the x coordinate in [nmi] */
    double xCoordinate() const;
    /** Return the y coordinate in [nmi] */
    double yCoordinate() const;
    /** Return the z coordinate in [ft] */
    double zCoordinate() const;
    /** Return the time [s] */
    double time() const;

    Trk_TCPType getTrkTCP() const;
    Gs_TCPType getGsTCP() const;
    Vs_TCPType getVsTCP() const;

    /** Return the label (or name) for this point */
    const std::string& label() const;
    
	/** Return a string including both the point name (if set) and any tcp metadata (if any).  If neither exists, return the empty string. */
	std::string fullLabel() const;

	/**
	 * Return a string representation of TCP meta data (or the empty string if none)
	 */
	std::string metaDataLabel(int precision) const;
	/**
	 * This parses the label generated for TCP types to reconstruct the meta-data of the point.  
	 * It returns either the same point (if no significant data is in the label) or a fully reconstructed TCP type.
	 * The reconstructed TCP will have a label field that does not contain any TCP metadata -- you will need to call name()
	 * or tcpLabel() to retrieve the full string.
	 * If this detects an old metadata format, it will return an invalid Navpoint.
	 */
	const NavPoint parseMetaDataLabel(const std::string& tlabel) const;
	/**
	 * Re-parse this point as a TCP, if its label describes it as such.
	 * If this point is already a TCP or has no appropriate label, return unchanged.
	 * If this detects an old metadata format, it will return an invalid Navpoint.
	 * @return
	 */
	const NavPoint parseMetaDataLabel() const;

	const std::string tcpTypeString() const;

	/** Returns true if the "name" label of this NavPoint has been set. */
    bool isNameSet() const;
    
    
    /** Is this point specified in lat/lon? */
    bool isLatLon() const;
    /** Is this point Virtual? */
    bool isVirtual() const;
    /** Returns true if the point at index i is an added point, 
     false otherwise.  0 <= i < size() */
    bool isAdded() const;
    /** Returns true if the point at index i is an unmodified original point, 
     false otherwise.  0 <= i < size() */
    bool isOriginal() const;

	/** Returns true if the point at index i is a modified point, 
	   false otherwise.  0 <= i < size() */
    bool isAltPreserve() const;
	/**
	 * If there is a ground speed in constraint defined, return it (in internal units), otherwise return a negative value.
	 * All TCP points will have this defined, at a minimum
	 */
  	//double goalGsIn() const;


    /** String representation of the type */
    const std::string& strType() const;

    //double getRadius() const;

    /**
     * This returns the radius of the current turn.  If this is not a turn point or if the associated acceleration is 0.0, this returns
     * a radius of zero.
     */
    double turnRadius() const;


    /**
     * This returns the signed radius of the current turn.  If this is not a turn point or if the associated acceleration is 0.0, this returns
     * a radius of zero.  The sign indicates the direction of the turn.
     */
    double signedRadius() const;


    /**
     * This returns a center of turn position with the same altitude as the current point.  If the current point is not a turn point,
     * this returns an invalid position.
     */
    Position turnCenter() const;

    /** If associated with a trajectory change, returns the (original) time of the TCP, otherwise has no meaning. */
    double tcpSourceTime() const;
    /** If associated with a trajectory change, returns the (original) time of the TCP, otherwise has no meaning. */
    Position tcpSourcePosition() const;

    /** true if this point is a TCP */
	bool isTCP() const;

    /** true if this point is part of a turn */
	bool isTrkTCP() const;
    /** true if this point is the start of a turn */
	bool isBOT() const;
    ///** true if this point is the midpoint of a turn */
	//bool isTurnMid() const;
    /** true if this point is the end of a turn */
	bool isEOT() const;

	bool isGsTCP() const;
	bool isBGS() const;
	bool isEGS() const;

	double sourceTime() const ;

	Position sourcePosition() const ;

	double hasSource() const;

    /** true if this point is part of a vs change */
	bool isVsTCP() const;
    /** true if this point is the start of a vs change */
	bool isBVS() const;
    /** true if this point is the end of a vs change */
	bool isEVS() const;
    
	bool isBeginTCP() const;

	bool isEndTCP() const;


    /** Make a new NavPoint from the current one with the latitude changed */
    const NavPoint mkLat(const double lat) const;
    /** Make a new NavPoint from the current one with the X coordinate changed */
    const NavPoint mkX(const double x) const;  
    /** Make a new NavPoint from the current one with the longitude changed */
    const NavPoint mkLon(const double lon) const;  
    /** Make a new NavPoint from the current one with the Y coordinate changed */
    const NavPoint mkY(const double y) const;  
    /** Make a new NavPoint from the current one with the altitude changed */
    const NavPoint mkAlt(const double alt) const;
    /** Make a new NavPoint from the current one with the Z coordinate changed */
    const NavPoint mkZ(const double z) const;  
    /** Make a new NavPoint from the current one with the time changed */
    const NavPoint makeTime(const double time) const;

    //const NavPoint makeTCP(TCPType x) const;

    const NavPoint makeTrkTCP(Trk_TCPType tcp) const;

    const NavPoint makeGsTCP(Gs_TCPType tcp) const;

    const NavPoint makeVsTCP(Vs_TCPType tcp) const;

    const NavPoint makeSource(const Position& sp, double st) const;

    const NavPoint makeSourceTime(double x) const;
    const NavPoint makeSourcePosition(const Position& x) const;
//	/** Make a new NavPoint with current point's information as the "source" */
//	const NavPoint makeSourceClear() const;

    const NavPoint makeRadius(double r) const;

	const NavPoint makeTrkAccel(double omega) const;
	const NavPoint makeGsAccel(double ga) const;
	const NavPoint makeVsAccel(double va) const;

    const NavPoint makeVelocityIn(const Velocity& x) const;
//	/** Make a new NavPoint from the current one with the ground speed in metadata changed.  Set to -1 to remove the constraint. */
//    const NavPoint makeGoalGsIn(double gs) const;

    /** Make a new NavPoint from the current that is "Virtual" */
    const NavPoint makeVirtual() const;
    /** Make a new NavPoint from the current that is "Added" */
    const NavPoint makeAdded() const;
    /** Make a new NavPoint from the current that is "Original" */
    const NavPoint makeOriginal() const;
//    /** Make a new NavPoint from the current that is "Modified" */
//    const NavPoint makeModified() const;
	/** Make a new NavPoint from the current that is "AltPreserve" */
    const NavPoint makeAltPreserve() const;
//	/** If the current point is "Virtual", make a new point that is "Added", otherwise return this point */
//    const NavPoint makeVirtualAdded() const;

    const NavPoint makeStandardRetainSource() const;

    const NavPoint makeNewPoint() const;
//
//	/** Makes a new NavPoint that is devoid of any "turn" or "ground speed" tags. */
//	const NavPoint makeTCPClear() const;

    /** Make a new NavPoint from the current with the given name */
    const NavPoint makeLabel(const std::string& label) const;
    const NavPoint appendLabel(const std::string& label) const;
	/** Make a new "beginning of turn" NavPoint at the given position and time where the current NavPoint is the "center of turn" */
	const NavPoint makeBOT(const Position& p, double t,  const Velocity& v_velocityIn, double sRadius) const;
//	/** Make a new "middle of turn" NavPoint at the given position and time where the current NavPoint is the "center of turn" */
//	const NavPoint makeTurnMid(const Position& p, double t, double d_turnRate, const Velocity& v_velocityIn) const;
	/** Make a new "end of turn" NavPoint at the given position and time where the current NavPoint is the "center of turn" */
	const NavPoint makeEOT(const Position& p, double t, const Velocity& v_velocityIn) const;

	const NavPoint makeEOTBOT(const Position& p , double t, const Velocity& v_velocityIn, double sRadius) const;
	/** Make a new "beginning of ground speed change" NavPoint at the given position and time where the current NavPoint is the "center of gsc" */
	const NavPoint makeBGS(const Position& p, double t, double gsAccel, const Velocity& velocityIn) const;
	/** Make a new "end of ground speed change" NavPoint at the given position and time where the current NavPoint is the "center of gsc" */
	const NavPoint makeEGS(const Position& p, double t, const Velocity& velocityIn) const;
	const NavPoint makeEGSBGS(const Position& p , double t, double a, const Velocity& v_velocityIn) const;

	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the current NavPoint is the "center of vsc" */
	const NavPoint makeBVS(const Position& p, double t, double d_vsAccel, const Velocity& d_velocityIn) const;
	/** Make a new "end of vertical speed change" NavPoint at the given position and time where the current NavPoint is the "center of vsc" */
	const NavPoint makeEVS(const Position& p, double t, const Velocity& d_velocityIn) const;

	const NavPoint makeEVSBVS(const Position& p , double t, double a, const Velocity& v_velocityIn) const ;

	const NavPoint makeMidpoint(const Position& p, double t, const Velocity& d_velocityIn) const;

    /** Makes a new NavPoint that is qualified as a "begin of turn" (with no other special information)  */
	const NavPoint makeTCPTurnBegin() const;
    /** Makes a new NavPoint that is qualified as a "end of turn" (with no other special information)  */
	const NavPoint makeTCPTurnEnd() const;    
    /** Makes a new NavPoint that is qualified as a "middle of turn" (with no other special information) */
	//const NavPoint makeTCPTurnMid() const;
    /** Makes a new NavPoint that is qualified as a "begin of ground speed change" (with no other special information)  */
	const NavPoint makeTCPGSCBegin() const;
    /** Makes a new NavPoint that is qualified as a "end of ground speed change" (with no other special information)  */
	const NavPoint makeTCPGSCEnd() const;
    /** Makes a new NavPoint that is qualified as a "begin of vertical speed change" (with no other special information)  */
	const NavPoint makeTCPVSCBegin() const;
    /** Makes a new NavPoint that is qualified as a "end of vertical speed change" (with no other special information)  */
	const NavPoint makeTCPVSCEnd() const;
	/** Return a new NavPoint that shares all attributes with the specified NavPoint except Position and Time */
    const NavPoint makeMovedFrom(const NavPoint& o) const;
    /** Return a new NavPoint that shares all attributes with this one except position */
    const NavPoint makePosition(const Position& p) const;

    //const NavPoint revertToSource(const Position& p) const;

	/** 
	 * Calculate and return the initial velocity between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 */
    Velocity initialVelocity(const NavPoint& s) const;
    

	/** 
	 * Calculate and return the final velocity between the current point and the given point
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 *
	 * @param s the given NavPoint
	 */
    Velocity finalVelocity(const NavPoint& s) const;


	/**
	 * Calculate and return the vertical speed between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 */
    double verticalSpeed(const NavPoint& s) const;

	/** 
	 * Calculate and return the REPRESENTATIVE LINEAR velocity between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 */
    Velocity averageVelocity(const NavPoint& s) const;
    
    
	/** Perform a linear projection of the current NavPoint with given velocity and time.  If the 
	 * current NavPoint is in LatLonAlt, then a constant track angle linear projection is used.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 * 	 * @param time  relative time 
	 */
    const NavPoint linear(const Velocity& v, const double time) const;

	/** Compute a new lat/lon that is obtained by moving with velocity vo for tm secs
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 *
	 * @param vo   velocity away from original position
	 * @param tm   time of relocation
	 * @return new lat/lon position in direction v0
	 */
    const NavPoint linearEst(const Velocity& vo, double tm) const;

    
	/**
	 * return a new Navpoint between this Navpoint and np that corresponds to time by interpolation
	 * This function is commutative with respect to the points: direction between points is always determined by the time ordering of the two points.
	 * Negative time results in a velocity in the opposite direction (along the same great circle, if in latlon)
	 * 
	 * @param np    next NavPoint
	 * @param time  time 
	 * @return      interpolated NavPoint
	 */
    const NavPoint interpolate(const NavPoint& np, const double time) const;


	/** Horizontal distance */
    double distanceH(const NavPoint& np2) const;

	/** Vertical distance */
	double distanceV(const NavPoint& np2) const;


	/**
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * @param np   next NavPoint
	 * @return     ground speed between this Navpoint and np 
	 */
    double groundSpeed(const NavPoint& np) const;

//	/**
//	 * If this is a turn point, return the signed rate of turn in rad/sec (positive is clockwise/right, negative is counterclockwise/left), otherwise return 0.0;
//	 */
//    double rateOfTurn() const;

	/**
	 * If this is a TCP, return the velocity in metadata.  Otherwise return an invalid velocity.
	 * Not that this value is only well-defined for two case: the beginning of acceleration points (BGSC, BVSC, BOT) and points with a fixed gs mutability (where trk and vs may still not be well-defined)
	 */
    Velocity velocityIn() const;


	/**
	 * Return the associated (signed) acceleration, otherwise return 0.0.
	 */
    double trkAccel() const;

    /**
	 * Return the associated (signed) acceleration, otherwise return 0.0.
	 */
    double gsAccel() const;

    /**
	 * Return the associated (signed) acceleration, otherwise return 0.0.
	 */
    double vsAccel() const;

    static NavPoint parseLL(const std::string& s);

    static NavPoint parseXYZ(const std::string& s);


	/** String representation, using the default output precision (see Contants.get_output_precision()) */
    std::string toStringShort() const;
	/** String representation, using the give precision */
    std::string toStringShort(int precision) const;

	/** String representation, using the default output precision (see Contants.get_output_precision()) */
    std::string toString() const;
	/** String representation, using the give precision */
    std::string toString(int precision) const;
    
    std::vector<std::string> toStringList(int precision, bool tcp) const;

    /** Return a string representation that includes additional information */
    std::string toStringFull() const;
    /** Return a string representation that is consistent with the PathReader input format. */
    std::string toOutput() const;
	/** Return a string representation that is consistent with the PathReader input format.  If extended = true, include all fields. (mutability + tcpinfo + label) */
    std::string toOutput(int precision, bool tcp) const;

    static const std::string& toStringType(NavPoint::WayType ty);
    //const std::string& toStringMutabilityImpl(NavPoint::Mutability m) const;
    static const std::string& toStringTrkTCP(NavPoint::Trk_TCPType ty);
    static const std::string& toStringGsTCP(NavPoint::Gs_TCPType ty);
    static const std::string& toStringVsTCP(NavPoint::Vs_TCPType ty);

    static WayType WayTypeValueOf(const std::string& s);
    static Trk_TCPType Trk_TCPTypeValueOf(const std::string& s);
    static Gs_TCPType Gs_TCPTypeValueOf(const std::string& s);
    static Vs_TCPType Vs_TCPTypeValueOf(const std::string& s);

    static Trk_TCPType toTrkTCP(int ty);
    static Gs_TCPType toGsTCP(int ty);
    static Vs_TCPType toVsTCP(int ty);


  private:

    //unsigned int minorV_i;

    const NavPoint copy(const Position& p) const;
    const NavPoint copy(WayType ty) const;
    //const NavPoint copy(const PointMutability& mut) const;
//    std::string toStringMutability(std::string prefix, std::string suffix) const;
    

    static const std::string ORIG_STR;
    static const std::string ADDED_STR;
    static const std::string VIRTUAL_STR;
    static const std::string ALTPRESERVE_STR;
    static const std::string ELSE_STR;

    static const std::string NONE_STR;
    static const std::string BOT_STR;
    static const std::string EOT_STR;
    static const std::string EOTBOT_STR;
    static const std::string BVS_STR;
    static const std::string EVS_STR;
    static const std::string EVSBVS_STR;
    static const std::string BGS_STR;
    static const std::string EGS_STR;
    static const std::string EGSBGS_STR;

  };
  
  
}

#endif /* NAVPOINT_H_ */
