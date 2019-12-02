/*
 * Copyright (c) 2015-2018 United States Government as represented by
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
/**
 * This class contains static methods to create ParameterData objects that contain information about DetectionPolygon 
 * instance definitions in a format consistent with DetectionPolygonParameterReader.
 */
class DetectionPolygonParameterWriter {
public:

	/**
	 * Return a ParameterData suitable to be read by readPolygonDetection() based on the supplied DetectionPolygon instances. 
	 * @param dlist list of DetectionPolygon instances -- this may be empty.  det and res instances will be automatically added to the list (if they are not already in it)
	 * @param det detection instance -- this may be null
	 * @param res resolution instance -- this may be null
	 * Note: this may modify the instance identifiers if they are not already unique.
	 */
  static ParameterData writePolygonDetection(std::vector<DetectionPolygon*> dlist, DetectionPolygon* det, DetectionPolygon* res);
	/**
	 * Return a ParameterData suitable to be read by readPolygonDetection() based on the supplied DetectionPolygon instances. 
	 * @param dlist list of DetectionPolygon instances -- this may be empty.  det and res instances will be automatically added to the list (if they are not already in it)
	 * @param det detection instance -- this may be null
	 * @param res resolution instance -- this may be null
	 * @param ordered true to modify detection identifiers to ensure they retain the input list's ordering when decoded, false will only modify identifiers if they are not unique
	 */
  static ParameterData writePolygonDetection(std::vector<DetectionPolygon*> dlist, DetectionPolygon* det, DetectionPolygon* res, bool ordered);

};
}
#endif
