/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_TCPA_H_
#define WCV_TCPA_H_

#include "Detection3D.h"
#include "Vect3.h"
#include "Velocity.h"
#include "WCVTable.h"
#include "WCV_tvar.h"
#include "LossData.h"
#include <string>

namespace larcfm {
class WCV_TCPA : public WCV_tvar {


public:
  
  /** Constructor that uses the default TCAS tables. */
  WCV_TCPA();

  /** Constructor that specifies a particular instance of the WCV tables. */
  WCV_TCPA(const WCVTable& tab);

  double horizontal_tvar(const Vect2& s, const Vect2& v) const;

  LossData horizontal_WCV_interval(double T, const Vect2& s, const Vect2& v) const ;

//  bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const;

//  // The semantics of this function is "Time of Minimum Modified Tau"
//  double timeOfClosestApproach(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const;


  Detection3D* make() const;

  /**
   * Returns a deep copy of this WCV_TCPA object, including any results that have been calculated.
   */
  Detection3D* copy() const;

  std::string getSimpleClassName() const;

  bool contains(const Detection3D* cd) const;

};
}
#endif
