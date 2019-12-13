/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DETECTION3DRESACCEPTOR_H_
#define DETECTION3DRESACCEPTOR_H_

#include "Detection3D.h"

namespace larcfm {

class Detection3DResAcceptor {
public:
  virtual ~Detection3DResAcceptor() = 0;

  /**
   * Apply a deep copy of the referenced Detection3D object to this object at the lowest level.  The user is responsible for cleaning up the original instance.
   */
  virtual void setResDetectionRef(const Detection3D& cd) = 0;

  /**
   * Apply a deep copy of the Detection3D object pointed to to this object at the lowest level.  The user is responsible for cleaning up the original instance.
   */
  virtual void setResDetectionPtr(const Detection3D* cd) = 0;

  /**
   * Return a reference to this object's Detection3D instance.
   */
  virtual Detection3D& getResDetectionRef() const = 0;

  /**
   * Return a pointer to this object's Detection3D instance.
   */
  virtual Detection3D* getResDetectionPtr() const = 0;


};

inline Detection3DResAcceptor::~Detection3DResAcceptor(){}

} /* namespace larcfm */
#endif
