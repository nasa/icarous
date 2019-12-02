/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_TAUMOD_SUM_H_
#define WCV_TAUMOD_SUM_H_

#include "WCV_TAUMOD.h"
#include <string>

namespace larcfm {
class WCV_TAUMOD_SUM : public WCV_TAUMOD {


public:

  /** Constructor that a default instance of the WCV tables. */
  WCV_TAUMOD_SUM();

  /** Constructor that specifies a particular instance of the WCV tables. */
  explicit WCV_TAUMOD_SUM(const WCVTable& tab);

  virtual ~WCV_TAUMOD_SUM() {};

private:
  bool sumof(const Vect2& v1, const Vect2& v2, const Vect2& w) const;

  Vect2 average_direction(const Vect2& v1, const Vect2& v2) const;

  std::pair<Vect2,Vect2> optimal_pair(const Vect2& v1, const Vect2& v2, const Vect2& w1, const Vect2& w2) const;

  std::pair<Vect2,Vect2> optimal_wcv_pair_comp_init(const Vect2& v1, const Vect2& v2, double s_err, double v_err) const;

  std::pair<Vect2,Vect2> optimal_wcv_pair(const Vect2& s, const Vect2& v, double s_err, double v_err, int eps1, int eps2) const;

  bool horizontal_wcv_taumod_uncertain(const Vect2& s, const Vect2& v, double s_err, double v_err) const;

  bool vertical_WCV_uncertain(double sz, double vz, double sz_err, double vz_err) const;

  bool WCV_taumod_uncertain(const Vect3& s, const Vect3& v, double s_err, double sz_err, double v_err, double vz_err) const;

  double horizontal_wcv_taumod_uncertain_entry(const Vect2& s, const Vect2& v, double s_err, double v_err, double T) const;

  double Theta_D_uncertain(const Vect2& s, const Vect2& v, double s_err, double v_err, int eps) const;

  double horizontal_wcv_taumod_uncertain_exit(const Vect2& s, const Vect2& v,double s_err, double v_err, double T) const;

  LossData horizontal_wcv_taumod_uncertain_interval(const Vect2& s, const Vect2& v,double s_err, double v_err, double T) const;

  LossData vertical_WCV_uncertain_full_interval_szpos_vzpos(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_full_interval_szpos_vzneg(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_full_interval_szpos(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_full_interval_split(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_interval(double B, double T, double sz, double vz, double sz_err, double vz_err) const;

  LossData WCV_taumod_uncertain_interval(double B, double T, const Vect3& s, const Vect3& v,
      double s_err, double sz_err, double v_err, double vz_err) const;

  bool WCV_taumod_uncertain_detection(double B, double T, const Vect3& s, const Vect3& v,
      double s_err, double sz_err, double v_err, double vz_err) const;


public:

  virtual Detection3D* make() const;

  /**
   * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.
   */
  virtual Detection3D* copy() const;

  virtual std::string getSimpleClassName() const;

  bool violationSUMAt(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
          double s_err, double sz_err, double v_err, double vz_err, double t) const;

  bool conflictSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T,
          double s_err, double sz_err, double v_err, double vz_err) const;

  virtual ConflictData conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
      return WCV_TAUMOD::conflictDetection(so,vo,si,vi,B,T);
  }

  ConflictData conflictDetectionSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T,
          double s_err, double sz_err, double v_err, double vz_err) const;

  virtual bool contains(const Detection3D* cd) const;

};
}
#endif
