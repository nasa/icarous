/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_TAUMOD_H_
#define WCV_TAUMOD_H_

#include "Detection3D.h"
#include "Vect3.h"
#include "Velocity.h"
#include "WCVTable.h"
#include "WCV_tvar.h"
#include "LossData.h"
#include <string>

namespace larcfm {
class WCV_TAUMOD : public WCV_tvar {


public:
  
  /** Constructor that a default instance of the WCV tables. */
  WCV_TAUMOD();

  /** Constructor that specifies a particular instance of the WCV tables. */
  WCV_TAUMOD(const WCVTable& tab);

  virtual ~WCV_TAUMOD() {};

  double horizontal_tvar(const Vect2& s, const Vect2& v) const;

  LossData horizontal_WCV_interval(double T, const Vect2& s, const Vect2& v) const ;

  Detection3D* make() const;

  /**
   * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.
   */
  Detection3D* copy() const;

  std::string getSimpleClassName() const;

  bool contains(const Detection3D* cd) const;

};
}
#endif
