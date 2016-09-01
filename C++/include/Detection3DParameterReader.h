/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DETECTION3DPARAMETERREADER_H_
#define DETECTION3DPARAMETERREADER_H_

#include <vector>
#include <map>
#include "Detection3D.h"
#include "ParameterData.h"
#include "Triple.h"

namespace larcfm {

/**
 * This class differs from the Java version in that you need to explicitly register the types of detection classes you will be loading.
 */
class Detection3DParameterReader {
private:
  static std::map<std::string, Detection3D*> registeredDetection3DClasses;
  static bool registered;
public:

  /**
   * Register standard FormalATM classes.  This may also be done through explicit calls to registerDetection3D and registerDetectionPolygon, below.
   * Nonstandard classed need to be registered individually.
   */
  static void registerDefaults();

  /**
   * Register a new Detection3D class with this reader.
   */
  static void registerDetection3D(Detection3D* cd);
  static void registerDetection3D(Detection3D* cd, const std::string& aternate_name);


  /**
   * The user is responsible for deleting the entries in the list (which will contain the second and third tuple elements).
   */
  static Triple<std::vector<Detection3D*>,Detection3D*,Detection3D*> readCoreDetection(const ParameterData& params);
  static Triple<std::vector<Detection3D*>,Detection3D*,Detection3D*> readCoreDetection(const ParameterData& params, bool verbose);

};
}

#endif
