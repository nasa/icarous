/* ====================================================================
 * Authors:      George Hagen, Rick Butler and Jeff Maddalon,
 * Organization: NASA/Langley Research Center
 * Website:      http://shemesh.larc.nasa.gov/fm/
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * ====================================================================
 */

#ifndef ACCORDCONFIG_H_
#define ACCORDCONFIG_H_

namespace larcfm {

  /**
   * This class captures the default parameters for ACCORD.
   *
   */
  class ACCoRDConfig {
    
    private:

    public:

	  static const double minHorizExitSpeedLoS_default;   //  = Units.from("kn",100.0);
	  static const double minVertExitSpeedLoS_default;
	  static const double maxGs_default;                  //  = Units.from("kn",700.0);
	  static const double minGs_default;                  // = Units.from("kn",150.0);       // must be greater than 0
	  static const double maxVs_default;
	  static double gsSearchLosDiscard;     // = Units.from("nm",1.5);
	  static double vsDiscretization_default; //  = Units::from("fpm",10.0);

	  static const double NMAC_D; // Defined in RTCA SC-147
	  static const double NMAC_H; // Defined in RTCA SC-147

	  static void setLosRepulsiveCrit(bool flag);
	  static void setNewCesarAlgorithm(bool flag);

	  static void setGsSearchLosDiscard(double val);

  }; // ACCORDCONFIG
}

#endif /* ACCORDCONFIG_H_ */
