/*
 * Position.java 
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

/**
 * This class captures a single position represented in either
 * Euclidean or Lat/Lon coordinates.  This class is immutable.<p>
 * 
 * This class is designed to be resilient, The methods do not
 * return errors when a position in the "wrong" geometry is provided.  So 
 * the method x() will return a value even when the original
 * position was provided in LatLonAlt.  The correspondence is as follows:
 * <ul>
 * <li> latitude <-> Y
 * <li> longitude <-> X
 * <li> altitude <-> alt
 * </ul>
 *  
 */
public final class Position implements OutputList {
  private final LatLonAlt ll;
  private final Point s3;
  private final boolean latlon;

  /** Construct a new Position object from a LatLonAlt object. The position will be a Lat/Lon position. */
  public Position(LatLonAlt lla) {
    ll = lla;
    s3 = Point.mk(lla.lon(), lla.lat(), lla.alt());
    this.latlon = true;
  }

  private Position(double x, double y, double z) {
    s3 = Point.mk(x, y, z);
    ll = LatLonAlt.mk(y, x, z);
    this.latlon = false;
  }

  /** Construct a new Position object from a Vect3 object. This method
   * assumes the Vect3 is in internal units. 
   */
  public Position(Vect3 v) {
    s3 = Point.mk(v);
    ll = LatLonAlt.mk(v.y, v.x, v.z);
    this.latlon = false;
  }

  /** Construct a new Position object from a Point object.  The position will be a Euclidean position. */
  public Position(Point v) {
    s3 = v;
    ll = LatLonAlt.mk(v.y, v.x, v.z);
    this.latlon = false;
  }

  /** Copy this Position object */
  public Position copy() {
    if (latlon) return new Position(ll);
    else return new Position(s3);
  }

  /** A zero position in lat/lon */
  public static final Position ZERO_LL = new Position(LatLonAlt.ZERO);
  /** A zero position in Euclidean */
  public static final Position ZERO_XYZ = new Position(Point.mk(Vect3.Zero()));  
  /** An invalid position.  Note that this is not necessarily equal to other invalid positions -- use the isInvalid() test instead. */
  public static final Position INVALID = new Position(Point.mk(Vect3.INVALID));


  /**
   * Creates a new lat/lon position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
   * 
   * @param lat latitude [deg north latitude]
   * @param lon longitude [deg east longitude]
   * @param alt altitude [ft]
   */
  public static Position makeLatLonAlt(double lat, double lon, double alt) {
    return new Position(LatLonAlt.make(lat,lon,alt));
  }


  /**
   * Creates a new lat/lon position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
   * 
   * @param lat latitude [radians]
   * @param lon longitude [radians]
   * @param alt altitude [m]
   */
  public static Position mkLatLonAlt(double lat, double lon, double alt) {
    return new Position(LatLonAlt.mk(lat,lon,alt));
  }


  /**
   * Creates a new lat/lon position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
   * 
   * @param lat latitude [lat_unit north latitude]
   * @param lat_unit units of latitude
   * @param lon longitude [lon_unit east longitude]
   * @param lon_unit units of latitude
   * @param alt altitude [alt_unit]
   * @param alt_unit units of altitude
   */
  public static Position makeLatLonAlt(double lat, String lat_unit, double lon, String lon_unit, double alt, String alt_unit) {
    return new Position(LatLonAlt.make(lat, lat_unit, lon, lon_unit, alt, alt_unit));
  }

  /**
   * Creates a new Euclidean position with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
   * 
   * @param x coordinate [nmi]
   * @param y coordinate [nmi]
   * @param z altitude [ft]
   */
  public static Position makeXYZ(double x, double y, double z) {
    return new Position(Units.from(Units.NM, x), Units.from(Units.NM, y), Units.from(Units.ft,z));
  }


  /**
   * Creates a new Euclidean position with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
   * 
   * @param x coordinate [m]
   * @param y coordinate [m]
   * @param z altitude [m]
   */
  public static Position mkXYZ(double x, double y, double z) {
    return new Position(x,y,z);
  }

  /**
   * Creates a new Euclidean position with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
   * 
   * @param x coordinate [x_unit]
   * @param x_unit units of x coordinate
   * @param y coordinate [y_unit]
   * @param y_unit units of y coordinate
   * @param z altitude [z_unit]
   * @param z_unit units of z coordinate
   */
  public static Position makeXYZ(double x, double x_unit, double y, double y_unit, double z, double z_unit) {
    return new Position(Units.from(x_unit, x), Units.from(y_unit, y), Units.from(z_unit,z));
  }

  /**
   * Creates a new Euclidean position with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
   * 
   * @param x coordinate [x_unit]
   * @param x_unit units of x coordinate
   * @param y coordinate [y_unit]
   * @param y_unit units of y coordinate
   * @param z altitude [z_unit]
   * @param z_unit units of z coordinate
   */
  public static Position makeXYZ(double x, String x_unit, double y, String y_unit, double z, String z_unit) {
    return new Position(Units.from(x_unit, x), Units.from(y_unit, y), Units.from(z_unit,z));
  }


  //	public boolean equals(Object o) {
  //		if (!(o instanceof Position)) return false;
  //		Position v = (Position) o;
  //		return s3.equals(v.s3) && ll.equals(v.ll);
  //	}


  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + (latlon ? 1231 : 1237);
    result = prime * result + ((ll == null) ? 0 : ll.hashCode());
    result = prime * result + ((s3 == null) ? 0 : s3.hashCode());
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
    Position other = (Position) obj;
    if (latlon != other.latlon)
      return false;
    if (ll == null) {
      if (other.ll != null)
        return false;
    } else if (!ll.equals(other.ll))
      return false;
    if (s3 == null) {
      if (other.s3 != null)
        return false;
    } else if (!s3.equals(other.s3))
      return false;
    return true;
  }

  /**
   * Checks if two Positions are almost the same.
   * This is based on values in gov.nasa.larcfm.Constants
   * 
   * @param pp Position for comparison
   * 
   * @return <code>true</code>, if <code>this</code> Position is almost equal 
   * to <code>v</code>.
   */
  public boolean almostEquals(Position pp) {
    if (latlon) {
      return lla().almostEquals(pp.lla());
    } else
      return Constants.almost_equals_xy(s3.x,s3.y,pp.s3.x,pp.s3.y) 
          && Constants.almost_equals_alt(s3.z,pp.s3.z);
  }


  /**
   * Checks if two Positions are almost the same.
   * 
   * @param pp Position for comparison
   * @param epsilon_horiz allowable horizontal deviation [m] 
   * @param epsilon_vert allowable vertical deviation [m] 
   * 
   * @return <code>true</code>, if <code>this</code> Position is almost equal 
   * to <code>v</code>.
   */
  public boolean almostEquals(Position pp, double epsilon_horiz, double epsilon_vert) {
    if (latlon) {
      return lla().almostEquals(pp.lla(),epsilon_horiz,epsilon_vert);
    } else {
      return s3.within_epsilon(pp.point(),epsilon_vert);
    }
  }

  /** Return the horizontal position as a standard vect2().  This returns either (x,y), or, equivalently, (lon, lat). */
  public Vect2 vect2() {
    return new Vect2(s3.x,s3.y);
  }

  /** Return the three dimensional position vector.  This returns either (x,y,z), or, equivalently, (lon,lat,alt). */
  public Point point() {
    return s3;
  }

  /** Return the associated LatLonAlt object */
  public LatLonAlt lla() {
    return ll;
  }

  /** Return the x coordinate */
  public double x() {
    return s3.x;
  }

  /** Return the y coordinate */
  public double y() {
    return s3.y;
  }

  /** Return the z coordinate */
  public double z() {
    return s3.z;
  }

  /** Return the latitude */
  public double lat() {
    return ll.lat();
  }

  /** Return the longitude */
  public double lon() {
    return ll.lon();
  }

  /** Return the altitude (internal units) */
  public double alt() {
    return ll.alt();
  }

  /** Return the latitude in degrees north */
  public double latitude() {
    return ll.latitude();
  }

  /** Return the longitude in degrees east */
  public double longitude() {
    return ll.longitude();
  }

  /** Return the altitude in feet */
  public double altitude() {
    return ll.altitude();
  }

  /** Return the x coordinate in [NM] */ 
  public double xCoordinate() {
    return Units.to(Units.NM, s3.x);
  }

  /** Return the y coordinate in [NM] */
  public double yCoordinate() {
    return Units.to(Units.NM, s3.y);
  }

  /** Return the z coordinate in [ft] */
  public double zCoordinate() {
    return Units.to(Units.ft, s3.z);
  }


  /** Return if this Position is a latitude or longitude */
  public boolean isLatLon() {
    return latlon;
  }

  /** Returns true if this Position is invalid */
  public boolean isInvalid() {
    return s3.isInvalid() || ll.isInvalid();
  }

  /** Make a new Position from the current one with the X coordinate changed */
  public Position mkX(double x) {
    if (latlon) {
      return new Position(LatLonAlt.mk(ll.lat(), x, ll.alt()));
    } else {
      return new Position(x, s3.y, s3.z);
    }
  }

  /** Make a new Position from the current one with the longitude changed */
  public Position mkLon(double lon) {
    return mkX(lon);
  }

  /** Make a new Position from the current one with the Y coordinate changed */
  public Position mkY(double y) {
    if (latlon) {
      return new Position(LatLonAlt.mk(y, ll.lon(), ll.alt()));
    } else {
      return new Position(s3.x, y, s3.z);
    }
  }

  /** Make a new Position from the current one with the latitude changed */
  public Position mkLat(double lat) {
    return mkY(lat);
  }

  /** Make a new Position from the current one with the Z coordinate changed */
  public Position mkZ(double z) {
    if (latlon) {
      return new Position(LatLonAlt.mk(ll.lat(), ll.lon(), z));
    } else {
      return new Position(s3.x, s3.y, z);
    }
  }

  /** Make a new Position from the current one with the altitude changed (internal units) */
  public Position mkAlt(double alt) {
    return mkZ(alt);
  }

  /** Make a new Position from the current one with an altitude of zero */
  public Position zeroAlt() {
    return mkZ(0);
  }

  /** Return the horizontal distance between the current Position and the given Position */
  public double distanceH(Position p) {
    if (latlon) {
      return GreatCircle.distance(ll,p.ll);
    } else {
      return s3.vect2().Sub(p.vect2()).norm(); 
    }
  }
  
  /** Return the curved horizontal distance between the current Position and the given Position
   * 
   * @param p position 2
   * @param center the center of rotation
   * @param radius the radius of curvature
   * @return the curved horizontal distance
   */
  public double distanceH(Position p, Position center, double radius) {
	  double theta = PositionUtil.angle_between(this,center,p);
	  return Math.abs(theta*radius);	    	
  }

  /** Return the vertical distance between the current Position and the given Position. 
   */
  public double distanceV(Position p) {
    return Math.abs(s3.z - p.s3.z);
  }

  /** Return the vertical distance between the current Position and the given Position. Positive values mean 
   * the current Position is above the given Position */
  public double signedDistanceV(Position p) {
    return s3.z - p.s3.z;
  }

  /** 
   * Perform a linear projection of the current Position with given velocity and time.  
   * If isLatLon() is true, then a great circle route is followed and the velocity 
   * represents the initial velocity along the great circle.
   * 
   * Reminder: If this is used in a stepwise fashion over lat/lon, be careful when passing 
   * over or near the poles and keep the velocity track in mind.
   * 
   *  @param v the velocity
   *  @param time the time from the current point
   *  Note: using a negative time value is the same a velocity moving in the opposite direction (along the great circle, if appropriate)
   * @return linear projection of the position
   */
  public Position linear(Velocity v, double time) {
    if (time == 0 || v.isZero()) {
      return latlon ? new Position(ll) : new Position(s3);
    }
    if (latlon) {
      return new Position(GreatCircle.linear_initial(ll,v,time));
    } else {
      return new Position(s3.linear(v, time)); 
    }
  }

  /**
   * Calculate a new position that is offset from the current position by (dn,de)
   * @param dn  offset in north direction (m)
   * @param de  offset in east direction  (m)
   * @return linear projection of the position
   */
  public Position linearEst(double dn, double de) {
    Position newNP;
    if (latlon) {
      newNP = new Position(lla().linearEst(dn,de));		
    } else {
      return new Position(Point.mk(s3.x + de, s3.y + dn, s3.z));
    }
    return newNP;
  }
  
  public Position linearDist(double track, double d) {
	  if (latlon) {
		  return new Position(GreatCircle.linear_initial(ll,track,d));
	  } else {
		  double fakeGs = 100;
		  Velocity v = Velocity.makeTrkGsVs(track,fakeGs,0.0);
		  double dt = d/fakeGs;
		  return new Position(s3.linear(v, dt)); 
	  }
  }
  
  /**
   * Perform a estimation of a linear projection of the current Position with the 
   * given velocity and time.
   * @param vo the velocity
   * @param time the time from the current point
   * @return linear projection of the position
   */
  public Position linearEst(Velocity vo, double time) {
    //double tm = dist/vel.gs();
    Position newNP;
    if (latlon) {
      if (lat() > Units.from("deg",85) || lat() < Units.from("deg",-85)) {
        newNP = new Position (GreatCircle.linear_initial(ll,vo,time));
      } else {
        newNP = new Position(lla().linearEst(vo,time));
      }
    } else {
      newNP = linear(vo,time);
    }
    return newNP;
  }


  public Position linearEstPerp(Velocity vo, double dist) {
    double t = dist/vo.gs();
    Velocity voPerp = Velocity.make(vo.PerpR());
    return linearEst(voPerp,t);
  }



  /**
   * Return the mid point between the current position and the given position
   * @param p2 the other position
   * @return the midpoint
   */
  public Position midPoint(Position p2) {
    if (latlon) {
      return new Position(GreatCircle.interpolate(ll,p2.lla(),0.5));
    } else {
      return new Position(Point.mk(VectFuns.midPoint(s3,Point.mk(p2.point())))); 
    }
  }

  public Position interpolate(Position p2, double f) {
    if (latlon) {
      return new Position(GreatCircle.interpolate(ll,p2.lla(),f));
    } else {
      return new Position(Point.mk(VectFuns.interpolate(s3,Point.mk(p2.point()),f))); 
    }
  }

  public Position interpolateEst(Position p2, double f) {
    if (latlon) {
      return new Position(GreatCircle.interpolate(ll,p2.lla(),f));
    } else {
      return new Position(Point.mk(VectFuns.interpolate(s3,Point.mk(p2.point()),f))); 
    }
  }


  //	/** Return a new Position, relative to a given position (which becomes the new origin).  
  //	 * This is equivalent to so.Sub(si), or the GreatCircle projection.
  //	 * Altitude will not be preserved. 
  //	 */ 
  //	public Position relativeProjection(Position si) {
  //		if (latlon) {
  //			Vect3 s3 = Projection.createProjection(si.lla()).project(lla()); 
  //		    return new Position(LatLonAlt.mk(s3.y, s3.x, s3.z));
  //		} else {
  //			return new Position(s3.Sub(Point.mk(si.point())));
  //		}
  //	}

  /** Return the track angle of the vector from the current Position to the given Position, based on initial course */
  public double track(Position p) {  
    if (p.latlon != latlon) {
      f.dln("Position.track call given an inconsistent argument: "+toString()+" "+p.toString());	
      return 0;
    }
    if (latlon) {
      return GreatCircle.initial_course(ll,p.ll);
    } else {
      Vect2 v = p.s3.Sub(s3).vect2();
      return v.compassAngle();
    }
  }

  /** return the velocity going from this to p over dt seconds.
   * Returns a ZERO velocity if dt <= 0 */
  public Velocity initialVelocity(Position p, double dt) {
    if (dt<=0) {
      return Velocity.ZERO;
    } else {			
      if (isLatLon()) {
        return GreatCircle.velocity_initial(lla(), p.lla(), dt);
      } else {
        return Velocity.make((p.point().Sub(point())).Scal(1.0/dt));
      }
    }
  }

  /** return the velocity going from this to p over dt seconds.
   * Returns a ZERO velocity if dt <= 0 */
  public Velocity finalVelocity(Position p, double dt) {
    if (dt<=0) {
      return Velocity.ZERO;
    } else {			
      if (isLatLon()) {
        return GreatCircle.velocity_final(lla(), p.lla(), dt);
      } else {
        return Velocity.make((p.point().Sub(point())).Scal(1.0/dt));
      }
    }
  }


  /** Return the track angle of the vector from the current Position to the given Position, based on representative course */
  public double representativeTrack(Position p) {  
    if (p.latlon != latlon) {
      f.dln("Position.representativeTrack call given an inconsistent argument.");	
      return 0.0;
    }
    if (latlon) {
      return GreatCircle.representative_course(ll,p.ll);
    } else {
      Vect2 v = p.s3.Sub(s3).vect2();
      return v.compassAngle();
    }
  }

  // returns intersection point and time of intersection relative to position so
  // a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1)
  public static Pair<Position,Double> intersection(Position so, Velocity vo, Position si, Velocity vi) {
    if (so.latlon != si.latlon) {
      f.dln("Position.intersection call was given an inconsistent argument.");	
      return new Pair<Position,Double>(Position.INVALID,-1.0);
    }
    if (so.latlon) {
      boolean checkBehind = false;
      Pair<LatLonAlt,Double> pgc = GreatCircle.intersection(so.lla(),vo, si.lla(),vi,checkBehind);
      return new Pair<Position,Double>(new Position(pgc.first),pgc.second );
    } else {
      Pair<Vect3,Double> pvt = VectFuns.intersection(so.point(),vo,si.point(),vi);
      return new Pair<Position,Double>(new Position(pvt.first),pvt.second );
    }
  }

	/**
	 * This function considers the line from p1 to p2 and computes 
	 * the shortest distance (i.e. perpendicular) of another point (offCircle) to that line.  This is the 
	 * cross track distance.
	 *  
	 * @param p1 the starting point of the line
	 * @param p2 another point on the line
	 * @param offCircle the point through which the perpendicular distance is desired
	 * @return the cross track distance [m]
	 */
  public static double perp_distance(Position p1, Position p2, Position offCircle) {
	  if (p1.latlon != p2.latlon) {
		  f.dln("Position.perp_distance call was given inconsistent arguments.");	
		  return -1;
	  }
	  if (p1.isLatLon()) {
		  return Math.abs(GreatCircle.cross_track_distance(p1.lla(), p2.lla(), offCircle.lla()));
	  } else {
		  Vect2 v = p2.vect2().Sub(p1.vect2());
		  return Vect2.distPerp(p1.vect2(), v, offCircle.vect2());
	  }
  }
		
  //	// returns intersection point and time of intersection relative to position so
  //	// for time return value, it assumes that an aircraft travels from so1 to so2 in dto seconds and the other aircraft from si to si2
  //	// a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1)
  //    public static Pair<Position,Double> intersection(Position so, Position so2, double dto, Position si, Position si2) {
  // 		if (so.latlon != si.latlon) {
  // 		    f.dln("Position.intersection call was given an inconsistent argument.");	
  // 		    return new Pair<Position,Double>(Position.INVALID,-1.0);
  // 		}
  // 		if (so.latlon) {
  // 			LatLonAlt lgc = GreatCircle.intersection(so.lla(),so2.lla(), si.lla(),si2.lla());
  // 			Position pgc = new Position(lgc);
  // 			double gso = so.distanceH(so2)/dto;
  // 			f.pln(" ## gso = "+Units.str("kn", gso));
  // 			double intTm = so.distanceH(pgc)/gso;  // relative to so 			
  // 			boolean behind = GreatCircle.behind(lgc, so.lla(), GreatCircle.velocity_average(so.lla(), so2.lla(), 1.0));
  // 			if (behind) intTm = -intTm;			
  // 			// compute a better altitude
  // 			double vs = (so2.alt() - so.alt())/dto;
  // 			double nAlt = so.alt() + vs*(intTm);	
  //			f.pln(" $$ Position.intersection: intTm = "+intTm+" vs = "+Units.str("fpm",vs)+" nAlt = "+Units.str("ft",nAlt)+" "+behind);			 
  // 			pgc = Position.mkLatLonAlt(lgc.lat(),lgc.lon(),nAlt);
  // 			return new Pair<Position,Double>(pgc,intTm);
  // 		} else {
  // 			Pair<Vect3,Double> pvt = VectFuns.intersection(so.point(),so2.point(),dto,si.point(),si2.point());
  // 			return new Pair<Position,Double>(new Position(pvt.first),pvt.second );
  // 		}
  //     }

  /** Returns intersection point and time of intersection relative to the time of position so
   *  for time return value, it assumes that an aircraft travels from so1 to so2 in dto seconds and the other aircraft from si to si2
   *  a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1). 
   *  This method computes an altitude which is the average of the altitudes of the nearest points.
   * @param so     first point of line A 
   * @param so2    second point of line A 
   * @param dto    the delta time between point so and point so2.
   * @param si     first point of line B
   * @param si2    second point of line B 
   * @return a pair: intersection point and the delta time from point "so" to the intersection, can be negative if intersect
   *                 point is in the past
   *                if intersection point is invalid then the returned delta time is -1
   */
  public static Pair<Position,Double> intersection(Position so, Position so2, double dto, Position si, Position si2) {
    if (so.latlon != si.latlon && so2.latlon != si2.latlon && so.latlon != so2.latlon) {
      f.dln("Position.intersection call was given an inconsistent argument.");	
      return new Pair<Position,Double>(Position.INVALID,-1.0);
    }
    if (so.latlon) {
      Pair<LatLonAlt,Double> lgc = GreatCircle.intersection(so.lla(),so2.lla(), dto, si.lla(), si2.lla());
      return new Pair<Position,Double>(new Position(lgc.first),lgc.second);
    } else {
      Pair<Vect3,Double> pvt = VectFuns.intersection(so.point(),so2.point(),dto,si.point(),si2.point());
      return new Pair<Position,Double>(new Position(pvt.first),pvt.second );
    }
  }


  /** Return the average velocity between the current position and the given position, with the given speed [internal units]. */
  public Velocity averageVelocity(Position p2, double speed) {
    if (p2.latlon != latlon) {
      f.dln("Position.averageVelocity call given an inconsistent argument.");	
      return Velocity.ZERO;
    }
    if (latlon) {
      return GreatCircle.velocity_average_speed(ll,p2.ll,speed);
    } else {
      return Velocity.mkVel(s3,p2.s3,speed);
    }
  }

  /** Return the average velocity between the current position and the given position, with the given delta time dt. */
  public Velocity avgVelocity(Position p2, double dt) {
    if (p2.latlon != latlon) {
      f.dln("Position.averageVelocity call given an inconsistent argument.");	
      return Velocity.ZERO;
    }
    if (latlon) {
      return GreatCircle.velocity_average(ll,p2.ll,dt);
    } else {
      return Velocity.genVel(s3,p2.s3,dt);
    }

  }

  public boolean isWest(Position a) {
	  if (isLatLon()) {
		  return lla().isWest(a.lla());
	  } else {
		  return x() < a.x();
	  }
  }
  
  /** Determine if a loss of separation has occured (using either geodesic or Euclidean calculations)
   * 
   * @param p2 the position of the other aircraft
   * @param D horizontal distance to specify loss of separation 
   * @param H vertical distance to specify loss of separation
   * @return true if there is a loss of separation
   */
  public boolean LoS(Position p2, double D, double H) {
    if (p2.latlon != latlon) {
      f.dln("Position.LoS call given an inconsistent argument: "+toString()+" "+p2.toString());	
      return false;
    }
    double distH = distanceH(p2);
    double distV = distanceV(p2);
    //f.pln ("distH "+distH+"  distV "+distV+" D "+D+" H "+H);
    return (distH < D && distV < H);
  }

  /** Return a string representation */
  public String toString() {
    return toString(Constants.get_output_precision());
  }

  /** Return a string representation */
  public String toString(int prec) {
    if (latlon)
      return "("+Units.str("deg",ll.lat(),prec)+", "+Units.str("deg",ll.lon(),prec)+", "+Units.str("ft",ll.alt(),prec)+")";
    else
      return "("+Units.str("NM",s3.x,prec)+", "+Units.str("NM",s3.y,prec)+", "+Units.str("ft",s3.z,prec)+")";
  }

  /** Return a string representation */
  public String toStringUnicode(int prec) {
    if (latlon) {
      return ll.toStringUnicode(prec);
    } else {
      return "("+Units.str("NM",s3.x,prec)+", "+Units.str("NM",s3.y,prec)+", "+Units.str("ft",s3.z,prec)+")";
    }
  }

  public String toStringUnits() {
    return toStringUnits("NM", "NM", "ft");
  }

  /**
   * Return a string representation using the given unit conversions (latitude and longitude, if appropriate, are always in degrees, 
   * so only the z unit is used in that case).
   */
  public String toStringUnits(String xUnits, String yUnits, String zUnits) {
    if (latlon)
      return "("+Units.str("deg",ll.lat())+", "+Units.str("deg",ll.lon())+", "+Units.str(zUnits,ll.alt())+")";
    else
      return "("+Units.str(xUnits,s3.x)+", "+Units.str(yUnits,s3.y)+", "+Units.str(zUnits,s3.z)+")";
  }

//  /** Return a string representation, with a user-specified digits of precision (0-15) and units without parentheses. 
//   * @param xlat x unit (if Euclidean) or latitude unit (if geodetic) 
//   * @param ylon y unit (if Euclidean) or longitude unit (if geodetic) 
//   * @param z altitude unit  
//   * @param precision degree of precision (fractional decimal places)
//   */
//  public String toStringNP(String xlat, String ylon, String z, int precision) {
//    if (latlon)
//      return ll.toStringNP(xlat, ylon, z, precision);
//    else
//      return s3.toStringNP(xlat, ylon, z, precision);
//  }

  /** Return a string representation, with a user-specified digits of precision (0-15) without parentheses. */
  public String toStringNP(int precision) {
    if (latlon)
      return ll.toStringNP(precision);
    else
      return s3.toStringNP(precision);
  }

  /** Return a string representation with a default precision but without parentheses. */
  public String toStringNP() {
    return toStringNP(Constants.get_output_precision());
  }

  /** Return a string representation, for Euclidean use the units [NM,NM,ft] and for LatLon use the units [deg,deg,ft] */
  public List<String> toStringList() {
    ArrayList<String> ret = new ArrayList<String>(3);
    if (isInvalid()) {
      ret.add("-");
      ret.add("-");
      ret.add("-");
    } else if (latlon) {
      ret.add(Double.toString(ll.latitude()));
      ret.add(Double.toString(ll.longitude()));
      ret.add(Double.toString(ll.altitude()));
    } else {
      ret.add(Double.toString(Units.to("NM",s3.x)));
      ret.add(Double.toString(Units.to("NM",s3.y)));
      ret.add(Double.toString(Units.to("ft",s3.z)));
    }
    return ret;
  }


  /** Return a string representation, for Euclidean use the units [NM,NM,ft] and for LatLon use the units [deg,deg,ft] 
   * 
   * @param precision the number of digits to display
   * @return a string representation of the position
   */
  public List<String> toStringList(int precision) {
    ArrayList<String> ret = new ArrayList<String>(3);
    if (isInvalid()) {
      ret.add("-");
      ret.add("-");
      ret.add("-");
    } else if (latlon) {
      ret.add(f.FmPrecision(ll.latitude(),precision));
      ret.add(f.FmPrecision(ll.longitude(),precision));
      ret.add(f.FmPrecision(ll.altitude(),precision));
    } else {
      ret.add(f.FmPrecision(Units.to("NM",s3.x),precision));
      ret.add(f.FmPrecision(Units.to("NM",s3.y),precision));
      ret.add(f.FmPrecision(Units.to("ft",s3.z),precision));
    }
    return ret;
  }


  /** This interprets a string as a LatLonAlt position with units in deg/deg/ft or the specified units (inverse of toString()) */
  public static Position parseLL(String s) {
    return new Position(LatLonAlt.parse(s));
  }

  /** This interprets a string as a XYZ position with units in NM/NM/ft or the specified units (inverse of toString()) */
  public static Position parseXYZ(String s) {
    Point v = Point.parse(s);
    return new Position(v);
  }

  /**
   * This interprets a string into a LatLonAlt or XYZ position, if appropriate units are given.
   * If no units are present, it returns an invalid Position.
   */
  public static Position parse(String s) {
    String[] fields = s.split(Constants.wsPatternParens);
    if (fields[0].equals("")) {
      fields = Arrays.copyOfRange(fields,1,fields.length);
    }
    if (fields.length == 6) {
      if (Units.isCompatible(Units.clean(fields[1]), "deg") && Units.isCompatible(Units.clean(fields[3]), "deg") && Units.isCompatible(Units.clean(fields[5]), "ft")) return parseLL(s); // latlonalt
      if (Units.isCompatible(Units.clean(fields[1]), "m") && Units.isCompatible(Units.clean(fields[3]), "m") && Units.isCompatible(Units.clean(fields[5]), "m")) return parseXYZ(s); // Euclidean
    }
    return Position.INVALID;
  }
}
