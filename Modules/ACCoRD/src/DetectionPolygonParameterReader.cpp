/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DetectionPolygonParameterReader.h"
#include "DetectionPolygon.h"
#include "ParameterData.h"
#include "Triple.h"
#include "string_util.h"
#include "CDPolyIter.h"
//#include "CDPolycarp.h"
#include "format.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace larcfm {

std::map<std::string, DetectionPolygon*> DetectionPolygonParameterReader::registeredDetectionPolygonClasses;
bool DetectionPolygonParameterReader::registered = false;

void DetectionPolygonParameterReader::registerDefaults() {
  if (!registered) {
    CDPolyIter cdpi;
    registerDetectionPolygon(&cdpi);
    registerDetectionPolygon(&cdpi, "gov.nasa.larcfm.ACCoRD.CDPolyIter"); //java name

    //  CDPolycarp cdpc;
    //  registerDetectionPolygon(&cdpc);
    //  registerDetectionPolygon(&cdpc, "gov.nasa.larcfm.ACCoRD.CDPolycarp"); //java name
    registered = true;
  }
}


void DetectionPolygonParameterReader::registerDetectionPolygon(DetectionPolygon* cd) {
  registerDetectionPolygon(cd,cd->getClassName());
}

void DetectionPolygonParameterReader::registerDetectionPolygon(DetectionPolygon* cd, const string& name) {
  if (registeredDetectionPolygonClasses.find(name) == registeredDetectionPolygonClasses.end()) {
    registeredDetectionPolygonClasses[name] = cd->make();
  }
}




Triple<vector<DetectionPolygon*>,DetectionPolygon*,DetectionPolygon*> DetectionPolygonParameterReader::readPolygonDetection(const ParameterData& params) {
  return readPolygonDetection(params,true);
}

Triple<vector<DetectionPolygon*>,DetectionPolygon*,DetectionPolygon*> DetectionPolygonParameterReader::readPolygonDetection(const ParameterData& params, bool verbose) {
  vector<DetectionPolygon*> cdlist;
  DetectionPolygon* chosenD = NULL;
  DetectionPolygon* chosenR = NULL;
  vector<string> mlist = params.matchList("load_polygon_detection_");
  std::sort(mlist.begin(), mlist.end());
  for (int i = 0; i < (int) mlist.size(); i++) {
    string pname = mlist[i];
    string instanceName = pname.substr(23);
    string dname = params.getString(pname);
    if (registeredDetectionPolygonClasses.find(dname) != registeredDetectionPolygonClasses.end()) {
      DetectionPolygon* d = registeredDetectionPolygonClasses[dname]->make();
      if (verbose) std::cout << ">>>>> Polygon detection "+dname+" ("<<instanceName<<") loaded <<<<<"<<std::endl;
      ParameterData instpd = params.extractPrefix(instanceName+"_");
      if (instpd.size() > 0) {
        d->setParameters(instpd);
        if (verbose) std::cout << ">>>>> Polygon detection parameters for "<<instanceName<<" set <<<<<"<<std::endl;
      }
      if (equals(d->getIdentifier(),"")) {
        d->setIdentifier(instanceName);
      }
      if (params.contains("set_det_polygon_detection") && equalsIgnoreCase(params.getString("set_det_polygon_detection"),instanceName)) {
        chosenD = d;
        //        std::cout << ">>>>> Polygon detection det set to "<<instanceName<<" <<<<<"<<std::endl;

      }
      if (params.contains("set_res_polygon_detection") && equalsIgnoreCase(params.getString("set_res_polygon_detection"),instanceName)) {
        chosenR = d;
        //        std::cout << ">>>>> Polygon detection res set to "<<instanceName<<" <<<<<"<<std::endl;
      }
      cdlist.push_back(d);
    } else {
      std::cout << "Error loading polygon detection: Class "<<dname<<" not registered"<<std::endl;
    }
  }
  if (params.contains("set_det_polygon_detection")  && chosenD == NULL) {
    if (verbose) std::cout << ">>>>> Polygon detection "<<params.getString("set_det_polygon_detection")<<" does not appear to be loaded, det cannot be set <<<<<"<<std::endl;
  }
  if (params.contains("set_res_polygon_detection")  && chosenR == NULL) {
    if (verbose) std::cout << ">>>>> Polygon detection "<<params.getString("set_res_polygon_detection")<<" does not appear to be loaded, res cannot be set <<<<<"<<std::endl;
  }
  return Triple<vector<DetectionPolygon*>,DetectionPolygon*,DetectionPolygon*>(cdlist,chosenD,chosenR);
}



}

