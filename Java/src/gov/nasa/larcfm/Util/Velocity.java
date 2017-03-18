/*
 * Velocity.java 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * 
 * NOTES: 
 * Track is True North/clockwise
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.List;
import java.util.ArrayList;

/**
 * This class represents a 3-dimensional velocity vector.  The 
 * track angle is defined by the "true north, clockwise convention."
 *
 */
public final class Velocity extends Vect3 implements OutputList {

  /**
   * Instantiates a new velocity in internal units.
   * 
   * @param vx the vx
   * @param vy the vy
   * @param vz the vz
   */
  private Velocity(double vx, double vy, double vz) {
    super(vx,vy,vz);
  } 

  /** A zero velocity */
  public final static Velocity ZERO = new Velocity(0.0,0.0,0.0);

  /** An invalid velocity.  Note that this is not necessarily equal to other invalid velocities -- use the isInvalid() test instead. */
  public final static Velocity INVALID = new Velocity(Double.NaN,Double.NaN,Double.NaN);



  /**
   * New velocity from Vect3.
   * 
   * @param v the 3-D velocity vector (in internal units).
   * 
   * @return the velocity
   */
  public static Velocity make(Vect3 v) {
    return new Velocity(v.x,v.y,v.z);
  }     

  /**
   * New velocity from Vect2 (setting the vertical speed to 0.0).
   * 
   * @param v the 2-D velocity vector (in internal units).
   * 
   * @return the velocity
   */
  public static Velocity make(Vect2 v) {
    return new Velocity(v.x,v.y,0.0);
  }     

  /**
   * New velocity from Euclidean coordinates in internal units.
   * 
   * @param vx the x-velocity [internal]
   * @param vy the y-velocity [internal]
   * @param vz the z-velocity [internal]
   * 
   * @return the velocity
   * 
   */
  public static Velocity mkVxyz(double vx, double vy, double vz) {
    return new Velocity(vx,vy,vz);
  }     

  /**
   * New velocity from Euclidean coordinates in "conventional" units.
   *
   * @param vx the x-velocity [knot]
   * @param vy the y-velocity [knot]
   * @param vz the z-velocity [fpm]
   *
   * @return the velocity
   */
  public static Velocity makeVxyz(double vx, double vy, double vz) {
    return new Velocity(Units.from("knot",vx),Units.from("knot",vy),Units.from("fpm",vz));
  }     



  /**
   * New velocity from Euclidean coordinates in explicit units.
   * 
   * @param vx the x-velocity [uvxy]
   * @param vy the y-velocity [uvzy]
   * @param uvxy the units of vx and vy
   * @param vz the z-velocity [uvz]
   * @param uvz the units of vz
   * 
   * @return the velocity
   */
  public static Velocity makeVxyz(
      double vx, double vy, String uvxy,
      double vz, String uvz) {
    return new Velocity(Units.from(uvxy,vx), Units.from(uvxy, vy), Units.from(uvz,vz));
  }     

  /**
   * New velocity from Track, Ground Speed, and Vertical speed in internal units.
   * Note that this uses trigonometric functions, and may introduce numeric instability.
   * 
   * @param trk the track angle [internal]
   * @param gs the ground speed [internal]
   * @param vs the vs [internal]
   * 
   * @return the velocity
   */
  public static Velocity mkTrkGsVs(double trk, double gs, double vs) {
    return new Velocity(trkgs2vx(trk,gs), trkgs2vy(trk,gs), vs);
  }


  /**
   * New velocity from Track, Ground Speed, and Vertical speed in explicit units.
   * Note that this uses trigonometric functions, and may introduce numeric instability.
   * 
   * @param trk the track angle [deg]
   * @param gs the ground speed [knot]
   * @param vs the vertical speed [fpm]
   * 
   * @return the velocity
   */
  public static Velocity makeTrkGsVs(double trk,double gs, double vs) {
    return mkTrkGsVs(Units.from("deg",trk), Units.from("knot",gs), Units.from("fpm",vs));
  }


  /**
   * New velocity from Track, Ground Speed, and Vertical speed in explicit units.
   * Note that this uses trigonometric functions, and may introduce numeric instability.
   * 
   * @param trk the track angle [utrk]
   * @param utrk the units of trk
   * @param gs the ground speed [ugs]
   * @param ugs the units of gs
   * @param vs the vertical speed [uvs]
   * @param uvs the units of vs
   * 
   * @return the velocity
   */
  public static Velocity makeTrkGsVs(
      double trk, String utrk,
      double gs, String ugs,
      double vs, String uvs) {
    return mkTrkGsVs(Units.from(utrk,trk), Units.from(ugs,gs), Units.from(uvs,vs));
  }

  /**
   * Return the velocity along the line from p1 to p2 at the given speed
   * @param p1 first point
   * @param p2 second point
   * @param speed speed [internal units] (composite 3 dimensional speed, Not ground or vertical speed!)
   * @return the velocity
   */
  public static Velocity mkVel(Vect3 p1, Vect3 p2, double speed) {
    return make(p2.Sub(p1).Hat().Scal(speed));
  }

  /**
   * Return the velocity if moving from p1 to p2 over the given time
   * @param p1 first point
   * @param p2 second point
   * @param dt time 
   * @return the velocity
   */
  public static Velocity genVel(Vect3 p1, Vect3 p2, double dt) {
    return make(p2.Sub(p1).Scal(1/dt));
  }



  public static Velocity makePerp(Velocity v, int dir) {
    if (dir >= 0) return Velocity.make(v.PerpR());
    else return Velocity.make(v.PerpL());
  }

  /**
   * New velocity from existing velocity by adding the given track angle to this 
   * vector's track angle.  Essentially, this rotates the vector, a positive
   * angle means a clockwise rotation.
   * @param trk track angle [rad]
   * @return new velocity
   */
  public Velocity mkAddTrk(double trk) {
    double s = Math.sin(trk);
    double c = Math.cos(trk);
    return mkVxyz(x*c+y*s, -x*s+y*c, z);
  }

  /**
   * New velocity from existing velocity, changing only the track
   * @param trk track angle [rad]
   * @return new velocity
   */
  public Velocity mkTrk(double trk) {
    return mkTrkGsVs(trk, gs(), vs());
  }

  /**
   * New velocity from existing velocity, changing only the track
   * @param trk track angle [u]
   * @param u  units
   * @return new velocity
   */
  public Velocity mkTrk(double trk, String u) {
    return mkTrk(Units.from(u,trk));
  }

  /**
   * New velocity from existing velocity, changing only the ground speed
   * @param gs ground speed [m/s]
   * @return new velocity
   */
  public Velocity mkGs(double gs) {
    if (gs < 0) return INVALID;
    //		return mkTrkGsVs(trk(), gs, vs());    // optimzation due to Aaron Dutle
    double gs0 = gs();
    if (gs0 > 0.0) {
      double scal = gs/gs0;
      return mkVxyz(x*scal, y*scal, vs());
    } else { // old gs was 0.0, reset track to 0
      return mkVxyz(0,gs,vs());
    }
  }

  /**
   * New velocity from existing velocity, changing only the ground speed
   * @param gs ground speed [u]
   * @param u  units
   * @return new velocity
   */
  public Velocity mkGs(double gs, String u) {
    return mkGs(Units.from(u, gs));
  }

  /**
   * New velocity from existing velocity, changing only the vertical speed
   * @param vs vertical speed [m/s]
   * @return new velocity
   */
  public Velocity mkVs(double vs) {
    return mkVxyz(x, y, vs);
  }

  /**
   * New velocity from existing velocity, changing only the vertical speed
   * @param vs vertical speed [u]
   * @param u  units
   * @return new velocity
   */
  public Velocity mkVs(double vs, String u) {
    return mkVs(Units.from(u,vs));
  }

  /**
   * Returns a unit velocity vector in the direction of the original velocity
   */
  public Velocity Hat() {
    // This method means:
    //    return make(this.Hat());
    // but for efficiency, I am implementing it explicitly
    double n = norm();
    if ( n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
      return this;
    }
    return mkVxyz(x / n, y / n, z / n);
  }

  /**
   * Returns a unit velocity vector in the direction of the original velocity in the XY plane
   */
  public Velocity Hat2D() {
	  return mkVs(0).Hat();
  }
  
  public Velocity Neg() {
    return mkVxyz(-x, -y, -z);
  }

  public Velocity Add(Velocity v) {
    return new Velocity(x+v.x, y+v.y, z+v.z);
  }

  public Velocity Sub(Velocity v) {
    return new Velocity(x-v.x, y-v.y, z-v.z);
  }

  /**
   * If the z component of the velocity vector is smaller than the threshold, return a new vector with this component set to 0.  
   * Return the original vector if the vertical speed is greater than the threshold.
   * @param threshold   level of vertical speed below which the vector is altered
   * @return the new velocity
   */
  public Velocity zeroSmallVs(double threshold) {
    if (Math.abs(z) < threshold) return mkVxyz(x,y,0.0);
    return this;
  }

  /**
   * Angle in radians in the range [-<code>Math.PI</code>, <code>Math.PI</code>].
   * Convention is counter-clockwise with respect to east.
   * 
   * @return the track angle [rad]
   */
  public double angle() {
    return vect2().angle();
  }

  /**
   * Angle in explicit units in corresponding range [-<code>Math.PI</code>, <code>Math.PI</code>].
   * Convention is counter-clockwise with respect to east.
   * 
   * @param uangle the explicit units of track angle
   * 
   * @return the track angle [rad]
   */
  public double angle(String uangle) {
    return Units.to(uangle,angle());
  }

  /**
   * Track angle in radians in the range [-<code>Math.PI</code>, <code>Math.PI</code>].
   * Convention is clockwise with respect to north.
   * 
   * @return the track angle [rad]
   */
  public double trk() {
    return vect2().trk();
  }

  /**
   * Track angle in explicit units in the corresponding range [-<code>Math.PI</code>, <code>Math.PI</code>]. 
   * Convention is clockwise with respect to north.
   * 
   * @param utrk the explicit units of track angle
   * 
   * @return the track angle [utrk]
   */
  public double track(String utrk) {
    return Units.to(utrk,trk());
  }

  /**
   * Compass angle in radians in the range [<code>0</code>, <code>2*Math.PI</code>).
   * Convention is clockwise with respect to north.
   * 
   * @return the compass angle [rad]
   */
  public double compassAngle() {
    return vect2().compassAngle();
  }

  /**
   * Compass angle in explicit units in corresponding range [<code>0</code>, <code>2*Math.PI</code>).
   * Convention is clockwise with respect to north.
   *  
   *  @param u the explicit units of compass angle
   *  
   *  @return the compass angle [u]
   */
  public double compassAngle(String u) {
    return Units.to(u,compassAngle());
  }

  /**
   * Ground speed in internal units.
   * 
   * @return the ground speed
   */
  public double gs() {
    return Util.sqrt_safe(x*x+y*y); //vect2().norm();
  }

  /**
   * Ground speed in explicit units.
   * 
   * @param ugs the explicit units of ground speed
   * 
   * @return the ground speed [ugs]
   */
  public double groundSpeed(String ugs) {
    return Units.to(ugs,gs()); 
  }

  /**
   * Vertical speed in internal units.
   * 
   * @return the vertical speed
   */
  public double vs() {
    return z;
  }

  /**
   * Vertical speed in explicit units.
   * 
   * @param uvs the explicit units of vertical speed
   * 
   * @return the vertical speed [uvs]
   */
  public double verticalSpeed(String uvs) {
    return Units.to(uvs,z);
  }

  /** 
   * Compare Velocities: return true iff delta is within specified limits 
   * 
   * @param v       the other velocity
   * @param maxTrk
   * @param maxGs
   * @param maxVs
   * @return true, if the velocities compare correctly
   */
  public boolean compare(Velocity v, double maxTrk, double maxGs, double maxVs) {
    if (Util.turnDelta(v.trk(),trk()) > maxTrk) return false;
    if (Math.abs(v.gs() - gs()) > maxGs) return false;
    if (Math.abs(v.vs() - vs()) > maxVs) return false;
    return true;
  }

  /**
   * Compare two velocities based on horizontal and vertical components.  This could be used against a set of nacV ADS-B limits, for example.
   * @param v other Velocity
   * @param horizDelta horizontal tolerance (absolute value)
   * @param vertDelta vertical tolerance (absolute value)
   * @return true if the velocities are within both horizontal and vertical tolerances of each other.
   */
  public boolean compare(Velocity v, double horizDelta, double vertDelta) {
    return Math.abs(z-v.z) <= vertDelta && vect2().Sub(v.vect2()).norm() <= horizDelta;
  }


  // Utilities

  /** Return the x component of velocity given the track and ground
   * speed.  The track angle is assumed to use the radians from true
   * North-clockwise convention.
   * 
   * @param trk
   * @param gs
   * @return x component of velocity
   */
  public static double trkgs2vx(double trk, double gs) {
    return gs * Math.sin(trk);
  }

  /** Return the y component of velocity given the track and ground
   *	speed.  The track angle is assumed to use the radians from
   *	true North-clockwise convention. 
   * 
   * @param trk
   * @param gs
   * @return y component of velocity
   */
  public static double trkgs2vy(double trk, double gs) {
    return gs * Math.cos(trk);
  }

  /** Return the 2-dimensional Euclidean vector for velocity given the track and ground
   *	speed.  The track angle is assumed to use the radians from
   *	true North-clockwise convention. 
   * 
   * @param trk
   * @param gs
   * @return 2-D velocity
   */
  public static Vect2 trkgs2v(double trk, double gs) {
    return new Vect2(trkgs2vx(trk,gs), trkgs2vy(trk,gs));
  }


  /** String representation of the velocity in polar coordinates (compass angle and groundspeed) */
  public String toString() {
    return toString(Constants.get_output_precision());
  }

  /** String representation of the velocity in polar coordinates (compass angle and groundspeed) in [deg, knot, fpm].  This 
   * method does not output units. 
   * @param prec precision (0-15)
   */
  public String toString(int prec) {
    return "("+Units.str("deg",compassAngle(),prec)+", "+Units.str("knot",gs(),prec)+", "+Units.str("fpm",vs(),prec)+")";
  }

  /** String representation of the velocity in polar coordinates (compass angle and groundspeed) 
   * @return a string representation
   * */
  public String toStringUnits() {
    return toStringUnits("deg","knot","fpm");
  }

  /** String representation (trk,gs,vs) with the given units 
   * 
   * @param trkUnits
   * @param gsUnits
   * @param vsUnits
   * @return a string representation
   */
  public String toStringUnits(String trkUnits, String gsUnits, String vsUnits) {
    return "("+Units.str(trkUnits,compassAngle())+", "+ Units.str(gsUnits,gs())+", "+ Units.str(vsUnits,vs())+")";
  }

  /** String representation (trk,gs,vs) with the given units 
   * 
   * @param trkUnits
   * @param gsUnits
   * @param vsUnits
   * @param prec
   * @return a string representation
   */
  public String toStringUnitsNP(String trkUnits, String gsUnits, String vsUnits, int prec) {
    return Units.str(trkUnits,compassAngle(), prec)+", "+ Units.str(gsUnits,gs(), prec)+", "+ Units.str(vsUnits,vs(),prec);
  }

  /** String representation, default number of decimal places, without parentheses 
   * @return a string representation
   * */
  public String toStringNP() {
    return toStringNP(Constants.get_output_precision());
  }

  /**
   * String representation, with user-specified precision
   * @param precision number of decimal places (0-15)
   * @return a string representation
   */
  public String toStringNP(int precision) {
    return f.FmPrecision(Units.to("deg", compassAngle()), precision)+", "+f.FmPrecision(Units.to("knot", gs()), precision)+", "+f.FmPrecision(Units.to("fpm", vs()), precision);	
  }
  
  /**
   * String representation, with user-specified precision
   * @param precision number of decimal places (0-15)
   * @param utrk units of track
   * @param ugs units of ground speed
   * @param uvs units of vertical speed
   * @return a string representation
   */
  public String toStringNP(String utrk, String ugs, String uvs, int precision) {
    return f.FmPrecision(Units.to(utrk, compassAngle()), precision)+", "+f.FmPrecision(Units.to(ugs, gs()), precision)+", "+f.FmPrecision(Units.to(uvs, vs()), precision);	
  }

  /**
   * Euclidean vector representation to arbitrary precision, in [knot,knot,fpm]
   * @param prec precision (0-15)
   * @return a string representation
   */
  public String toXYZ(int prec) {
	    return "("+f.FmPrecision(Units.to("knot", x),prec)+", "+f.FmPrecision(Units.to("knot", y),prec)+", "+f.FmPrecision(Units.to("fpm", z),prec)+")";
	  }

  public String toXYZ(String xu, String yu, String zu, int prec) {
	    return "("+f.FmPrecision(Units.to(xu, x),prec)+", "+f.FmPrecision(Units.to(yu, y),prec)+", "+f.FmPrecision(Units.to(zu, z),prec)+")";
	  }

  /**
   * Euclidean vector representation to arbitrary precision, in [knot,knot,fpm]
   * @return a string representation
   */
  public String toStringXYZ() {
    return toXYZ(Constants.get_output_precision());
  }

  /**
   * Euclidean vector representation to arbitrary precision, in [knot,knot,fpm]
   * @return a string representation
   */
  public String toStringXYZUnits() {
    return "("+Units.str("knot", x)+", "+Units.str("knot", y)+", "+Units.str("fpm", z)+")";
  }

  /**
   * Return an array of string representing each value of the velocity in the units deg, knot, fpm.
   * @return array of strings
   */
  public List<String> toStringList() {
    ArrayList<String> ret = new ArrayList<String>(3);
    if (isInvalid()) {
      ret.add("-");
      ret.add("-");
      ret.add("-");
    } else {
      ret.add(Double.toString(Units.to("deg", compassAngle())));
      ret.add(Double.toString(Units.to("knot", gs())));
      ret.add(Double.toString(Units.to("fpm", vs())));
    }
    return ret;
  }

  /**
   * Return an array of string representing each value of the velocity in the units deg, knot, fpm.
   * @param precision the number of digits to display
   * @return array of strings
   */
  public List<String> toStringList(int precision) {
    ArrayList<String> ret = new ArrayList<String>(3);
    if (isInvalid()) {
      ret.add("-");
      ret.add("-");
      ret.add("-");
    } else {
      ret.add(f.FmPrecision(Units.to("deg", compassAngle()),precision));
      ret.add(f.FmPrecision(Units.to("knot", gs()),precision));
      ret.add(f.FmPrecision(Units.to("fpm", vs()),precision));
    }
    return ret;
  }

  /**
   * Return an array of string representing each value of the velocity in terms of its Cartesian dimensions in units knot, knot, fpm.
   * @return array of strings
   */
  public List<String> toStringXYZList() {
    ArrayList<String> ret = new ArrayList<String>(3);
    if (isInvalid()) {
      ret.add("-");
      ret.add("-");
      ret.add("-");

    } else {
      ret.add(Double.toString(Units.to("knot", x())));
      ret.add(Double.toString(Units.to("knot", y())));
      ret.add(Double.toString(Units.to("fpm", z())));
    }
    return ret;
  }

  /**
   * Return an array of string representing each value of the velocity in terms of its Cartesian dimensions in units knot, knot, fpm.
   * 
   * @param precision the number of digits to display
   * @return array of strings
   */
  public List<String> toStringXYZList(int precision) {
    ArrayList<String> ret = new ArrayList<String>(3);
    if (isInvalid()) {
      ret.add("-");
      ret.add("-");
      ret.add("-");
    } else {
      ret.add(f.FmPrecision(Units.to("knot", x()),precision));
      ret.add(f.FmPrecision(Units.to("knot", y()),precision));
      ret.add(f.FmPrecision(Units.to("fpm", z()),precision));
    }
    return ret;
  }

  /** This parses a space or comma-separated string as a XYZ Velocity (an inverse to the toStringXYZ method).  If three bare values are present, then it is interpreted as internal units.
   * If there are 3 value/unit pairs then each values is interpreted wrt the appropriate unit.  If the string cannot be parsed, an INVALID value is
   * returned. 
   * 
   * @param str string to parse
   * @return Velocity object
   */
  public static Velocity parseXYZ(String str) {
    return Velocity.make(Vect3.parse(str));
  }

  /** This parses a space or comma-separated string as a Trk/Gs/Vs Velocity (an inverse to the toString method).  If three bare values are 
   * present, then it is interpreted as degrees/knots/fpm. If there are 3 value/unit pairs then each values is 
   * interpreted wrt the appropriate unit.  If the string cannot be parsed, an INVALID value is
   * returned. */
  public static Velocity parse(String str) {
    Vect3 v3 = Vect3.parse(str);
    String[] fields = str.split(Constants.wsPatternParens);
    if (fields.length == 3) {
      return Velocity.makeTrkGsVs(v3.x, v3.y, v3.z);
      //		} else if (fields.length == 6 && !fields[1].substring(0,3).equalsIgnoreCase("deg") && !fields[1].substring(0,3).equalsIgnoreCase("rad")) {
      //			return parseXYZ(str);
    }
    return Velocity.mkTrkGsVs(v3.x, v3.y, v3.z);
  }

}
