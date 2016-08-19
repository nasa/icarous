/* 
 * Core Bands Computations
 *
 * Contact: Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.ACCoRD.CriticalVectors;

import java.util.List;
import java.util.Arrays;


/**
 * Objects of class "BandsCore" compute the conflict prevention bands
 * using linear state-based predictions of ownship and (multiple)
 * traffic aircraft positions.  This class presumes the use "internal" units
 * and Cartesian coordinates and it doesn't perform bookkeeping of its parameters.
 * For instance, if D, H, etc are set after traffic has been added, the method
 * clear() has to be explicitly called. Otherwise, the state of the bands object
 * is undetermined.  For all these reasons, this class is not intended for 
 * general use. It is most appropriate for building algorithms that include 
 * prevention band information. For a more "user-friendly" version, try the Bands class. <p>
 *
 * The bands consist of ranges of guidance maneuvers: track angles,
 * ground speeds, and vertical speeds. If a path is (immediately)
 * taken that is within one these bands and no traffic aircraft
 * maneuvers, then a loss of separation will occur within the
 * specified lookahead time.  Implicitly, any path that is not in a
 * band does not have a loss of separation within the lookahead
 * time.<p>
 *
 * Disclaimers: Only parts of these algorithms have been formally
 * verified.  We plan to advance the formal verification, but it is
 * currently not complete. Even with a 'complete' verification, there
 * will still be certain assumptions.  For instance, the formal proofs
 * use real numbers while these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodesic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 *
 * The basic usages is
 * <pre>
 * Bands b(..with configuration parameters..);
 * b.clear();
 * b.addTraffic(relative position of ownship and traffic,
 *              velocity of ownship, 
 *              velocity of one traffic aircraft);
 * b.addTraffic(relative position of ownship and traffic,
 *              velocity of ownship, 
 *              velocity of one traffic aircraft);
 * ...add other traffic aircraft...
 * 
 * IntervalSet track_bands = b.trackBands();
 * IntervalSet ground_speed_bands = b.groundSpeedBands();
 * IntervalSet vertical_speed_bands = b.verticalSpeedBands();
 * </pre>
 *
 * When any "parameter" to this class is set (lookahead time, etc.),
 * any previous bands information is cleared; therefore, all traffic
 * aircraft must be added <em>after</em> the parameters are
 * configured.
 *
 */
public final class BandsCore {                     

  private double D;    
  private double H;    
  private double B;
  private double T; 
  private double max_gs;
  private double max_vs;

  private double trkTol;
  private double gsTol;
  private double vsTol;

  private IntervalSet trk_regions;
  private IntervalSet gs_regions;
  private IntervalSet vs_regions;

  private IntervalSet regions; // a temporary variable

  // For debuging and display
  //private Set<Vect2> critical_trk;  
  //private Set<Vect2> critical_gs;  
  //private Set<Vect3> critical_vs;

  /** Construct a BandsCore object with default values for configuration
   *  parameters.
   */
  public BandsCore() {
    init(Units.from("nmi", 5),
        Units.from("ft", 1000),
        Units.from("min", 0),
        Units.from("min", 3),
        Units.from("kn",  1000), 
        Units.from("fpm", 5000));
  }

  /** 
   * Construct a BandsCore object with the given configuration
   * parameters.  The bands always begin as "empty" bands: green
   * track bands from 0 to 2pi, green ground speed bands from 0 to
   * max_gs, and green vertical speed bands from -max_vs to max_vs.
   *
   * @param D   the minimum horizontal separation distance 
   * @param H   the minimum vertical separation distance
   * @param T   the lookahead range end time (start time is 0)
   * @param max_gs the maximum ground speed that is output by BandsCore,
   * the minimum is 0.
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  public BandsCore(double D, double H, double T, double max_gs, double max_vs) {
    init(D, H, 0, T, max_gs, max_vs);
  }

  /** 
   * Construct a BandsCore object with the given configuration
   * parameters.  The bands always begin as "empty" bands: green
   * track bands from 0 to 2pi, green ground speed bands from 0 to
   * max_gs, and green vertical speed bands from -max_vs to max_vs.
   *
   * @param D   the minimum horizontal separation distance 
   * @param H   the minimum vertical separation distance
   * @param B   the lookahead range start
   * @param T   the lookahead range end
   * @param max_gs the maximum ground speed that is output by BandsCore,
   * the minimum is 0.
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  public BandsCore(double D, double H, double B, double T, double max_gs, double max_vs) {
    init(D, H, B, T, max_gs, max_vs);
  }

  public BandsCore(BandsCore b) {
    init(b.D, b.H, b.B, b.T, b.max_gs, b.max_vs);
    trkTol = b.trkTol;
    gsTol = b.gsTol;
    vsTol = b.vsTol;

    trk_regions = new IntervalSet(b.trk_regions);
    gs_regions = new IntervalSet(b.gs_regions);
    vs_regions = new IntervalSet(b.vs_regions);
  }

  private void init(double D, double H, double B, double T, double max_gs, double max_vs) {

    trk_regions = new IntervalSet();
    gs_regions = new IntervalSet();
    vs_regions = new IntervalSet();
    regions = new IntervalSet();

    //critical_trk = new HashSet<Vect2>(20);
    //critical_gs = new HashSet<Vect2>(20);
    //critical_vs = new HashSet<Vect3>(10);

    setDiameter(D);
    setHeight(H);
    setTimeRange(B,T);
    setMaxGroundSpeed(max_gs);
    setMaxVerticalSpeed(max_vs);

    trkTol = Units.from("deg", 0.0);
    gsTol = Units.from("knot", 0.0);
    vsTol = Units.from("fpm", 0.0);
  }

  /** Set the end lookahead time in internal units. The lookahead range start is set to 0. */
  public void setTime(double t) {
    B = 0;
    T = t;
  }
  
  public void setStartTime(double t) {
    B = t;
  }

  /** Returns the end lookahead time in internal units. */
  public double getTime() {
    return T;
  }

  /** Set the lookahead time range in internal units with start and end times. */
  public void setTimeRange(double b, double t) {
    B = b;
    T = t;
  }

  /** Returns the lookahead start time in internal units. */
  public double getStartTime() {
    return B;
  }

  /** Sets the minimum horizontal separation distance in internal units. */
  public void setDiameter(double d) {
    D = Math.abs(d);
  }

  /** Returns the minimum horizontal separation distance in internal
   * units. */
  public double getDiameter() {
    return D;
  }

  /** Sets the minimum vertical separation distance in internal
   * units. */
  public void setHeight(double h) {
    H = Math.abs(h);
  }

  /** Returns the minimum vertical separation distance in internal
   * units. */
  public double getHeight() {
    return H;
  }

  /** Sets the maximum ground speed in internal units, the minimum
   * is 0. */
  public void setMaxGroundSpeed(double gs) {
    max_gs = Math.abs(gs);
  }

  /** Returns the maximum ground speed in internal units.  */
  public double getMaxGroundSpeed() {
    return max_gs;
  }

  /** Sets the range of vertical speeds in internal units, -max_vs
   * to max_vs. */
  public void setMaxVerticalSpeed(double vs) {
    max_vs = Math.abs(vs);
  }

  /** 
   * Returns the range of vertical speed in internal units output by
   * BandsCore. 
   */
  public double getMaxVerticalSpeed() {
    return max_vs;
  }

  /** Sets a minimum size for green/no conflict bands to be allowed, to avoid
   * "eye of the needle" situations. Any green bands smaller than this will not be reported.
   * @param trk minimum acceptable track angle
   */
  public void setTrackTolerance(double trk) {
    if (trk >= 0) {
      trkTol = trk;
    }
  }
  /** Returns minimum acceptable track angle */
  public double getTrackTolerance() {
    return trkTol;
  }

  /** Sets a minimum size for green/no conflict bands to be allowed, to avoid
   * "eye of the needle" situations. Any green bands smaller than this will not be reported.
   * @param gs minimum acceptable ground speed
   */
  public void setGroundSpeedTolerance(double gs) {
    if (gs >= 0) {
      gsTol = gs;
    }
  }
  /** Returns minimum acceptable ground speed */
  public double getGroundSpeedTolerance() {
    return gsTol;
  }

  /** Sets a minimum size for green/no conflict bands to be allowed, to avoid
   * "eye of the needle" situations. Any green bands smaller than this will not be reported.
   * @param vs minimum acceptable vertical speed
   */
  public void setVerticalSpeedTolerance(double vs) {
    if (vs >= 0) {
      vsTol = vs;
    }
  }
  /** Returns minimum acceptable ground speed */
  public double getVerticalSpeedTolerance() {
    return vsTol;
  }

  /** 
   * Compute the conflict prevention band information for one
   * ownship/traffic aircraft pair and add this information to any
   * existing band information.  For a collection of "band"
   * information to make sense, all ownship aircraft must be the
   * same.  All parameters are in cartesian coordinates in
   * "internal" units.
   *
   * @param s the relative position of the traffic and ownship aircraft
   * @param vo the velocity of the ownship
   * @param vi the velocity of the traffic
   */
  public void addTraffic(Vect3 s, Velocity vo, Velocity vi, boolean do_trk, boolean do_gs, boolean do_vs) {

    if (do_trk) {
      calcTrkBands(s, vo, vi);
      trk_regions.union(regions);
    }
    if (do_gs) {
      calcGsBands(s, vo, vi);
      gs_regions.union(regions);
    }
    if (do_vs) {
      calcVsBands(s, vo, vi);
      vs_regions.union(regions);
    }

    //f.pln(" ## this.toString() = "+this.toString());
    //f.pln(" ## BandsCore.addTraffic: s = "+f.sStr(s)+" vo = "+vo+" vi = "+vi);
  }

  public void addTraffic(Vect3 s, Velocity vo, Velocity vi) {
    addTraffic(s,vo,vi,true,true,true);
  }
  
  /** 
   * Clear all bands to "empty" bands.
   */
  
  public void clearTrackRegions() {
    trk_regions.clear();
    //critical_trk.clear();
  }
 
  public void clearGroundSpeedRegions() {
    gs_regions.clear();
    //critical_gs.clear();
  }
  
  public void clearVerticalSpeedRegions() {
    vs_regions.clear();
    //critical_vs.clear();
  }
  
  public void clear(boolean do_trk, boolean do_gs, boolean do_vs) {
    if (do_trk) {
      clearTrackRegions();
    }
    if (do_gs) {
      clearGroundSpeedRegions();
    }
    if (do_vs) {
      clearVerticalSpeedRegions();
    }
  }
  
  public void clear() {
    clear(true,true,true);
  }
  
  /** 
   * Provide a copy of the track angle prevention bands.  The angles
   * in this set will result in a loss of separation within the
   * given lookahead time.  The angles are in 'compass' angles: 0 to
   * 2*pi, counter-clockwise from true north.
   */
  public IntervalSet trackBands() {
    return new IntervalSet(trk_regions);
  }

  /*package*/ int trackSize() {
    return trk_regions.size();
  }

  /** Is there a conflict (within the given lookahead time) for this track angle? */ 
  public boolean trackBands(double trk) {
    return trk_regions.in(trk);
  }

  /** Provide a copy of the ground speed prevention bands in
   * internal units.  The speeds in this set will result in a loss
   * of separation within the given lookahead time.  The range of
   * ground speed bands is from 0 to max_gs. 
   */
  public IntervalSet groundSpeedBands() {
    return new IntervalSet(gs_regions);
  }

  /*package*/ int groundSpeedSize() {
    return gs_regions.size();
  }

  /** Is there a conflict (within the given lookahead time) for this ground speed? */ 
  public boolean groundSpeedBands(double gs) {
    return gs_regions.in(gs);
  }

  /** Provide a copy of the vertical speed prevention bands in
   * internal units.  The speeds in this set will result in a loss
   * of separation within the given lookahead time.  The range of
   * vertical speeds is -max_vs to max_vs. 
   */
  public IntervalSet verticalSpeedBands() {
    return new IntervalSet(vs_regions);
  }

  /** Is there a conflict (within the given lookahead time) for this vertical speed? */ 
  public boolean verticalSpeedBands(double vs) {
    return vs_regions.in(vs);
  }

  /*package*/ int verticalSpeedSize() {
    return vs_regions.size();
  }


  /** Return a string representation of this object */
  public String toString() {
    return "Distance: " + Units.str("nmi", D) + " "
        + "Height " + Units.str("ft", H) + " "
        + "Start Time: " + Units.str("s", B) + " "
        + "End Time: " + Units.str("s", T) + " "
        + "Max GS: " + Units.str("knot", max_gs) + " "
        + "Max VS: " + Units.str("ft/min", max_vs) + " ";
  }

  //
  // *************** Compute Bands Information ********************
  //

  /** This method computes the conflict bands for track angles for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  public void calcTrkBands(Vect3 s3, Vect3 vo3, Vect3 vi3) {

    Vect2 vo = vo3.vect2();
    //        Vect2 s = s3.vect2();

    List<Vect2> cv = CriticalVectors.tracks(s3, vo3, vi3, D, H, B, T);
    double[] end_pts = new double[cv.size()+1];
    int i = 0;

    for (Vect2 v : cv) {
      //critical_trk.add(v);
      end_pts[i] = v.compassAngle();
      i++;
    }

    end_pts[i] = 2*Math.PI;
    Arrays.sort(end_pts);

    regions.clear();   
    double lang = 0.0;
    double speed = vo.norm(); 

    for (double nang : end_pts) {      
      double mid = (lang + nang) / 2.0;
      Velocity v_mid = Velocity.mkTrkGsVs(mid, speed, vo3.z);

      if (CD3D.cd3d(s3, v_mid, vi3, D,H,B,T)){
        regions.union(new Interval(lang, nang));
      }

      lang = nang;
    }//for
  } // calcTrackBands


  /** This method computes the conflict bands for ground speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  public void calcGsBands(Vect3 s3, Vect3 vo3, Vect3 vi3) {

    Vect2 vo = vo3.vect2();

    List<Vect2> cv = CriticalVectors.groundSpeeds(s3, vo3, vi3, D, H, B, T);
    double[] end_pts = new double[cv.size()+1];
    int i = 0;

    for (Vect2 v : cv) {
      //critical_gs.add(v);
      end_pts[i] = v.norm();
      i++;
    }

    end_pts[i] = max_gs;
    Arrays.sort(end_pts);

    regions.clear();
    double lastpt = 0.0;
    for (Double pt : end_pts) {      
      if (max_gs < pt) {
        pt = max_gs;
      }
      Velocity v_mid = Velocity.mkTrkGsVs(vo.trk(), (lastpt + pt)/2.0, vo3.z);
      if (CD3D.cd3d(s3, v_mid, vi3, D,H,B,T)){
        regions.union(new Interval(lastpt, pt));
      }
      lastpt = pt;
    }
  }//calcGsBands


  /** This method computes the conflict bands for vertical speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  public void calcVsBands(Vect3 s3, Vect3 vo3, Vect3 vi3) {
    List<Double> cv = CriticalVectors.verticalSpeeds(s3, vo3, vi3, D, H, B, T);
    Double[] end_pts = new Double[cv.size()+1];
    cv.toArray(end_pts);

    end_pts[end_pts.length-1] = max_vs;
    Arrays.sort(end_pts);

    regions.clear();
    double lastvs = -max_vs;

    for (double vs: end_pts) {
      if (vs < -max_vs) vs = -max_vs; 
      if (vs > max_vs)  vs =  max_vs; 
      //critical_vs.add(new Vect3(vo3.x, vo3.y, vs));
      double mid_pt = (lastvs + vs)/2.0;
      if (CD3D.cd3d(s3, new Vect3(vo3.x, vo3.y, mid_pt), vi3, D, H, B, T)){
        regions.union(new Interval(lastvs, vs));
      } 

      lastvs = vs;
    }
  } // calcVsBands

  //
  // *************** Refine Bands Information ********************
  //

  /**
   * Internally modify the bands to remove narrow bands (----X---- becomes ---------).
   * This should be run after the bands have been populated (if at all).
   * This should be used on "green" bands.  Tolerances define the filter sizes.
   * trkTol in [rad], gsTol in [m/s], vsTol in [m/s]
   */
  public void clearNarrowBands(boolean do_trk, boolean do_gs, boolean do_vs) {
    if (do_trk)
      trk_regions.sweepSingle(trkTol);
    if (do_gs)
      gs_regions.sweepSingle(gsTol);
    if (do_vs)
      vs_regions.sweepSingle(vsTol);
  }

  /**
   * Internally modify the bands to remove narrow gaps (XXXX-XXXX becomes XXXXXXXXX).
   * This should be run after the bands have been populated (if at all).
   * This should be used on "red" bands.  Tolerances define the filter sizes.
   * trkTol in [rad], gsTol in [m/s], vsTol in [m/s]
   */
  public void clearTrackBreaks() {
    trk_regions.sweepBreaks(trkTol);
    if (trk_regions.getInterval(0).low <= trkTol &&
        trk_regions.getInterval(trk_regions.size()-1).up+trkTol >= Math.PI*2 &&
        trk_regions.getInterval(0).low <= Util.to_2pi(trk_regions.getInterval(trk_regions.size()-1).up+trkTol)) {
      trk_regions.union(new Interval(0.0,trk_regions.getInterval(0).up));
      trk_regions.union(new Interval(trk_regions.getInterval(trk_regions.size()-1).low,Math.PI*2));
    }   
  }

  public void clearGroundSpeedBreaks() {
    gs_regions.sweepBreaks(gsTol);
  }

  public void clearVerticalSpeedBreaks() {
    vs_regions.sweepBreaks(vsTol);    
  }

  public void clearBreaks(boolean do_trk, boolean do_gs, boolean do_vs) {
    if (do_trk) 
      clearTrackBreaks();
    if (do_gs) 
      clearGroundSpeedBreaks();
    if (do_vs)
      clearVerticalSpeedBreaks();    
  }
  
  public void clearBreaks() {
    clearBreaks(true,true,true);
  }

  //
  // -------------------- Output operations ---------------------
  //

  @SuppressWarnings("unused")
  private static void pln(String str) {
    System.out.println(str);
  }

  @SuppressWarnings("unused")
  private static void p(String str) {
    System.out.print(str);
  }

}
