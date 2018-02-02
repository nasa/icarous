/*
 * CRSSConfig.h
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CRSSConfig_H_
#define CRSSConfig_H_

#include "CDSS.h"
#include "CRSS.h"
#include "StateReader.h"
#include "Projection.h"

namespace larcfm {

  class CRSSConfig {



  public:

    //static bool debugPrintTime;   // ************* bCRSS has an option "-t" to print the time
    static ProjectionType CRSSProjType;


    static double D_default;
    static double H_default;
    static double T_default;
    static double Th_default;
    static double Tv_default;


    static void setDefaultsForParameters(ParameterReader& sRdr);
    static void load_parameters(ParameterReader& sRdr, CDSS& cdss, CRSS& crss);


  private:

  };

}

#endif /* CRSSConfig_H_ */
