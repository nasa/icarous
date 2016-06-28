/* 
 * Complete Bands for Conflict Prevention (single time version)
 *
 * Contact: Jeff Maddalon, Rick Butler, George Hagen, and Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;

/**
 * Objects of class "Bands" compute the conflict prevention 
 * bands using linear state-based preditions of ownship and (multiple)
 * traffic aircraft positions.  The bands consist of ranges of
 * guidance maneuvers: track angles, ground speeds, and vertical
 * speeds. <p>
 *
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver.  If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will be a loss of separation within the lookahead time.<p>
 *
 * If bands are set to conflict bands only, e.g, setConflictBands,
 * only NEAR bands are computed. In this case, guidance maneuvers for avoiding 
 * conflicts, which are represented by NONE bands, are not computed. Furthermore,
 * bands can be set to certain types of maneuvers by using the methods:
 * setTrackBands, setGroundSpeedBands, and setVerticalSpeedBands. 
 * These methods have to be used before adding any traffic information.
 *
 * Note that in the case of geodetic coordinates, bands performs an 
 * internal projection of the coordinates and velocities into the
 * Euclidean frame (see Util/Projection).  An error will
 * be logged if the distance between traffic and ownship exceeds 
 * Util.Projection.projectionMaxRange().<p>
 * 
 * Disclaimers: Only parts of these algorithms have been formally
 * verified.  We plan to advance the formal verifcation, but it is
 * currently not complete. Even with a 'complete' verification, there
 * will still be certain assumptions.  For instance, the formal proofs
 * use real numbers while these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodesic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 * The basic usages is
 * <pre>
 * Bands b(..with configuration parameters..);
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of (one) traffic aircraft, velocity of traffic);
 * b.addTraffic(position of (another) traffic aircraft, velocity of traffic);
 * ... add other traffic aircraft ...
 *
 * for (int i = 0; i < b.trackLength(); i++ ) {  
 *    intrval = b.track(i);
 *    lower_ang = intrval.low;
 *    upper_ang = intrval.up;
 *    regionType = b.trackRegion(i);
 *    ..do something with this information..
 * } 
 *
 * ...similar for ground speed and vertical speed...
 * </pre>
 * <p>
 * The variable b may be reused for a different set of aircraft, but it has to be cleared first, i.e.,
 * <pre>
 * b.clear();
 * b.setOwnship(...);
 * b.addTraffic(...);
 * ... etc ...
 * </pre>
 *
 * When any "parameter" to this class is set (separation distance,
 * lookahead time, positions in latitude/longitude, etc.), all
 * previous bands information is cleared; therefore, all traffic
 * aircraft must be added <em>after</em> the parameters are
 * configured.  For more complete example usage see the file
 * <tt>Batch.java</tt>
 *
 */
public class Bands implements GenericDHStateBands {                     

  private BandsCore red; 

  protected ErrorLog error = new ErrorLog("Bands");

  protected TrafficState ownship;
  protected int traffic;

  protected ArrayList<Interval> trackArray;
  protected ArrayList<BandsRegion> trackRegionArray;
  protected ArrayList<Interval> groundArray;
  protected ArrayList<BandsRegion> groundRegionArray;
  protected ArrayList<Interval> verticalArray;
  protected ArrayList<BandsRegion> verticalRegionArray;

  // [CAM] The following variables handle how stateless bands are recomputed
  protected boolean conflictBands; // Only compute conflict bands 
  protected boolean doTrk; // Do compute track bands
  protected boolean doGs;  // Do compute gs bands
  protected boolean doVs;  // Do compute vs bands
  protected boolean computeTrk; // Need compute track bands
  protected boolean computeGs;  // Need compute gs bands
  protected boolean computeVs;  // Need compute vs bands;

  /** 
   * Construct a Bands object with default values for
   * configuration parameters.  These default values include: 5 nmi
   * horizontal separation, 1000 ft vertical separation, 3 minutes
   * for the lookahead time, 1000 knots max ground
   * speed, 5000 fpm max vertical speed.  The bands always begin as
   * 'no conflict' bands: track bands from 0 to 2pi, ground
   * speed bands from 0 to max_gs, and vertical speed bands
   * from -max_vs to max_vs. Both the positions and velocities of
   * the aircraft are assumed to be in Euclidean coordinates (see
   * setLatLon() and setTrackVelocity() to change this behavior).
   */
  public Bands() {
    init(5,"nmi", 1000, "ft", 180 ,"s", 700, "knot", 5000, "fpm"); 
  }

  /** 
   * Construct a Bands object with the given configuration
   * parameters.  The bands always begin as 'no conflict' bands: 
   * track bands from 0 to 2pi, ground speed bands from 0 to
   * max_gs, and vertical speed bands from -max_vs to max_vs.
   * Both the positions and velocities of the aircraft are assumed
   * to be in Euclidean coordinates (see setLatLon() and
   * setTrackVelocity() to change this behavior).
   *
   * @param D      the minimum horizontal separation distance [nmi]
   * @param H      the minimum vertical separation distance [feet]
   * @param T      the near-term lookahead time [s]
   * @param max_gs the maximum ground speed that is output by Bands,
   * the minimum is 0. [knots]
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  public Bands(double D, String dunit, double H, String hunit, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit) {
    init(D, dunit, H, hunit, T, tunit, max_gs, gsunit, max_vs, vsunit);
  }

  @SuppressWarnings("unchecked")
  public Bands(Bands b) {
    red = new BandsCore(b.red);
    doTrk = b.doTrk;
    doGs = b.doGs;
    doVs = b.doVs;
    computeTrk = b.computeTrk;
    computeGs = b.computeGs;
    computeVs = b.computeVs;
    trackArray = (ArrayList<Interval>)b.trackArray.clone();
    groundArray = (ArrayList<Interval>)b.groundArray.clone();
    verticalArray = (ArrayList<Interval>)b.verticalArray.clone();
    trackRegionArray = (ArrayList<BandsRegion>)b.trackRegionArray.clone();
    groundRegionArray = (ArrayList<BandsRegion>)b.groundRegionArray.clone();
    verticalRegionArray = (ArrayList<BandsRegion>)b.verticalRegionArray.clone();
    conflictBands = b.conflictBands;
    ownship = b.ownship;
    traffic = b.traffic;
    error = new ErrorLog("Bands");
  }

  protected void init(double D, String dunit, double H, String hunit, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit) {
    red = new BandsCore(Units.from(dunit,D),Units.from(hunit,H),Units.from(tunit,T),Units.from(gsunit,max_gs),Units.from(vsunit,max_vs));
    doGs = true;
    doTrk = true;
    doVs = true;
    computeTrk = true;
    computeGs = true;
    computeVs = true; 
    trackArray = new ArrayList<Interval>();
    groundArray = new ArrayList<Interval>();
    verticalArray = new ArrayList<Interval>();
    trackRegionArray = new ArrayList<BandsRegion>();
    groundRegionArray = new ArrayList<BandsRegion>();
    verticalRegionArray = new ArrayList<BandsRegion>();
    conflictBands = false;
    ownship = TrafficState.INVALID;
    traffic = 0;
    error = new ErrorLog("Bands");
  }

  protected void needComputeBands() {
    computeTrk = doTrk;
    computeGs = doGs;
    computeVs = doVs;
  }

  /* Enable or disable the computation of conflict bands only. Should be called before adding traffic */
  public void setConflictBands(boolean cb) {
    conflictBands = cb; 
    resetRegions();
  }

  public void doConflictBands() {
    setConflictBands(true);
  }

  public boolean areConflictBands() {
    return conflictBands;
  }

  /* Enable or disable the computation of track bands. Should be called before adding traffic */
  public void setTrackBands(boolean dotrk) {
    doTrk = dotrk;
    computeTrk = dotrk;
    red.clearTrackRegions();
  }

  public void enableTrackBands() {
    setTrackBands(true);
  }

  /* Enable or disable the computation of ground speed bands. Should be called before adding traffic */
  public void setGroundSpeedBands(boolean dogs) {
    doGs = dogs;
    computeGs = dogs;    
    red.clearGroundSpeedRegions();
  }

  public void enableGroundSpeedBands() {
    setGroundSpeedBands(true);
  }

  /* Enable or disable the computation of vertical speed bands. Should be called before adding traffic */
  public void setVerticalSpeedBands(boolean dovs) {
    doVs = dovs;
    computeVs = dovs;
    red.clearVerticalSpeedRegions();
  }

  public void enableVerticalSpeedBands() {
    setVerticalSpeedBands(true);
  }

  /* Enable the computation of track bands only. Should be called before adding traffic */
  public void onlyTrackBands() {
    setTrackBands(true);
    setGroundSpeedBands(false);
    setVerticalSpeedBands(false);
  }

  /* Enable the computation of ground speed bands only. Should be called before adding traffic */
  public void onlyGroundSpeedBands() {
    setTrackBands(false);
    setGroundSpeedBands(true);
    setVerticalSpeedBands(false);
  }

  /* Enable the computation of vertical speed bands only. Should be called before adding traffic */
  public void onlyVerticalSpeedBands() {
    setTrackBands(false);
    setGroundSpeedBands(false);
    setVerticalSpeedBands(true);
  }

  public void allBands() {
    setTrackBands(true);
    setGroundSpeedBands(true);
    setVerticalSpeedBands(true);
  }

  /** 
   * Set the lookahead time [s].  When this parameter is
   * set any existing band information is cleared. 
   */
  public void setLookaheadTime(double t, String unit) {
    red.setTime(Units.from(unit,t));
    resetRegions();
  }

  public void setTime(double t) {
    red.setTime(t);
    resetRegions();
  }
  
  /** Returns the time of the lookahead time in specified units */
  public double getLookahedTime(String unit) {
    return Units.to(unit,red.getTime());
  }
  
  /** Returns the time of the lookahead time in seconds */
  public double getTime() {
    return red.getTime();
  }
  
  public void setTimeRange(double b, double t) {
    red.setTimeRange(b,t);
    resetRegions();   
  }

  public void setTimeRange(double b, double t, String unit) {
    red.setTimeRange(Units.from(unit,b), Units.from(unit,t));
    resetRegions();	  
  }

  public double getStartTime() {
    return red.getStartTime();
  } 

  public double getStartTime(String unit) {
    return Units.to(unit, red.getStartTime());
  }

  //
  // Interface Methods...
  //

  public boolean isLatLon() {
    if (ownship.isValid()) return ownship.isLatLon();
    return false;  // need some default
  }

  public void setDistance(double d, String unit) {
    double D = Math.abs(Units.from(unit,d));
    red.setDiameter(D);
    resetRegions();
  }

  public double getDistance(String unit) {
    return Units.to(unit,red.getDiameter());
  }

  public void setHeight(double h, String unit) {
    double H = Math.abs(Units.from(unit,h));
    red.setHeight(H);
    resetRegions();
  }

  public double getHeight(String unit) {
    return Units.to(unit,red.getHeight());
  }

  public void setMaxGroundSpeed(double gs, String unit) {
    double max_gs = Math.abs(Units.from(unit,gs));
    red.setMaxGroundSpeed(max_gs);
    resetRegions();
  }

  public double getMaxGroundSpeed() {
    return red.getMaxGroundSpeed();
  }

  public double getMaxGroundSpeed(String unit) {
    return Units.to(unit, red.getMaxGroundSpeed());
  }

  public void setMaxVerticalSpeed(double vs, String unit) {
    double max_vs = Math.abs(Units.from(unit,vs));
    red.setMaxVerticalSpeed(max_vs);
    resetRegions();
  }

  public double getMaxVerticalSpeed() {
    return red.getMaxVerticalSpeed();
  }

  public double getMaxVerticalSpeed(String unit) {
    return Units.to(unit, red.getMaxVerticalSpeed());
  }

  public void setTrackTolerance(double trk, String unit) {
    if (trk >= 0) {
      red.setTrackTolerance(Units.from(unit, trk));
      resetRegions();
    }
  }
  public double getTrackTolerance(String unit) {
    return Units.to(unit, red.getTrackTolerance());
  }

  public void setGroundSpeedTolerance(double gs, String unit) {
    if (gs >= 0) {
      red.setGroundSpeedTolerance(Units.from(unit, gs));
      resetRegions();
    }
  }
  public double getGroundSpeedTolerance(String unit) {
    return Units.to(unit, red.getGroundSpeedTolerance());
  }

  public void setVerticalSpeedTolerance(double vs, String unit) {
    if (vs >= 0) {
      red.setVerticalSpeedTolerance(Units.from(unit, vs));
      resetRegions();
    }
  }

  public double getVerticalSpeedTolerance(String unit) {
    return Units.to(unit, red.getVerticalSpeedTolerance());
  }

  public void setOwnship(TrafficState o) {
    needComputeBands();
    ownship = o;
  }

  public void setOwnship(String id, Position p, Velocity v) {
    setOwnship(TrafficState.makeOwnship(id,p,v));
  }

  public void setOwnship(Position p, Velocity v) {
    setOwnship("Ownship",p,v);
  }

  public void addTraffic(TrafficState ac) {
    if (!ac.isValid()) {
      error.addError("addTraffic: invalid aircraft.");
      return;
    }
    if (!ownship.isValid()) {
      error.addError("addTraffic: setOwnship must be called first.");
      return;
    }
    if (ac.isLatLon() != isLatLon()) {
      error.addError("addTraffic: inconsistent use of lat/lon and Euclidean data.");
      return;
    }
    traffic++;
    Vect3 si0 = ac.get_s();
    Vect3 s0 = ownship.get_s().Sub(si0);
    Velocity vi0 = ac.get_v();
    red.addTraffic(s0,ownship.get_v(),vi0,doTrk,doGs,doVs); 
    needComputeBands();
  }
  
  public void addTraffic(String id, Position pi, Velocity vi) {
    if (!ownship.isValid()) {
      error.addError("addTraffic: setOwnship must be called first.");
      return;
    }
    addTraffic(ownship.makeIntruder(id,pi,vi));
  }
  
  public void addTraffic(Position pi, Velocity vi) {
    addTraffic("AC_"+(traffic+1),pi,vi);
  }

  public void clear() {
    resetRegions();
    ownship = TrafficState.INVALID;
  }

  protected void resetRegions() {
    red.clear(doTrk,doGs,doVs);
    needComputeBands();
  }

  // Return false if track bands cannot turned on
  public boolean turnTrackBandsOn() {
    if (!ownship.isValid()) {
      return false;
    }
    if (!doTrk) {
      enableTrackBands();
    }
    if (computeTrk) {
      trackCompute();
      computeTrk = false;
      red.clearTrackBreaks();
    }
    return true;
  }

  public boolean turnGroundSpeedBandsOn() {
    if (!ownship.isValid()) {
      return false;
    }
    if (!doGs) {
      enableGroundSpeedBands();
    }
    if (computeGs) {
      groundCompute();
      computeGs = false;
      red.clearGroundSpeedBreaks();
    }
    return true;
  }

  public boolean turnVerticalSpeedBandsOn() {
    if (!ownship.isValid()) {
      return false;
    }
    if (!doVs) {
      enableVerticalSpeedBands();
    } if (computeVs) {
      verticalCompute();
      computeVs = false;
      red.clearVerticalSpeedBreaks();
    }
    return true;
  }

  public int trackLength() {
    if (!turnTrackBandsOn()) {
      return -1;
    }
    return trackArray.size();
  }

  public Interval track(int i, String unit) {
    return track(i, Units.getFactor(unit));
  }

  private Interval track(int i, double unit) {
    if (!turnTrackBandsOn() || i >= trackArray.size()) {
      return Interval.EMPTY;
    }
    double loval = trackArray.get(i).low;
    double upval = trackArray.get(i).up;
    if (isLatLon()) {
      Velocity lo1 = Velocity.mkTrkGsVs(trackArray.get(i).low, ownship.get_v().gs(), ownship.get_v().vs());
      Velocity hi1 = Velocity.mkTrkGsVs(trackArray.get(i).up, ownship.get_v().gs(), ownship.get_v().vs());
      Velocity lo2 = ownship.inverseVelocity(lo1);
      Velocity hi2 = ownship.inverseVelocity(hi1);
      // deal with special cases around 0, 2pi
      loval = lo2.compassAngle();
      upval = hi2.compassAngle();
      if (trackArray.get(i).low == 0.0 || trackArray.get(i).low == 2*Math.PI) {
        loval = trackArray.get(i).low;
      }
      if (trackArray.get(i).up == 0.0 || trackArray.get(i).up == 2*Math.PI) {
        upval = trackArray.get(i).up;
      }
      // There is the potential for a problem if the unprojected bands are right near the 0/2pi boundary and the 
      // projection causes one (but not both) bounds to cross -- in this special case we have to return return a band that 
      // is outside of the 0..2pi range (this will be converted to a -pi..pi range instead).
      if (trackArray.get(i).low < trackArray.get(i).up && upval < loval) {
        loval = Util.to_pi(loval);
        upval = Util.to_pi(upval);
      }
    }
    return new Interval(Units.to(unit, loval), Units.to(unit, upval));
  }

  public BandsRegion trackRegion(int i) {
    if (!turnTrackBandsOn() || i >= trackArray.size()) {
      return BandsRegion.UNKNOWN;
    }
    return trackRegionArray.get(i);
  }

  public BandsRegion regionOfTrack(double trk, String unit) {
    trk = Util.to_2pi(Units.from(unit, trk));
    if (!turnTrackBandsOn()) {
      return BandsRegion.UNKNOWN;
    }    
    for (int i = 0; i < trackArray.size(); i++) {
      if (trackArray.get(i).inCC(trk)) {
        return trackRegionArray.get(i); 
      }
    }
    if (conflictBands) {
      return BandsRegion.NONE;
    } else {
      return BandsRegion.UNKNOWN;
    }
  }

  public int groundSpeedLength() {
    if (!turnGroundSpeedBandsOn()) {
      return -1;
    }
    return groundArray.size();
  }

  public Interval groundSpeed(int i, String unit) {
    return groundSpeed(i, Units.getFactor(unit));
  }

  private Interval groundSpeed(int i, double unit) {
    if (!turnGroundSpeedBandsOn() || i >= groundArray.size()) {
      return Interval.EMPTY;
    }
    if (isLatLon()) {
      Velocity lo1 = Velocity.mkTrkGsVs(ownship.get_v().trk(), groundArray.get(i).low, ownship.get_v().vs());
      Velocity hi1 = Velocity.mkTrkGsVs(ownship.get_v().trk(), groundArray.get(i).up, ownship.get_v().vs());
      Velocity lo2 = ownship.inverseVelocity(lo1);
      Velocity hi2 = ownship.inverseVelocity(hi1);
      return new Interval(Units.to(unit, lo2.gs()), Units.to(unit, hi2.gs()));
    } else {
      return new Interval(Units.to(unit, groundArray.get(i).low), Units.to(unit, groundArray.get(i).up));
    }
  }

  public BandsRegion groundSpeedRegion(int i) {
    if (!turnGroundSpeedBandsOn() || i >= groundArray.size()) {
      return BandsRegion.UNKNOWN;
    }
    return groundRegionArray.get(i);
  }

  public BandsRegion regionOfGroundSpeed(double gs, String unit) {
    gs = Units.from(unit, gs);
    if (gs < 0.0 || gs > red.getMaxGroundSpeed() || !turnGroundSpeedBandsOn()) {
      return BandsRegion.UNKNOWN;
    }
    for (int i = 0; i < groundArray.size(); i++) {
      if (groundArray.get(i).inCC(gs)) {
        return groundRegionArray.get(i); 
      }
    }
    if (conflictBands) {
      return BandsRegion.NONE;
    } else {
      return BandsRegion.UNKNOWN;
    }
  }

  public int verticalSpeedLength() {
    if (!turnVerticalSpeedBandsOn()) {
      return -1;
    }
    return verticalArray.size();
  }

  public Interval verticalSpeed(int i, String unit) {
    return verticalSpeed(i, Units.getFactor(unit));
  }

  private Interval verticalSpeed(int i, double unit) {
    if (!turnVerticalSpeedBandsOn() || i >= verticalArray.size()) {
      return Interval.EMPTY;
    }
    return new Interval(Units.to(unit, verticalArray.get(i).low), Units.to(unit, verticalArray.get(i).up));
  }

  public BandsRegion verticalSpeedRegion(int i) {
    if (!turnVerticalSpeedBandsOn() || i >= verticalArray.size()) {
      return BandsRegion.UNKNOWN;
    }
    return verticalRegionArray.get(i);
  }

  public BandsRegion regionOfVerticalSpeed(double vs, String unit) {
    vs = Units.from(unit, vs);
    if (vs < -red.getMaxVerticalSpeed() || vs > red.getMaxVerticalSpeed() || !turnVerticalSpeedBandsOn()) {
      return BandsRegion.UNKNOWN;
    }
    for (int i = 0; i < verticalArray.size(); i++) {
      if (verticalArray.get(i).inCC(vs)) {
        return verticalRegionArray.get(i); 
      }
    }
    if (conflictBands) {
      return BandsRegion.NONE;
    } else {
      return BandsRegion.UNKNOWN;
    }
  }

  //
  // Internal methods...
  //

  /** 
   * Provide a copy of the track angle bands.  The angles are in
   * 'compass' angles: 0 to 360 degrees, counter-clockwise from true
   * north. 
   */
  protected void trackCompute() {
    IntervalSet trk_red = red.trackBands(); 
    if (conflictBands) {
      toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,trk_red);
    } else {
      toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,trk_red,0.0,2*Math.PI);
    }
  }

  /** Provide a copy of the ground speed bands [knots].  The range of
	ground speed bands is from 0 to max_gs. */
  protected void groundCompute() {
    IntervalSet gs_red = red.groundSpeedBands(); 
    if (conflictBands) {
      toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,gs_red);
    } else {
      toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,gs_red,0.0,red.getMaxGroundSpeed());
    }
  }

  /** Provide a copy of the vertical speed bands [feet/min].  The
	range of vertical speeds is -max_vs to max_vs. */
  protected void verticalCompute() {
    IntervalSet vs_red = red.verticalSpeedBands(); 
    if (conflictBands) {
      toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,vs_red);
    } else {
      toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,vs_red,
          -red.getMaxVerticalSpeed(),red.getMaxVerticalSpeed());
    }
  }

  /* if n is empty, then this method returns an undefined value */
  private static int order(ArrayList<Interval> arr, Interval n) {
    if (arr.size() == 0) {
      return 0; // add to empty ArrayList
    }
    for (int i = 0; i < arr.size(); i++) {
      if (n.low < arr.get(i).low || n.low == arr.get(i).low && n.up < arr.get(i).up) {
        return i;
      }
    }
    return arr.size();
  }

  /** Return true if track bands consist of a solid region of the given color  */
  public boolean solidTrackBand(BandsRegion br) {
    if (trackLength() < 0) {
      return false;
    } else if (conflictBands && br == BandsRegion.NONE) {
      return trackLength() == 0;
    } else if (trackLength() > 0){
      return false;
    } else {
      return 
          trackRegion(0) == br &&
          Util.almost_equals(track(0,1).low,0,Util.PRECISION5) &&
          Util.almost_equals(track(0,1).up,2*Math.PI,Util.PRECISION5);
    }
  }

  /** Return true if ground speed bands consist of a region band of the given color  */
  public boolean solidGroundSpeedBand(BandsRegion br) {
    if (groundSpeedLength() < 0) {
      return false;
    } else if (conflictBands && br == BandsRegion.NONE) {
      return groundSpeedLength() == 0;
    } else if (groundSpeedLength() > 0){
      return false;
    } else {
      return 
          groundSpeedRegion(0) == br &&
          Util.almost_equals(groundSpeed(0,1).low,0,Util.PRECISION5) &&
          Util.almost_equals(groundSpeed(0,1).up,red.getMaxGroundSpeed(),Util.PRECISION5);
    }
  }

  /** Return true if vertical speed bands consist of a region band of the given color  */
  public boolean solidVerticalSpeedBand(BandsRegion br) {
    if (verticalSpeedLength() < 0) {
      return false;
    } else if (conflictBands && br == BandsRegion.NONE) {
      return verticalSpeedLength() == 0;
    } else if (verticalSpeedLength() > 0){
      return false;
    } else {
      return 
          verticalSpeedRegion(0) == br &&
          Util.almost_equals(verticalSpeed(0,1).low,-red.getMaxVerticalSpeed(),Util.PRECISION5) &&
          Util.almost_equals(verticalSpeed(0,1).up,red.getMaxVerticalSpeed(),Util.PRECISION5);
    }
  }

  /* 
   * Find first band that is equal/not equal to a given bands region and 
   * whose size is greater than or equal to a given tolerance. It returns -1 
   * if no such band exists. 
   */
  private static int find_first_band(boolean eq, BandsRegion br, double tolerance,
      ArrayList<Interval> arraylist, ArrayList<BandsRegion> regions) {
    for (int i = 0; i < arraylist.size(); ++i) {
      if ((eq ? regions.get(i) == br : regions.get(i) != br) &&
          arraylist.get(i).up - arraylist.get(i).low >= tolerance) {
        return i;
      }
    }
    return -1;
  }

  /* 
   * Find first band that is equal/not equal to an implicit NONE and whose size is 
   * greater than or equal to a given tolerance. Returned index i is such that 
   * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition. 
   * It returns -1 if no such band exists.  
   * CAVEAT: This function returns 0, if arraylist is empty. In this case the whole
   * band is NONE.
   * 
   */
  private static int find_first_implicit_none(double tolerance,
      ArrayList<Interval> arraylist, ArrayList<BandsRegion> regions,      
      double lb, double ub) {
    if (arraylist.size() == 0) {
      return 0;
    }
    for (int i = 0; i <= arraylist.size(); ++i) {
      if (i==0 ? arraylist.get(i).low - lb > tolerance :
        i == arraylist.size() ? ub - arraylist.get(i-i).up > tolerance :
          arraylist.get(i).low -arraylist.get(i-1).up > tolerance) {
        return i;
      }
    }
    return -1;
  }

  /* 
   * Find first band that is equal/not equal to a given bands region and whose size is
   * greater than or equal to a given tolerance. It returns -1 if no such band exists. 
   * If finding a band equal to NONE and the bands is a conflict bands, i.e., it has been
   * set through setConflictBands(), the returned value corresponds to index i such that 
   * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition. 
   * The parameters lb and up are upper and lower bound for the whole band.
   * CAVEAT: This function returns 0, if arraylist is empty. In this case the whole
   * band is NONE.
   */
  public int find_first_band(boolean eq, BandsRegion br, double tolerance,
      ArrayList<Interval> arraylist, ArrayList<BandsRegion> regions, 
      double lb, double ub) {
    if (conflictBands && br == BandsRegion.NONE && eq) {
      return find_first_implicit_none(tolerance,arraylist,regions,lb,ub);
    } else {
      return find_first_band(eq,br,tolerance,arraylist,regions);
    }     
  }

  /* 
   * Find first track band that is equal/not equal to a given bands region and whose size is
   * greater than or equal to a given tolerance [rad]. It returns -1 if no such band exists. 
   * If finding a band equal to NONE and the bands is a conflict bands, i.e., it has been
   * set through setConflictBands(), the returned value corresponds to index i such that 
   * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition. 
   * CAVEAT: This function returns 0 when when the band is empty. In this case the whole
   * band is NONE.
   */
  protected int firstTrackBand(boolean eq, BandsRegion br, double trk) {
    if (!turnTrackBandsOn()) {
      return -1;
    }
    return find_first_band(eq,br,trk,trackArray,trackRegionArray,0,2*Math.PI);
  }

  /* 
   * Find first ground speed band that is equal/not equal to a given bands region and 
   * whose size is greater than or equal to a given tolerance [m/s]. It returns -1 if 
   * no such band exists. If the bands region is NONE and the bands is a conflict bands, 
   * i.e., it has been set through setConflictBands(), the returned value corresponds to 
   * index i such that (i-1).up and i.low is an implicit NONE that satisfies the tolerance 
   * condition. 
   * CAVEAT: This function returns 0 when the band is empty. In this case the whole
   * band is NONE.
   */
  protected int firstGroundSpeedBand(boolean eq, BandsRegion br, double gs) {
    if (!turnGroundSpeedBandsOn()) {
      return -1;
    }
    return find_first_band(eq,br,gs,groundArray,groundRegionArray,0,red.getMaxGroundSpeed());
  }

  /* 
   * Find first vertical speed band that is equal/not equal to a given bands region and 
   * whose size is greater than or equal to a given tolerance [m/s]. It returns -1 if 
   * no such band exists. If the bands region is NONE and the bands is a conflict bands, 
   * i.e., it has been set through setConflictBands(), the returned value corresponds to 
   * index i such that (i-1).up and i.low is an implicit NONE that satisfies the tolerance 
   * condition. 
   * CAVEAT: This function returns 0 when the band is empty. In this case the whole
   * band is NONE.
   */
  protected int firstVerticalSpeedBand(boolean eq, BandsRegion br, double vs) {
    if (!turnVerticalSpeedBandsOn()) {
      return -1;
    }
    return find_first_band(eq,br,vs,verticalArray,verticalRegionArray,
        -red.getMaxVerticalSpeed(),red.getMaxVerticalSpeed());
  }

  protected static void toIntervalSet_fromIntervalArray(IntervalSet intervalset, 
      ArrayList<Interval> intervalarray, ArrayList<BandsRegion> regions, BandsRegion br) {
    for (int i=0; i < intervalarray.size(); ++i) {
      if (regions.get(i) == br)
        intervalset.union(intervalarray.get(i));
    }
  } 

  protected static void toArrays(ArrayList<Interval> intervalarray, ArrayList<BandsRegion> regions, 
      IntervalSet red, IntervalSet green) {
    int i;

    intervalarray.clear();
    regions.clear();

    for (Interval n : green) {
      i = order(intervalarray, n);
      intervalarray.add(i, n);
      regions.add(i, BandsRegion.NONE);

    }
    for (Interval n : red) {
      i = order(intervalarray, n);
      intervalarray.add(i, n);
      regions.add(i,BandsRegion.NEAR);
    }
  }

  protected static void toIntervalArray_fromIntervalSet(ArrayList<Interval> intervalarray, 
      ArrayList<BandsRegion> regions, IntervalSet intervalset) {
    intervalarray.clear();
    regions.clear();
    for (Interval n : intervalset) {
      intervalarray.add(n);
      regions.add(BandsRegion.NEAR);
    }
  }

  protected static void toIntervalArray_fromIntervalSet(ArrayList<Interval> intervalarray, 
      ArrayList<BandsRegion> regions, IntervalSet intervalset, double lowBound, double upBound) {
    IntervalSet green_bands = new IntervalSet();

    green_bands.union(new Interval(lowBound, upBound));
    green_bands.diff(intervalset);

    green_bands.sweepSingle(); //only eliminate actual singles

    toArrays(intervalarray, regions, intervalset, green_bands);
  }

  public void mergeTrackBands(Bands bands) {
    if (!turnTrackBandsOn() || !bands.turnTrackBandsOn() ||
        bands.solidTrackBand(BandsRegion.NONE)) {
      return;
    }
    IntervalSet red_bands = new IntervalSet();
    toIntervalSet_fromIntervalArray(red_bands,trackArray,trackRegionArray,BandsRegion.NEAR);
    toIntervalSet_fromIntervalArray(red_bands,bands.trackArray,bands.trackRegionArray,BandsRegion.NEAR);
    if (conflictBands) {
      toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,red_bands);
    } else {
      toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,red_bands,0.0,2*Math.PI);
    }
    computeTrk = false;
  }

  public void mergeGroundSpeedBands(Bands bands) {
    if (!turnGroundSpeedBandsOn() || !bands.turnGroundSpeedBandsOn() ||
        bands.solidGroundSpeedBand(BandsRegion.NONE)) {
      return;
    }
    IntervalSet red_bands = new IntervalSet();
    toIntervalSet_fromIntervalArray(red_bands,groundArray,groundRegionArray,BandsRegion.NEAR);
    toIntervalSet_fromIntervalArray(red_bands,bands.groundArray,bands.groundRegionArray,BandsRegion.NEAR);
    if (conflictBands) {
      toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,red_bands);
    } else {
      toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,red_bands,0.0,red.getMaxGroundSpeed());
    }
    computeGs = false;
  }

  public void mergeVerticalSpeedBands(Bands bands) {
    if (!turnVerticalSpeedBandsOn() || !bands.turnVerticalSpeedBandsOn() || 
        bands.solidVerticalSpeedBand(BandsRegion.NONE)) {
      return;
    }
    IntervalSet red_bands = new IntervalSet();
    toIntervalSet_fromIntervalArray(red_bands,verticalArray,verticalRegionArray,BandsRegion.NEAR);
    toIntervalSet_fromIntervalArray(red_bands,bands.verticalArray,bands.verticalRegionArray,BandsRegion.NEAR);
    if (conflictBands) {
      toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,red_bands);
    } else {
      toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,red_bands,-red.getMaxVerticalSpeed(),red.getMaxVerticalSpeed());
    }
    computeVs = false;
  }

  public String toString() {
    //    String s;
    //    s = getClass().getSimpleName()+"\n";
    //    s+="D: "+getDistance()+" [nm], H: "+getHeight()+" [ft], T: "+getTime()+" [s]\n";
    //    s+="maxgs: "+getMaxGroundSpeed()+" [knot] maxvs: "+getMaxVerticalSpeed()+" [fpm]\n";
    //    s+="conlictBands: "+conflictBands+"\n";
    //    s+="doTrk: "+doTrk+", doGs: "+doGs+", doVs: "+doVs+"\n";
    //    s+="computeTrk: "+computeTrk+", computeGs: "+computeGs+", computeVs: "+computeVs+"\n";
    //    s+="ownship: "+ownship+", traffic: "+traffic+"\n";
    //    String units = isLatLon() ? "[deg,deg,ft]" : "[nm,nm,ft]";
    //    if (ownship) {
    //      s+="ownship so: ("+po.toString4NP()+") "+units+", vo: ("+vo.toString4NP()+") [deg,knot,fpm]\n";
    //    }
    //    s+="Track Bands [rad]:\n";
    //    for (int i = 0; i < trackArray.size(); i++) {
    //      s+=trackArray.get(i)+" "+trackRegionArray.get(i)+"\n";
    //    }
    //    s+="Ground Speed Bands [m/s]:\n";
    //    for (int i = 0; i < groundArray.size(); i++) {
    //      s+=groundArray.get(i)+" "+groundRegionArray.get(i)+"\n";
    //    }
    //    s+="Vertical Speed Bands [m/s]:\n";
    //    for (int i = 0; i < verticalArray.size(); i++) {
    //      s+=verticalArray.get(i)+" "+verticalRegionArray.get(i)+"\n";
    //    }
    //    s+="red: "+red.toString(); 
    //    return s;
    return red.toString();
  }

  public String strBands(String msg) {
    String rtn = msg;
    f.pln(" !!!!!!!!!!!!!! strBands: "+trackLength()+"  "+groundSpeedLength()+"  "+verticalSpeedLength());
    rtn = rtn +"Type: "+getClass().getSimpleName();
    rtn = rtn +("\nTrack Bands [deg,deg]:"); 
    for (int i=0; i < trackLength(); ++i) {
      rtn = rtn +("  "+track(i,"deg")+" "+trackRegion(i));
    } 
    rtn = rtn +("\nGround Speed Bands [knot,knot]:");
    for (int i=0; i < groundSpeedLength(); ++i) {
      rtn = rtn +("  "+groundSpeed(i,"kn")+" "+groundSpeedRegion(i));
    } 
    rtn = rtn +("\nVertical Speed Band [fpm,fpm]:");
    for (int i=0; i < verticalSpeedLength(); ++i) {
      rtn = rtn +("  "+verticalSpeed(i,"fpm")+" "+ verticalSpeedRegion(i));
    } 
    return rtn;
  }

  public void mergeBands(Bands bands) {
    mergeTrackBands(bands);
    mergeGroundSpeedBands(bands);
    mergeVerticalSpeedBands(bands);
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
