/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * LossData.h
 *
 *  Created on: Nov 21, 2014
 *      Author: rbutler
 */

#ifndef SRC_CONFLICTDATA_H_
#define SRC_CONFLICTDATA_H_

#include "LossData.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Horizontal.h"
#include "Vertical.h"

#include <string>

namespace larcfm {

class ConflictData : public LossData {

private:
  Vect3  s_; // Relative position
  Velocity  v_; // Relative velocity

public:

  double  time_crit;   // relative time to critical point
  double  dist_crit;   // distance or severity at critical point (0 is most critical, +inf is least severe)

  std::string toString() const ;

  ConflictData(double t_in, double t_out, double t_crit, double d_crit, const Vect3& s, const Velocity& v);
  ConflictData(const LossData& ld, double t_crit, double d_crit, const Vect3& s, const Velocity& v);
  ConflictData();

  /**
   * Returns internal vector representation of relative aircraft position.
   */
  Vect3 const & get_s() const;

  /**
   * Returns internal vector representation of relative aircraft velocity.
   */
  Vect3 const & get_v() const;

  /**
   * Returns HMD, in internal units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  double HMD(double T) const;

  /**
   * Returns HMD, in specified units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  double HMD(const std::string& u, double T) const;

  /**
   * Returns VMD, in internal units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  double VMD(double T) const;

  /**
   * Returns VMD, in specified units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  double VMD(const std::string& u, double T) const;

  double getCriticalTimeOfConflict() const;

  double getDistanceAtCriticalTime() const;
};

} /* namespace larcfm */

#endif /* SRC_LOSSDATA_H_ */
