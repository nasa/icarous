/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

import java.io.IOException;
import java.io.ObjectStreamException;
import java.io.Serializable;
import java.io.StreamCorruptedException;
import java.util.ArrayList;
import java.util.Arrays;

import com.google.gson.Gson;

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Triple;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

/**
 * Current Messagetypes:
 * 
 */

// this is interpreted in the AircraftCommunications object
public class Message implements Serializable, Cloneable {
	private static final long serialVersionUID = 105240952832374116L;

	public enum Type {PlanUpdate, ContainmentPolygon, AvoidancePolygon};

	final int maxPoints = 200;

	Type type;
	long timeStamp;			// time message created
	double startTime;		// first time in plan
	public String id = ""; // target
	double gs_height; // nominal ground speed for plan or height of polygon above given points
	ArrayList<LatLonAlt> points;	// this encoding may have problems with hovering or vertical movement.  4d points?

	public Message() {} // [CAM] Temporarly added to compile IcarousCodec and IcarousListener
	
	public Message(Type type, long timeStamp, String id, double gs_height, ArrayList<LatLonAlt> points) {
		this.type = type;
		this.timeStamp = timeStamp;
		this.startTime = 0.0; 
		this.id = id;
		this.gs_height = gs_height;	// ground speed (for plan) or height above (for polygon)
		this.points = points;
	}

	/**
	 * Create a new plan message
	 * @param ts timestamp
	 * @param p plan
	 */
	public Message(long ts, Plan p) {
		type = Type.PlanUpdate;
		timeStamp = ts;
		startTime = p.getFirstTime();
		id = p.getName();
		gs_height = p.pathDistance()/(p.getLastTime()-p.getFirstTime());
		points = new ArrayList<LatLonAlt>();
		for (int i = 0; i < p.size(); i++) {
			points.add(p.point(i).lla());
		}
	}

	
	/**
	 * Create a new polygon message
	 * @param ts timestamp
	 * @param name polygon name
	 * @param isContainment true if this is a containment polygon, false if it is an avoidance polygon
	 * @param p polygon
	 */
	public Message(long ts, String name, boolean isContainment, SimplePoly p) {
		type = isContainment ? Type.ContainmentPolygon : Type.AvoidancePolygon;
		timeStamp = ts;
		startTime = 0.0;
		id = name;
		gs_height = p.getTop()-p.getBottom();
		points = new ArrayList<LatLonAlt>();
		for (int i = 0; i < p.size(); i++) {
			points.add(p.getVertex(i).lla());
		}
	}

	public Type getType() {
		return this.type;
	}
	
	public String getName() {
		return id;
	}
	
	public long getTimeStamp() {
		return timeStamp;
	}
	
	/**
	 * Return this message's Plan, or null, if not a Plan type.
	 * @return
	 */
	public Plan getPlan() {
		if (type == Type.PlanUpdate) {
			Plan p = new Plan(id);
			if (points.size() == 0) return p;
			double t = startTime;
			Position pos = new Position(points.get(0));
			p.add(new NavPoint(pos,t));
			for (int i = 1; i < points.size(); i++) {
				Position pos2 = new Position(points.get(i));
				double dist = pos.distanceH(pos2);
				double dt = dist/gs_height;
				t = t + dt;
				p.add(new NavPoint(pos2,t));
				pos = pos2;
			}
			return p;
		}
		return null;
	}

	/**
	 * Return the polygon in this message, or null if this is a plan type.
	 * @return
	 */
	public SimplePoly getPolygon() {
		if (type != Type.PlanUpdate) {
			SimplePoly p = new SimplePoly();
			for (int i = 0; i < points.size(); i++) {
				p.addVertex(new Position(points.get(i)));
			}
			p.setTop(p.getBottom()+gs_height);
			return p;
		}
		return null;
	}

	/**
	 * Only one field will be defined, the others will be null
	 * @return
	 */
	public Triple<Plan,SimplePoly,SimplePoly> getObject() {
		switch (type) {
		case PlanUpdate:
			Plan p = new Plan(id);
			double t0 = 0;
			Position p0 = new Position(points.get(0));
			NavPoint np = new NavPoint(p0, t0);
			p.add(np);
			for (int i = 1; i < points.size(); i++) {
				Position p1 = new Position(points.get(i));
				double d = p0.distanceH(p1);
				t0 = t0 + d/gs_height;
				np = new NavPoint(p1,t0);
				p0 = p1;
				p.add(np);
			}
			return Triple.make(p,null,null);
		case ContainmentPolygon:
			SimplePoly sp1 = new SimplePoly();
			for (int i = 0; i < points.size(); i++) {
				Position p1 = new Position(points.get(i));
				sp1.addVertex(p1);
			}
			return Triple.make(null,sp1,null);
		case AvoidancePolygon:
			SimplePoly sp2 = new SimplePoly();
			for (int i = 0; i < points.size(); i++) {
				Position p1 = new Position(points.get(i));
				sp2.addVertex(p1);
			}
			return Triple.make(null,null,sp2);
		default:
			return Triple.make(null,null,null);
		}
	}
	
	
	@Override
	public String toString() {
		return "Message [maxPoints=" + maxPoints + ", type=" + type
				+ ", timeStamp=" + timeStamp + ", id=" + id + ", gs_height="
				+ gs_height + ", points=" + points + "]";
	}

	public Object clone() {
		return new Message(type, timeStamp, id, gs_height, (ArrayList<LatLonAlt>) points.clone());
	}

	public String toJson() {
		Gson gson = new Gson();
		return gson.toJson(this, this.getClass());
	}
	
	public Message fromJson(String j) {
		Gson gson = new Gson();
		return gson.fromJson(j, this.getClass());
	}
	
	// serialize this object!

	
	private void writeObject(java.io.ObjectOutputStream out) throws IOException {
		out.writeObject(type); // version 2
		out.writeLong(timeStamp);
		out.writeUTF(id);
		out.writeDouble(gs_height);
		out.writeByte(points.size());
		for (int i = 0; i < points.size(); i++) {
			out.writeDouble(points.get(i).lat());
			out.writeDouble(points.get(i).lon());
			out.writeDouble(points.get(i).alt());
		}
	}

	private void readObject(java.io.ObjectInputStream in) throws IOException, ClassNotFoundException {
		type = (Type)in.readObject();
		timeStamp = in.readLong();
		id = in.readUTF();
		gs_height = in.readDouble();
		int sz = in.readByte();
		points = new ArrayList<LatLonAlt>();
		for (int i = 0; i < sz; i++) {
			double lat = in.readDouble();
			double lon = in.readDouble();
			double alt = in.readDouble();
			points.add(LatLonAlt.mk(lat, lon, alt));
		}
	}

	private void readObjectNoData() throws ObjectStreamException {
		throw new StreamCorruptedException("Message.readObjectNoData()");
	}
}
