/*
 * Plan -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * Authors: George Hagen,  Jeff Maddalon,  Rick Butler
 */

package gov.nasa.larcfm.Util;

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
public class TcpData {
	/** total number of output columns for linear (not including aircraft name) */
	private static final int MIN_OUTPUT_COLUMNS = 5; //sx,sy,sz,time,label 
	/** total number of output columns for full TCP (not including aircraft name).  
	 * The column order is defined in Plan.toStringList().  
	 * There may be additional deprecated fields that are still supported by the readers
	 * This value is used in the readers, writers, and PolyPath (in order to have the correct number of columns in its toStringList() call). 
	 * */
	public static final int TCP_OUTPUT_COLUMNS = 20;
	
	
	//TODO make this private once conversion is complete
	public static enum WayType {
		Orig, Virtual, AltPreserve
	};

	//TODO make this private once conversion is complete
	public static enum TrkTcpType {
		NONE, BOT, EOT, EOTBOT
	};
	
	//TODO make this private once conversion is complete
	public static enum GsTcpType  {
		NONE, BGS, EGS, EGSBGS
	};
	
	//TODO make this private once conversion is complete
	public static enum VsTcpType  {
		NONE, BVS, EVS, EVSBVS
	};	
	
	private WayType    ty;              // type of point

	private TrkTcpType tcpTrk;	        // Trk TCP type
	private GsTcpType  tcpGs;		    // Gs TCP type
	private VsTcpType  tcpVs;		    // Vs TCP type
	private double     radiusSigned;	// signed turn radius, positive values indicate right turn, negative values indicate left turn, zero indicates no specified radius
	private Position   centerTurn; 

	private double     accelGs;         // signed gs-acceleration value
	private double     accelVs;         // signed vs-acceleration value
	private Position   sourcePosition;  // source position
	private double     sourceTime;		// source time (if < 0, no valid source)
	private int        linearIndex;	    // in a kinematic plan provides index to original linear plan
	private String     information;

	public TcpData() {
		ty = WayType.Orig;
		tcpTrk = TrkTcpType.NONE;
		tcpGs = GsTcpType.NONE;
		tcpVs = VsTcpType.NONE;
		radiusSigned = 0.0;	
		centerTurn = Position.INVALID;
		accelGs = 0.0;       
		accelVs = 0.0;       
		sourcePosition = Position.INVALID; 
		sourceTime = -1; // This means the point will be considered "ADDED"		
		linearIndex = -1;	
		information = "";			
	}
	
	public TcpData(TcpData data) {
		ty = data.ty;
		tcpTrk = data.tcpTrk;
		tcpGs = data.tcpGs;
		tcpVs = data.tcpVs;
		radiusSigned = data.radiusSigned;	
		centerTurn = data.centerTurn;
		accelGs = data.accelGs;       
		accelVs = data.accelVs;       
//		velocityInit = data.velocityInit;	  
		sourcePosition = data.sourcePosition; 
		sourceTime = data.sourceTime;		
		linearIndex = data.linearIndex;	
		information = data.information;			
	}
	
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(accelGs);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(accelVs);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result + ((centerTurn == null) ? 0 : centerTurn.hashCode());
		result = prime * result + ((information == null) ? 0 : information.hashCode());
		result = prime * result + linearIndex;
		temp = Double.doubleToLongBits(radiusSigned);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result + ((sourcePosition == null) ? 0 : sourcePosition.hashCode());
		temp = Double.doubleToLongBits(sourceTime);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result + ((tcpGs == null) ? 0 : tcpGs.hashCode());
		result = prime * result + ((tcpTrk == null) ? 0 : tcpTrk.hashCode());
		result = prime * result + ((tcpVs == null) ? 0 : tcpVs.hashCode());
		result = prime * result + ((ty == null) ? 0 : ty.hashCode());
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
		TcpData other = (TcpData) obj;
		if (Double.doubleToLongBits(accelGs) != Double.doubleToLongBits(other.accelGs))
			return false;
		if (Double.doubleToLongBits(accelVs) != Double.doubleToLongBits(other.accelVs))
			return false;
		if (centerTurn == null) {
			if (other.centerTurn != null)
				return false;
		} else if (!centerTurn.equals(other.centerTurn))
			return false;
		if (information == null) {
			if (other.information != null)
				return false;
		} else if (!information.equals(other.information))
			return false;
		if (linearIndex != other.linearIndex)
			return false;
		if (Double.doubleToLongBits(radiusSigned) != Double.doubleToLongBits(other.radiusSigned))
			return false;
		if (sourcePosition == null) {
			if (other.sourcePosition != null)
				return false;
		} else if (!sourcePosition.equals(other.sourcePosition))
			return false;
		if (Double.doubleToLongBits(sourceTime) != Double.doubleToLongBits(other.sourceTime))
			return false;
		if (tcpGs != other.tcpGs)
			return false;
		if (tcpTrk != other.tcpTrk)
			return false;
		if (tcpVs != other.tcpVs)
			return false;
		if (ty != other.ty)
			return false;
		return true;
	}

	public String getTypeString() {
		return ty.toString();
	}

	public boolean isVirtual() {
		return ty == WayType.Virtual;
	}

	public boolean isOriginal() {
		return ty == WayType.Orig;
	}

	public boolean isAltPreserve() {
		return ty == WayType.AltPreserve;
	}
	
	public void setType(String t) {
		WayType x;
		try {
			x = WayType.valueOf(t);
		} catch (IllegalArgumentException e) {
			x = ty;
		}
		ty = x;
	}
	
	/** Set this object to "Virtual" 
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	public TcpData setVirtual() {
		ty = WayType.Virtual;
		return this;
	}


	/** Set this object to "Original" 
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	public TcpData setOriginal() {
		ty = WayType.Orig;
		return this;
	}

	/** Set this object to "AltPreserve" 
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	public TcpData setAltPreserve() {
		ty = WayType.AltPreserve;
		return this;
	}
	
	/**
	 * Reset the TcpData into a blank configuration, except for the source position, 
	 * source time, and linear index, these are retained.
	 * 
	 */
	public void reset() {
		ty = WayType.Orig;
		tcpTrk = TrkTcpType.NONE;
		tcpGs = GsTcpType.NONE;
		tcpVs = VsTcpType.NONE;		
		radiusSigned = 0.0;	
		centerTurn = Position.INVALID;
		accelGs = 0.0;       
		accelVs = 0.0;       
		//setSource(source);     // retain the source information from the original TcpData object
		//linearIndex = -1;      // retain the linear index from the original TcpData object
		information = "";
	}
	
	/** Make a new TcpData from the current
	 * @return a new TcpData object 
	 */
	public TcpData copy() {
		return new TcpData(this);
	}

	public String getTrkTypeString() {
		return tcpTrk.toString();
	}

	public String getGsTypeString() {
		return tcpGs.toString();
	}

	public String getVsTypeString() {
		return tcpVs.toString();
	}
	
	public double getRadiusSigned() {
		return radiusSigned;
	}

	public double signedRadius() {
		return radiusSigned;
	}
	
	public double turnRadius() {
		return Math.abs(radiusSigned);
	}
	
	public Position turnCenter() {
		return centerTurn;
	}
	
	public TcpData setTurnCenter(Position center) {
		centerTurn = center;
		return this;
	}

	public TcpData setRadiusSigned(double r) {
		radiusSigned = r;
		return this;
	}

	public double getGsAccel() {
		return accelGs;
	}

	public double gsAccel() {
		return accelGs;
	}
	
	public TcpData setGsAccel(double ga) {
		accelGs = ga;
		return this;
	}

	public double getVsAccel() {
		return accelVs;
	}

	public double vsAccel() {
		return accelVs;
	}
	
	/** Set the vertical acceleration rate to the given value 
	 * @param va vertical acceleration
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	public TcpData setVsAccel(double va) {
		accelVs = va;
		return this;
	}

	public Position getSourcePosition() {
		return sourcePosition;
	}

//	public Position sourcePosition() {
//		return sourcePosition;
//	}
	
	public NavPoint getSource() {
		return new NavPoint(sourcePosition,sourceTime);
	}
	
	public boolean hasSource() {
    	return sourceTime >= 0;
    }

	
	/** Make a new TcpData from the current one with the source time metadata changed 
	 * @param sp new source position
	 * @param st new source time
	 * @return this TcpData object (for a.setX() type operations)
	 * */
	public TcpData setSource(Position sp, double st) {
		sourcePosition = sp;
		sourceTime = st;
		return this;
	}
	
	public TcpData setSource(NavPoint np) {
		sourcePosition = np.position();
		sourceTime = np.time();
		return this;
	}
	
	public static TcpData makeSource(NavPoint np) {
		TcpData tcp = new TcpData();
		tcp.setSource(np);
		return tcp;
	}

	public TcpData setSourcePosition(Position sp) {
		sourcePosition = sp;
		return this;
	}

	public double getSourceTime() {
		return sourceTime;
	}

//	public double sourceTime() {
//		return sourceTime;
//	}

	public TcpData setSourceTime(double st) {
		sourceTime = st;
		return this;
	}

	public int getLinearIndex() {
		return linearIndex;
	}

	public TcpData setLinearIndex(int ix) {
		linearIndex = ix;
		return this;
	}

	public String getInformation() {
		return information;
	}

	public TcpData setInformation(String information) {
		this.information = information;
		return this;
	}

	public TcpData appendInformation(String information) {
		this.information += information;
		return this;
	}

	public TcpData appendInformationNoDuplication(String information) {	
		if (this.information.contains(information) ||		// do nothing if this string is already in the label
				information.contains("$virtual") || // do not add virtual labels
				!information.startsWith("$")){ 
			return this;		// do nothing if this is a 4-character airport label
		}
		return appendInformation(information);
	}

	
	public static TcpData makeFull(String ty, String tcp_trk, String tcp_gs, String tcp_vs, double radiusSigned, Position center, double accel_gs, double accel_vs, 
			Velocity velocityInit, Position sourcep, double sourcet, int linearIndex) {
		
		TcpData d = new TcpData();
		d.ty = WayType.valueOf(ty.trim()/*.toUpperCase()*/);             
		d.tcpTrk = TrkTcpType.valueOf(tcp_trk.trim().toUpperCase());	 
		d.tcpGs = GsTcpType.valueOf(tcp_gs.trim().toUpperCase());		 
		d.tcpVs = VsTcpType.valueOf(tcp_vs.trim().toUpperCase());		
		d.setRadiusSigned(radiusSigned);
		d.setTurnCenter(center);
		d.setGsAccel(accel_gs);
		d.setVsAccel(accel_vs);
		//d.setVelocityInit(velocityInit);
		d.setSource(sourcep, sourcet);
		d.setLinearIndex(linearIndex);	
		//d.setInformation(information);
		
		return d;
	}
	
	public static TcpData makeInvalid() {
		TcpData d = new TcpData();
		d.setInformation("INVALID");		
		return d;
	}
	
	/** 
	 * Make a new TcpData object with default parameters. This is essentially a 
	 * factory method for the no parameter constructor.
	 * Sometimes it is convenient to have a constructor.
	 * 
	 * @return a new TcpData object with default parameters.
	 */
	public static TcpData makeOrig() {
		TcpData d = new TcpData();
		return d;
	}
	
	
	/** Set this point as a "beginning of turn" point.
	 * 
	 * @param signedRadius right turns have a positive radius, left turns have a negative radius
	 * @param center center of turn
	 * @param linearIndex
	 * @return a reference to the current TcpData object
	 */
	public TcpData setBOT(double signedRadius, Position center, int linearIndex) {
		//velocityInit = velocityIn;
		radiusSigned = signedRadius;
		centerTurn = center;
		this.linearIndex = linearIndex;
		tcpTrk = TrkTcpType.BOT;
		return this;
	}
	
	/** Set this point as a "beginning of turn" point.
	 * 
	 * @param signedRadius right turns have a positive radius, left turns have a negative radius
	 * @param bot
	 * @param track
	 * @param linearIndex
	 * @return a reference to the current TcpData object
	 */
	public TcpData setBotFindCenter(double signedRadius, Position bot, double track, int linearIndex) {
		//velocityInit = velocityIn;
		radiusSigned = signedRadius;
		centerTurn = KinematicsPosition.centerFromRadius(bot, signedRadius, track);
		this.linearIndex = linearIndex;
		tcpTrk = TrkTcpType.BOT;
		return this;
	}
	
//	/** Set this point as a "beginning of turn" point.
//	 * 
//	 * @param velocityIn
//	 * @param trkAccel  the track turn rate, sometimes called omega.
//	 * @param linearIndex
//	 * @return a reference to the current TcpData object
//	 */
//	public Velocity velocityIn, double trkAccel, int linearIndex) {
//		double radius = velocityIn.gs()/trkAccel;
//		return setBOT(velocityIn, radius, linearIndex);
//	}
		

	/** Set this point as a "end of turn" point. 
	 * 
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setEOT(int linearIndex) {
//		velocityInit = velocityIn;
		this.linearIndex = linearIndex;
		tcpTrk = TrkTcpType.EOT;
		return this;
	}

	/** Set this point as a combined "end of turn and beginning of turn" point. 
	 * 
	 * @param signedRadius
	 * @param center the center of turn
	 * @param linearIndex the reference index in the associated linear plan for this TCP point
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setEOTBOT(double signedRadius, Position center, int linearIndex) {
//		velocityInit = velocityIn;
		radiusSigned = signedRadius;
		centerTurn = center;
		this.linearIndex = linearIndex;
		tcpTrk = TrkTcpType.EOTBOT;
		return this;
	}


	/**
	 * Set this point as a "beginning of ground speed change" point
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setBGS(double a, int linearIndex) {
//		velocityInit = velocityIn;
		accelGs = a;
		this.linearIndex = linearIndex;
		tcpGs = GsTcpType.BGS;
		return this;
	}

	public void clearGs() {
		tcpGs = GsTcpType.NONE;
	}


	/**
	 * Set this point as a "ending of ground speed change" point
	 * 
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setEGS(int linearIndex) {
//		velocityInit = velocityIn;
		this.linearIndex = linearIndex;
		tcpGs = GsTcpType.EGS;
		return this;
	}

	/**
	 * Set this point as a combined "end of ground speed change and beginning of ground speed change" point
	 * 
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setEGSBGS(double a, int linearIndex) {
//		velocityInit = velocityIn;
		accelGs = a;
		this.linearIndex = linearIndex;
		tcpGs = GsTcpType.EGSBGS;
		return this;
	}

	/**
	 * Set this point as a "beginning of vertical speed change" point
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setBVS(double a, int linearIndex) {
//		velocityInit = velocityIn;
		accelVs = a;
		this.linearIndex = linearIndex;
		tcpVs = VsTcpType.BVS;
		return this;
	}
	
	public void clearVs() {
		tcpVs = VsTcpType.NONE;
	}

	/**
	 * Set this point as an "ending of vertical speed change" point
	 * 
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setEVS(int linearIndex) {
//		velocityInit = velocityIn;
		this.linearIndex = linearIndex;
		tcpVs = VsTcpType.EVS;
		return this;
	}

	/**
	 * Set this point as a combined "ending of vertical speed change and beginning of vertical speed change" point
	 * 
	 * @param a
	 * @param linearIndex
	 * @return this TcpData object (for a.setX() type operations)
	 */
	public TcpData setEVSBVS(double a, int linearIndex) {
//		velocityInit = velocityIn;
		accelVs = a;
		this.linearIndex = linearIndex;
		tcpVs = VsTcpType.EVSBVS;
		return this;
	}
	
	public boolean isInvalid() {
		return information.equals("INVALID");
	}
	
	/** true if this point is a TCP 
	 * @return tcp status 
	 * */
	public boolean isTCP() {
		return tcpTrk != TrkTcpType.NONE || tcpGs != GsTcpType.NONE || tcpVs != VsTcpType.NONE;
	}

	/** true if this point is part of a turn 
	 * @return track tcp status
	 * */
	public boolean isTrkTCP() {
		return tcpTrk != TrkTcpType.NONE;
	}

	/** true if this point is the start of a turn 
	 * @return BOT status
	 * */
	public boolean isBOT() {
		return tcpTrk == TrkTcpType.BOT || tcpTrk == TrkTcpType.EOTBOT;
	}

	/** true if this point is the end of a turn 
	 * @return EOT status
	 * */
	public boolean isEOT() {
		return tcpTrk == TrkTcpType.EOT || tcpTrk == TrkTcpType.EOTBOT;
	}

	/** true if this point is a ground speed point 
	 * @return groundspeed tcp status
	 * */
	public boolean isGsTCP() {
		return tcpGs != GsTcpType.NONE;
	}

	/** true if this point is a beginning of ground speed change point 
	 * @return BGS status
	 * */
	public boolean isEGSBGS() {
		return tcpGs == GsTcpType.EGSBGS;
	}
	
	/** true if this point is a beginning of ground speed change point 
	 * @return BGS status
	 * */
	public boolean isBGS() {
		return tcpGs == GsTcpType.BGS || tcpGs == GsTcpType.EGSBGS;
	}

	/** true if this point is a end of ground speed change point 
	 * @return EGS status
	 * */
	public boolean isEGS() {
		return tcpGs == GsTcpType.EGS || tcpGs == GsTcpType.EGSBGS;
	}

	/** true if this point is part of a vs change 
	 * @return vertical speed tcp status
	 * */
	public boolean isVsTCP() {
		return tcpVs != VsTcpType.NONE;
	}

	
	/** true if this point is the start of a vs change 
	 * @return BVS status
	 * */
	public boolean isEVSBVS() {
		return tcpVs == VsTcpType.EVSBVS;
	}

	/** true if this point is the start of a vs change 
	 * @return BVS status
	 * */
	public boolean isBVS() {
		return tcpVs == VsTcpType.BVS || tcpVs == VsTcpType.EVSBVS;
	}

	/** true if this point is the end of a vs change 
	 * @return EVS status
	 * */
	public boolean isEVS() {
		return tcpVs == VsTcpType.EVS || tcpVs == VsTcpType.EVSBVS;
	}

	/** true if this point is one of the beginning TCP points, including the combined points (e.g., EOTBOT)
	 * 
	 * @return beginning TCP point status
	 */
	public boolean isBeginTCP() {
		return tcpTrk == TrkTcpType.BOT || tcpGs == GsTcpType.BGS || tcpVs == VsTcpType.BVS ||
				tcpTrk == TrkTcpType.EOTBOT || tcpGs == GsTcpType.EGSBGS || tcpVs == VsTcpType.EVSBVS;
	}

	/** true if this point is one of the ending TCP points, including the combined points (e.g., EOTBOT)
	 * 
	 * @return ending TCP point status
	 */
	public boolean isEndTCP() {
		return tcpTrk == TrkTcpType.EOT || tcpGs == GsTcpType.EGS || tcpVs == VsTcpType.EVS ||
				tcpTrk == TrkTcpType.EOTBOT || tcpGs == GsTcpType.EGSBGS || tcpVs == VsTcpType.EVSBVS;
	}
	
	public void clearTrk() {
		tcpTrk = TrkTcpType.NONE;
	}
	
	public TcpData clearBOT() {
		if (tcpTrk == TrkTcpType.BOT) {
			tcpTrk = TrkTcpType.NONE;
		} else if (tcpTrk == TrkTcpType.EOTBOT) {
			tcpTrk = TrkTcpType.EOT;
		}
		return this;
	}
	
	public TcpData clearEOT() {
		if (tcpTrk == TrkTcpType.EOT) {
			tcpTrk = TrkTcpType.NONE;
		} else if (tcpTrk == TrkTcpType.EOTBOT) {
			tcpTrk = TrkTcpType.BOT;
		}
		return this;
	}

	public TcpData clearBGS() {
		if (tcpGs == GsTcpType.BGS) {
			tcpGs = GsTcpType.NONE;
		} else if (tcpGs == GsTcpType.EGSBGS) {
			tcpGs = GsTcpType.EGS;
		}
		return this;
	}

	public TcpData clearEGS() {
		if (tcpGs == GsTcpType.EGS) {
			tcpGs = GsTcpType.NONE;
		} else if (tcpGs == GsTcpType.EGSBGS) {
			tcpGs = GsTcpType.BGS;
		}
		return this;
	}

	public TcpData clearBVS() {
		if (tcpVs == VsTcpType.BVS) {
			tcpVs = VsTcpType.NONE;
		} else if (tcpVs == VsTcpType.EVSBVS) {
			tcpVs = VsTcpType.EVS;
		}
		return this;
	}

	public TcpData clearEVS() {
		if (tcpVs == VsTcpType.EVS) {
			tcpVs = VsTcpType.NONE;
		} else if (tcpVs == VsTcpType.EVSBVS) {
			tcpVs = VsTcpType.BVS;
		}
		return this;
	}

	/**
	 * Can this point be merged with the given point p?  
	 * @param tcp the other point
	 * @return true, if the points can be merged.
	 */
	public boolean mergeable(TcpData tcp) {
		// these are bad!!!
		//f.pln(" $$ mergeable: this =  "+this+" tcp = "+tcp);
		boolean r1 = (this.isBOT() && tcp.isBOT()) || (this.isEOT() && tcp.isEOT());
		boolean r2 = (this.isBGS() && tcp.isBGS()) || (this.isEGS() && tcp.isEGS());
		boolean r3 = (this.isBVS() && tcp.isBVS()) || (this.isEVS() && tcp.isEVS());
		return !r1 && !r2 && !r3;
	}


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
	public TcpData mergeTCPInfo(TcpData point) {
		TcpData d = new TcpData();
		// position & time -- keep either (should be the same)

		WayType ty = (this.ty == WayType.AltPreserve || point.ty == WayType.AltPreserve) ? WayType.AltPreserve : 
			((this.ty == WayType.Orig || point.ty == WayType.Orig) ? WayType.Orig : WayType.Virtual);
		d.ty = ty;
		//String label = this.label+point.label;
		d.tcpTrk = ((this.tcpTrk == TrkTcpType.BOT && point.tcpTrk == TrkTcpType.EOT) 
				|| (this.tcpTrk == TrkTcpType.EOT && point.tcpTrk == TrkTcpType.BOT)) ? TrkTcpType.EOTBOT : 
					((this.tcpTrk == TrkTcpType.BOT || point.tcpTrk == TrkTcpType.BOT) ? TrkTcpType.BOT : 
						((this.tcpTrk == TrkTcpType.EOT || point.tcpTrk == TrkTcpType.EOT) ? TrkTcpType.EOT : 
							((this.tcpTrk == TrkTcpType.EOTBOT || point.tcpTrk == TrkTcpType.EOTBOT) ? TrkTcpType.EOTBOT : TrkTcpType.NONE)));
		d.tcpGs = ((this.tcpGs == GsTcpType.BGS && point.tcpGs == GsTcpType.EGS) 
				|| (this.tcpGs == GsTcpType.EGS && point.tcpGs == GsTcpType.BGS)) ? GsTcpType.EGSBGS : 
					((this.tcpGs == GsTcpType.BGS || point.tcpGs == GsTcpType.BGS) ? GsTcpType.BGS : 
						((this.tcpGs == GsTcpType.EGS || point.tcpGs == GsTcpType.EGS) ? GsTcpType.EGS : 
							((this.tcpGs == GsTcpType.EGSBGS || point.tcpGs == GsTcpType.EGSBGS) ? GsTcpType.EGSBGS : GsTcpType.NONE)));
		d.tcpVs = ((this.tcpVs == VsTcpType.BVS && point.tcpVs == VsTcpType.EVS) 
				|| (this.tcpVs == VsTcpType.EVS && point.tcpVs == VsTcpType.BVS)) ? VsTcpType.EVSBVS : 
					((this.tcpVs == VsTcpType.BVS || point.tcpVs == VsTcpType.BVS) ? VsTcpType.BVS : 
						((this.tcpVs == VsTcpType.EVS || point.tcpVs == VsTcpType.EVS) ? VsTcpType.EVS : 
							((this.tcpVs == VsTcpType.EVSBVS || point.tcpVs == VsTcpType.EVSBVS) ? VsTcpType.EVSBVS : VsTcpType.NONE)));
//		double accel_trk = (this.tcpTrk == TrkTcpType.BOT || this.tcpTrk == TrkTcpType.EOT || this.tcpTrk == TrkTcpType.EOTBOT) ? this.accel_trk : point.accel_trk; 
		double radius = 0.0; // (this.tcpTrk != TrkTcpType.NONE || point.tcpTrk != TrkTcpType.NONE) ? 0 : this.sgnRadius;
		if (this.tcpTrk == TrkTcpType.BOT || this.tcpTrk == TrkTcpType.EOTBOT) radius = this.radiusSigned;
		else radius = point.radiusSigned;
		
		d.setRadiusSigned(radius);
		
		double accel_gs = (this.tcpGs == GsTcpType.BGS || this.tcpGs == GsTcpType.EGSBGS) ? this.accelGs : point.accelGs; 
		double accel_vs = (this.tcpVs == VsTcpType.BVS || this.tcpVs == VsTcpType.EVSBVS) ? this.accelVs : point.accelVs; 
		
		d.setGsAccel(accel_gs);
		d.setVsAccel(accel_vs);

		//Velocity velocityIn = (this.isBeginTCP()) ? this.velocityInit : point.velocityInit;
		
		//d.setVelocityInit(velocityIn);
		
		Position sourcePosition = (this.isBeginTCP()) ? this.sourcePosition : point.sourcePosition; 
		double sourceTime = (this.isBeginTCP()) ? this.sourceTime : point.sourceTime;
		
		d.setSource(sourcePosition, sourceTime);
		
		int linearIndex = (this.isBeginTCP()) ? this.linearIndex : point.linearIndex; 

		d.setLinearIndex(linearIndex);
		
		return d;
		//		
		//		
		//		// original code
		//		if (this.tcp_trk == TrkTcpType.BOT && point.tcp_trk == TrkTcpType.EOT) { // add end
		//			np = np.makeTrkTCP(TrkTcpType.EOTBOT);
		//		} else if (this.tcp_trk == TrkTcpType.EOT && point.tcp_trk == TrkTcpType.BOT) { // start
		//			np = np.makeTrkTCP(TrkTcpType.EOTBOT).makeTrkAccel(point.accel_trk).makeSource(point.sourcePosition,point.sourceTime);
		//		} else if (point.tcp_trk != TrkTcpType.NONE) { // general case
		//			np = np.makeTrkTCP(point.tcp_trk).makeTrkAccel(point.accel_trk).makeSource(point.sourcePosition,point.sourceTime);
		//		}
		//		
		//		if (this.tcp_gs == GsTcpType.BGS && point.tcp_gs == GsTcpType.EGS) { // add end
		//			np = np.makeGsTCP(GsTcpType.EGSBGS);
		//		} else if (this.tcp_gs == GsTcpType.EGS && point.tcp_gs == GsTcpType.BGS) { // start
		//			np = np.makeGsTCP(GsTcpType.EGSBGS).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition,point.sourceTime);
		//		} else if (point.tcp_gs != GsTcpType.NONE) { // general case
		//			np = np.makeGsTCP(point.tcp_gs).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition,point.sourceTime);
		//		}
		//
		//		if (this.tcp_vs == VsTcpType.BVS && point.tcp_vs == VsTcpType.EVS) { // add end
		//			np = np.makeVsTCP(VsTcpType.EVSBVS);
		//		} else if (this.tcp_vs == VsTcpType.EVS && point.tcp_vs == VsTcpType.BVS) {  // start
		//			np = np.makeVsTCP(VsTcpType.EVSBVS).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition,point.sourceTime);
		//		} else if (point.tcp_vs != VsTcpType.NONE) { // general case
		//			np = np.makeVsTCP(point.tcp_vs).makeVsAccel(point.accel_vs).makeSource(point.sourcePosition,point.sourceTime);
		//		}
		//		
		//		if (this.velocityIn.isInvalid()) {
		//			np = np.makeVelocityIn(point.velocityIn);
		//		}
		//		
		//		if (this.tcp_trk == TrkTcpType.BOT && point.tcp_vs == VsTcpType.BVS ||  // add warning
		//			this.tcp_vs == VsTcpType.BVS && point.tcp_trk ==  TrkTcpType.BOT) {
		//			f.pln(" $$$$!!!!!!!!!!!!! Warning: this point:"+np+" may not properly revert via removeTCPS");
		//		}
		//		
		//		np = np.appendLabel(point.label);
		//        return np;		
	}
	
	
	/**
	 * This parses the label generated for TCP types to reconstruct the meta-data of the point.  
	 * It returns either the same point (if no significant data is in the label) or a fully reconstructed TCP type.
	 * The reconstructed TCP will have a label field that does not contain any TCP metadata -- you will need to call name()
	 * or tcpLabel() to retrieve the full string.
	 * If it is detected that this string uses the old metatdata format, this returns an invalid NavPoint.
	 * 
	 * @param default_source the default source if one is not provided in the string
	 * @param string_to_parse the string to parse the TCP data out of
	 * @return A pair representing (1) the TCP data in the given string, and (2) any remaining part to the string that
	 * was not parsed (perhaps to be added to the label of the NavPoint.
	 */
	public Pair<TcpData,String> parseMetaDataLabel(NavPoint default_source, String string_to_parse) {
		//f.pln(" $$$$$ parseMetaDataLabel: tlabel = "+tlabel);
		int i = string_to_parse.indexOf(":ACC:");
		if (i >= 0) {
			return new Pair<TcpData,String>(TcpData.makeInvalid(),"INVALID"); //NavPoint.INVALID;       
		}
		TcpData point = new TcpData(this);
		int lowIndex = string_to_parse.length(); // then current end of the user label
		int j;
		i = string_to_parse.indexOf(":VIRT:");
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			point = point.setVirtual();
		}

		i = string_to_parse.indexOf(":ADDED:");
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			point = point.setSource(Position.INVALID,-1.0);
		} else {
			Position sp = default_source.position();
			double st = default_source.time();
			i = string_to_parse.indexOf(":SRC:");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				j = string_to_parse.indexOf(":",i+5);
				// first try general parse, then coordinate-specific if that fails
				sp = Position.parse(string_to_parse.substring(i+5, j).replaceAll("_", " "));
				if (sp.isInvalid()) {
					if (default_source.isLatLon()) {
						sp = Position.parseLL(string_to_parse.substring(i+5, j).replaceAll("_", " "));
					} else {
						sp = Position.parseXYZ(string_to_parse.substring(i+5, j).replaceAll("_", " "));
					}
				}
			}
			i = string_to_parse.indexOf(":STM:");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				j = string_to_parse.indexOf(":",i+5);
				st = Util.parse_double(string_to_parse.substring(i+5,j));
			}
			//f.pln(" $$$$$ parseMetaDataLabel: i = "+i+" st = "+st);
			point = point.setSource(sp,st);			
		}

		i = string_to_parse.indexOf(":VEL:"); // velocity in
		Velocity v = Velocity.ZERO;
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = string_to_parse.indexOf(":",i+5);
			v = Velocity.parse(string_to_parse.substring(i+5, j).replaceAll("_", " "));
			//point = point.setVelocityInit(v);
		}
		i = string_to_parse.indexOf(":ATRK:"); // acceleration
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = string_to_parse.indexOf(":",i+6);
			double a = Util.parse_double(string_to_parse.substring(i+6,j));
			//point = point.setTrkAccel(a);
			double r = 0.0;
			if ( a != 0.0) { // avoid divide by zero
				r = v.gs()/a; 
			}
			point.setRadiusSigned(r);
		}
		i = string_to_parse.indexOf(":AGS:"); // acceleration
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = string_to_parse.indexOf(":",i+5);
			double a = Util.parse_double(string_to_parse.substring(i+5,j));
			point = point.setGsAccel(a);
		}
		i = string_to_parse.indexOf(":AVS:"); // acceleration
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = string_to_parse.indexOf(":",i+5);
			double a = Util.parse_double(string_to_parse.substring(i+5,j));
			point = point.setVsAccel(a);
		}
		for (TrkTcpType t : TrkTcpType.values()) {
			i = string_to_parse.indexOf(":"+t+":");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				point.tcpTrk = t;
			}
		}
		for (GsTcpType t : GsTcpType.values()) {
			i = string_to_parse.indexOf(":"+t+":");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				point.tcpGs = t;
			}
		}
		for (VsTcpType t : VsTcpType.values()) {
			i = string_to_parse.indexOf(":"+t+":");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				point.tcpVs = t;
			}
		}
		String name = "";   //tlabel;
		//f.pln("lowIndex: "+lowIndex);
 		if (lowIndex > 0) { // lowIndex>=0 && lowIndex < string_to_parse.length()) {
			name = string_to_parse.substring(0,lowIndex);
			//point = point.makeLabel(name);
		}
		return new Pair<TcpData,String>(point, name);
	}


	public String tcpTypeString() {
		String s = "";
		if (tcpTrk != TrkTcpType.NONE) s += tcpTrk;
		if (tcpGs != GsTcpType.NONE) {
			if (!s.equals("")) s+= ",";
			s += tcpGs;
		}
		if (tcpVs != VsTcpType.NONE) {
			if (!s.equals("")) s+= ",";
			s += tcpVs;
		}
		if (s.equals("")) s += "NONE";
		return s;
	}
	
	/**
	 * Return a string representation of TCP meta data (or the empty string if none)
	 * 
	 * @param t time of the associated NavPoint
	 * @param precision
	 * @return a string
	 */
	public String metaDataLabel(double t, int precision) {
		String tlabel = "";
		if (isVirtual()) {
			tlabel = tlabel + "VIRT:";
		} 
		if (isTrkTCP()) {
			tlabel = tlabel + tcpTrk.toString() +":";
			if (isBOT()) {
				tlabel = tlabel + "ATRK:"+f.FmPrecision(radiusSigned,precision)+":";
			}
		}
		if (isGsTCP()) {
			tlabel = tlabel + tcpGs.toString() +":";
			if (isBGS()) {
				tlabel = tlabel + "AGS:"+f.FmPrecision(accelGs,precision)+":";
			}
		}
		if (isVsTCP()) {
			tlabel = tlabel + tcpVs.toString() +":";
			if (isBVS()) {
				tlabel = tlabel + "AVS:"+f.FmPrecision(accelVs,precision)+":";
			}
		}
//		if (velocityInit != null && !velocityInit.isInvalid()) {
//			String v = velocityInit.toStringNP(precision).replaceAll("[, ]+", "_");
//			tlabel = tlabel + "VEL:"+v+":";
//		}
		if (isTCP()){ // generated TCP points
			tlabel = tlabel + "SRC:"+sourcePosition.toStringNP(precision).replaceAll("[, ]+", "_")+":";
			tlabel = tlabel + "STM:"+f.FmPrecision(sourceTime,precision)+":";
		} else if (sourceTime != t) { // linear timeshifted point
			tlabel = tlabel + "STM:"+f.FmPrecision(sourceTime,precision)+":";
		}
		// add starting colon, if there is any data;
		if (tlabel.length() > 0) {
			tlabel = ":"+tlabel;
		}
		return tlabel;
	}

	

	
	public static Pair<NavPoint,TcpData> parseLL(String s) {
		String[] fields = s.split(Constants.wsPatternParens);
		Vect3 v = Vect3.parse(fields[0]+" "+fields[1]+" "+fields[2]);
		double time = Util.parse_double(fields[3]);
		Position pos = Position.makeLatLonAlt(v.x, v.y, v.z);
		if (fields.length == MIN_OUTPUT_COLUMNS+1) {
			String extra = fields[MIN_OUTPUT_COLUMNS];
			
			NavPoint np = new NavPoint(pos, time, extra);
			Pair<TcpData,String> p = (new TcpData()).parseMetaDataLabel(np,extra);
			if (! p.second.equals("")) {
				np.makeLabel(p.second);
			}
			return new Pair<NavPoint,TcpData>(np,p.first);
		} else if (fields.length == TCP_OUTPUT_COLUMNS+1) {
			String wt = fields[4];
			Velocity vv = Velocity.parse(fields[5]+" "+fields[6]+" "+fields[7]);
			String trkty = fields[8];
			//double trkacc = Units.from("NM", Util.parse_double(fields[9]));
			String gsty = fields[10];
			double gsacc = Units.from("m/s^2", Util.parse_double(fields[11]));
			String vsty = fields[12];
			double vsacc = Units.from("m/s^2", Util.parse_double(fields[13]));
			double radius = Util.parse_double(fields[14]);
			LatLonAlt slla = LatLonAlt.parse(fields[15]+" "+fields[16]+" "+fields[17]);
			Position sp = new Position(slla);
			double st = Util.parse_double(fields[18]);
			String lab = fields[19];
			//return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);			
			Position turnCenter = KinematicsPosition.centerFromRadius(pos, radius, vv.trk());
			return new Pair<NavPoint, TcpData>(new NavPoint(pos, time, lab), TcpData.makeFull(wt, trkty, gsty, vsty, radius, turnCenter, gsacc, vsacc, vv, sp, st, -1));
		} else {
			return new Pair<NavPoint, TcpData>(new NavPoint(pos, time), new TcpData());
		}
	}

	public static Pair<NavPoint,TcpData> parseXYZ(String s) {
		String[] fields = s.split(Constants.wsPatternParens);
		//Pair<PointMutability,String> extra = parseExtra(fields);
		Vect3 v = Vect3.parse(fields[0]+" "+fields[1]+" "+fields[2]);
		double time = Util.parse_double(fields[3]);
		Position pos = Position.makeXYZ(v.x, v.y, v.z);
		if (fields.length == MIN_OUTPUT_COLUMNS+1) {
			String extra = fields[MIN_OUTPUT_COLUMNS];
			NavPoint np = new NavPoint(pos, time, extra);
			Pair<TcpData,String> p = (new TcpData()).parseMetaDataLabel(np,extra);
			if (! p.second.equals("")) {
				np.makeLabel(p.second);
			}
			return new Pair<NavPoint,TcpData>(np,p.first);			
			//return (new NavPoint(pos, time, extra)).parseMetaDataLabel(extra);
		} else if (fields.length == TCP_OUTPUT_COLUMNS+1) {
			String wt = fields[4];
			Velocity vv = Velocity.parse(fields[5]+" "+fields[6]+" "+fields[7]);
			String trkty = fields[8];
			//double accel_trk = Units.from("deg/s", Util.parse_double(fields[9]));
			String gsty = fields[10];
			double gsacc = Units.from("m/s^2", Util.parse_double(fields[11]));
			String vsty = fields[12];
			double vsacc = Units.from("m/s^2", Util.parse_double(fields[13]));
			double radius = Util.parse_double(fields[14]);
			Vect3 sv = Vect3.parse(fields[15]+" "+fields[16]+" "+fields[17]);
			Position sp = Position.makeXYZ(sv.x, sv.y, sv.z);
			double st = Util.parse_double(fields[18]);
			String lab = fields[19];
			//return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
			Position turnCenter = KinematicsPosition.centerFromRadius(pos, radius, vv.trk());
			return new Pair<NavPoint, TcpData>(new NavPoint(pos, time, lab), TcpData.makeFull(wt, trkty, gsty, vsty, radius, turnCenter, gsacc, vsacc, vv, sp, st, -1));
		} else {
			return new Pair<NavPoint,TcpData>(new NavPoint(pos, time), new TcpData());
		}
	}

	/** Return a string including both the point name (if set) and any 
	 * tcp metadata represented as a string.  If neither exists, return the empty string. 
	 * 
	 * @param p associated NavPoint
	 * @param d TcpData object
	 * @return the full label
	 * */
	public static String fullLabel(NavPoint p, TcpData d) {
		return p.label()+d.metaDataLabel(p.time(),4);
	}


	public String toString(boolean showSource) {
		String rtn =  "TcpData [ty=" + ty + ", <" + tcpTrk + " " + tcpGs + " " + tcpVs + ">";
	           if (radiusSigned != 0.0) rtn = rtn + " radiusSigned="+ radiusSigned;
	           if (!centerTurn.isInvalid()) rtn = rtn + " turnCenter="+ centerTurn;
	           rtn = rtn + ", accelGs=" + accelGs + ", accelVs=" + accelVs + 
				", linearIndex=" + linearIndex 	+ ", information=" + information ;
		if (showSource) rtn = rtn + ", sourcePosition=" + sourcePosition; //", sourceTime=" + sourceTime + 		               
		rtn = rtn + "]";
		return rtn;
	}

	@Override
	public String toString() {
		return toString(false);
	}


}

