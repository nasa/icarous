/* 
 * Complete Intent Bands for Conflict Prevention (single time version)
 *
 * Contact: Jeff Maddalon, Rick Butler and George Hagen
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.Util;
//import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;

/**
 * Objects of class "Bands" compute the 'no conflict' and near term
 * conflict prevention (CP) bands using linear state-based preditions
 * of ownship and (multiple) traffic aircraft positions.  The bands
 * consist of ranges of guidance maneuvers: track angles, ground
 * speeds, and vertical speeds. If the ownship immediately executes a
 * "NONE" guidance maneuver and no traffic aircraft maneuvers, then
 * the new path is conflict free within a lookahead time.  If the
 * ownship immediately executes a "NEAR" guidance maneuver and no
 * traffic aircraft maneuvers, then there will be a loss of separation
 * within a lookahead time. <p>
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
 * Disclaimer: Only the mathematical core of these algorithms have
 * been formally verified, certain book-keeping operations have not
 * been verified.  Even with a formal verification, there will still
 * be certain assumptions.  For instance, the formal proofs use real
 * numbers while these implementations use floating point numbers, so
 * numerical differences could result. In addition, the geodesic
 * computations include certain inaccuracies, especially near the
 * earth's poles.<p>
 *
 * The basic usage is
 * <pre>
 * Bands b(..with configuration parameters..);
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of traffic, velocity of traffic);
 * b.addTraffic(position of traffic, velocity of traffic);
 * ...add other aircraft...
 *
 * for (int i = 0; i < b.trackLength(); i++) {
 *    use band information from b.track(i) and b.trackRegion(i);
 * }
 * ..similar for ground speed and vertical speed bands..
 * </pre>
 *
 * When any configuration parameter is set (horizontal separation
 * distance, etc.), any previous bands information is cleared out;
 * therefore, all configuration parameters must be set, before any
 * traffic aircraft information is added.  For more complete example
 * usage see the file <tt>Batch.cpp</tt>.
 *
 */
public class IntentBands implements GenericIntentBands, ErrorReporter {                     

  /*package*/ IntentBandsCore red; 

  /*package*/ boolean ownship;
  /*package*/ boolean needCompute;

  protected ErrorLog error = new ErrorLog("IntentBands");

  /*package*/ Position so;
  /*package*/ Velocity vo;
  /*package*/ double to;

  /*package*/ ArrayList<Interval> trackArray;
  /*package*/ ArrayList<BandsRegion> trackRegionArray;
  /*package*/ ArrayList<Interval> groundArray;
  /*package*/ ArrayList<BandsRegion> groundRegionArray;
  /*package*/ ArrayList<Interval> verticalArray;
  /*package*/ ArrayList<BandsRegion> verticalRegionArray;

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
  public IntentBands() {
    init(5, "nmi", 1000, "ft", 0, 3*60, "s", 1000, "kn", 5000, "fpm");
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
  public IntentBands(double D, String dunit, double H, String hunit, double B, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit) {
    init(D, dunit, H, hunit, 0, T, tunit, max_gs, gsunit, max_vs, vsunit);
  }

  protected void init(double D, String dunit, double H, String hunit, double B, double T, String tunit, double max_gs, String gsunit, double max_vs, String vsunit) {

    red = new IntentBandsCore();

    //	critical_horiz = new HashSet<Vect2>(30);
    //	critical_vert = new HashSet<Vect3>(10);

    setDistance(D, dunit);
    setHeight(H, hunit);
    setTimeRange(B, T, tunit);
    setMaxGroundSpeed(max_gs, gsunit);
    setMaxVerticalSpeed(max_vs, vsunit);

    ownship = false;

    needCompute = true;
  }

  /** Set the lookahead time [s].  Any existing bands information is
   * cleared. */
  public void setLookaheadTime(double t, String unit) {
    red.setTime(Units.from(unit,t));
    reset();
  }

  /** Set the lookahead time range [s].  Any existing bands information is
   * cleared. */
  public void setTimeRange(double b, double t, String unit) {
    red.setTimeRange(Units.from(unit,b), Units.from(unit,t));
    reset();
  }

  /** Returns the time of the lookahead time. [s] */
  public double getLookahedTime(String unit) {
    return Units.to(unit,red.getTime());
  }

  /** Returns the time of the lookahead time. [s] */
  public double getStartTime(String unit) {
    return Units.to(unit,red.getStartTime());
  }


  /** 
   * Add a traffic aircraft flight plan to this set of conflict prevention band
   *
   * @param fp traffic flight plan (may be either latlon or Euclidean)
   */
  public boolean addTraffic(Plan fp) {	  
//f.pln("IntentBands.addTraffic "+fp);
    if ( ! ownship) {
      error.addError("SetOwnship must be called before addTraffic!");
      return false;
    }
    // latlong mismatch
    if ( isLatLon() != fp.isLatLon()) {
      error.addError("Some data is Euclidean and some data is Lat/Lon");
      return false;
    }
    if (isLatLon()) {
      // error checking
      double T = red.getTime();
      for (int i = 0; i < fp.size()-1 && fp.time(i) <= to+T; i++) {
        if (fp.time(i+1) >= to) {
          double dt = fp.time(i) - to;
          LatLonAlt so2 = GreatCircle.linear_initial(so.lla(), vo, dt);
          if (GreatCircle.distance(so2, fp.point(i).lla()) > Projection.projectionMaxRange()) {
            error.addError("Distances are too great for this projection");
          }
        }
      }
      red.addTrafficLL(so.lat(), so.lon(), so.alt(), vo, to, fp); 
    } else {
      Vect3 sr = so.point();
      red.addTraffic(sr,vo,to,fp); 
    }
    needCompute = true;
    return true;
  }



  //
  // Interface Methods...
  //

  public boolean isLatLon() {
    if (ownship) return so.isLatLon();
    return true;  // need some default
  }

  // nmi
  public void setDistance(double d, String unit) {
    double D = Math.abs(Units.from(unit,d));
    red.setDistance(D);
    //amber.setDiameter(D);
    reset();
  }

  public double getDistance(String unit) {
    return Units.to(unit,red.getDistance());
  }

  // ft
  public void setHeight(double h, String unit) {
    double H = Math.abs(Units.from(unit,h));
    red.setHeight(H);
    //amber.setHeight(H);
    reset();
  }

  public double getHeight(String unit) {
    return Units.to(unit,red.getHeight());
  }

  public void setMaxGroundSpeed(double gs, String unit) {
    double max_gs = Math.abs(Units.from(unit,gs));
    red.setMaxGroundSpeed(max_gs);
    //amber.setMaxGroundSpeed(max_gs);
    reset();
  }

  public double getMaxGroundSpeed(String unit) {
    return Units.to(unit, red.getMaxGroundSpeed());
  }

  public void setMaxVerticalSpeed(double vs, String unit) {
    double max_vs = Math.abs(Units.from(unit,vs));
    red.setMaxVerticalSpeed(max_vs);
    //amber.setMaxVerticalSpeed(max_vs);
    reset();
  }

  public double getMaxVerticalSpeed(String unit) {
    return Units.to(unit, red.getMaxVerticalSpeed());
  }

  public void setTrackTolerance(double trk, String unit) {
    if (trk >= 0) {
      red.setTrackTolerance(Units.from(unit, trk));
      needCompute = true;
      red.clear();
    }
  }
  public double getTrackTolerance(String unit) {
    return Units.to(unit, red.getTrackTolerance());
  }

  public void setGroundSpeedTolerance(double gs, String unit) {
    if (gs >= 0) {
      red.setGroundSpeedTolerance(Units.from(unit, gs));
      needCompute = true;
      red.clear();
    }
  }
  public double getGroundSpeedTolerance(String unit) {
    return Units.to(unit, red.getGroundSpeedTolerance());
  }

  public void setVerticalSpeedTolerance(double vs, String unit) {
    if (vs >= 0) {
      red.setVerticalSpeedTolerance(Units.from(unit, vs));
      needCompute = true;
      red.clear();
    }
  }
  public double getVerticalSpeedTolerance(String unit) {
    return Units.to(unit, red.getVerticalSpeedTolerance());
  }


  //  SPECIAL: in internal units
  public void setOwnship(Position s, Velocity v, double time, String tunit) {
//    f.pln("IntentBands.setOwnship "+s+" "+v+" "+time+" "+tunit);
    reset();
    so = s;
    vo = v;
    to = Units.from(tunit, time);
    ownship = true;
  }


   public void setOwnshipLL(double lat, double lon, String hpunit, double alt, String vpunit, double trk, String trkunit, double gs, String gsunit, double vs, String vsunit, double time, String tunit) {
    setOwnship(new Position(LatLonAlt.mk(Units.from(hpunit, lat),  Units.from(hpunit, lon), Units.from(vpunit, alt))), 
        Velocity.makeTrkGsVs(trk, trkunit, gs, gsunit, vs, vsunit), time, tunit);
  }

  public void setOwnshipXYZ(double sx, double sy, String hpunit, double sz, String vpunit, double vx, double vy, String hvunit, double vz, String vvunit, double time, String tunit) {
    setOwnship(new Position(new Vect3(Units.from(hpunit, sx), Units.from(hpunit, sy), Units.from(vpunit, sz))),
        Velocity.makeVxyz(vx, vy, hvunit, vz, vvunit), time, tunit);
  }

  public void clear() {
    reset();
    ownship = false;
  }

  protected void reset() {
    red.clear();
    needCompute = true;
  }

  public int trackLength() {
    if (needCompute) {
      recompute();
    }

    return trackArray.size();
  }
  public Interval track(int i, String unit) {
    return track(i, Units.getFactor(unit));
  }
//  public Interval track(int i) {
//    return track(i, "deg");
//  }
  private Interval track(int i, double unit) {
    if (needCompute) {
      recompute();
    }
    if ( i >= trackArray.size()) {
      i = trackArray.size();
    }

    double loval = trackArray.get(i).low;
    double upval = trackArray.get(i).up;
    if (isLatLon()) {
      Velocity lo1 = Velocity.mkTrkGsVs(trackArray.get(i).low, vo.gs(), vo.vs());
      Velocity hi1 = Velocity.mkTrkGsVs(trackArray.get(i).up, vo.gs(), vo.vs());
      EuclideanProjection sp = Projection.createProjection(so.lla()); // this does NOT preserve altitudes (but those are not needed for the velocity projections)
      Vect3 so3 = sp.project(so);
      Velocity lo2 = sp.inverseVelocity(so3,lo1,true);
      Velocity hi2 = sp.inverseVelocity(so3,hi1,true);
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
    if (needCompute) {
      recompute();
    }
    if ( i >= trackArray.size()) {
      i = trackArray.size();
    }

    return trackRegionArray.get(i);
  }
  public BandsRegion regionOfTrack(double trk, String unit) {
    trk = Units.from(unit, trk);
    if (isLatLon()) {
      EuclideanProjection sp = Projection.createProjection(so.lla()); // this does NOT preserve altitudes (but those are not needed for the velocity projections)
      Velocity v1 = Velocity.mkTrkGsVs(trk, vo.gs(), vo.vs());
      Velocity v2 = sp.projectVelocity(so.lla(), v1);
      trk = v2.compassAngle();
    }
    if (red.trackBands(trk)) return BandsRegion.NEAR;
    return BandsRegion.NONE;
  }



  public int groundSpeedLength() {
    if (needCompute) {
      recompute();
    }

    return groundArray.size();
  }
  public Interval groundSpeed(int i, String unit) {
    return groundSpeed(i, Units.getFactor(unit));
  }
//  public Interval groundSpeed(int i) {
//    return groundSpeed(i, "kn");
//  }
  private Interval groundSpeed(int i, double unit) {
    if (needCompute) {
      recompute();
    }
    if ( i >= groundArray.size()) {
      i = groundArray.size();
    }

    if (isLatLon()) {
      Velocity lo1 = Velocity.mkTrkGsVs(vo.trk(), groundArray.get(i).low, vo.vs());
      Velocity hi1 = Velocity.mkTrkGsVs(vo.trk(), groundArray.get(i).up, vo.vs());
      EuclideanProjection sp = Projection.createProjection(so.lla()); // this does NOT preserve altitudes (but those are not needed for the velocity projections)
      Vect3 so3 = sp.project(so);
      Velocity lo2 = sp.inverseVelocity(so3,lo1,true);
      Velocity hi2 = sp.inverseVelocity(so3,hi1,true);
      return new Interval(Units.to(unit, lo2.gs()), Units.to(unit, hi2.gs()));
    } else {
      return new Interval(Units.to(unit, groundArray.get(i).low), Units.to(unit, groundArray.get(i).up));
    }
  }
  public BandsRegion groundSpeedRegion(int i) {
    if (needCompute) {
      recompute();
    }
    if ( i >= groundArray.size()) {
      i = groundArray.size();
    }

    return groundRegionArray.get(i);
  }
  public BandsRegion regionOfGroundSpeed(double gs, String unit) {
    gs = Units.from(unit, gs);
    if (gs < 0.0 || gs > red.getMaxGroundSpeed()) return BandsRegion.NEAR;
    if (isLatLon()) {
      EuclideanProjection sp = Projection.createProjection(so.lla()); // this does NOT preserve altitudes (but those are not needed for the velocity projections)
      Velocity v1 = Velocity.mkTrkGsVs(vo.trk(), gs, vo.vs());
      Velocity v2 = sp.projectVelocity(so.lla(), v1);
      gs = v2.gs();
    }
    if (red.groundSpeedBands(gs)) return BandsRegion.NEAR;
    return BandsRegion.NONE;
  }


  public int verticalSpeedLength() {
    if (needCompute) {
      recompute();
    }

    return verticalArray.size();
  }
  public Interval verticalSpeed(int i, String unit) {
    return verticalSpeed(i, Units.getFactor(unit));
  }
//  public Interval verticalSpeed(int i) {
//    return verticalSpeed(i, "fpm");
//  }
  private Interval verticalSpeed(int i, double unit) {
    if (needCompute) {
      recompute();
    }
    if ( i >= verticalArray.size()) {
      i = verticalArray.size();
    }

    return new Interval(Units.to(unit, verticalArray.get(i).low), Units.to(unit, verticalArray.get(i).up));
  }
  public BandsRegion verticalSpeedRegion(int i) {
    if (needCompute) {
      recompute();
    }
    if ( i >= verticalArray.size()) {
      i = verticalArray.size();
    }

    return verticalRegionArray.get(i);
  }
  public BandsRegion regionOfVerticalSpeed(double vs, String unit) {
    vs = Units.from(unit, vs);
    if (vs < -red.getMaxVerticalSpeed() || vs > red.getMaxVerticalSpeed()) return BandsRegion.NEAR;
    if (red.verticalSpeedBands(vs)) return BandsRegion.NEAR;
    return BandsRegion.NONE;
  }




  //
  // Internal methods...
  //

  /*private*/ void recompute() {
    trackCompute();
    groundCompute();
    verticalCompute();
    red.clearBreaks();

    needCompute = false;
  }



  /** 
   * Provide a copy of the track angle bands.  The angles are in
   * 'compass' angles: 0 to 360 degrees, counter-clockwise from true
   * north. 
   */
  /*private*/ void trackCompute() {
    IntervalSet trk_red = red.trackBands(); 
    //IntervalSet trk_amber = amber.trackBands();
    IntervalSet trk_green = new IntervalSet();

    trk_green.union(new Interval(0.0, 2*Math.PI));
    trk_green.diff(trk_red);
    trk_green.sweepSingle(); //only eliminate actual singles

    int len = trk_green.size()+trk_red.size();
    trackArray = new ArrayList<Interval>(len);
    trackRegionArray = new ArrayList<BandsRegion>(len);

    toArrays(trackArray, trackRegionArray, trk_red, trk_green);
  }

  /** Provide a copy of the ground speed bands [knots].  The range of
	ground speed bands is from 0 to max_gs. */
  /*private*/ void groundCompute() {
    IntervalSet gs_red = red.groundSpeedBands(); 
    IntervalSet gs_green = new IntervalSet();

    gs_green.union(new Interval(0.0, red.getMaxGroundSpeed()));
    gs_green.diff(gs_red); 
    gs_green.sweepSingle(); //only eliminate actual singles

    int len = gs_green.size()+gs_red.size();
    groundArray = new ArrayList<Interval>(len);
    groundRegionArray = new ArrayList<BandsRegion>(len);

    toArrays(groundArray, groundRegionArray, gs_red, gs_green);
  }

  /** Provide a copy of the vertical speed bands [feet/min].  The
	range of vertical speeds is -max_vs to max_vs. */
  /*private*/ void verticalCompute() {
    IntervalSet vs_red = red.verticalSpeedBands(); 
    IntervalSet vs_green = new IntervalSet();

    vs_green.union(new Interval(-red.getMaxVerticalSpeed(), red.getMaxVerticalSpeed()));
    vs_green.diff(vs_red);
    vs_green.sweepSingle(); //only eliminate actual singles

    int len = vs_green.size()+vs_red.size();
    verticalArray = new ArrayList<Interval>(len);
    verticalRegionArray = new ArrayList<BandsRegion>(len);

    toArrays(verticalArray, verticalRegionArray, vs_red, vs_green);
  }

  /** Provide a copy of the vertical speed bands [feet/min].  The
	range of vertical speeds is -max_vs to max_vs. */
  private void toArrays(ArrayList<Interval> interval, ArrayList<BandsRegion> intRegion, 
      IntervalSet red, IntervalSet green) {
    int i;

    for (Interval n : green) {
      i = order(interval, n);
      interval.add(i, n);
      intRegion.add(i, BandsRegion.NONE);

    }
    for (Interval n : red) {
      i = order(interval, n);
      interval.add(i, n);
      intRegion.add(i, BandsRegion.NEAR);
    }
  }


  /* if n is empty, then this method returns an undefined value */
  private int order(ArrayList<Interval> arr, Interval n) {
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

  public String toString() {
    String str = red.toString()+"\nownship = "+so+" "+vo+" "+to+" "+ownship+"\n";
    for (int i = 0; i < trackLength(); i++) {
      str += "T"+i+":\t"+track(i, "deg")+"\t"+trackRegion(i)+"\n";
    }
    for (int i = 0; i < groundSpeedLength(); i++) {
      str += "G"+i+":\t"+groundSpeed(i, "kn")+"\t"+groundSpeedRegion(i)+"\n";
    }
    for (int i = 0; i < verticalSpeedLength(); i++) {
      str += "V"+i+":\t"+verticalSpeed(i, "fpm")+"\t"+verticalSpeedRegion(i)+"\n";
    }
    return str;
  }

  /** Return true if track bands consist of a solid region of the given color  */
  public boolean solidTrackBand(BandsRegion br) {
    boolean solid = trackLength() > 0;
    for (int i = 0; solid && i < trackLength(); i++) {
      solid = trackRegion(i) == br;
    }
    return solid;
  }

  /** Return true if ground speed bands consist of a region band of the given color  */
  public boolean solidGroundSpeedBand(BandsRegion br) {
    boolean solid = groundSpeedLength() > 0;
    for (int i = 0; solid && i < groundSpeedLength(); i++) {
      solid = groundSpeedRegion(i) == br;
    }
    return solid;
  }

  /** Return true if vertical speed bands consist of a region band of the given color  */
  public boolean solidVerticalSpeedBand(BandsRegion br) {
    boolean solid = verticalSpeedLength() > 0;
    for (int i = 0; solid && i < verticalSpeedLength(); i++) {
      solid = verticalSpeedRegion(i) == br;
    }
    return solid;
  }

  /** Return true if track bands exist but don't contain a region of the given color  */
  public boolean noTrackBand(BandsRegion br) {
    boolean noband = trackLength() > 0;
    for (int i = 0; noband && i < trackLength(); i++) {
      noband = trackRegion(i) != br;
    }
    return noband;
  }

  /** Return true if ground speed bands exist but don't contain a region of the given color  */
  public boolean noGroundSpeedBand(BandsRegion br) {
    boolean noband = groundSpeedLength() > 0;
    for (int i = 0; noband && i < groundSpeedLength(); i++) {
      noband = groundSpeedRegion(i) != br;
    }
    return noband;
  }

  /** Return true if vertical speed bands exist but don't contain a region of the given color  */
  public boolean noVerticalSpeedBand(BandsRegion br) {
    boolean noband = verticalSpeedLength() > 0;
    for (int i = 0; noband && i < verticalSpeedLength(); i++) {
      noband = verticalSpeedRegion(i) != br;
    }
    return noband;
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
