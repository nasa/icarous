#include "Daidalus.h"

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

void printContours(Daidalus& daa) {
	std::vector< std::vector<Position> > blobs = std::vector< std::vector<Position> >();
	// Aircraft at index 0 is ownship
	for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
		// Compute all contours
		daa.horizontalContours(blobs,ac_idx);
		for (unsigned int i=0; i < blobs.size(); ++i) {
			std::cout << "Counter-clockwise Conflict Contour " << i << " with Aircraft " << daa.getAircraftState(ac_idx).getId() << ": " << std::endl;
			for (unsigned int i=0; i < blobs[0].size(); ++i) {
				std::cout << blobs[0][i].toString() << " ";
			}
			std::cout << std::endl;
		}
	}
}

int main(int argc, char* argv[]) {
	std::cout << "##" << std::endl;
	std::cout << "## " << Daidalus::release() << std::endl;
	std::cout << "##\n" << std::endl;

	// Create an object of type Daidalus for a well-clear volume based on TAUMOD
	Daidalus daa;

	// By default, DAIDALUS is configured to unbuffered WC, instantaneous bands.
	// A Daidulos object can be configure either programatically or by using a configuration file.
	for (int a=1;a < argc; ++a) {
		std::string arga = argv[a];
		if (arga == "--noma") {
			// Configure DAIDALUS to nominal A parameters: Kinematic Bands, Turn Rate 1.5 [deg/s])
			daa.set_Buffered_WC_SC_228_MOPS(false);
		} else if (arga == "--nomb") {
			// Configure DAIDALUS to nominal A parameters: Kinematic Bands, Turn Rate 3.0 [deg/s])
			daa.set_Buffered_WC_SC_228_MOPS(true);
		} else if (arga == "--conf" && a+1 < argc) {
			// Load configuration file
			arga = argv[++a];
			if (!daa.parameters.loadFromFile(arga)) {
				std::cerr << "File " << arga << "not found" << std::endl;
				exit(0);
			} else {
				std::cout << "Loading configuration file " << arga << std::endl;
			}
		} else {
			std::cerr << "Unknown option " << arga << std::endl;
			exit(0);
		}
	}

	// Save current parameters
	std::string parameters = "parameters.txt";
	daa.parameters.saveToFile(parameters);
	std::cout << "Current parameters written to file " << parameters << std::endl << std::endl;

	double t = 0.0;
	// for all times t (in this example, only one time step is illustrated)
	// Add ownship state at time t
	Position so = Position::makeLatLonAlt(33.95,"deg", -96.7,"deg", 8700.0,"ft");
	Velocity vo = Velocity::makeTrkGsVs(206.0,"deg", 151.0,"knot", 0.0,"fpm");
	daa.setOwnshipState("ownship",so,vo,t);

	// Add all traffic states at time t
	// ... some traffic ...
	Position si = Position::makeLatLonAlt(33.86191658,"deg", -96.73272601,"deg", 9000.0,"ft");
	Velocity vi = Velocity::makeTrkGsVs(0.0,"deg", 210.0,"knot", 0,"fpm");
	daa.addTrafficState("ith-intruder",si,vi);
	// ... more traffic ...

	// Set wind information
	Velocity wind = Velocity::makeTrkGsVs(45,"deg", 10,"knot", 0,"fpm");
	daa.setWindField(wind);

	// Print Daidalus Object
	std::cout << daa.toString() << std::endl;

	// Print information about the Daidalus Object
	std::cout << "Number of Aircraft: " << daa.numberOfAircraft() << std::endl;
	std::cout << "Last Aircraft Index: " << daa.lastTrafficIndex() << std::endl;
	std::cout <<  std::endl;

	// Detect conflicts with every traffic aircraft
	printDetection(daa);

	// Call alerting logic for each traffic aircraft.
	printAlerts(daa);

	// Define multi bands object
	KinematicMultiBands bands;

	// Compute kinematic bands
	daa.kinematicMultiBands(bands);
	printBands(daa,bands);

	// Print points of well-clear violation contours, i.e., blobs
	printContours(daa);
	// continue with next time step

}

