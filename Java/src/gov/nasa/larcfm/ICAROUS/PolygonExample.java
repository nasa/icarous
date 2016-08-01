package gov.nasa.larcfm.ICAROUS;

import gov.nasa.larcfm.ACCoRD.CDPolycarp;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Poly3D;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;


/***
 * This is just an example class, not intended to actually be used!!!
 * 
 * Initialize detection object
 * 1. Create the detection object (constructor here)
 * 
 * For each polygon, get Poly3D and projection:
 * 1. Get Poly3D and its projection (getPoly3D)
 * 2. check if polygon is nice (isNicePolygon)
 * 
 * For each definitely inside check:
 * 1. get ownship current projected position (getOwnship)
 * 2. check inside with projected ownship info and poly3d (definitelyInside)
 * 
 * For near edge warning:
 * 1. get ownship current projected position (getOwnship)
 * 2. check near edge with projected ownship info and poly3d (nearEdge)
 * 
 */
public class PolygonExample {

	CDPolycarp cd;
	
	public PolygonExample() {
		cd = new CDPolycarp();
		cd.setIdentifier("ICAROUS_Polycarp");
		CDPolycarp.setCheckNice(false);
	}
	
	public Pair<Poly3D, EuclideanProjection> getPoly3D(SimplePoly p) {
		EuclideanProjection proj = Projection.createProjection(p.centroid());
		Poly3D poly = p.poly3D(proj);
		return Pair.make(poly, proj);
	}

	public boolean isNicePolygon(Poly3D poly) {
		return cd.isNicePolygon(poly);
	}

	public Vect3 getOwnship(Position p, EuclideanProjection proj) {
		return proj.project(p);
	}
	
	public boolean definitelyInside(Vect3 so, Poly3D poly) {
		return cd.definitelyInside(so, poly);
	}

	public boolean nearEdge(Vect3 so, Poly3D poly, double h, double v) {
		return cd.nearEdge(so, poly, h, v);
	}

}
