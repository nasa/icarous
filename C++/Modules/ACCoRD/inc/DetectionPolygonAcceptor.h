/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DETECTIONPOLYGONACCEPTOR_H_
#define DETECTIONPOLYGONACCEPTOR_H_

#include "DetectionPolygon.h"

namespace larcfm {

class DetectionPolygonAcceptor {
public:

  virtual ~DetectionPolygonAcceptor() = 0;

  /**
   * Apply a deep copy of the referenced Detection3D object to this object at the lowest level.  The user is responsible for cleaning up the original instance.
   */
  virtual void setCorePolygonDetectionRef(const DetectionPolygon& cd) = 0;

  /**
   * Apply a deep copy of the Detection3D object pointed to to this object at the lowest level.  The user is responsible for cleaning up the original instance.
   */
  virtual void setCorePolygonDetectionPtr(const DetectionPolygon* cd) = 0;

  /**
   * Return a reference to this object's Detection3D instance.
   */
  virtual DetectionPolygon& getCorePolygonDetectionRef() const = 0;

  /**
   * Return a pointer to this object's Detection3D instance.
   */
  virtual DetectionPolygon* getCorePolygonDetectionPtr() const = 0;


};

inline DetectionPolygonAcceptor::~DetectionPolygonAcceptor(){}

} /* namespace larcfm */
#endif /* DETECTION3D_H_ */
