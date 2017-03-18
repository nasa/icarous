/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Detection3DParameterWriter.h"
#include "Detection3D.h"
#include "ParameterData.h"
#include "string_util.h"
#include "format.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace larcfm {



ParameterData Detection3DParameterWriter::writeCoreDetection(std::vector<Detection3D*> dlist, Detection3D* det, Detection3D* res) {
  return writeCoreDetection(dlist, det, res, false);
}

ParameterData Detection3DParameterWriter::writeCoreDetection(std::vector<Detection3D*> dlist, Detection3D* det, Detection3D* res, bool ordered) {
  // make sure det and res are in the list, if necessary
  if (det != NULL && std::find(dlist.begin(), dlist.end(),det) == dlist.end()) {
    dlist.push_back(det);
  }
  if (res != NULL &&  std::find(dlist.begin(), dlist.end(),res) == dlist.end()) {
    dlist.push_back(res);
  }
  ParameterData p;
  std::vector<std::string> names;
  int counter = 1;
  for (int i = 0; i < (int) dlist.size(); i++) {
    Detection3D* cd = dlist[i];
    //make sure each instance has a unique name
    if (ordered) {
      if (equals(cd->getIdentifier(),"")) {
        cd->setIdentifier("det_"+FmLead(counter, 4)+"_"+cd->getCanonicalClassName());
      } else {
        cd->setIdentifier("det_"+FmLead(counter, 4)+"_"+cd->getIdentifier());
      }
      counter++;

    } else {
      while (equals(cd->getIdentifier(),"") ||  contains(names, cd->getIdentifier())) {
        cd->setIdentifier(cd->getCanonicalClassName()+"_instance_"+Fmi(counter));
        counter++;
      }
    }
    std::string id = cd->getIdentifier();
    names.push_back(id);
    p.set("load_core_detection_"+id+" = "+cd->getCanonicalClassName());
    p.copy(cd->getParameters().copyWithPrefix(id+"_"),true);
  }
  if (det != NULL) {
    p.set("set_det_core_detection = "+det->getIdentifier());
  }
  if (res != NULL) {
    p.set("set_res_core_detection = "+res->getIdentifier());
  }
  return p;
}


/**
 * This removes all standard core detection parameters from a ParameterData object
 * @param p
 */
void Detection3DParameterWriter::clearCoreDetectionParameters(ParameterData& p) {
  // strip set core parameters
  p.remove("set_det_core_detection");
  p.remove("set_res_core_detection");
  std::vector<std::string> ids;
  // strip load_core_deteciton_ parameters
  std::vector<std::string> keys = p.getList();
  for (int i = 0; i < (int) keys.size(); i++) {
    std::string key = keys[i];
    if (startsWith(key, "load_core_detection_")) {
      ids.push_back(key.substr(20));
      p.remove(key);
    }
  }
  // strip detection subparameters
  keys = p.getList();
  for (int j = 0; j < (int) ids.size(); j++) {
    std::string id = ids[j];
    for (int i = 0; i < (int) keys.size(); i++) {
      std::string key = keys[i];
      if (startsWith(key,id+"_")) {
        p.remove(key);
      }
    }
  }

}


}

