/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DETECTION3DPARAMETERWRITER_H_
#define DETECTION3DPARAMETERWRITER_H_

#include <vector>
#include "Detection3D.h"
#include "ParameterData.h"

namespace larcfm {
  class Detection3DParameterWriter {
  public:

    static ParameterData writeCoreDetection(std::vector<Detection3D*> dlist, Detection3D* det, Detection3D* res);
    static ParameterData writeCoreDetection(std::vector<Detection3D*> dlist, Detection3D* det, Detection3D* res, bool ordered);

    /**
     * This removes all standard core detection parameters from a ParameterData object
     * @param p
     */
    static void clearCoreDetectionParameters(ParameterData& p);

  };
}

#endif
