/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.List;

import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

public class TrafficState extends TrafficCoreState {

  private final Vect3    s_; // Projected position
  private final Velocity v_; // Projected velocity
  private final EuclideanProjection eprj_; // Projection 

  public TrafficState() {
    s_ = Vect3.INVALID;
    v_ = Velocity.INVALID;
    eprj_ = Projection.createProjection(Position.ZERO_LL);
  }

  public static final TrafficState INVALID = new TrafficState();

  public static final List<TrafficState> INVALIDL = new ArrayList<TrafficState>();

  private TrafficState(String id, Position pos, Velocity vel, EuclideanProjection eprj) {
    super(id,pos,vel);
    if (isLatLon()) {
      s_ = eprj.project(pos);
      v_ = eprj.projectVelocity(pos,vel);
    } else {
      s_ = pos.point();
      v_ = vel;
    }    
    eprj_ = eprj;
  }

  public static TrafficState makeOwnship(String id, Position pos, Velocity vel) {
    return new TrafficState(id,pos,vel,pos.isLatLon()?Projection.createProjection(pos.lla().zeroAlt()):
      Projection.createProjection(Position.ZERO_LL));
  }

  public TrafficState makeIntruder(String id, Position pos, Velocity vel) {
    if (isLatLon() != pos.isLatLon()) { // || 
      //        GreatCircle.distance(pos.lla(),getPosition().lla()) > eprj_.maxRange()) {
      //      Thread.dumpStack();        
      return INVALID;
    }
    return new TrafficState(id,pos,vel,get_eprj());
  }

  public Vect3 get_s() {
    return s_;
  }

  public Velocity get_v() {
    return v_;
  }

  public EuclideanProjection get_eprj() {
    return eprj_;
  }

  public Vect3 pos_to_s(Position p) {
    if (p.isLatLon()) {
      if (!getPosition().isLatLon()) {
        //        Thread.dumpStack();        
        return Vect3.INVALID;
      }
      return eprj_.project(p);
    } 
    return p.point();
  }

  public Velocity vel_to_v(Position p,Velocity v) {
    if (p.isLatLon()) {
      if (!getPosition().isLatLon()) {
        //        Thread.dumpStack();        
        return Velocity.INVALID;
      }     
      return eprj_.projectVelocity(p,v);
    } 
    return v;
  }

  public Velocity inverseVelocity(Velocity v) {
    return eprj_.inverseVelocity(s_,v,true);
  }

  public TrafficState linearProjection(double offset) {
    return new TrafficState(getId(),getPosition().linear(getVelocity(),offset),getVelocity(),eprj_);
  }

  public static TrafficState findAircraft(List<TrafficState> traffic, String id) {
    if (!id.equals(TrafficState.INVALID.getId())) {
      for (int i=0; i < traffic.size(); ++i) {
        TrafficState ac = traffic.get(i);
        if (ac.getId().equals(id))
          return ac;
      }
    }
    return TrafficState.INVALID;
  }

  public static String listToString(List<TrafficState> traffic) {
    String s = "{";
    boolean comma = false;
    for (TrafficState ac : traffic) {
      if (comma) {
        s+=", ";
      } else {
        comma = true;
      }
      s += ac.getId();
    }
    return s+"}";
  }

  public String formattedTraffic(List<TrafficState> traffic, double time) {
    String s="";
    if (isLatLon()) {
      s += "NAME lat lon alt trk gs vs time\n";
      s += "[none] [deg] [deg] [ft] [deg] [knot] [fpm] [s]\n";
    } else {
      s += "NAME sx sy sz trk gs vs time\n";
      s += "[none] [NM] [NM] [ft] [deg] [knot] [fpm] [s]\n";
    }
    s += getId()+", "+getPosition().toStringNP()+", "+getVelocity().toStringNP()+", "+
        f.Fm1(time)+"\n";

    for (TrafficState ac : traffic) {
      s += ac.getId()+", "+ac.getPosition().toStringNP()+", "+ac.getVelocity().toStringNP()+", "+
          f.Fm1(time)+"\n";
    }
    return s;
  }

  public String toPVS(int prec) {
    return "(# id := \"" + getId() + "\", s := "+s_.toPVS(prec)+", v := "+v_.toPVS(prec)+" #)";   
  }

  public String listToPVSAircraftList(List<TrafficState> traffic, int prec) {
    String s = "";
    s += "(: ";
    s += toPVS(prec); 
    for (TrafficState ac : traffic) {
      s += ", ";
      s += ac.toPVS(prec);
    }
    return s+" :)";
  }

  public static String listToPVSStringList(List<TrafficState> traffic, int prec) {
    if (traffic.isEmpty()) {
      return "null[string]";
    } else {
      String s = "(:";
      boolean comma = false;
      for (TrafficState ac : traffic) {
        if (comma) {
          s += ", ";
        } else {
          s += " ";
          comma = true;
        }
        s += "\"" + ac.getId() + "\"";
      }
      return s+" :)";
    }
  }

}
