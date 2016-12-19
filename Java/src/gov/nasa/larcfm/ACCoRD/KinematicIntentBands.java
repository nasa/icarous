/* 
 * Complete Kinematic Intent Bands for Conflict Prevention (single time version)
 *
 * Contact: Jeff Maddalon, Rick Butler and George Hagen
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

//import gov.nasa.larcfm.ACCoRD.CD3D;
import gov.nasa.larcfm.ACCoRD.CDSICore;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.EuclideanProjection;
//import gov.nasa.larcfm.Util.KinematicPlanCore;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.Kinematics;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.ProjectedKinematics;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
//import gov.nasa.larcfm.Util.f;
//import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Velocity;

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
 * Note that in the case of geodetic coordinates this version of bands
 * performs an internal projection of the coordinates and velocities
 * into the Euclidean frame (see Util/Projection).  Accuracy may be 
 * reduced if the traffic plans involve any segments longer than
 * Util.Projection.projectionConflictRange(lat,acc), and an error will
 * be logged if the distance between traffic and ownship exceeds 
 * Util.Projection.projectionMaxRange() at any point in the lookahead
 * range.<p>
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
 * b.clear();
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of (one) traffic aircraft, velocity of traffic);
 * b.addTraffic(position of (another) traffic aircraft, velocity of traffic);
 * ...add other traffic aircraft...
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
 *
 * When any "parameter" to this class is set (separation distance,
 * lookahead time, positions in latitude/longitude, etc.), all
 * previous bands information is cleared; therefore, all traffic
 * aircraft must be added <em>after</em> the parameters are
 * configured.  For more complete example usage see the file
 * <tt>Batch.java</tt>
 *
 */
public final class KinematicIntentBands extends IntentBands implements GenericIntentBands, ErrorReporter {


  protected double timeStep;
  protected double trkStep;
  protected double gsStep;
  protected double vsStep;
  protected double bankAngle;
  protected double vsAccel;
  protected double gsAccel;
  protected double losThreshold;

  protected boolean adjustStepSizes = false;

  protected ArrayList<Plan> traffic = new ArrayList<Plan>(); // we need this information for the kinematics


  private boolean checkParams = true;

  protected IntervalSet currentLosSet; // this stores a list of intervals where conflict detection has detected an immediate los > losThreshold

  private CDSICore cdsicore;

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
  public KinematicIntentBands() {
    super();
    timeStep = 1;
    trkStep = Units.from("deg", 1.0);
    gsStep = Units.from("knot", 10.0);
    vsStep = Units.from("fpm", 50.0);
    bankAngle = Units.from("deg",30.0);
    gsAccel = Units.gn/4;
    vsAccel = Units.gn/4;
    losThreshold = timeStep;
    error = new ErrorLog("KinematicIntentBands");
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
  public KinematicIntentBands(double D, String dunit, double H, String hunit, double B, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit) {
    super(D, dunit, H, hunit, B, T, tunit, max_gs, gsunit, max_vs, vsunit);
    timeStep = 1;
    trkStep = Units.from("deg", 1.0);
    gsStep = Units.from("knot", 10.0);
    vsStep = Units.from("fpm", 50.0);
    bankAngle = Units.from("deg",30.0);
    gsAccel = Units.gn/4;
    vsAccel = Units.gn/4;
    losThreshold = timeStep;
    error = new ErrorLog("KinematicIntentBands");
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
   * @param time_step the size of time steps in the iterative phase [sec]
   * @param trk_step the size of track steps in the iterative phase [deg]
   * @param gs_step the size of ground speed steps in the iterative phase [knot]
   * @param vs_step the size of vertical speed steps in the iterative phase [fpm]
   * @param bank_angle the intended bank angle of turns [deg]
   * @param gs_accel the intended ground speed acceleration [m/s^2]
   * @param vs_accel the intended vertical speed acceleration [m/s^2]
   */
  public KinematicIntentBands(double D, String dunit, double H, String hunit, double B, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit,
      double time_step, double trk_step, String trkunit, double gs_step, double vs_step,
      double bank_angle, String bankunit, double gs_accel, String gsaccelunit, double vs_accel, String vsaccelunit) {
    super(D, dunit, H, hunit, B, T, tunit, max_gs, gsunit, max_vs, vsunit);
    timeStep = Units.from(tunit, time_step);
    trkStep = Units.from(trkunit, trk_step);
    gsStep = Units.from(gsunit, gs_step);
    vsStep = Units.from(vsunit, vs_step);
    bankAngle = Units.from(bankunit,bank_angle);
    gsAccel = Units.from(gsaccelunit, gs_accel);
    vsAccel = Units.from(vsaccelunit, vs_accel);
    losThreshold = timeStep;
    error = new ErrorLog("KinematicIntentBands");
  }

  @SuppressWarnings("unchecked")
  public KinematicIntentBands(KinematicIntentBands b) {
    super();
    traffic = (ArrayList<Plan>)b.traffic.clone();
    //	    cores = (ArrayList<Detection3D>)b.cores.clone();
    red = b.red;
    setDistance(b.getDistance("m"),"m");
    setHeight(b.getHeight("m"),"m");
    setLookaheadTime(b.getLookahedTime("s"),"s");
    setMaxGroundSpeed(b.getMaxGroundSpeed("kn"),"kn");
    setMaxVerticalSpeed(b.getMaxVerticalSpeed("fpm"),"fpm");
    ownship = b.ownship;
    needCompute = b.needCompute;
    timeStep = b.timeStep;
    trkStep = b.trkStep;
    gsStep = b.gsStep;
    vsStep = b.vsStep;
    bankAngle = b.bankAngle;
    gsAccel = b.gsAccel;
    vsAccel = b.vsAccel;
    losThreshold = b.losThreshold;
    error = new ErrorLog("KinematicIntentBands");
  }

  protected void init(double D, String dunit, double H, String hunit, double B, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit) {
    traffic = new ArrayList<Plan>();
    //		cores = new ArrayList<Detection3D>();
    cdsicore = new CDSICore(CDCylinder.mk(Units.from(dunit, D), Units.from(hunit, H)));
    super.init(D, dunit, H, hunit, B, T, tunit, max_gs, gsunit, max_vs, vsunit);
  }


//  /**
//   * Horizontal separation distance [nmi]
//   */
//  public void setDistance(double D, String unit) {
//    super.setDistance(D, unit);
//    cdsicore.setDistance(red.getDistance());
//  }
//
//  /**
//   * Vertical separation distance [ft]
//   */
//  public void setHeight(double H, String unit) {
//    super.setHeight(H, unit);
//    cdsicore.setHeight(red.getHeight());
//  }

  
  /** Returns a copy of these bands projected to the specified absolute time (in seconds).  The ownship follows a state projection, 
   * traffic follow their plans.  Note that these bands are only valid at the time and projected position of the ownship at absoluteTime.
   */
  public KinematicIntentBands getFutureBands(double absoluteTime, String unit) {
    KinematicIntentBands retBands = new KinematicIntentBands(
        getDistance("m"),"m",
        getHeight("m"), "m",
        getStartTime("s"),
        getLookahedTime("s"),"s",
        getMaxGroundSpeed("m/s"), "m/s",
        getMaxVerticalSpeed("m/s"), "m/s",
        timeStep,
        trkStep, "rad",
        gsStep,
        vsStep,
        bankAngle, "rad",
        gsAccel, "m/s^2",
        vsAccel, "m/s^2");
    double offset = Units.from(unit, absoluteTime) - to;
    retBands.setOwnship(so.linear(vo, offset), vo, Units.from(unit, absoluteTime), null);
    for (int i = 0; i < traffic.size(); i++) {
      retBands.addTraffic(traffic.get(i));
    }
    return retBands;
  }

  /** Returns the time step used in the iterative phase -- smaller numbers provide more precise results [sec] */
  public double getTimeStep(String unit) {
    return Units.to(unit,timeStep);
  }

  /** Returns the track search step used in the iterative phase (resolutions will be multiples of this value) [deg] */
  public double getTrackStep(String unit) {
    return Units.to(unit, trkStep);
  }

  /** Returns the ground speed search step used in the iterative phase (resolutions will be multiples of this value) [knot] */
  public double getGsStep(String unit) {
    return Units.to(unit, gsStep);
  }

  /** Returns the vertical speed search step used in the iterative phase (resolutions will be multiples of this value) [fpm] */
  public double getVsStep(String unit) {
    return Units.to(unit, vsStep);
  }

  /** Returns the constant bank angle to be used in turns [deg] */
  public double getBankAngle(String unit) {
    return Units.to(unit, bankAngle);
  }

  /** Returns the constant ground speed acceleration to be used [m/s^2] */
  public double getGsAccel(String unit) {
    return Units.to(unit,  gsAccel);
  }

  /** Returns the constant vertical speed acceleration to be used [m/s^2] */
  public double getVsAccel(String unit) {
    return Units.to(unit, vsAccel);
  }


  /** Sets the time step used in the iterative phase -- smaller numbers provide more precise results [sec] */
  public void setTimeStep(double t, String unit) {
    t = Units.from(unit, t);
    timeStep = t;
    losThreshold = t;
    reset();
  }

  /** Sets the track search step used in the iterative phase (resolutions will be multiples of this value) [deg] */
  public void setTrackStep(double t, String unit) {
    trkStep = Units.from(unit,t);
    reset();
  }

  /** Sets the ground speed search step used in the iterative phase (resolutions will be multiples of this value) [knot] */
  public void setGsStep(double t, String unit) {
    gsStep = Units.from(unit,t);
    reset();
  }

  /** Sets the vertical speed search step used in the iterative phase (resolutions will be multiples of this value) [fpm] */
  public void setVsStep(double t, String unit) {
    vsStep = Units.from(unit,t);
    reset();
  }

  /** Sets the constant bank angle to be used in turns [deg] */
  public void setBankAngle(double t, String unit) {
    bankAngle = Units.from(unit,t);
    reset();
  }

  /** Sets the constant ground speed acceleration to be used [m/s^2] */
  public void setGsAccel(double t, String unit) {
    gsAccel = Units.from(unit, t);
    reset();
  }

  /** Sets the constant vertical speed acceleration to be used [m/s^2] */
  public void setVsAccel(double t, String unit) {
    vsAccel = Units.from(unit, t);
    reset();
  }

  public void clear() {
    super.clear();
    traffic.clear();
  }

  protected void reset() {
    super.reset();
    error.getMessage();
  }

  // note: this should work if you seed the Plans with KinematicPlanCores to allow for kinematic maneuvers of traffic
  public boolean addTraffic(Plan fp) {
    traffic.add(fp);
    return super.addTraffic(fp);
  }	

  private void computeBandsRegions(IntervalSet losSet, IntervalSet bandsSet, double low, double top, ArrayList<Interval> bandsArray, ArrayList<BandsRegion> bandsRegionArray) {
    // remove any values outside the range
    IntervalSet oob = new IntervalSet();
    oob.union(new Interval(-Double.MAX_VALUE, low));
    oob.union(new Interval(top, Double.MAX_VALUE));
    bandsSet.union(losSet);
    bandsSet.diff(oob);
    // insert initial empty interval, if necessary
    if (bandsSet.size() > 0 && Util.almost_greater(bandsSet.getInterval(0).low, low)) {
      bandsArray.add(new Interval(low, bandsSet.getInterval(0).low));
      bandsRegionArray.add(BandsRegion.NONE);
    }
    for (int i = 0; i < bandsSet.size()-1; i++) {
      bandsArray.add(bandsSet.getInterval(i)); // NEAR
      bandsArray.add(new Interval(bandsSet.getInterval(i).up, bandsSet.getInterval(i+1).low)); // NONE
      bandsRegionArray.add(BandsRegion.NEAR);
      bandsRegionArray.add(BandsRegion.NONE);
    }
    // get last NEAR region and anything after
    if (bandsSet.size() > 0) {
      Interval last = bandsSet.getInterval(bandsSet.size()-1); 
      bandsArray.add(last);
      bandsRegionArray.add(BandsRegion.NEAR);
      if (Util.almost_less(last.up, top)) {
        bandsArray.add(new Interval(last.up, top));
        bandsRegionArray.add(BandsRegion.NONE);
      }
    } else {
      bandsArray.add(new Interval(low, top));
      bandsRegionArray.add(BandsRegion.NONE);
    }
  }

  protected void trackCompute() {
    boundedTrackCompute(0.0, Math.PI*2);
  }

  void boundedTrackCompute(double low, double high) {
    currentLosSet = new IntervalSet();
    double turnStepTime = Kinematics.turnTime(vo.gs(), trkStep, bankAngle);
    IntervalSet trkSet = new IntervalSet();
    for (double a = low; a < high; a += trkStep) {
      Interval in = Interval.EMPTY;
      if (a < vo.compassAngle() && vo.compassAngle() < a+trkStep) {
        boolean turnDir = Util.clockwise(vo.compassAngle(), a);
        double tst1 = Kinematics.turnTime(vo.gs(), Util.turnDelta(vo.compassAngle(), a), bankAngle);
        double tst2 = turnStepTime - tst1;
        // we do not want to shortcut the second check
        boolean chk1 = conflictAfterTurn(so, vo, to, traffic, tst1, a, bankAngle, turnDir, low, high);
        boolean chk2 = conflictAfterTurn(so, vo, to, traffic, tst2, vo.compassAngle(), bankAngle, !turnDir, low, high); 
        if ( chk1 || chk2 ) {
          in = new Interval(a-trkStep, a+trkStep);
        }		        
      } else if ( conflictAfterTurn(so, vo, to, traffic, timeStep, a, bankAngle, Util.clockwise(vo.trk(), a), low, high) ) {
        in = new Interval(a-trkStep, a+trkStep);				
      }
      trkSet.union(in);
    }
    trackArray = new ArrayList<Interval>();
    trackRegionArray = new ArrayList<BandsRegion>();
    // get any empty space before first NEAR interval
    computeBandsRegions(losSetDuringTurn(trkSet), trkSet, low, high, trackArray, trackRegionArray);
  }

  protected void groundCompute() {
    boundedGroundCompute(0.0, red.getMaxGroundSpeed());
  }

  void boundedGroundCompute(double low, double high) {
    currentLosSet = new IntervalSet();
    double gsStepTime = Kinematics.gsAccelTime(vo, vo.gs()+gsStep, gsAccel);
    // re-do with kinematic info for track
    IntervalSet gsSet = new IntervalSet();
    for (double a = low; a < high; a += gsStep) {
      Interval in = Interval.EMPTY;
      if (a < vo.gs() && vo.gs() < a+gsStep) {
        double tst1 = Kinematics.gsAccelTime(vo, a, gsAccel);
        double tst2 = gsStepTime - tst1;
        // we do not want to shortcut the second check
        boolean chk1 = conflictAfterGS(so, vo, to, traffic, tst1, a, gsAccel, low, high);
        boolean chk2 = conflictAfterGS(so, vo, to, traffic, tst2, vo.gs(), gsAccel, low, high);
        if ( chk1 || chk2 ) {
          in = new Interval(a-gsStep, a+gsStep);
        }        
        //general case
      } else if ( conflictAfterGS(so, vo, to, traffic, timeStep, a, gsAccel, low, high) ) {
        in = new Interval(a-gsStep, a+gsStep);				
      }
      gsSet.union(in);
    }
    groundArray = new ArrayList<Interval>();
    groundRegionArray = new ArrayList<BandsRegion>();
    // get any empty space before first NEAR interval
    computeBandsRegions(losSetDuringGS(gsSet), gsSet, low, high, groundArray, groundRegionArray);		
  }

  protected void verticalCompute() {
    boundedVerticalCompute(-red.getMaxVerticalSpeed(), red.getMaxVerticalSpeed());
  }


  void boundedVerticalCompute(double low, double high) {
    currentLosSet = new IntervalSet();
    double vsStepTime = Kinematics.vsAccelTime(vo, vo.vs()+vsStep, vsAccel);
    // re-do with kinematic info for track
    IntervalSet vsSet = new IntervalSet();
    for (double a = low; a < high; a += vsStep) {
      Interval in = Interval.EMPTY;
      if (a < vo.vs() && vo.vs() < a+vsStep) {
        double tst1 = Kinematics.vsAccelTime(vo, a, vsAccel);
        double tst2 = vsStepTime - tst1;
        // we do not want to shortcut the second check
        boolean chk1 = conflictAfterVS(so, vo, to, traffic, tst1, a, vsAccel, low, high);
        boolean chk2 = conflictAfterVS(so, vo, to, traffic, tst2, vo.vs(), vsAccel, low, high); 
        if ( chk1 || chk2 ) {
          in = new Interval(a-vsStep, a+vsStep);
        }        
        //general case
      } else if ( conflictAfterVS(so, vo, to, traffic, timeStep, a, vsAccel, low, high) ) {
        in = new Interval(a-vsStep, a+vsStep);				
      }
      vsSet.union(in);
    }
    verticalArray = new ArrayList<Interval>();
    verticalRegionArray = new ArrayList<BandsRegion>();
    // get any empty space before first NEAR interval
    computeBandsRegions(losSetDuringVS(vsSet), vsSet, low, high, verticalArray, verticalRegionArray);		
  }


  public BandsRegion regionOfTrack(double trk, String unit) {
    if (needCompute) {
      recompute();
    }
    trk = Units.from(unit, trk);
    for (int i = 0; i < trackArray.size(); i++) {
      if (trackArray.get(i).inOO(trk)) return trackRegionArray.get(i); 
    }
    return BandsRegion.NONE;
  }

  public BandsRegion groundRegionOf(double gs, String unit) {
    if (needCompute) {
      recompute();
    }
    gs = Units.from(unit, gs);
    for (int i = 0; i < groundArray.size(); i++) {
      if (groundArray.get(i).inOO(gs)) return groundRegionArray.get(i); 
    }
    return BandsRegion.NONE;
  }

  public BandsRegion VerticalRegionOf(double vs, String unit) {
    if (needCompute) {
      recompute();
    }
    vs = Units.from(unit, vs);
    for (int i = 0; i < verticalArray.size(); i++) {
      if (verticalArray.get(i).inOO(vs)) return verticalRegionArray.get(i); 
    }
    return BandsRegion.NONE;
  }

  /** All in internal Units, returns true if there is a LoS during a turn or a conflict following it */
  private boolean conflictAfterTurn(Position so, Velocity vo, double to, ArrayList<Plan> intent, double stepSize, double trk, double bank, boolean turnRight, double low, double high) {
    double R = Kinematics.turnRadius(vo.gs(), bank);
    double dt = Kinematics.turnTime(vo, trk, bank, turnRight);
    if (dt > red.getTime()) return false;
    double endTime = to+dt;
    Pair<Position,Velocity> end = ProjectedKinematics.turn(so, vo, dt, R, turnRight);
    for (int i = 0; i < intent.size(); i++) {
      //          if (CDSICore.cdsicore(end.first, end.second, endTime, 10.0e+300, intent.get(i), red.getDistance(), red.getHeight(), 0.0, red.getTime()-dt)) {
      if (conflictDetectionSI(end.first, end.second, endTime, intent.get(i), red.getStartTime()-dt, red.getTime()-dt)) {
        //f.pln("INTENT "+Units.to("deg",end.second.compassAngle()));              
        Interval nint;
        double endpt;
        if (turnRight) {
          endpt = Util.to_2pi(vo.compassAngle()+Math.PI);
          if (trk > endpt) {
            nint = new Interval(trk,Math.PI*2);
            currentLosSet.union(nint);                
            nint = new Interval(0,endpt);
            currentLosSet.union(nint);                
          } else {
            nint = new Interval(trk,endpt);
            currentLosSet.union(nint);
          }
        } else {
          endpt = Util.to_2pi(vo.compassAngle()-Math.PI);
          if (trk < endpt) {
            nint = new Interval(endpt,Math.PI*2);
            currentLosSet.union(nint);                
            nint = new Interval(0,trk);
            currentLosSet.union(nint);                
          } else {
            nint = new Interval(endpt,trk);
            currentLosSet.union(nint);
          }
        }
        return true;
      }
    }
    return false;
  }

  private boolean conflictAfterGS(Position so, Velocity vo, double to, ArrayList<Plan> intent, double stepSize, double goalGs, double gsAccel, double low, double high) {
    double dt = Kinematics.gsAccelTime(vo, goalGs, gsAccel);
    double endTime = to+dt;
    double accel = gsAccel;
    if (goalGs < vo.gs()) accel = -accel;
    Pair<Position,Velocity> end = ProjectedKinematics.gsAccel(so, vo, dt, accel);
    for (int i = 0; i < intent.size(); i++) {
      //          if (CDSICore.cdsicore(end.first, end.second, endTime, 10.0e+300, intent.get(i), red.getDistance(), red.getHeight(), 0.0, red.getTime()-dt)) {
      if (conflictDetectionSI(end.first, end.second, endTime, intent.get(i), red.getStartTime()-dt, red.getTime()-dt)) {
        Interval nint;
        if (accel > 0) nint = new Interval(goalGs,high);
        else nint = new Interval(low, goalGs);
        currentLosSet.union(nint);
        return true;
      }
    }
    return false;
  }

  private boolean conflictAfterVS(Position so, Velocity vo, double to, ArrayList<Plan> intent, double stepSize, double goalVs, double vsAccel, double low, double high) {
    double dt = Kinematics.vsAccelTime(vo, goalVs, vsAccel);
    double endTime = to+dt;
    double accel = vsAccel;
    if (goalVs < vo.vs()) accel = -accel;
    Pair<Position,Velocity> end = ProjectedKinematics.vsAccel(so, vo, dt, accel);
    for (int i = 0; i < intent.size(); i++) {
      //          if (CDSICore.cdsicore(end.first, end.second, endTime, 10.0e+300, intent.get(i), red.getDistance(), red.getHeight(), 0.0, red.getTime()-dt)) {
      if (conflictDetectionSI(end.first, end.second, endTime, intent.get(i), red.getStartTime()-dt, red.getTime()-dt)) {
        Interval nint;
        if (accel > 0) nint = new Interval(goalVs,high);
        else nint = new Interval(low, goalVs);
        currentLosSet.union(nint);
        return true;
      }
    }
    return false;
  }

  private IntervalSet losSetDuringTurn(IntervalSet conflictSet) {
    IntervalSet losSet = new IntervalSet();
    double D = red.getDistance();
    double H = red.getHeight();

    double maxTime = Kinematics.turnTime(Velocity.mkTrkGsVs(0.0, vo.gs(), 0.0), Math.PI, bankAngle, true);
    ArrayList<Plan> relevantTraffic = buildRelevantTraffic(D, H, maxTime, vo.gs(), vo.vs());

    // now start to turn both left and right.  If we hit LoS on a right turn, goRight = false, and all 
    // further turns in that direction will also be LoS.  Similarly for left. 
    boolean goLeft = true;
    boolean goRight = true;
    double R = Kinematics.turnRadius(vo.gs(), bankAngle);
    for (double turn = 0; turn <= Math.PI; turn += trkStep) {
      double trk1 = Util.to_2pi(vo.trk()+turn);
      double trk2 = Util.to_2pi(vo.trk()-turn);
      double dt = Kinematics.turnTime(vo, trk1, bankAngle, true);
      double dt2 = Kinematics.turnTime(vo, trk1+trkStep, bankAngle, true);
      // initially in LoS
      for (int i = 0; i < relevantTraffic.size(); i++) {
        // turn right and left
        if (!goRight) { // shortcut
          losSet.union(new Interval(trk1-trkStep, trk1+trkStep));
        } else {
          for (double t = dt; goRight && dt <= maxTime && t < dt2 && conflictSet.in(trk1); t += timeStep) {
            Position si = relevantTraffic.get(i).position(to+t);
            Velocity vi = relevantTraffic.get(i).velocity(to+t);				  
            Position s1 = ProjectedKinematics.turn(so, vo, dt, R, true).first;
            if (currentLosSet.in(trk1) || violation(s1, si, vo, vi)) {
              losSet.union(new Interval(trk1-trkStep, trk1+trkStep));
              goRight = false;
              //f.pln("LOS RIGHT TURN AT "+Units.to("deg", trk1) +" "+ Units.to("nmi", s1.distanceH(si))+" "+Units.to("nmi",D));
            }
          }
        }
        if (!goLeft) { // shortcut
          losSet.union(new Interval(trk2-trkStep, trk2+trkStep));					
        } else {
          for (double t = dt; goLeft && dt <= maxTime && t < dt2 && conflictSet.in(trk2); t += timeStep) {
            Position si = relevantTraffic.get(i).position(to+t);
            Velocity vi = relevantTraffic.get(i).velocity(to+t);
            Position s2 = ProjectedKinematics.turn(so, vo, dt, R, false).first;
            if (currentLosSet.in(trk2) || violation(s2, si, vo, vi)) {
              losSet.union(new Interval(trk2-trkStep, trk2+trkStep));
              goLeft = false;
              //f.pln("LOS LEFT TURN AT "+Units.to("deg", trk2));
            }					
          }
        }
      }
    }
    return losSet;
  }

  private IntervalSet losSetDuringGS(IntervalSet conflictSet) {
    IntervalSet losSet = new IntervalSet();
    double D = red.getDistance();
    double H = red.getHeight();

    double maxTime = Util.max(Kinematics.gsAccelTime(vo, 0, gsAccel), Kinematics.gsAccelTime(vo, red.getMaxGroundSpeed(), gsAccel));
    ArrayList<Plan> relevantTraffic = buildRelevantTraffic(D, H, maxTime, red.getMaxGroundSpeed(), vo.vs());

    // now start to slow down and speed up. 
    boolean goFast = true;
    boolean goSlow = true;
    double limit = Util.max(vo.gs(), red.getMaxGroundSpeed()-vo.gs());
    for (double gs = 0; gs <= limit; gs += gsStep) {
      double gs1 = vo.gs()+gs;
      double gs2 = vo.gs()-gs;
      double dt = Kinematics.gsAccelTime(vo, gs1, gsAccel);
      double dt2 = Kinematics.gsAccelTime(vo, gs1+gsStep, gsAccel);
      // initially in LoS
      for (int i = 0; i < relevantTraffic.size(); i++) {
        if (!goFast) { // shortcut
          losSet.union(new Interval(gs1-gsStep, gs1+gsStep));
        } else {
          for (double t = dt; goFast && dt <= maxTime && t < dt2 && conflictSet.in(gs1); t += timeStep) {
            Position si = relevantTraffic.get(i).position(to+dt);
            Velocity vi = relevantTraffic.get(i).velocity(to+dt);
            Position s1 = ProjectedKinematics.gsAccel(so, vo, dt, gsAccel).first; 
            if (currentLosSet.in(gs1) || violation(s1, si, vo, vi)) {
              losSet.union(new Interval(gs1-gsStep, gs1+gsStep));
              goFast = false;
            }
          }
        }
        if (!goSlow) { // shortcut
          losSet.union(new Interval(gs2-gsStep, gs2+gsStep));					
        } else {
          for (double t = dt; goSlow && dt < maxTime && t < dt2 && conflictSet.in(gs2); t += timeStep) {
            Position si = relevantTraffic.get(i).position(to+dt);
            Velocity vi = relevantTraffic.get(i).velocity(to+dt);
            Position s2 = ProjectedKinematics.gsAccel(so, vo, dt, -gsAccel).first; // will this work if GS is negative?
            if (currentLosSet.in(gs2) || (gs2 >=0 && (violation(s2, si, vo, vi)))) {
              losSet.union(new Interval(gs2-gsStep, gs2+gsStep));
              goSlow = false;
            }
          }
        }
      }
    }
    return losSet;
  }


  private IntervalSet losSetDuringVS(IntervalSet conflictSet) {
    IntervalSet losSet = new IntervalSet();
    double D = red.getDistance();
    double H = red.getHeight();

    double maxTime = Util.max(Kinematics.vsAccelTime(vo, red.getMaxVerticalSpeed(), vsAccel), Kinematics.vsAccelTime(vo, -red.getMaxVerticalSpeed(), vsAccel));
    ArrayList<Plan> relevantTraffic = buildRelevantTraffic(D, H, maxTime, vo.gs(), red.getMaxVerticalSpeed());

    // now start to slow down and speed up. 
    boolean goUp = true;
    boolean goDown = true;
    double limit = Util.max(vo.vs()-red.getMaxVerticalSpeed(), red.getMaxVerticalSpeed()-vo.vs());
    for (double vs = 0; vs <= limit; vs += trkStep) {
      double vs1 = vo.vs()+vs;
      double vs2 = vo.vs()-vs;
      double dt = Kinematics.vsAccelTime(vo, vs1, vsAccel);
      double dt2 = Kinematics.vsAccelTime(vo, vs1+vsStep, vsAccel);
      // initially in LoS
      for (int i = 0; i < relevantTraffic.size(); i++) {
        if (!goUp) { // shortcut
          losSet.union(new Interval(vs1-vsStep, vs1+vsStep));
        } else {
          for (double t = dt; goUp && dt < maxTime && t < dt2 && conflictSet.in(vs1); t += timeStep) {
            Position si = relevantTraffic.get(i).position(to+dt);
            Velocity vi = relevantTraffic.get(i).velocity(to+dt);
            Position s1 = ProjectedKinematics.vsAccel(so, vo, dt,  vsAccel).first; 
            if (currentLosSet.in(vs1) || violation(s1, si, vo, vi)) {
              losSet.union(new Interval(vs1-vsStep, vs1+vsStep));
              goUp = false;
            }
          }
        }
        if (!goDown) { // shortcut
          losSet.union(new Interval(vs2-vsStep, vs2+vsStep));					
        } else {
          for (double t = dt; goDown && dt < maxTime && t < dt2 && conflictSet.in(vs2); t += timeStep) {
            Position si = relevantTraffic.get(i).position(to+dt);
            Velocity vi = relevantTraffic.get(i).velocity(to+dt);
            Position s2 = ProjectedKinematics.vsAccel(so, vo, dt, -vsAccel).first; // will this work if GS is negative?
            if (currentLosSet.in(vs2) || violation(s2, si, vo, vi)) {
              losSet.union(new Interval(vs2-vsStep, vs2+vsStep));
              goDown = false;
            }
          }
        }
      }
    }
    return losSet;
  }


  protected boolean violation(Position so, Position si, Velocity vo, Velocity vi) {
    Vect3 so3 = so.point();
    Vect3 si3 = si.point();
    if (isLatLon()) {
      EuclideanProjection sp = Projection.createProjection(so.lla().zeroAlt());
      vi = sp.projectVelocity(si, vi);
      vo = sp.projectVelocity(so, vo);
      so3 = sp.project(so);
      si3 = sp.project(si);
    }
    return cdsicore.getCoreDetection().violation(so3, vo, si3, vi);
  }


  /**
   * Returns true if the aircraft will be in LoS within time T
   */
  protected boolean conflictDetectionSI(Position so, Velocity vo, double tm, Plan intent, double B, double T) {
    return cdsicore.detection(so, vo, tm, 10.0e+300, intent, B, T);
  }


  private ArrayList<Plan> buildRelevantTraffic(double D, double H, double maxTime, double ownGs, double ownVs) {
    // toss out those that are not even possible to be nearby
    ArrayList<Plan> relevantTraffic = new ArrayList<Plan>();
    for (int i = 0; i < traffic.size(); i++) {
      Plan ifp = traffic.get(i);
      Position si0 = ifp.position(to);
      boolean go = true;
      for (int j = ifp.getSegment(to); go && j <= ifp.getSegment(to+maxTime); j++) {
        if (so.distanceH(si0) - maxTime*(ownGs+ifp.initialVelocity(j).gs()) < D && 
            so.distanceV(si0) - maxTime*(Math.abs(ownVs)+Math.abs(ifp.initialVelocity(j).vs())) < H) {
          relevantTraffic.add(ifp);
          go = false;
        }
      }
    }
    return relevantTraffic;
  }

  /** Experimental.  This will also clear any results (but not traffic info) */
  public void setCoreDetection(Detection3D d) {
    cdsicore.setCoreDetection(d);
    red.clear();
    needCompute = true;
  }

}
