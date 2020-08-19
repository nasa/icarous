/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Detection3DParameterReader.h"
#include "Detection3D.h"
#include "ParameterData.h"
#include "Triple.h"
#include "string_util.h"
#include "CDCylinder.h"
#include "TCAS3D.h"
#include "WCV_TAUMOD.h"
#include "WCV_TAUMOD_SUM.h"
#include "WCV_TCPA.h"
#include "WCV_TEP.h"
#include "WCV_HZ.h"
#include "format.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace larcfm {

std::map<std::string, Detection3D*> Detection3DParameterReader::registeredDetection3DClasses;
bool Detection3DParameterReader::registered = false;

void Detection3DParameterReader::registerDefaults() {
  if (!registered) {
    CDCylinder cd3d;
    registerDetection3D(&cd3d);
    registerDetection3D(&cd3d, "gov.nasa.larcfm.ACCoRD.CDCylinder"); // java name

    TCAS3D tcas3d;
    registerDetection3D(&tcas3d);
    registerDetection3D(&tcas3d, "gov.nasa.larcfm.ACCoRD.TCAS3D"); // java name

    WCV_TAUMOD wcv_taumod;
    registerDetection3D(&wcv_taumod);
    registerDetection3D(&wcv_taumod, "gov.nasa.larcfm.ACCoRD.WCV_TAUMOD"); // java name

    WCV_TAUMOD_SUM wcv_taumod_sum;
    registerDetection3D(&wcv_taumod_sum);
    registerDetection3D(&wcv_taumod_sum, "gov.nasa.larcfm.ACCoRD.WCV_TAUMOD_SUM"); // java name

    WCV_TEP wcv_tep;
    registerDetection3D(&wcv_tep);
    registerDetection3D(&wcv_tep, "gov.nasa.larcfm.ACCoRD.WCV_TEP"); // java name

    WCV_TCPA wcv_tcpa;
    registerDetection3D(&wcv_tcpa);
    registerDetection3D(&wcv_tcpa, "gov.nasa.larcfm.ACCoRD.WCV_TCPA"); // java name

    WCV_HZ wcv_hz;
    registerDetection3D(&wcv_hz);
    registerDetection3D(&wcv_hz, "gov.nasa.larcfm.ACCoRD.WCV_HZ"); // java name

    registered = true;
  }
}

void Detection3DParameterReader::registerDetection3D(Detection3D* cd) {
  registerDetection3D(cd,cd->getCanonicalClassName());
}

void Detection3DParameterReader::registerDetection3D(Detection3D* cd, const string& name) {
  if (registeredDetection3DClasses.find(name) == registeredDetection3DClasses.end()) {
    registeredDetection3DClasses[name] = cd->make();
  }
}

Triple<vector<Detection3D*>,Detection3D*,Detection3D*> Detection3DParameterReader::readCoreDetection(const ParameterData& params){
  return readCoreDetection(params,false);
}

Triple<vector<Detection3D*>,Detection3D*,Detection3D*> Detection3DParameterReader::readCoreDetection(const ParameterData& params, bool verbose){
  vector<Detection3D*> cdlist;
  Detection3D* chosenD = NULL;
  Detection3D* chosenR = NULL;
  vector<string> mlist = params.matchList("load_core_detection_");
  std::sort(mlist.begin(), mlist.end());
  for (int i = 0; i < (int) mlist.size(); i++) {
    string pname = mlist[i];
    string instanceName = pname.substr(20);
    string dname = params.getString(pname);
    if (registeredDetection3DClasses.find(dname) != registeredDetection3DClasses.end()) {
      Detection3D* d = registeredDetection3DClasses[dname]->make();
      if (verbose) std::cout << ">>>>> Core detection "+dname+" ("<<instanceName<<") loaded <<<<<"<<std::endl;
      ParameterData instpd = params.extractPrefix(instanceName+"_");
      if (instpd.size() > 0) {
        d->setParameters(instpd);
        if (verbose) std::cout << ">>>>> Core detection parameters for "<<instanceName<<" set <<<<<"<<std::endl;
      }
      if (equals(d->getIdentifier(),"")) {
        d->setIdentifier(instanceName);
      }
      if (params.contains("set_det_core_detection") && equalsIgnoreCase(params.getString("set_det_core_detection"),instanceName)) {
        chosenD = d;
        //        std::cout << ">>>>> Core detection det set to "<<instanceName<<" <<<<<"<<std::endl;

      }
      if (params.contains("set_res_core_detection") && equalsIgnoreCase(params.getString("set_res_core_detection"),instanceName)) {
        chosenR = d;
        //        std::cout << ">>>>> Core detection res set to "<<instanceName<<" <<<<<"<<std::endl;
      }
      cdlist.push_back(d);
    } else {
      std::cout << "Error loading core detection: Class "<<dname<<" not registered"<<std::endl;
    }
  }
  if (params.contains("set_det_core_detection")  && chosenD == NULL) {
    if (verbose) std::cout << ">>>>> Core detection "<<params.getString("set_det_core_detection")<<" does not appear to be loaded, det cannot be set <<<<<"<<std::endl;
  }
  if (params.contains("set_res_core_detection")  && chosenR == NULL) {
    if (verbose) std::cout << ">>>>> Core detection "<<params.getString("set_res_core_detection")<<" does not appear to be loaded, res cannot be set <<<<<"<<std::endl;
  }
  return Triple<vector<Detection3D*>,Detection3D*,Detection3D*>(cdlist,chosenD,chosenR);
}





}

