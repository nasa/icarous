/* Priority Rules Computation  
 * Authors: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Priority.h"
#include "Vect3.h"
//#include "UnitSymbols.h"
#include "PriorityCore.h"
#include "Projection.h"
#include "EuclideanProjection.h"

namespace larcfm {

        double PriorityCore::TC = Units::from("s", 30);
		double PriorityCore::CRUISE_DEF = Units::from("fpm", 150.0);
		double PriorityCore::VERTICAL_DEADBAND = Units::from("fpm", 20.0);
		double PriorityCore::LATERAL_DEADBAND = Units::from("kn", 1.0);
		double PriorityCore::LATERAL_RELATIVE_DEADBAND = Units::from("kn", 1.0);
	    double Priority::TIME_MANEUVER_RESTRICTION = Units::from("s", 0.0);
	    double Priority::TIME_PROXIMITY = Units::from("s", 0.0);
	    double Priority::TIME_LOOKAHEAD = Units::from("s", 0.0);

	
	bool Priority::lateral_only(
			double sox, double soy, double soz, 
			double vox, double voy, double voz, 
			double six, double siy, double siz,
			double vix, double viy, double viz ) { 
	 
	   return PriorityCore::lateral_only(
			   Vect3(Units::from("nm",sox),  Units::from("nm",soy),  Units::from("ft",soz)),
			   Vect3(Units::from("kn",vox), Units::from("kn",voy), Units::from("fpm",voz)),
			   Vect3(Units::from("nm",six),  Units::from("nm",siy),  Units::from("ft",siz)),
			   Vect3(Units::from("kn",vix), Units::from("kn",viy), Units::from("fpm",viz)));
	}
	
	bool Priority::check(
		double sox, double soy, double soz, 
		double vox, double voy, double voz,
		bool mo,
		double six, double siy, double siz,
		double vix, double viy, double viz,
		bool mi) {
		
		return PriorityCore::check(
		   Vect3(Units::from("nm",sox),  Units::from("nm",soy),  Units::from("ft",soz)),
		   Vect3(Units::from("kn",vox), Units::from("kn",voy), Units::from("fpm",voz)),
		   mo,
		   Vect3(Units::from("nm",six),  Units::from("nm",siy),  Units::from("ft",siz)),
		   Vect3(Units::from("kn",vix), Units::from("kn",viy), Units::from("fpm",viz)),
		   mi);
	}
	
    bool Priority::checkLL(
            double lato, double lono, double alto,
            double trko, double gso, double vso,
            bool mo,
            double lati, double loni, double alti,
            double trki, double gsi, double vsi,
            bool mi) {

        Position po(LatLonAlt::make(lato,lono,alto));
        Velocity vo = Velocity::makeTrkGsVs(trko,"deg", gso,"kn", vso,"fpm");
        Position pi(LatLonAlt::make(lati,loni,alti));
        Velocity vi = Velocity::makeTrkGsVs(trki,"deg", gsi,"kn", vsi,"fpm");

        return check(po, vo, mo, pi, vi, mi);
    }

    bool Priority::check(
            const Position& po, const Velocity& vo, bool mo,
            const Position& pi, const Velocity& vi, bool mi) {

        if (po.isLatLon()) {
            EuclideanProjection sp = Projection::createProjection(po.lla());

            return PriorityCore::check(
                    Vect3(0.0,0.0,0.0), vo, mo,
                    sp.project(pi.lla()), sp.projectVelocity(pi.lla(), vi), mi);
        } else {
            return PriorityCore::check(po.point(), vo, mo, pi.point(), vi, mi);
        }
    }
}
