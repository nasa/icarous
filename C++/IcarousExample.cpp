/* ICAROUS Example implementation:
 *   - Using DAIDALUS for detect and avoid.
 *   - Using Polycarp for geofence containment checks.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <iostream>
#include <vector>
#include "Daidalus.h"
#include "SimplePoly.h"
#include "CDPolycarp.h"
#include "PolycarpResolution.h"
#include "Projection.h"

using namespace larcfm;

void printDetection(Daidalus& daa) {
	// Aircraft at index 0 is ownship
	for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
		double t2los = daa.timeToViolation(ac_idx);
		if (t2los >= 0) {
			std::cout << "Predicted Time to Loss of Well Clear with " << daa.getAircraftState(ac_idx).getId() << ": " <<
					Fm2(t2los) << " [s]" << std::endl;
		}
	}
}

void printAlerts(Daidalus& daa) {
	// Aircraft at index 0 is ownship
	for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
		int alert = daa.alerting(ac_idx);
		if (alert > 0) {
			std::cout << "Alert Level " << alert << " with " <<
					daa.getAircraftState(ac_idx).getId() << std::endl;
		}
	}
}

// Converts numbers, possible NaN or infinities, to string
static std::string num2str(double res, const std::string& u) {
	if (ISNAN(res)) {
		return "N/A";
	} else if (!ISFINITE(res)) {
		return "None";
	} else {
		return Fm2(res)+" ["+u+"]";
	}
}

void printBands(Daidalus& daa, KinematicMultiBands& bands) {
	bool nowind = daa.getWindField().isZero();
	TrafficState own = daa.getOwnshipState();
	std::string trkstr = nowind ? "Track" : "Heading";
	std::string gsstr = nowind ? "Ground Speed" : "Airspeed";
	std::cout << std::endl;

	for (int alert_level = 1; alert_level <= daa.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
		std::cout << "Conflict Aircraft for Alert Level " << Fmi(alert_level) << ": " <<
				TrafficState::listToString(bands.conflictAircraft(alert_level)) << std::endl;
	}

	std::cout << std::endl;

	// Track/Heading
	double trk_deg = own.track("deg");
	std::cout << "Ownship " << trkstr << ": "+Fm2(trk_deg) << " [deg]" << std::endl;
	std::cout << "Region of Current " << trkstr+": " <<
			BandsRegion::to_string(bands.regionOfTrack(trk_deg,"deg")) << std::endl;
	std::cout << trkstr << " Bands [deg,deg]" << std::endl;
	for (int i=0; i < bands.trackLength(); ++i) {
		Interval ii = bands.track(i,"deg");
		std::cout << "  " << BandsRegion::to_string(bands.trackRegion(i)) << ":\t" << ii.toString(2) << std::endl;
	}
	for (int alert_level = 1; alert_level <= daa.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
		std::cout << "Peripheral " << trkstr << " Aircraft for Alert Level " << Fmi(alert_level) << ": " <<
				TrafficState::listToString(bands.peripheralTrackAircraft(alert_level)) << std::endl;
	}
	std::cout << trkstr << " Resolution (right): " << num2str(bands.trackResolution(true,"deg"),"deg") << std::endl;
	std::cout << trkstr << " Resolution (left): " << num2str(bands.trackResolution(false,"deg"),"deg") << std::endl;
	std::cout << "Preferred "+trkstr+" Direction: ";
	if (bands.preferredTrackDirection()) {
		std::cout << "right" << std::endl;
	} else {
		std::cout << "left" << std::endl;
	}
	std::cout << "Time to " << trkstr << " Recovery: " << num2str(bands.timeToTrackRecovery(),"s") << std::endl;

	// Ground Speed/Air Speed
	double gs_knot = own.groundSpeed("knot");
	std::cout << "Ownship " << gsstr << ": "+Fm2(gs_knot) << " [knot]" << std::endl;
	std::cout << "Region of Current " << gsstr+": " <<
			BandsRegion::to_string(bands.regionOfGroundSpeed(gs_knot,"knot")) << std::endl;
	std::cout << gsstr << " Bands [knot,knot]:" << std::endl;
	for (int i=0; i < bands.groundSpeedLength(); ++i) {
		Interval ii = bands.groundSpeed(i,"knot");
		std::cout << "  " << BandsRegion::to_string(bands.groundSpeedRegion(i)) << ":\t" << ii.toString(2) << std::endl;
	}
	for (int alert_level = 1; alert_level <= daa.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
		std::cout << "Peripheral " << gsstr << " Aircraft for Alert Level " << Fmi(alert_level) << ": " <<
				TrafficState::listToString(bands.peripheralGroundSpeedAircraft(alert_level)) << std::endl;
	}
	std::cout << gsstr << " Resolution (up): " << num2str(bands.groundSpeedResolution(true,"knot"),"knot") << std::endl;
	std::cout << gsstr << " Resolution (down): " << num2str(bands.groundSpeedResolution(false,"knot"),"knot") << std::endl;
	std::cout << "Preferred "+gsstr+" Direction: ";
	if (bands.preferredGroundSpeedDirection()) {
		std::cout << "up" << std::endl;
	} else {
		std::cout << "down" << std::endl;
	}
	std::cout << "Time to " << gsstr << " Recovery: " << num2str(bands.timeToGroundSpeedRecovery(),"s") << std::endl;

	// Vertical Speed
	double vs_fpm = own.verticalSpeed("fpm");
	std::cout << "Ownship Vertical Speed: "+Fm2(vs_fpm) << " [fpm]" << std::endl;
	std::cout << "Region of Current Vertical Speed: " <<
			BandsRegion::to_string(bands.regionOfVerticalSpeed(vs_fpm,"fpm")) << std::endl;
	std::cout << "Vertical Speed Bands [fpm,fpm]:" << std::endl;
	for (int i=0; i < bands.verticalSpeedLength();  ++i) {
		Interval ii = bands.verticalSpeed(i,"fpm");
		std::cout << "  " << BandsRegion::to_string(bands.verticalSpeedRegion(i)) << ":\t" << ii.toString(2) << std::endl;
	}
	for (int alert_level = 1; alert_level <= daa.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
		std::cout << "Peripheral Vertical Speed Aircraft for Alert Level " << Fmi(alert_level) << ": " <<
				TrafficState::listToString(bands.peripheralVerticalSpeedAircraft(alert_level)) << std::endl;
	}
	std::cout << "Vertical Speed Resolution (up): " << num2str(bands.verticalSpeedResolution(true,"fpm"),"fpm") << std::endl;
	std::cout << "Vertical Speed Resolution (down): " << num2str(bands.verticalSpeedResolution(false,"fpm"),"fpm") << std::endl;
	std::cout << "Preferred Vertical Speed Direction: ";
	if (bands.preferredVerticalSpeedDirection()) {
		std::cout << "up" << std::endl;
	} else {
		std::cout << "down" << std::endl;
	}
	std::cout << "Time to Vertical Speed Recovery: " << num2str(bands.timeToVerticalSpeedRecovery(),"s") << std::endl;

	// Altitude
	double alt_ft = own.altitude("ft");
	std::cout << "Ownship Altitude: "+Fm2(alt_ft) << " [ft]" << std::endl;
	std::cout << "Region of Current Altitude: " <<
			BandsRegion::to_string(bands.regionOfAltitude(alt_ft,"ft")) << std::endl;
	std::cout << "Altitude Bands [ft,ft]:" << std::endl;
	for (int i=0; i < bands.altitudeLength(); ++i) {
		Interval ii = bands.altitude(i,"ft");
		std::cout << "  " << BandsRegion::to_string(bands.altitudeRegion(i)) << ":\t" << ii.toString(2) << std::endl;
	}
	for (int alert_level = 1; alert_level <= daa.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
		std::cout << "Peripheral Altitude Aircraft for Alert Level " << Fmi(alert_level) << ": " <<
				TrafficState::listToString(bands.peripheralAltitudeAircraft(alert_level)) << std::endl;
	}
	std::cout << "Altitude Resolution (up): " << num2str(bands.altitudeResolution(true,"ft"),"ft") << std::endl;
	std::cout << "Altitude Resolution (down): " << num2str(bands.altitudeResolution(false,"ft"),"ft") << std::endl;
	std::cout << "Preferred Altitude Direction: ";
	if (bands.preferredAltitudeDirection()) {
		std::cout << "up" << std::endl;
	} else {
		std::cout << "down" << std::endl;
	}
	std::cout << "Time to Altitude Recovery: " << num2str(bands.timeToAltitudeRecovery(),"s") << std::endl;
	std::cout << std::endl;

	// Last times to maneuver
	for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
		TrafficState ac = daa.getAircraftState(ac_idx);
		std::cout << "Last Times to Maneuver with Respect to " << ac.getId() << ":" << std::endl;
		std::cout << "  "+trkstr+" Maneuver: "+num2str(bands.lastTimeToTrackManeuver(ac),"s") << std::endl;
		std::cout << "  "+gsstr+" Maneuver: "+num2str(bands.lastTimeToGroundSpeedManeuver(ac),"s") << std::endl;
		std::cout <<"  Vertical Speed Maneuver: "+num2str(bands.lastTimeToVerticalSpeedManeuver(ac),"s") << std::endl;
		std::cout <<"  Altitude Maneuver: "+num2str(bands.lastTimeToAltitudeManeuver(ac),"s") << std::endl;
	}
	std::cout << std::endl;

}

int main(int argc,char* argv[]){

  // Create a DAIDALUS object
  Daidalus daa;
  
  daa.parameters.loadFromFile("../DaidalusConfigurations/DaidalusSmallUAS.txt");

  // Position and velocity of ownship
  Position so = Position::makeLatLonAlt(37.102456,"deg", -76.387094,"deg", 16.4,"ft");
  Velocity vo = Velocity::makeTrkGsVs(90.0,"deg", 2.5,"kts", 0.0,"fpm");

  // Position and velocity of intruder
  Position si = Position::makeLatLonAlt(37.102450,"deg", -76.386889,"deg", 16.4,"ft");
  Velocity vi = Velocity::makeTrkGsVs(270.0,"deg", 1.0,"kts", 0.0,"fpm");

  // Add new plans
  daa.setOwnshipState("ownship",so,vo,0.0);
  daa.addTrafficState("intruder",si,vi);

  // Set wind information
  Velocity wind = Velocity::makeTrkGsVs(90,"deg", 1,"knot", 0,"fpm");
  daa.setWindField(wind);

  printDetection(daa);

  KinematicMultiBands bands;
  daa.kinematicMultiBands(bands);

  printBands(daa,bands);

  // Add geofence
  double floor     = 0;  // 0 m
  double ceiling   = 10; // 10 m
  
  // Make a geofence with 4 vertices
  SimplePoly geoPolyLLA(floor,ceiling);
  geoPolyLLA.addVertex(Position::makeLatLonAlt(37.102545,"degree",-76.387213,"degree",0,"m"));
  geoPolyLLA.addVertex(Position::makeLatLonAlt(37.102344,"degree",-76.387163,"degree",0,"m"));
  geoPolyLLA.addVertex(Position::makeLatLonAlt(37.102351,"degree",-76.386844,"degree",0,"m"));
  geoPolyLLA.addVertex(Position::makeLatLonAlt(37.102575,"degree",-76.386962,"degree",0,"m"));

  CDPolycarp geoPolyCarp;
  PolycarpResolution geoRes;
  
  // Project geofence vertices to a local euclidean coordinate system to use with polycarp functions
  EuclideanProjection proj  = Projection::createProjection(geoPolyLLA.getVertex(0));
  Poly3D geoPoly3D          = geoPolyLLA.poly3D(proj);

  Vect3 so_3 = proj.project(so); // Project ownship position into local euclidean frame
  if(geoPolyCarp.definitelyInside(so_3,geoPoly3D)){
    std::cout<<"Definitely inside\n";
  }else{
    std::cout<<"Definitely outside\n";
  }

  std::vector<Vect2> fenceVertices;
  fenceVertices.reserve(4);

  for(int i=0;i<4;i++){
    fenceVertices.push_back(geoPoly3D.getVertex(i));
  }

  //Check if ownship is near any edge (nearness is defined based on horizontal and vertical thresholds)
  double hthreshold = 1; // 1 m
  double vthreshold = 1; // 1 m	
  if(geoPolyCarp.nearEdge(so_3,geoPoly3D,hthreshold,vthreshold)){
    std::cout<<"Ownship is near geofence edge";
    
    // Compute a safe point to goto
    double BUFF = 0.1; // Buffer used for numerical stability within polycarp
    Vect2 so_2  = so_3.vect2(); // 2D projection of ownship position        
    
    // Compute safe point
    Vect2 recpoint = geoRes.inside_recovery_point(BUFF,hthreshold,fenceVertices,so_2);
    
    // Convert safe point from euclidean coordinates to Lat, Lon and Alt
    LatLonAlt LLA = proj.inverse(recpoint,so.alt());
    Position RecoveryPoint = Position::makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
    std::cout<<RecoveryPoint.toString();
  }
}
