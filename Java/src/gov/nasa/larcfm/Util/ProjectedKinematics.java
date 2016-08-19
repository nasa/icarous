/*
 * ProjectedKinematics.java 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;


import java.util.ArrayList;

/**
 * This class contains versions of the Kinematics functions that have been lifted to deal with Position objects instead of Vect3 objects.
 *
 */
public final class ProjectedKinematics {


	  public static Pair<Position,Velocity> linear(Pair<Position,Velocity> p, double t) {
		    return linear(p.first, p.second, t);
	  }
		  
	  public static Pair<Position,Velocity> linear(Position so ,Velocity vo, double t) {
		  Vect3 s3 = so.point();
		  if (so.isLatLon()) {
			  s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
		  }
		  Vect3 ns = s3.linear(vo,t);
		  if (so.isLatLon()) {
			  return Projection.createProjection(so.lla().zeroAlt()).inverse(ns,vo,true);
		  } else {
			  return new Pair<Position,Velocity>(new Position(ns),vo);  
		  }
	  }

  /**
   * Calculate the angle of a constant-radius turn from two points and the radius
   */
  public static double turnAngle(Position s1, Position s2, double R) {
    double distAB = s1.distanceH(s2);
    return 2*(Math.asin(distAB/(2*R))); 
  }

  /**
   * Horizontal distance covered in a turn
   */
  public static double turnDistance(Position s1, Position s2, double R) {
    return turnAngle(s1,s2,R)*R;
  }

  /**
   * Given two points on a turn and the velocity (direction) at the first point, determine the direction for the shortest turn going through the second point,
   * returning true if that relative direction is to the right
   */
  public static boolean clockwise(Position s1, Velocity v1, Position s2) {
    double trk1 = v1.trk();
    double trk2;
    if (s1.isLatLon()) {
      trk2 = GreatCircle.velocity_initial(s1.lla(), s2.lla(), 1).trk();
    } else {
      trk2 = s2.point().Sub(s1.point()).vect2().trk();
    }
    return Util.clockwise(trk1, trk2);
  }



  //	  /**
  //	   * Turn velocity at point s1, given addition position s2 on turn at relative time t 
  //	   * @param s1 position to take tangent
  //	   * @param s2 additional position on turn
  //	   * @param C center of circle of rotation
  //	   * @param t time to get from s1 to s2 (negative if s1 after s2)
  //	   * @return velocity at point s1
  //	   */
  //	  public static Velocity turnVelocity(Position s1, Position s2, Position C, double t) {
  //		  double R = s1.distanceH(C);
  //		  double gs = turnDistance(s1,s2,R)/t;
  //		  double vs = (s2.z() - s1.z())/t;
  //		  double trk = s1.track(C); // work from this due to potential GC issues
  //		  double trk1 = C.track(s1);
  //		  double trk2 = C.track(s2);
  //		  boolean clockwise = Util.clockwise(trk1, trk2);
  //		  if (t < 0) {
  //			  clockwise = !clockwise; 
  //		  }
  //		  if (clockwise) {
  //			  trk = Util.to_2pi(trk - Math.PI/2);
  //		  } else if (clockwise) {
  //			  trk = Util.to_2pi(trk + Math.PI/2);
  //		  } else if (t < 0) {
  //			  
  //		  }
  //		  return Velocity.mkTrkGsVs(trk, gs, vs);
  //	  }
  //	
  //	  

  public static double closestTimeOnTurn(Position so, Velocity v1, double omega, Position s2) {
    Vect3 s3 = so.point();
    Vect3 x3 = s2.point();
    if (so.isLatLon()) {
      EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt()); 
      s3 = proj.project(so); 
      x3 = proj.project(s2); 
    } 
    return Kinematics.closestTimeOnTurn(s3, v1, omega, x3);
  }

  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
   *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param R   turn radius
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turn(Position so, Velocity vo, double t, double R,  boolean turnRight) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Pair<Vect3,Velocity> resp = Kinematics.turn(s3,vo,t,R,turnRight);
    Vect3 pres = resp.first;
    Velocity vres = resp.second;
    //f.pln("Kin.turnProjection so = "+so+" pres = "+pres+" vo = "+vo+" vres=  "+vres);	  
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }

  
  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
   *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param R   turn radius
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnOmega(Position so, Velocity vo, double t, double omega, EuclideanProjection proj) {
    Vect3 s3 = so.point();
    //Velocity vo3 = vo;
    if (so.isLatLon()) {
      s3 = proj.project(so); 
    }
    Pair<Vect3,Velocity> resp = Kinematics.turnOmega(s3,vo,t,omega);
    Vect3 pres = resp.first;
    Velocity vres = resp.second;
    //f.pln("ProjectedKinematics.turnOmega t = "+t+" so = "+so.toString4()+"  vo = "+vo+" omega = "+omega);	  
    if (so.isLatLon()) {
      return proj.inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }



  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
   *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param R   turn radius
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnOmega(Position so, Velocity vo, double t, double omega) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    //f.pln("ProjectedKinematics.turnOmega t = "+t+" s3 = "+s3+" omega = "+Units.str("deg/s",omega));	  
    Pair<Vect3,Velocity> resp = Kinematics.turnOmega(s3,vo,t,omega);
    Vect3 pres = resp.first;
    Velocity vres = resp.second;
    //f.pln("ProjectedKinematics.turnOmega t = "+t+" so = "+so.toString4()+"  vo = "+vo+" omega = "+omega);	  
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }

  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
   *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param R   turn radius
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnOmega(Pair<Position,Velocity> pp, double t, double omega) {
    Position so = pp.first;
    Velocity vo = pp.second;
    return turnOmega(so,vo,t,omega);
  }



  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnUntil
   *  and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param goalTrack the target track angle
   * @param bankAngle the aircraft's bank angle
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnUntil(Position so, Velocity vo, double t, double goalTrack, double bankAngle) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    //Vect3 pres = Kinematics.turnUntilPosition(s3,vo,goalTrack, bankAngle,t,turnRight);
    //Velocity vres = Kinematics.turnUntilVelocity(vo,goalTrack, bankAngle,t,turnRight);
    Pair<Vect3,Velocity> svres = Kinematics.turnUntil(s3, vo, t, goalTrack, bankAngle);
    Vect3 pres = svres.first;
    Velocity vres = svres.second;
    //f.pln("Kin.turnProjection so = "+so+" pres = "+pres+" vo = "+vo+" vres=  "+vres);	  
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }

  public static Pair<Position,Velocity> turnUntil(Pair<Position,Velocity> sv, double t, double goalTrack, double bankAngle) {
    return turnUntil(sv.first, sv.second,t, goalTrack, bankAngle);
  }


  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnUntil
   *  and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param goalTrack the target track angle
   * @param bankAngle the aircraft's bank angle
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnUntilTimeOmega(Position so, Velocity vo, double t, double goalTrack, double bankAngle) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    //Vect3 pres = Kinematics.turnUntilTimeOmegaPosition(s3,vo,goalTrack, bankAngle,t,turnRight);
    //Velocity vres = Kinematics.turnUntilTimeOmegaVelocity(vo,goalTrack, bankAngle,t,turnRight);
    Pair<Vect3,Velocity> svres = Kinematics.turnUntilTimeOmega(s3, vo, t, goalTrack, bankAngle);
    Vect3 pres = svres.first;
    Velocity vres = svres.second;
    //f.pln("Kin.turnProjection so = "+so+" pres = "+pres+" vo = "+vo+" vres=  "+vres);	  
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }

  public static Pair<Position,Velocity> turnUntilTimeOmega(Pair<Position,Velocity> sv, double t, double goalTrack, double bankAngle) {
    return turnUntilTimeOmega(sv.first, sv.second,t, goalTrack, bankAngle);
  }


  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnUntilTime
   *  and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param goalTrack the target track angle
   * @param bankAngle the aircraft's bank angle
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnUntilTime(Position so, Velocity vo, double t, double turnTime, double R, boolean turnRight) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Pair<Vect3,Velocity> svres = Kinematics.turnUntilTimeRadius(new Pair<Vect3,Velocity>(s3, vo), t, turnTime, R, turnRight);
    Vect3 pres = svres.first;
    Velocity vres = svres.second;
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }




  /**
   *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
   *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param goalTrack the target track angle
   * @param bankAngle the aircraft's bank angle
   * @param t   time of turn [secs]
   * @param turnRight true iff only turn direction is to the right
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> turnUntilWithRoll(Position so, Velocity vo, double t, double goalTrack, double bankAngle, 
      double rollTime) {
    //f.pln("Kin.turnProjection so = "+so+" vo = "+vo+" t = "+t+" goalTrack=  "+Units.str("deg",goalTrack)
    //	+" bankAngle=  "+Units.str("deg",bankAngle)+" rollRate = "+rollRate);	  
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Pair<Vect3,Velocity> svres = Kinematics.turnUntilWithRoll(s3,vo,t, goalTrack, bankAngle,rollTime);
    Vect3 pres = svres.first;
    Velocity vres = svres.second;
    //f.pln("Kin.turnProjection so = "+so+" pres = "+pres+" vo = "+vo+" vres=  "+vres);	  
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }



  //  public static boolean turnRightDirectTo(Position so, Velocity vo, Position goal) {
  //	  Vect3 s3 = so.vect3();
  //	  Vect3 g3 = goal.vect3();
  //	  if (so.isLatLon()) {
  //		  s3 = Projection.createProjection(so.lla()).project(so);
  //		  g3 = Projection.createProjection(so.lla()).project(goal);
  //	  }
  //	  return Kinematics.turnRightDirectTo(s3,vo,g3);	  
  //  }
  //  
  //  
  //  /** Wrapper around Kinematic.turnTimeDirecTo()
  //   */
  //  public static double turnTimeDirectTo(Position so, Velocity vo, Position goal, double bankAngle) {
  //	  Vect3 s3 = so.vect3();
  //	  Vect3 g3 = goal.vect3();
  //	  if (so.isLatLon()) {
  //		  s3 = Projection.createProjection(so.lla().zeroAlt()).project(so);
  //		  g3 = Projection.createProjection(so.lla().zeroAlt()).project(goal);
  //	  }
  //	  return Kinematics.turnTimeDirectTo(s3,vo,g3,bankAngle);
  //  }

  // if this fails, it returns a NaN time
  public static Pair<Position,Double> intersection(Position so, Velocity vo, Position si, Velocity vi) {
    Vect3 so3 = so.point();
    Vect3 si3 = si.point();
    EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt()); 
    if (so.isLatLon()) {
      so3 = proj.project(so); 
      si3 = proj.project(si); 
    }
    Pair<Vect3,Double> intersect = VectFuns.intersection(so3, vo, si3, vi);
    if (so.isLatLon()) {
      return new Pair<Position,Double>(new Position(proj.inverse(intersect.first)),intersect.second);
    } else {
      return new Pair<Position,Double>(new Position(intersect.first),intersect.second);  
    }
  }


  public static double timeOfintersection(Position so, Velocity vo, Position si, Velocity vi) {
    Vect3 so3 = so.point();
    Vect3 si3 = si.point();
    EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt()); 
    if (so.isLatLon()) {
      so3 = proj.project(so); 
      si3 = proj.project(si); 
    }
    double  intersectTime = VectFuns.timeOfIntersection(so3, vo, si3, vi);
    return intersectTime;
  }




  /** Wrapper around Kinematic.directToPoint()
   * Returns a quad: end of turn point, velocity at that point, time at that point, direction of turn
   * If no result is possible (for example the point lies within the given turn radius), this will return a negative time.
   */
  public static Quad<Position,Velocity,Double,Integer> directToPoint(Position so, Velocity vo, Position wp, double R) {
    Vect3 s3 = so.point();
    Vect3 g3 = wp.point();
    EuclideanProjection proj = null;
    if (so.isLatLon()) {
      proj = Projection.createProjection(so.lla().zeroAlt());
      s3 = proj.project(so);
      g3 = proj.project(wp);
    }
    Quad<Vect3,Velocity,Double,Integer> dtp = Kinematics.directToPoint(s3,vo,g3,R);
    Pair<Position,Velocity> pv;
    if (so.isLatLon()) {
      pv = proj.inverse(dtp.first,dtp.second,true);
    } else {
      pv = new Pair<Position,Velocity>(new Position(dtp.first), dtp.second);  
    }
    return new Quad<Position,Velocity,Double,Integer> (pv.first, pv.second, dtp.third,dtp.fourth);
  }

  /** Wrapper around Kinematic.genDirectToVertex
   *  Returns the vertex point (in a linear plan sense) between current point and directTo point.
   * 
   * @param so     current position
   * @param vo     current velocity
   * @param wp     first point (in a flight plan) that you are trying to connect to
   * @param bankAngle  turn bank angle
   * @param timeBeforeTurn   time to continue in current direction before beginning turn
   * @return (so,t0,t1) vertex point and delta time to reach the vertex point and delta time (from so) to reach end of turn
   *  If no result is possible this will return an invalid position and negative times.
   */
  public static Triple<Position,Double,Double> genDirectToVertex(Position sop, Velocity vo, Position wpp, double bankAngle, double timeBeforeTurn) {
    Vect3 s3 = sop.point();
    Vect3 g3 = wpp.point();
    EuclideanProjection proj = null;
    if (sop.isLatLon()) {
      proj = Projection.createProjection(sop.lla().zeroAlt());
      s3 = proj.project(sop);
      g3 = proj.project(wpp);
    }
    Triple<Vect3,Double,Double> vertTriple = Kinematics.genDirectToVertex(s3,vo,g3,bankAngle,timeBeforeTurn);
    if (vertTriple.third < 0) {
      return new Triple<Position,Double,Double>(Position.INVALID, -1.0, -1.0);
    }
    //f.pln(" $$$ genDirectToVertex: vertPair.second = "+vertTriple.second);
    Vect3 vertex = vertTriple.first;
    //Vect3 eot = vertTriple.third;
    Position pp;
    if (sop.isLatLon()) {
      pp = new Position(proj.inverse(vertex));
      //eotp = new Position(proj.inverse(eot));
    } else {
      pp = new Position(vertex);  
      //eotp = new Position(eot); 
    }
    //  f.pln("ProjectedKinematics.genDirectToVertex ipPair="+vertTriple);
    //  if (sop.isLatLon()) f.pln("name,lat,lon,alt,trk,gs,vs");
    //  else f.pln("name,SX,SY,SZ,TRK,GS,VS");
    //  f.pln("a1,"+sop.toStringNP(8)+","+vo.toString8NP());
    //  f.pln("b2,"+pp.toStringNP(8)+","+vo.toString8NP());
    //  f.pln("c3,"+wpp.toStringNP(8)+","+vo.toString8NP());
    return new Triple<Position,Double,Double>(pp,vertTriple.second,vertTriple.third);
  }

  static ArrayList<Pair<Position,Double>> genDirectToVertexList(Position so, Velocity vo, Position wp, double bankAngle, double timeBeforeTurn, double timeBetweenPieces) {
    Vect3 s3 = so.point();
    Vect3 g3 = wp.point();
    EuclideanProjection proj = null;
    if (so.isLatLon()) {
      proj = Projection.createProjection(so.lla().zeroAlt());
      s3 = proj.project(so);
      g3 = proj.project(wp);
    }
    ArrayList<Pair<Vect3, Double>> vertTriple = Kinematics.genDirectToVertexList(s3,vo,g3,bankAngle,timeBeforeTurn, timeBetweenPieces);

    ArrayList<Pair<Position,Double>> ptriple = new ArrayList<Pair<Position,Double>>();
    for (int i = 0; i < vertTriple.size(); i++) {
      if (so.isLatLon()) {
        Position pp = new Position(proj.inverse(vertTriple.get(i).first));
        ptriple.add(new Pair<Position,Double>(pp, vertTriple.get(i).second));  
      } else {
        ptriple.add(new Pair<Position,Double>(new Position(vertTriple.get(i).first), vertTriple.get(i).second));  
      }

    }
    return ptriple;
  }


  /**
   *  Position and velocity after t time units accelerating horizontally.  This is a wrapper around posAccelGS 
   *  for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param t   time of turn [secs]
   * @param a   ground speed acceleration (or deceleration) (signed)
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> gsAccel(Position so, Velocity vo, double t, double a) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Vect3 pres = Kinematics.gsAccelPos(s3,vo,t, a);
    Velocity vres = Velocity.mkTrkGsVs(vo.trk(),vo.gs()+a*t,vo.vs());
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }




  /**
   *  Position and velocity after t time units accelerating horizontally.  This is a wrapper around posAccelGS 
   *  for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param a   ground speed acceleration (or deceleration) (positive)
   * @param t   time of turn [secs]
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> gsAccelUntil(Position so, Velocity vo, double t, double goalGs, double a) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Pair<Vect3,Velocity> gat = Kinematics.gsAccelUntil(s3,vo,t,goalGs,a);
    Vect3 pres = gat.first;
    //		  f.pln(" ## ProjectedKinematics.gsAccelUntil"+f.sStr(pres));
    Velocity vres = gat.second;
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }



  /**
   *  Position and velocity after t time units accelerating vertically.  This is a wrapper around posAccelVS 
   *  for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param a   vertical speed acceleration (or deceleration) (signed)
   * @param t   time of turn [secs]
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> vsAccel(Position so, Velocity vo, double t, double a) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Vect3 pres = Kinematics.vsAccelPos(s3,vo,t,a);
    Velocity vres = Velocity.mkVxyz(vo.x, vo.y, vo.z+a*t);
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      //f.pln(" $$$ vsAccel: s3 = "+s3+" vo = "+vo+" t = "+t+" a = "+a+" pres = "+pres);
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }


  /**
   *  Position and velocity after t time units accelerating vertically.  This is a wrapper around vsAccelUntil 
   *  for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param a   vertical speed acceleration (a positive value)
   * @param t   time of turn [secs]
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> vsAccelUntil(Position so, Velocity vo, double t, double goalVs, double a) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Vect3 pres = Kinematics.vsAccelUntil(s3,vo,t,goalVs,a).first;
    Velocity vres = Kinematics.vsAccelUntil(s3,vo,t,goalVs,a).second;
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }

  /**
   *  Position and velocity after t time units accelerating vertically.  This is a wrapper around vsAccelUntilWithRampUp 
   *  for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param a   vertical speed acceleration (a positive value)
   * @param t   time of turn [secs]
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> vsAccelUntilWithRampUp(Position so, Velocity vo, double t, double goalVs, double a,double tRamp) {
    Vect3 s3 = so.point();
    if (so.isLatLon()) {
      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
    }
    Vect3 pres = Kinematics.vsAccelUntilWithRampUp(s3,vo,t,goalVs,a,tRamp).first;
    Velocity vres = Kinematics.vsAccelUntilWithRampUp(s3,vo,t,goalVs,a,tRamp).second;
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
    } else {
      return new Pair<Position,Velocity>(new Position(pres), vres);  
    }
  }

  public static double vsLevelOutTime(Position so, Velocity vo, double climbRate, double targetAlt, double a) {
    Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
    // we don't need horizontal components, so don't need to project
    return Kinematics.vsLevelOutTime(sv, climbRate, targetAlt, a); 
  }

  /**
   *  Position and velocity after t time units accelerating vertically.  This is a wrapper around posAccelVS 
   *  for Position objects,and uses the projection defined in the static Projection class.
   * @param so  starting position
   * @param vo  initial velocity
   * @param a   vertical speed acceleration (a positive value)
   * @param t   time of turn [secs]
   * @return Position and Velocity after t time
   */
  public static Pair<Position,Velocity> vsLevelOut(Position so, Velocity vo, double t, double climbRate, double targetAlt, double a, boolean allowClimbRateReduction) {
    Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
    if (so.isLatLon()) {
      sv = Projection.createProjection(so.lla().zeroAlt()).project(so, vo);
    }
    Pair<Vect3,Velocity> vat = Kinematics.vsLevelOut(sv, t, climbRate, targetAlt, a, allowClimbRateReduction); 
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(vat.first,vat.second,true);
    } else {
      return new Pair<Position,Velocity>(new Position(vat.first), vat.second);  
    }
  }

  public static Pair<Position,Velocity> vsLevelOut(Position so, Velocity vo, double t, double climbRate, double targetAlt, double a) {
    Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
    if (so.isLatLon()) {
      sv = Projection.createProjection(so.lla().zeroAlt()).project(so, vo);
    }
    Pair<Vect3,Velocity> vat = Kinematics.vsLevelOut(sv, t, climbRate, targetAlt, a); 
    if (so.isLatLon()) {
      return Projection.createProjection(so.lla().zeroAlt()).inverse(vat.first,vat.second,true);
    } else {
      return new Pair<Position,Velocity>(new Position(vat.first), vat.second);  
    }
  }

  public static Triple<Position,Velocity,Double> vsLevelOutFinal(Position so, Velocity vo, double climbRate, double targetAlt, double a) {
    if (climbRate == 0) {
      return new Triple<Position,Velocity,Double>(so.mkZ(targetAlt),vo.mkVs(0),0.0);
    } else {
      Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
      if (so.isLatLon()) {
        sv = Projection.createProjection(so.lla().zeroAlt()).project(so, vo);
      }
      StateVector vat = Kinematics.vsLevelOutFinal(sv, climbRate, targetAlt, a);
      if (vat.t < 0) return new Triple<Position,Velocity,Double>(Position.INVALID, Velocity.INVALID, vat.t);
      if (so.isLatLon()) {
        Pair<Position,Velocity>p = Projection.createProjection(so.lla().zeroAlt()).inverse(vat.s,vat.v,true);
        return new Triple<Position,Velocity,Double>(p.first, p.second, vat.t);
      } else {
        return new Triple<Position,Velocity,Double>(new Position(vat.s), vat.v, vat.t);
      }
    }
  }
  
  /**
   * Perform a turn delta calculation between trk1 and trk2, compensating for geodetic coordinates
   * @param so projection point
   * @param trk1 first track of interest
   * @param trk2 second track of interest
   * @return difference between tracks, in [-PI,+PI]
   */
  public static double turnDelta(Position so, double trk1, double trk2) {
	  double alpha = trk1;
	  double beta = trk2;
	  if (so.isLatLon()) {
		  Velocity v1 = Velocity.mkTrkGsVs(trk1, 100.0, 0.0);
		  Velocity v2 = Velocity.mkTrkGsVs(trk2, 100.0, 0.0);
		  EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt());
		  alpha = proj.projectVelocity(so.lla(), v1).trk();
		  beta = proj.projectVelocity(so.lla(), v2).trk();
	  }
	  return Util.turnDelta(alpha, beta);
  }
  
}
