/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_TAUMOD_H_
#define WCV_TAUMOD_H_

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
  explicit WCV_TAUMOD(const WCVTable& tab);

  virtual ~WCV_TAUMOD() {};

  /**
   * @return one static WCV_TAUMOD
   */
  static const WCV_TAUMOD& A_WCV_TAUMOD();

  /**
   * @return DO-365 preventive thresholds Phase I (en-route), i.e., DTHR=0.66nmi, ZTHR=700ft,
   * TTHR=35s, TCOA=0.
   */
  static const WCV_TAUMOD& DO_365_Phase_I_preventive();

  /**
   * @return DO-365 Well-Clear thresholds Phase I (en-route), i.e., DTHR=0.66nmi, ZTHR=450ft,
   * TTHR=35s, TCOA=0.
   */
  static const WCV_TAUMOD& DO_365_DWC_Phase_I();

  /**
   * @return DO-365 Well-Clear thresholds Phase II (DTA), i.e., DTHR=1500 [ft], ZTHR=450ft,
   * TTHR=0s, TCOA=0.
   */
  static const WCV_TAUMOD& DO_365_DWC_Phase_II();

  /**
   * @return buffered preventive thresholds Phase I (en-route), i.e., DTHR=1nmi, ZTHR=750ft,
   * TTHR=35s, TCOA=20.
   */
  static const WCV_TAUMOD& Buffered_Phase_I_preventive();

  /**
   * @return buffered Well-Clear thresholds Phase I (en-route), i.e., DTHR=1.0nmi, ZTHR=450ft,
   * TTHR=35s, TCOA=20.
   */
  static const WCV_TAUMOD& Buffered_DWC_Phase_I();

  virtual double horizontal_tvar(const Vect2& s, const Vect2& v) const;

  virtual LossData horizontal_WCV_interval(double T, const Vect2& s, const Vect2& v) const ;

  virtual Detection3D* make() const;

  /**
   * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.
   */
  virtual Detection3D* copy() const;

  virtual std::string getSimpleClassName() const;

  virtual bool contains(const Detection3D* cd) const;

  static Position TAU_center(const Position& po, const Velocity& v, double TTHR, double T);

  static double TAU_radius(const Velocity& v, double DTHR, double TTHR);

  virtual void hazard_zone_far_end(std::vector<Position>& haz,
      const Position& po, const Velocity& v, const Velocity& vD, double T) const;

};

}
#endif
