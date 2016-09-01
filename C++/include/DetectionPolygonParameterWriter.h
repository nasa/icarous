/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DETECTIONPOLYGONPARAMETERWRITER_H_
#define DETECTIONPOLYGONPARAMETERWRITER_H_

#include <vector>
#include "DetectionPolygon.h"
#include "ParameterData.h"

namespace larcfm {
class DetectionPolygonParameterWriter {
public:

  static ParameterData writePolygonDetection(std::vector<DetectionPolygon*> dlist, DetectionPolygon* det, DetectionPolygon* res);
  static ParameterData writePolygonDetection(std::vector<DetectionPolygon*> dlist, DetectionPolygon* det, DetectionPolygon* res, bool ordered);

};
}
#endif
