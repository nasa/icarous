/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.List;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

/** Horizontal solution */
public class TrafficState {

	private final EuclideanProjection eprj_; // Projection 
	private final String id_;
	private final Position pos_;
	private final Velocity vel_;
	private final Position posxyz_; // Projected position
	private final Velocity velxyz_; // Projected velocity

	public TrafficState() {
		id_ = "_NoAc_";
		pos_  = Position.INVALID;
		vel_  = Velocity.INVALID;
		posxyz_ = Position.INVALID;
		velxyz_ = Velocity.INVALID;
		eprj_ = Projection.createProjection(Position.ZERO_LL);
	}

	public static final TrafficState INVALID = new TrafficState();

	public static final List<TrafficState> INVALIDL = new ArrayList<TrafficState>();

	private TrafficState(String id, Position pos, Velocity vel, EuclideanProjection eprj) {
		id_ = id;
		pos_ = pos;
		vel_ = vel;
		if (pos.isLatLon()) {
			Vect3 s = eprj.project(pos);
			Velocity v = eprj.projectVelocity(pos,vel);
			posxyz_ = new Position(s);
			velxyz_ = Velocity.make(v);
		} else {
			posxyz_ = pos;
			velxyz_ = vel;
		}  
		eprj_ = eprj;
	}

	public static TrafficState makeOwnship(String id, Position pos, Velocity vel) {
		return new TrafficState(id,pos,vel,pos.isLatLon()?Projection.createProjection(pos.lla().zeroAlt()):
			Projection.createProjection(Position.ZERO_LL));
	}

	public TrafficState makeIntruder(String id, Position pos, Velocity vel) {
		if (pos_.isLatLon() != pos.isLatLon()) { 
			return INVALID;
		}
		return new TrafficState(id,pos,vel,eprj_);
	}

	public Vect3 get_s() {
		return posxyz_.point();
	}

	public Velocity get_v() {
		return velxyz_;
	}

	public Vect3 pos_to_s(Position p) {
		if (p.isLatLon()) {
			if (!pos_.isLatLon()) {
				//        Thread.dumpStack();        
				return Vect3.INVALID;
			}
			return eprj_.project(p);
		} 
		return p.point();
	}

	public Velocity vel_to_v(Position p,Velocity v) {
		if (p.isLatLon()) {
			if (!pos_.isLatLon()) {
				//        Thread.dumpStack();        
				return Velocity.INVALID;
			}     
			return eprj_.projectVelocity(p,v);
		} 
		return v;
	}

	public Velocity inverseVelocity(Velocity v) {
		return eprj_.inverseVelocity(get_s(),v,true);
	}

	public TrafficState linearProjection(double offset) {
		return new TrafficState(getId(),pos_.linear(vel_,offset),vel_,eprj_);
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

	public String formattedTraffic(List<TrafficState> traffic, double time,
			String uxy, String ualt, String ugs, String uvs) {
		String s="";
		if (pos_.isLatLon()) {
			s += "NAME lat lon alt trk gs vs time\n";
			s += "[none] [deg] [deg] ["+ualt+"] [deg] ["+ugs+"] ["+uvs+"] [s]\n";
			s += getId()+", "+pos_.lla().toStringNP(ualt,Constants.get_output_precision());
		} else {
			s += "NAME sx sy sz trk gs vs time\n";
			s += "[none] ["+uxy+"] ["+uxy+"] ["+ualt+"] [deg] ["+ugs+"] ["+uvs+"] [s]\n";
			s += getId()+", "+pos_.point().toStringNP(Constants.get_output_precision(),uxy,uxy,ualt);
		}
		s += ", "+vel_.toStringNP("deg",ugs,uvs,Constants.get_output_precision())+", "+
				f.FmPrecision(time,Constants.get_output_precision())+"\n";
		for (TrafficState ac : traffic) {
			s += ac.getId()+", ";
			if (pos_.isLatLon()) {
				s += ac.pos_.lla().toStringNP(ualt,Constants.get_output_precision());
			} else {
				s += ac.pos_.point().toStringNP(Constants.get_output_precision(),uxy,uxy,ualt);
			}
			s += ", "+ac.vel_.toStringNP("deg",ugs,uvs,Constants.get_output_precision())+", "+
					f.FmPrecision(time,Constants.get_output_precision())+"\n";
		}
		return s;
	}

	public String toPVS(int prec) {
		return "(# id := \"" + getId() + "\", s := "+get_s().toPVS(prec)+", v := "+get_v().toPVS(prec)+" #)";   
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

	public boolean isValid() {
		return !pos_.isInvalid() && !vel_.isInvalid();
	}

	public boolean isLatLon() {
		return pos_.isLatLon();
	}

	public String getId() {
		return id_;
	}

	public Position getPosition() {
		return pos_;
	}

	public Velocity getVelocity() {
		return vel_;
	}

	public Position getPositionXYZ() {
		return posxyz_;
	}

	public Velocity getVelocityXYZ() {
		return velxyz_;
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

	public boolean sameId(TrafficState ac) {
		return isValid() && ac.isValid() && id_.equals(ac.id_);
	}

	public String toString() {
		return "("+id_+", "+pos_.toString()+", "+vel_.toString()+")";
	}


}
