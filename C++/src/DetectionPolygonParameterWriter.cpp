/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DetectionPolygonParameterWriter.h"
#include "DetectionPolygon.h"
#include "ParameterData.h"
#include "string_util.h"
#include "format.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace larcfm {





ParameterData DetectionPolygonParameterWriter::writePolygonDetection(std::vector<DetectionPolygon*> dlist, DetectionPolygon* det, DetectionPolygon* res) {
  return writePolygonDetection(dlist, det, res, false);
}

ParameterData DetectionPolygonParameterWriter::writePolygonDetection(std::vector<DetectionPolygon*> dlist, DetectionPolygon* det, DetectionPolygon* res, bool ordered) {
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
    DetectionPolygon* cd = dlist[i];
    //make sure each instance has a unique name
    if (ordered) {
      if (equals(cd->getIdentifier(),"")) {
        cd->setIdentifier("det_"+FmLead(counter, 4)+"_"+cd->getClassName());
      } else {
        cd->setIdentifier("det_"+FmLead(counter, 4)+"_"+cd->getIdentifier());
      }
      counter++;

    } else {
      while (equals(cd->getIdentifier(),"") || contains(names, cd->getIdentifier())) {
        cd->setIdentifier(cd->getClassName()+"_instance_"+Fmi(counter));
        counter++;
      }
    }
    std::string id = cd->getIdentifier();
    names.push_back(id);
    p.set("load_polygon_detection_"+id+" = "+cd->getClassName());
    p.copy(cd->getParameters().copyWithPrefix(id+"_"),true);
  }
  if (det != NULL) {
    p.set("set_det_polygon_detection = "+det->getIdentifier());
  }
  if (res != NULL) {
    p.set("set_res_polygon_detection = "+res->getIdentifier());
  }
  return p;
}



}

