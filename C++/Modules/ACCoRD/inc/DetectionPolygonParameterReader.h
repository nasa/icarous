/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DETECTIONPOLYGONPARAMETERREADER_H_
#define DETECTIONPOLYGONPARAMETERREADER_H_

#include <vector>
#include <map>
#include "DetectionPolygon.h"
#include "ParameterData.h"
#include "Triple.h"

namespace larcfm {

/**
 * This class contains static methods to read in DetectionPolygon instance definitions from a ParameterData object in a format consistent with a ParameterData object created by DetectionPolygonParameterWriter.
 */
class DetectionPolygonParameterReader {
private:
  static std::map<std::string, DetectionPolygon*> registeredDetectionPolygonClasses;
  static bool registered;
public:

  /**
   * Register standard FormalATM classes.  This may also be done through explicit calls to registerDetection3D and registerDetectionPolygon, below.
   * Nonstandard classed need to be registered individually.
   */
  static void registerDefaults();

  /**
   * Register a new DetectionPolygon class with this reader.
   */
  static void registerDetectionPolygon(DetectionPolygon* cd);
  static void registerDetectionPolygon(DetectionPolygon* cd, const std::string& aternate_name);

  /**
   * The user is responsible for deleting the entries in the list (which will contain the second and third tuple elements).
   */
  static Triple<std::vector<DetectionPolygon*>,DetectionPolygon*,DetectionPolygon*> readPolygonDetection(const ParameterData& params);
  static Triple<std::vector<DetectionPolygon*>,DetectionPolygon*,DetectionPolygon*> readPolygonDetection(const ParameterData& params, bool verbose);
};
}

#endif
