/*
 * Copyright (c) 2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Detection3D.h"
#include "Detection3DSUM.h"
#include "ConflictData.h"
#include "CD3D.h"
#include "format.h"

namespace larcfm {


bool Detection3DSUM::violationSUMAt(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double s_err, double sz_err, double v_err, double vz_err, double t) const {
  Vect3 sot = vo.ScalAdd(t,so);
  Vect3 sit = vi.ScalAdd(t,si);
  return violation(sot,vo,sit,vi);
}

bool Detection3DSUM::conflictSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, double s_err, double sz_err, double v_err, double vz_err) const {
  return conflict(so,vo,si,vi,B,T);
}

ConflictData Detection3DSUM::conflictDetectionSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, double s_err, double sz_err, double v_err, double vz_err) const {
  return conflictDetection(so,vo,si,vi,B,T);
}

}
