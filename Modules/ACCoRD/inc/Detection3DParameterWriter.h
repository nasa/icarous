/*
 * Copyright (c) 2015-2017 United States Government as represented by
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
  /**
 * This class contains static methods to create ParameterData objects that contain information about Detection3D 
 * instance definitions in a format consistent with DetectionParameterReader.
 */
class Detection3DParameterWriter {
  public:

	/**
	 * Return a ParameterData suitable to be read by readCoreDetection() based on the supplied Detection3D instances. 
	 * @param dlist list of Detection3D instances -- this may be empty.  det and res instances will be automatically added to the list (if they are not already in it)
	 * @param det detection instance -- this may be null
	 * @param res resolution instance -- this may be null
	 * Note: this may modify the instance identifiers if they are not already unique.
	 */
    static ParameterData writeCoreDetection(std::vector<Detection3D*> dlist, Detection3D* det, Detection3D* res);
	/**
	 * Return a ParameterData suitable to be read by readCoreDetection() based on the supplied Detection3D instances. 
	 * @param dlist list of Detection3D instances -- this may be empty.  det and res instances will be automatically added to the list (if they are not already in it)
	 * @param det detection instance -- this may be null
	 * @param res resolution instance -- this may be null
	 * @param ordered true to modify detection identifiers to ensure they retain the input list's ordering when decoded, false will only modify identifiers if they are not unique
	 */
    static ParameterData writeCoreDetection(std::vector<Detection3D*> dlist, Detection3D* det, Detection3D* res, bool ordered);

    /**
     * This removes all standard core detection parameters from a ParameterData object
     * @param p
     */
    static void clearCoreDetectionParameters(ParameterData& p);

  };
}

#endif
