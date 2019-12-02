/*
 * Copyright (c) 2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * Detection3DSUM.h
 */

#ifndef DETECTION3DSUM_H_
#define DETECTION3DSUM_H_

#include "Detection3D.h"
#include "Vect3.h"
#include "Velocity.h"
#include "ParameterData.h"
#include "ConflictData.h"
#include "string_util.h"
#include "ParameterAcceptor.h"
#include <string>

namespace larcfm {

class Detection3DSUM : public Detection3D {
public:
  virtual bool violationSUMAt(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double s_err, double sz_err, double v_err, double vz_err, double t) const;
  virtual bool conflictSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, double s_err, double sz_err, double v_err, double vz_err) const;
  virtual ConflictData conflictDetectionSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, double s_err, double sz_err, double v_err, double vz_err) const;
};

}
#endif /* DETECTION3DSUM_H_ */
