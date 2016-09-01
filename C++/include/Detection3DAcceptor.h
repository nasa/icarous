/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DETECTION3DACCEPTOR_H_
#define DETECTION3DACCEPTOR_H_

#include "Detection3D.h"

namespace larcfm {

class Detection3DAcceptor {
public:

  virtual ~Detection3DAcceptor() = 0;

  /**
   * Apply a deep copy of the referenced Detection3D object to this object at the lowest level.  The user is responsible for cleaning up the original instance.
   */
  virtual void setCoreDetectionRef(const Detection3D& cd) = 0;

  /**
   * Apply a deep copy of the Detection3D object pointed to to this object at the lowest level.  The user is responsible for cleaning up the original instance.
   */
  virtual void setCoreDetectionPtr(const Detection3D* cd) = 0;

  /**
   * Return a reference to this object's Detection3D instance.
   */
  virtual Detection3D& getCoreDetectionRef() const = 0;

  /**
   * Return a pointer to this object's Detection3D instance.
   */
  virtual Detection3D* getCoreDetectionPtr() const = 0;


};

inline Detection3DAcceptor::~Detection3DAcceptor(){}

} /* namespace larcfm */
#endif /* DETECTION3D_H_ */
