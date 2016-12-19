/*
 * CRSSConfig
 *
 *Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "CRSSConfig.h"

#include "CDSSCore.h"
#include "Units.h"
#include "StateReader.h"
#include "Projection.h"
#include "CRSS.h"

namespace larcfm {

  //bool CRSSConfig::debugPrintTime = false;          // ************* bCRSS has an option "-t" to print the time
  ProjectionType CRSSConfig::CRSSProjType = SIMPLE;  // SIMPLE or ENU

  double CRSSConfig::D_default =  Units::from("NM",   5.0);   // [NM]
  double CRSSConfig::H_default =  Units::from("ft",1000.0);   // [ft]
  double CRSSConfig::T_default =  Units::from("s",  300.0);

  
  void CRSSConfig::setDefaultsForParameters(ParameterReader& sRdr) {
    sRdr.getParametersRef().setInternal("D", D_default, "NM");
    sRdr.getParametersRef().setInternal("H", H_default, "ft");
    sRdr.getParametersRef().setInternal("T", T_default, "s");

  }
  
  void CRSSConfig::load_parameters(ParameterReader& sRdr, CDSS& cdss, CRSS& crss) {
    crss.setDistance(sRdr.getParametersRef().getValue("D", "NM"));
    crss.setHeight(sRdr.getParametersRef().getValue("H", "ft"));
    cdss.setDistance(sRdr.getParametersRef().getValue("D", "NM"));
    cdss.setHeight(sRdr.getParametersRef().getValue("H", "ft"));
  }

}
