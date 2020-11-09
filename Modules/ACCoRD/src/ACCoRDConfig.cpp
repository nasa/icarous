/* ====================================================================
 * Authors:      George Hagen, Rick Butler and Jeff Maddalon,
 * Organization: NASA/Langley Research Center
 * Website:      http://shemesh.larc.nasa.gov/fm/
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * ====================================================================
 *
 * ACCoRDConfig 
 *
 */

#include "ACCoRDConfig.h"
#include "Units.h"

namespace larcfm {

const double ACCoRDConfig::minHorizExitSpeedLoS_default = Units::from("kn",100.0);
const double ACCoRDConfig::minVertExitSpeedLoS_default = Units::from("fpm",1499.99999999);
const double ACCoRDConfig::maxGs_default = Units::from("kn",700.0);
const double ACCoRDConfig::minGs_default = Units::from("kn",150.0);       // must be greater than 0
const double ACCoRDConfig::maxVs_default = Units::from("fpm",5000.0);       // must be greater than 0
double ACCoRDConfig::gsSearchLosDiscard = Units::from("nm",1.5);
double ACCoRDConfig::vsDiscretization_default = Units::from("fpm",10.0);

const double ACCoRDConfig::NMAC_D = Units::from("ft",500); // Defined in RTCA SC-147
const double ACCoRDConfig::NMAC_H = Units::from("ft",100); // Defined in RTCA SC-147

// internal units right now
void ACCoRDConfig::setGsSearchLosDiscard(double val) {
  gsSearchLosDiscard = val;
}


}


