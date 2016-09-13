/* ICAROUS Example implementation:
 *   - Using DAIDALUS for detect and avoid.
 *   - Using Polycarp for geofence containment checks.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ACCoRD.*;
import java.util.*;

public class IcarousExample{
    public static void main(String args[]){

	System.out.println("##");
	System.out.println("## ICAROUS");
	System.out.println("##\n");

	/** Detect and Avoid **/
	
	// Create an empty DAIDALUS object
	Daidalus daa = new Daidalus();

	// Load parameters for a small UAS
	daa.parameters.loadFromFile("DaidalusSmallUAS.txt");

	// Position and velocity data for ownship
	Position so = Position.makeLatLonAlt(37.102456,"deg", -76.387094,"deg", 16.4,"ft");	
	Velocity vo = Velocity.makeTrkGsVs(90.0,"deg", 2.5,"kts", 0.0,"fpm");

	// Position and velocity data for intruder(s)
	Position si = Position.makeLatLonAlt(37.102450,"deg", -76.386889,"deg", 16.4,"ft"); 
	Velocity vi = Velocity.makeTrkGsVs(270.0,"deg", 1.0,"kts", 0.0,"fpm"); 

	// Add ownship and traffic data to DAIDALUS object
	daa.setOwnshipState("Ownship",so,vo,0.0);
	daa.addTrafficState("Intruder",si,vi);

	// Set wind information
	Velocity wind = Velocity.makeTrkGsVs(90,"deg", 1,"knot", 0,"fpm");
	daa.setWindField(wind);
	
	// Check time to violation
	printTimeToViolation(daa);

	// Compute resolution bands 
	KinematicMultiBands bands = daa.getKinematicMultiBands();

	// Print track, ground speed, vertical speed and altitude bands
	System.out.print(bands.outputString());	

	/** Geofence **/
	
	// Add geofence
	double floor     = 0;  // 0 m
	double ceiling   = 10; // 10 m

	// Make a geofence with 4 vertices (keep in fence)
	SimplePoly geoPolyLLA1     = new SimplePoly(floor,ceiling);
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102545,"degree",-76.387213,"degree",0,"m"));
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102344,"degree",-76.387163,"degree",0,"m"));
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102351,"degree",-76.386844,"degree",0,"m"));
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102575,"degree",-76.386962,"degree",0,"m"));

	// Polycarp objects for geofence containment detection and recovery point calculation
	CDPolycarp geoPolyCarp    = new CDPolycarp();	
	PolycarpResolution geoRes = new PolycarpResolution();

	// Project geofence vertices to a local euclidean coordinate system to use with polycarp functions
	EuclideanProjection proj  = Projection.createProjection(geoPolyLLA1.getVertex(0));
	Poly3D geoPoly3D1         = geoPolyCarp.makeNicePolygon(geoPolyLLA1.poly3D(proj));
	
	// Check if ownship violates geofence (use definitelyInside(..) or definitelyOutside(..)
	// based on geofence type [ keep in vs keep out] )
	Vect3 so_3 = proj.project(so); // Project ownship position into local euclidean frame
	if(geoPolyCarp.definitelyInside(so_3,geoPoly3D1)){
	    System.out.println("Definitely inside of keep in fence");
	}else{
	    System.out.println("Definitely outside of keep out fence");
	}
	
	//Check if ownship is near any edge (nearness is defined based on horizontal and vertical thresholds)
	double hthreshold = 1; // 1 m
	double vthreshold = 1; // 1 m	
	if(geoPolyCarp.nearEdge(so_3,geoPoly3D1,hthreshold,vthreshold)){
	    System.out.println("Ownship is near geofence edge");

	    // Compute a safe point to goto
	    double BUFF = 0.1; // Buffer used for numerical stability within polycarp
	    Vect2 so_2  = so_3.vect2(); // 2D projection of ownship position

	    ArrayList<Vect2> fenceVertices = new ArrayList<Vect2>();
	    for(int i=0;i<4;i++){
		fenceVertices.add(geoPoly3D1.getVertex(i)); // Euclidean 2D coordinates of fence vertices.
	    }

	    // Compute safe point
	    Vect2 recpoint = geoRes.inside_recovery_point(BUFF,hthreshold,fenceVertices,so_2);

	    // Convert safe point from euclidean coordinates to Lat, Lon and Alt
	    LatLonAlt LLA = proj.inverse(recpoint,so.alt());
	    Position RecoveryPoint = Position.makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
	    System.out.println(RecoveryPoint.toString());
	}

	// Make a geofence with 4 vertices (keep out fence)
	SimplePoly geoPolyLLA2     = new SimplePoly(floor,ceiling);
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102247,"degree",-76.387269,"degree",0,"m"));
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102017,"degree",-76.387266,"degree",0,"m"));
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102060,"degree",-76.386997,"degree",0,"m"));
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102256,"degree",-76.387010,"degree",0,"m"));

	
	// Project geofence vertices to a local euclidean coordinate system to use with polycarp functions	
	Poly3D geoPoly3D2         = geoPolyCarp.makeNicePolygon(geoPolyLLA2.poly3D(proj));
	
	// Check if ownship violates geofence (use definitelyInside(..) or definitelyOutside(..)
	// based on geofence type [ keep in vs keep out] )
	if(geoPolyCarp.definitelyInside(so_3,geoPoly3D2)){
	    System.out.println("Definitely inside of keepout fence");
	}else{
	    System.out.println("Definitely outside of keepout fence");
	}
				
	
    }

    static void printTimeToViolation(Daidalus daa) {
    // Aircraft at index 0 is ownship
	for (int ac=1; ac < daa.numberOfAircraft(); ++ac) {
	    double tlos = daa.timeToViolation(ac);
	    if (tlos >= 0) {
		System.out.printf(
				  "Predicted violation with traffic aircraft %s in %.1f [s]\n",
				  daa.getAircraftState(ac).getId(),tlos);
	    }
	}
	
    }
    
    

}
