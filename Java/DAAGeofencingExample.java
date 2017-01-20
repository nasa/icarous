/* DAA and Geofencing Example example integrating
 *   - DAIDALUS for detect and avoid.
 *   - Polycarp for geofence containment checks.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ACCoRD.*;
import java.util.*;

public class DAAGeofencingExample{
    public static void main(String args[]) {

	System.out.println("##");
	System.out.println("## DAA and Geofencing Provided by ICAROUSj@FormalATM"+Constants.version);
	System.out.println("##\n");

	/** Detect and Avoid **/
	
	// Create an empty DAIDALUS object
	Daidalus daa = new Daidalus();

	// Load parameters for a small UAS
	daa.parameters.loadFromFile("params/DaidalusQuadConfig.txt");

	double t = 0.0;
	// for all times t (in this example, only one time step is illustrated)
	  // Add ownship state at time t
	  Position so = Position.makeLatLonAlt(37.102456,"deg", -76.387094,"deg", 16.4,"ft");	
	  Velocity vo = Velocity.makeTrkGsVs(90.0,"deg", 2.5,"kts", 0.0,"fpm");
	  daa.setOwnshipState("ownship",so,vo,t);

	  // Add all traffic states at time t
	  // ... some traffic ...
	  Position si = Position.makeLatLonAlt(37.102450,"deg", -76.386889,"deg", 16.4,"ft"); 
	  Velocity vi = Velocity.makeTrkGsVs(270.0,"deg", 1.0,"kts", 0.0,"fpm"); 
	  daa.addTrafficState("ith-intruder",si,vi);
	  // ... more traffic ...

	  // Set wind information
	  Velocity wind = Velocity.makeTrkGsVs(90,"deg", 1,"knot", 0,"fpm");
	  daa.setWindField(wind);
	
	  // Print information about the Daidalus Object
	  System.out.println("Number of Aircraft: "+daa.numberOfAircraft());
	  System.out.println("Last Aircraft Index: "+daa.lastTrafficIndex());
	  System.out.println();
	
	  // Check time to violation
	  printTimeToViolation(daa);

	  // Call alerting logic for each traffic aircraft.
	  printAlerts(daa);

	  // Compute resolution bands 
	  KinematicMultiBands bands = daa.getKinematicMultiBands();

	  // Print track, ground speed, vertical speed and altitude bands
	  printBands(daa,bands);
        // continue with next time step

        /** Geofencing **/
	
        // Add geofence
        double floor     = 0;  // 0 m
        double ceiling   = 10; // 10 m

        // Make a geofence with 4 vertices (keep in fence)
        SimplePoly geoPolyLLA1     = new SimplePoly(floor,ceiling);
        geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102545,"deg",-76.387213,"deg",0,"m"));
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102344,"deg",-76.387163,"deg",0,"m"));
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102351,"deg",-76.386844,"deg",0,"m"));
	geoPolyLLA1.addVertex(Position.makeLatLonAlt(37.102575,"deg",-76.386962,"deg",0,"m"));

	// Polycarp objects for geofence containment detection and recovery point calculation
	CDPolycarp geoPolyCarp    = new CDPolycarp();	
	PolycarpResolution geoRes = new PolycarpResolution();

	// Project geofence vertices to a local euclidean coordinate system to use with polycarp functions
	EuclideanProjection proj  = Projection.createProjection(geoPolyLLA1.getVertex(0));
	Poly3D geoPoly3D1         = geoPolyCarp.makeNicePolygon(geoPolyLLA1.poly3D(proj));
	
	// Check if ownship violates geofence (use definitelyInside(..) or definitelyOutside(..)
	// based on geofence type [ keep in vs keep out] )
	Vect3 so_3 = proj.project(so); // Project ownship position into local euclidean frame
	if (geoPolyCarp.definitelyInside(so_3,geoPoly3D1)) {
	    System.out.println("Definitely inside of keep in fence");
	} else {
	    System.out.println("Definitely outside of keep out fence");
	}
	
	//Check if ownship is near any edge (nearness is defined based on horizontal and vertical thresholds)
	double hthreshold = 1; // 1 m
	double vthreshold = 1; // 1 m	
	if (geoPolyCarp.nearEdge(so_3,geoPoly3D1,hthreshold,vthreshold)) {
	    System.out.println("Ownship is near geofence edge");

	    // Compute a safe point to goto
	    double BUFF = 0.1; // Buffer used for numerical stability within polycarp
	    Vect2 so_2  = so_3.vect2(); // 2D projection of ownship position

	    ArrayList<Vect2> fenceVertices = new ArrayList<Vect2>();
	    for(int i=0;i<4;i++) {
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
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102247,"deg",-76.387269,"deg",0,"m"));
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102017,"deg",-76.387266,"deg",0,"m"));
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102060,"deg",-76.386997,"deg",0,"m"));
	geoPolyLLA2.addVertex(Position.makeLatLonAlt(37.102256,"deg",-76.387010,"deg",0,"m"));

	
	// Project geofence vertices to a local euclidean coordinate system to use with polycarp functions	
	Poly3D geoPoly3D2 = geoPolyCarp.makeNicePolygon(geoPolyLLA2.poly3D(proj));
	
	// Check if ownship violates geofence (use definitelyInside(..) or definitelyOutside(..)
	// based on geofence type [ keep in vs keep out] )
	if (geoPolyCarp.definitelyInside(so_3,geoPoly3D2)) {
	    System.out.println("Definitely inside of keep out fence");
	} else {
	    System.out.println("Definitely outside of keep out fence");
	}			       	
    }

    static void printTimeToViolation(Daidalus daa) {
	// Aircraft at index 0 is ownship
	for (int ac=1; ac < daa.numberOfAircraft(); ++ac) {
	    double t2los = daa.timeToViolation(ac);
	    if (t2los >= 0) {
		System.out.println("Predicted time to violation with "+
				   daa.getAircraftState(ac).getId()+": "+
				   f.Fm2(t2los)+" [s]");
	    }
	}
    }
    
    static void printAlerts(Daidalus daa) {
	// Aircraft at index 0 is ownship
	for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
	    int alert = daa.alerting(ac_idx);
	    if (alert > 0) {
		System.out.println("Alert Level "+alert+" with "+daa.getAircraftState(ac_idx).getId());
	    }
	}
    }

    // Converts numbers, possible NaN or infinities, to string
    static String num2str(double res, String u) {
	if (Double.isNaN(res)) {
	    return "N/A";
	} else if (!Double.isFinite(res)) {
	    return "None";
	} else {
	    return f.Fm2(res)+" ["+u+"]";
	}
    }
    
    static void printBands(Daidalus daa, KinematicMultiBands bands) {
	boolean nowind = daa.getWindField().isZero();
	TrafficState own = daa.getOwnshipState();
	String trkstr = nowind ? "Track" : "Heading";
	String gsstr = nowind ? "Ground Speed" : "Airspeed";
	System.out.println();
	
	for (int alert_level = 1; alert_level <= daa.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
	    System.out.println("Conflict Aircraft for Alert Level "+alert_level+": "+
			       TrafficState.listToString(bands.conflictAircraft(alert_level)));
	}
	
	System.out.println();
	
	// Track/Heading
	double trk_deg = own.track("deg");
	System.out.println("Ownship "+trkstr+": "+f.Fm2(trk_deg)+" [deg]");
	System.out.println("Region of Current "+trkstr+": "+
			   bands.regionOfTrack(trk_deg,"deg").toString());
	System.out.println(trkstr+" Bands [deg,deg]"); 
	for (int i=0; i < bands.trackLength(); ++i) {
	    Interval ii = bands.track(i,"deg");
	    System.out.println("  "+bands.trackRegion(i)+":\t"+ii.toString(2));
	} 
	System.out.println(trkstr+" Resolution (right): "+num2str(bands.trackResolution(true,"deg"),"deg"));
	System.out.println(trkstr+" Resolution (left): "+num2str(bands.trackResolution(false,"deg"),"deg"));
	System.out.print("Preferred "+trkstr+" Direction: ");
	if (bands.preferredTrackDirection()) { 
	    System.out.println("right");
	} else {
	    System.out.println("left");
	}
	System.out.println("Time to "+trkstr+" Recovery: "+num2str(bands.timeToTrackRecovery(),"s"));
	
	// Ground Speed/Air Speed
	double gs_knot = own.groundSpeed("knot");
	System.out.println("Ownship "+gsstr+": "+f.Fm2(gs_knot)+" [knot]");
	System.out.println("Region of Current "+gsstr+": "+
			   bands.regionOfGroundSpeed(gs_knot,"knot").toString());
	System.out.println(gsstr+" Bands [knot,knot]:");
	for (int i=0; i < bands.groundSpeedLength(); ++i) {
	    Interval ii = bands.groundSpeed(i,"knot");
	    System.out.println("  "+bands.groundSpeedRegion(i)+":\t"+ii.toString(2));
	} 
	System.out.println(gsstr+" Resolution (up): "+num2str(bands.groundSpeedResolution(true,"knot"),"knot"));
	System.out.println(gsstr+" Resolution (down): "+num2str(bands.groundSpeedResolution(false,"knot"),"knot"));
	System.out.print("Preferred "+gsstr+" Direction: ");
	if (bands.preferredGroundSpeedDirection()) { 
	    System.out.println("up");
	} else {
	    System.out.println("down");
	}
	System.out.println("Time to "+gsstr+" Recovery: "+num2str(bands.timeToGroundSpeedRecovery(),"s"));
	
	// Vertical Speed
	double vs_fpm = own.verticalSpeed("fpm");
	System.out.println("Ownship Vertical Speed: "+f.Fm2(vs_fpm)+" [fpm]");
	System.out.println("Region of Current Vertical Speed: "+
			   bands.regionOfVerticalSpeed(vs_fpm,"fpm").toString());
	System.out.println("Vertical Speed Bands [fpm,fpm]:");
	for (int i=0; i < bands.verticalSpeedLength(); ++i) {
	    Interval ii = bands.verticalSpeed(i,"fpm");
	    System.out.println("  "+bands.verticalSpeedRegion(i)+":\t"+ii.toString(2));
	} 
	System.out.println("Vertical Speed Resolution (up): "+num2str(bands.verticalSpeedResolution(true,"fpm"),"fpm"));
	System.out.println("Vertical Speed Resolution (down): "+num2str(bands.verticalSpeedResolution(false,"fpm"),"fpm"));
	System.out.print("Preferred Vertical Speed Direction: ");
	if (bands.preferredVerticalSpeedDirection()) { 
	    System.out.println("up");
	} else {
	    System.out.println("down");
	}
	System.out.println("Time to Vertical Speed Recovery: "+num2str(bands.timeToVerticalSpeedRecovery(),"s"));
	
	// Altitude
	double alt_ft =  own.altitude("ft");
	System.out.println("Ownship Altitude: "+f.Fm2(alt_ft)+" [ft]");
	System.out.println("Region of Current Altitude: "+bands.regionOfAltitude(alt_ft,"ft").toString());
	System.out.println("Altitude Bands [ft,ft]:");
	for (int i=0; i < bands.altitudeLength(); ++i) {
	    Interval ii = bands.altitude(i,"ft");
	    System.out.println("  "+bands.altitudeRegion(i)+":\t"+ii.toString(2));
	} 
	System.out.println("Altitude Resolution (up): "+num2str(bands.altitudeResolution(true,"ft"),"ft"));
	System.out.println("Altitude Resolution (down): "+num2str(bands.altitudeResolution(false,"ft"),"ft"));
	System.out.print("Preferred Altitude Direction: ");
	if (bands.preferredAltitudeDirection()) { 
	    System.out.println("up");
	} else {
	    System.out.println("down");
	}
	System.out.println("Time to Altitude Recovery: "+num2str(bands.timeToAltitudeRecovery(),"s"));
	System.out.println();
    }

}
