/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * This class creates a local Euclidean projection around a given point.  This projection may be used to
 * transform geodesic coordinates (LatLonAlt objects) into this Euclidean frame, using the project() method.  Also points
 * within this frame, may be found in geodesic coordinates with the inverse() method.
 * 
 * This projection uses a relatively fast approximate projection that relies on the average distances between latitude and longitude points.
 * It is not very accurate in most cases, and should not be used for precise measurements; it is intended primarily for
 * rough display purposes, which mostly use the included static functions (which assume a zero projection point) 
 * to approximate the size of circles/ellipses. 
 * Velocities are not currently translated.
 *
 * Note: projection objects should never be made directly, and instead should be retrieved via Projection.getProjection()
 */
public class FastApproximateProjection implements EuclideanProjection {

	public static double simpleConvToY(double lat, double lon) {
		return Units.from("nm",(60.0*Units.to("deg",lat)));
	}

	public static double simpleConvToLat(double x, double y) {
		return Units.from("deg",Units.to("nm",y)/60.0);
	}

	public static double simpleConvToX(double lat, double lon) {
		double deg_lat = Units.to("deg",lat);
		double rad = deg_lat/57.3;
		double deg_lon = Units.to("deg",lon);
		return Units.from("nm",60.0*deg_lon*Math.cos(rad));
	}

	public static double simpleConvToLon(double x, double y) {
		double deg_lat = Units.to("deg",simpleConvToLat(x,y));
		double rad = deg_lat/57.3;
		return Units.from("deg",Units.to("nm",x)/(60.0*Math.cos(rad)));
	}

	private LatLonAlt ref = LatLonAlt.ZERO;

	public FastApproximateProjection(double lat, double lon, double alt) {
		ref = LatLonAlt.mk(lat,lon,alt);
	}

	public FastApproximateProjection(LatLonAlt lla) {
		ref = lla;
	}
	
	
	@Override
	public EuclideanProjection makeNew(LatLonAlt lla) {
		return new FastApproximateProjection(lla);
	}

	@Override
	public EuclideanProjection makeNew(double lat, double lon, double alt) {
		return new FastApproximateProjection(LatLonAlt.mk(lat,lon,alt));
	}

	@Override
	public double conflictRange(double lat, double accuracy) {
		return Units.from("nmi", 20);
	}

	@Override
	public double maxRange() {
		return Double.MAX_VALUE;	}

	@Override
	public LatLonAlt getProjectionPoint() {
		return ref;
	}

	@Override
	public Vect2 project2(LatLonAlt lla) {
		return new Vect2(simpleConvToX(lla.lat()-ref.lat(), lla.lon()-ref.lon()), simpleConvToY(lla.lat()-ref.lat(), lla.lon()-ref.lon()));
	}

	@Override
	public Vect3 project(LatLonAlt lla) {
		return new Vect3(simpleConvToX(lla.lat()-ref.lat(), lla.lon()-ref.lon()), simpleConvToY(lla.lat()-ref.lat(), lla.lon()-ref.lon()), lla.alt()-ref.alt());
	}

	@Override
	public LatLonAlt inverse(Vect3 xyz) {
		return LatLonAlt.mk(simpleConvToLat(xyz.x(),xyz.y())+ref.lat(), simpleConvToLon(xyz.x(),xyz.y())+ref.lon(), xyz.z()+ref.alt());
	}

	@Override
	public LatLonAlt inverse(Vect2 xy, double alt) {
		return inverse(new Vect3(xy,alt));
	}

	@Override
	public Vect3 project(Position sip) {
		if (sip.isLatLon()) {
			return project(sip.lla());
		} else {
			return sip.point();
		}
	}

	@Override
	public Point projectPoint(Position sip) {
		if (sip.isLatLon()) {
			return Point.mk(project(sip.lla()));
		} else {
			return sip.point();
		}
	}

	@Override
	public Velocity projectVelocity(LatLonAlt lla, Velocity v) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Velocity projectVelocity(Position ss, Velocity v) {
		return v;
	}

	@Override
	public Velocity inverseVelocity(Vect3 s, Velocity v, boolean toLatLon) {
		return v;
	}

	@Override
	public Pair<Vect3, Velocity> project(Position p, Velocity v) {
		return Pair.make(project(p), v);
	}

	@Override
	public Pair<Position, Velocity> inverse(Vect3 p, Velocity v, boolean toLatLon) {
		if (toLatLon) {
			return Pair.make(new Position(inverse(p)), v);
		} else {
			return Pair.make(new Position(p), v);
		}
	}

}
