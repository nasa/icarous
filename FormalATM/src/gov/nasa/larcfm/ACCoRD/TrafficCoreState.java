/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Velocity;

public class TrafficCoreState {
  
  private final String id_;
  private final Position pos_;
  private final Velocity vel_;

  public TrafficCoreState() {
    id_ = "_NoAc_";
    pos_ = Position.INVALID;
    vel_ = Velocity.INVALID;
  }

  public static final TrafficCoreState INVALID = new TrafficCoreState();

  public TrafficCoreState(String id, Position pos, Velocity vel) {
    id_ = id;
    pos_ = pos;
    vel_ = vel;
  }

  public TrafficCoreState(TrafficCoreState ac) {
    this(ac.id_,ac.pos_,ac.vel_);
  }

  public boolean isValid() {
    return !pos_.isInvalid() && !vel_.isInvalid();
  }

  public String getId() {
    return id_;
  }

  public boolean isLatLon() {
    return pos_.isLatLon();
  }

  public Position getPosition() {
    return pos_;
  }

  public Velocity getVelocity() {
    return vel_;
  }

  /**
   *  Returns current track in internal units [0 - 2pi] [rad] (clock wise with respect to North)
   */
  public double track() {
    return vel_.compassAngle();
  }

  /**
   *  Returns current track in given units [0 - 2pi] [u] (clock wise with respect to North)
   */
  public double track(String utrk) {
    return vel_.compassAngle(utrk);
  }

  /** 
   * Returns current ground speed in internal units
   */
  public double groundSpeed() {
    return vel_.gs();
  }

  /** 
   * Returns current ground speed in given units
   */
  public double groundSpeed(String ugs) {
    return vel_.groundSpeed(ugs);
  }

  /** 
   * Returns current vertical speed in internal units
   */
  public double verticalSpeed() {
    return vel_.vs();
  }

  /** 
   * Returns current vertical speed in given units
   */
  public double verticalSpeed(String uvs) {
    return vel_.verticalSpeed(uvs);
  }

  /** 
   * Returns current altitude in internal units
   */
  public double altitude() {
    return pos_.alt(); 
  }

  /** 
   * Returns current altitude in given units
   */
  public double altitude(String ualt) {
    return Units.to(ualt,pos_.alt()); 
  }

  public boolean sameId(TrafficCoreState ac) {
    return isValid() && ac.isValid() && id_.equals(ac.id_);
  }

  public String toString() {
    return "("+id_+", "+pos_.toString()+", "+vel_.toString()+")";
  }

}
