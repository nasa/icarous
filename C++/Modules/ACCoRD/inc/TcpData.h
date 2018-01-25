/*
 * TcpData
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef TCPDATA_H_
#define TCPDATA_H_

#include "Position.h"
#include "Point.h"
#include "Velocity.h"
#include "NavPoint.h"

namespace larcfm {

/** 
 * A data structure to maintain certain information related to a trajectory change point (TCP).  Typically,
 * this information is associated with a {@link NavPoint} through a {@link Plan}.  The major purpose of this class is to
 * store the relevant TCP information (e.g., status information, acceleration values, etc.).<p>
 *  
 * Possible types of points.  
 * <ul>
 * <li>"Orig" are from the original plan with no change
 * <li>"Virtual" are temporary points, generally interpolated points (long legs are broken into segments).  Virtual 
 *   points will be silently over-written, and never register as overlapping with other points.  
 *   Virtual points may be deleted unexpectedly.  Not for general use. Virtual points are internal 
 *   to Stratway and not expected to be returned to the pilot or plane's automation.
 * <li>"AltPreserve" are marked points used in trajectory generation
 * </ul>
 * 
 * Each point can be a turn point (essentially a horizontal acceleration point), a ground speed acceleration
 * point, or a vertical speed acceleration point.  These points are indicated by either being the 
 * beginning of an acceleration zone or the end of an acceleration zone.  Thus, there are methods
 * to indicate the beginning of a turn ({@link #isBOT}), or the end of the a turn ({@link #isEOT}), similarly for
 * ground speed of vertical speed. 
 */
class TcpData {

public:
	//TODO make this private once conversion is complete
	/** Major types of NavPoints, UNKNOWN_WT only used when parsing a string representation */
	enum WayType {Orig, Virtual, AltPreserve, UNKNOWN_WT};

	/** UNKNOWN values should ONLY be used when parsing a string representation fails */
	//TODO make this private once conversion is complete
	enum TrkTcpType {NONE, BOT, EOT, EOTBOT, UNKNOWN_TRK};
	//TODO make this private once conversion is complete
	enum GsTcpType {NONEg, BGS, EGS, EGSBGS, UNKNOWN_GS};
	//TODO make this private once conversion is complete
	enum VsTcpType {NONEv, BVS, EVS, EVSBVS, UNKNOWN_VS};

	static const int TCP_OUTPUT_COLUMNS = 20;   // total number of output columns for full TCP (not including aircraft name)
	static const int MIN_OUTPUT_COLUMNS = 5;    // total number of output columns for linear (not including aircraft name)

private:
	WayType    ty;              // type of point
	TrkTcpType tcpTrk;	        // Trk TCP type
	GsTcpType  tcpGs;		    // Gs TCP type
	VsTcpType  tcpVs;		    // Vs TCP type
	double     radiusSigned;	// signed turn radius, positive values indicate right turn, negative values indicate left turn, zero indicates no specified radius
	Position   centerTurn;
	double     accelGs;         // signed gs-acceleration value
	double     accelVs;         // signed vs-acceleration value
//	Velocity   velocityInit_v;	// the initial velocity at the start of an acceleration zone.
	Position   sourcePosition_p;  // source position
	double     sourceTime_d;		// source time (if < 0, no valid source)
	int        linearIndex;	    // in a kinematic plan provides index to original linear plan
	std::string  information;

public:
	TcpData() :
		ty(Orig),
		tcpTrk(NONE),
		tcpGs(NONEg),
		tcpVs(NONEv),
		radiusSigned(0.0),
		centerTurn(Position::INVALID()),
		accelGs(0.0),
		accelVs(0.0),
		//velocityInit_v(Velocity::INVALID()),
		sourcePosition_p(Position::INVALID()),
		sourceTime_d(-1),
		linearIndex(-1),
		information("")
	{ }
	
	TcpData(const NavPoint& p) :
		ty(Orig),
		tcpTrk(NONE),
		tcpGs(NONEg),
		tcpVs(NONEv),
		radiusSigned(0.0),
		centerTurn(Position::INVALID()),
		accelGs(0.0),
		accelVs(0.0),
		//velocityInit_v(Velocity::INVALID()),
		sourcePosition_p(p.position()),
		sourceTime_d(p.time()),
		linearIndex(-1),
		information("")
	{ }

	TcpData(const TcpData& data) :
		ty(data.ty),
		tcpTrk(data.tcpTrk),
		tcpGs(data.tcpGs),
		tcpVs(data.tcpVs),
		radiusSigned(data.radiusSigned),
		centerTurn(data.centerTurn),
		accelGs(data.accelGs),
		accelVs(data.accelVs),
		//velocityInit_v(data.velocityInit_v),
		sourcePosition_p(data.sourcePosition_p),
		sourceTime_d(data.sourceTime_d),
		linearIndex(data.linearIndex),
		information(data.information)
	{ }
	
	//TODO remove this method once the conversion is complete
	TcpData::WayType getType() const;

	std::string getTypeString() const;
	static std::string toStringWayType(WayType t);
	static TcpData::WayType valueOfWayType(const std::string& t);


	bool isVirtual() const;

	bool isOriginal() const;

	bool isAltPreserve() const;
	
	void setType(const std::string& t);
	
	/** Make a new TcpData from the current
	 * @return a new TcpData object */
	TcpData copy() const;

	/** Set this object to "Virtual" 
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	TcpData setVirtual();


	/** Set this object to "Original" 
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	TcpData setOriginal();

	/** Set this object to "AltPreserve" 
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	TcpData setAltPreserve();

	/**
	 * Reset the TcpData into a blank configuration, except for the source position, 
	 * source time, and linear index, these are retained.
	 */
	void reset();

//	//TODO remove this once conversion is complete
//	TrkTcpType getTrackType() const;

	std::string getTrkTypeString() const;
	static std::string toStringTrkType(TrkTcpType t);
	static TcpData::TrkTcpType valueOfTrkType(const std::string& t);

//	//TODO remove this once conversion is complete
//	void setTrkType(TrkTcpType ty);
//
//	//TODO remove this once conversion is complete
//	GsTcpType getGsType() const;

	std::string getGsTypeString() const;
	static std::string toStringGsType(GsTcpType t);
	static TcpData::GsTcpType valueOfGsType(const std::string& t);

//	//TODO remove this once conversion is complete
//	void setGsType(GsTcpType ty);

//	//TODO remove this once conversion is complete
//	VsTcpType getVsType() const;

	std::string getVsTypeString() const;
	static std::string toStringVsType(VsTcpType t);
	static TcpData::VsTcpType valueOfVsType(const std::string& t);
	
//	//TODO remove this once conversion is complete
//	void setVsType(VsTcpType ty);

	double getRadiusSigned() const;
	double signedRadius() const;
	double turnRadius() const;
	TcpData setRadiusSigned(double r);

	Position turnCenter() const;

	TcpData setTurnCenter(const Position& center);


//	/** Sets the point to have the given track acceleration.  The initial velocity should already be set before this method is called. */
//	TcpData setTrkAccel(double omega);

	
//	/**
//	 * Return the  (signed) turn rate (i.e., position
//	 * acceleration in the "track" dimension) associated with this point. Turn rate is in rad/sec (positive
//	 * is clockwise/right, negative is counterclockwise/left), otherwise return 0.0;
//	 */
//	double trkAccel() const;
	
	double getGsAccel() const;
	double gsAccel() const;

	TcpData setGsAccel(double ga);

	double getVsAccel() const;
	double vsAccel() const;

	/** Set the vertical acceleration rate to the given value 
	 * @param va vertical acceleration
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	TcpData setVsAccel(double va);

//	Velocity getVelocityInit() const;
//	Velocity velocityInit() const;

	/** Set the initial velocity for this TcpData data object to the given value.
	 *  This must be the correct value for points defining the entry into acceleration zones (BOT, BGSC, BVSC).
	 */
	TcpData setVelocityInit(const Velocity& v);

	Position getSourcePosition() const;
//	Position sourcePosition() const;
	NavPoint sourceNavPoint() const;
	bool hasSource() const;


	/** Make a new TcpData from the current one with the source time metadata changed 
	 * @param sp new source position
	 * @param st new source time
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	TcpData setSource(const Position& sp, double st);
	TcpData setSource(const NavPoint& np);
	static TcpData makeSource(const NavPoint& np);

	TcpData setSourcePosition(const Position& sp);

	double getSourceTime() const;
//	double sourceTime() const;

	TcpData setSourceTime(double st);

	int getLinearIndex() const;

	TcpData setLinearIndex(int ix);

	std::string getInformation() const;

	void setInformation(const std::string& information);

	
	static TcpData makeFull(const std::string& ty, const std::string& tcp_trk, const std::string& tcp_gs, const std::string& tcp_vs,
			double radiusSigned, const Position& center,  double accel_gs, double accel_vs,
			const Velocity& velocityInit, const Position& sourcep, double sourcet, int linearIndex);
		
	
	static TcpData makeInvalid();
	/** 
	 * Make a new TcpData object with default parameters. This is essentially a 
	 * factory method for the no parameter constructor.
	 * Sometimes it is convenient to have a constructor.
	 * 
	 * @return a new TcpData object with default parameters.
	 */
	static TcpData makeOrig();
	
	
	/** Set this point as a "beginning of turn" point.
	 * 
	 * @param signedRadius right turns have a positive radius, left turns have a negative radius
	 * @param center center of turn
	 * @param linearIndex
	 * @return a reference to the current TcpData object
	 */
	TcpData setBOT(double signedRadius, const Position& center, int linearIndex);

//	/** Set this point as a "beginning of turn" point.
//	 *
//	 * @param velocityIn
//	 * @param trkAccel  the track turn rate, sometimes called omega.
//	 * @param linearIndex
//	 * @return a reference to the current TcpData object
//	 */
//	TcpData setBotAccel(double trkAccel, int linearIndex);


	void clearTrk();
	
	TcpData clearBOT();

	TcpData clearEOT();

	TcpData clearBGS();

	TcpData clearEGS();

	TcpData clearBVS();

	TcpData clearEVS();


	/** Set this point as a "end of turn" point. 
	 * 
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setEOT(int linearIndex);

	/** Set this point as a combined "end of turn and beginning of turn" point. 
	 * 
	 * @param signedRadius
	 * @param center the center of turn
	 * @param linearIndex the reference index in the associated linear plan for this TCP point
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setEOTBOT(double signedRadius, const Position& center, int linearIndex);


	/**
	 * Set this point as a "beginning of ground speed change" point
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setBGS(double a, int linearIndex);

	void clearGs();

	/**
	 * Set this point as a "ending of ground speed change" point
	 * 
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setEGS(int linearIndex);

	/**
	 * Set this point as a combined "end of ground speed change and beginning of ground speed change" point
	 * 
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setEGSBGS(double a, int linearIndex);


	/**
	 * Set this point as a "beginning of vertical speed change" point
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setBVS(double a, int linearIndex);

	void clearVs();

	/**
	 * Set this point as an "ending of vertical speed change" point
	 * 
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setEVS(int linearIndex);

	/**
	 * Set this point as a combined "ending of vertical speed change and beginning of vertical speed change" point
	 * 
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	TcpData setEVSBVS(double a, int linearIndex);

//	/** Makes a new NavPoint that is an intermediate "mid" added point */
//	TcpData makeMidpoint(Position p, double t, int linearIndex) {
//		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,   this.tcp_gs, this.tcp_vs, 
//				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit,  this.sourcePosition, this.sourceTime, linearIndex);
//		return this;
//	}
	
	
	
	
	
	bool isInvalid() const;
	
	/** true if this point is a TCP 
	 * @return tcp status 
	 * */
	bool isTCP() const;

	/** true if this point is part of a turn 
	 * @return track tcp status
	 * */
	bool isTrkTCP() const;

	/** true if this point is the start of a turn 
	 * @return BOT status
	 * */
	bool isBOT() const;

	/** true if this point is the end of a turn 
	 * @return EOT status
	 * */
	bool isEOT() const;

	/** true if this point is a ground speed point 
	 * @return groundspeed tcp status
	 * */
	bool isGsTCP() const;

	/** true if this point is a beginning of ground speed change point 
	 * @return BGS status
	 * */
	bool isBGS() const;

	/** true if this point is a end of ground speed change point 
	 * @return EGS status
	 * */
	bool isEGS() const;

	/** true if this point is part of a vs change 
	 * @return vertical speed tcp status
	 * */
	bool isVsTCP() const;

	/** true if this point is the start of a vs change 
	 * @return BVS status
	 * */
	bool isBVS() const;

	/** true if this point is the end of a vs change 
	 * @return EVS status
	 * */
	bool isEVS() const;

	/** true if this point is one of the beginning TCP points, including the combined points (e.g., EOTBOT)
	 * 
	 * @return beginning TCP point status
	 */
	bool isBeginTCP() const;

	/** true if this point is one of the ending TCP points, including the combined points (e.g., EOTBOT)
	 * 
	 * @return ending TCP point status
	 */
	bool isEndTCP() const;
	

	/**
	 * Can this point be merged with the given point p?  
	 * @param point the other point
	 * @return true, if the points can be merged.
	 */
    bool mergeable(const TcpData& point) const;

	/**
	 * Creates a new point that is the merger of the this point and the given
	 * point.  Assumes that mergeable() on the two points would return true, if
	 * not, then approximately the original point is returned. <p>
	 *
	 * Merging rules:
	 * <ul>
	 * <li> mergeTCPInfo() is commutative
	 * <li> If both points are linear points, then the points are merged.
	 * <li> If one point is a linear point (aka, not a TCP), then the TCP info (velocity and acceleration)
	 * from the TCP point is used.
	 * <li> If both points are TCP (aka acceleration points) of different types (Trk, Gs, Vs), then the points
	 * should merge without an issue, note: they are required to have the same "velocity in"
	 * <li> If both points are TCP (aka acceleration points) of the same type (Trk, Gs, or Vs), then the
	 * resulting point should be a combined point (e.g., EOTBOT), and the TCP information should be the information
	 * from the beginning point.
	 * <li> If both points are TCP "begin" points, then a message is provided if they have different source positions
	 * </ul>
	 *
	 * @param point the other point
	 * @return a new point that is the merger of the two points.
	 */
    TcpData mergeTCPInfo(const TcpData& point) const;



	/** Return a string including both the point name (if set) and any 
	 * tcp metadata represented as a string.  If neither exists, return the empty string. 
	 * 
	 * @param p associated NavPoint
	 * @param d TcpData object
	 * @return the full label
	 * */
	static std::string fullLabel(const NavPoint& p, const TcpData& d);
	
	/**
	 * This parses the label generated for TCP types to reconstruct the meta-data of the point.  
	 * It returns either the same point (if no significant data is in the label) or a fully reconstructed TCP type.
	 * The reconstructed TCP will have a label field that does not contain any TCP metadata -- you will need to call name()
	 * or tcpLabel() to retrieve the full string.
	 * If it is detected that this string uses the old metatdata format, this returns an invalid NavPoint::
	 */
	std::pair<TcpData,std::string> parseMetaDataLabel(const NavPoint& default_source, const std::string& tlabel);


	std::string tcpTypeString() const;

	/**
	 * Return a string representation of TCP meta data (or the empty string if none)
	 * 
	 * @param t time of the associated NavPoint
	 * @param precision
	 * @return a string
	 */
	std::string metaDataLabel(double t, int precision) const;

	static std::pair<NavPoint,TcpData> parseLL(const std::string& s);
	
	static std::pair<NavPoint,TcpData> parseXYZ(const std::string& s);

	static TcpData::TrkTcpType toTrkTCP(int ty);
	static TcpData::GsTcpType toGsTCP(int ty);
	static TcpData::VsTcpType toVsTCP(int ty);


	std::string toString() const;

	//
	//
	//  THESE METHODS ARE JUST FOR THE CONVERSION
	//  REMOVE REFERENCES TO THESE METHODS ONCE THE CONVERSION IS COMPLETE
	//
	//
	//

//	static NavPoint make(const NavPoint& th, const TcpData& d);
//
//	static TcpData getTcpData(const NavPoint& th);



};

}

#endif /* TCPDATA_H_ */
