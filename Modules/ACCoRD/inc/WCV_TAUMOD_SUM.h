/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_TAUMOD_SUM_H_
#define WCV_TAUMOD_SUM_H_

#include "WCV_TAUMOD.h"
#include "DaidalusParameters.h"
#include <string>

namespace larcfm {
class WCV_TAUMOD_SUM : public WCV_TAUMOD {


public:

  /** Constructor that a default instance of the WCV tables. */
  WCV_TAUMOD_SUM();

  /** Constructor that specifies a particular instance of the WCV tables. */
  explicit WCV_TAUMOD_SUM(const WCVTable& tab);

  virtual ~WCV_TAUMOD_SUM() {};

  /**
   * @return one static WCV_TAUMOD_SUM
   */
  static const WCV_TAUMOD_SUM& A_WCV_TAUMOD_SUM();

  virtual ConflictData conflictDetectionWithTrafficState(const TrafficState& ownship, const TrafficState& intruder,
      double B, double T) const;

private:

  double  h_pos_z_score_;          // Number of horizontal position standard deviations
  bool h_pos_z_score_enabled_;     // True if value has been set independently. Otherwise,
  // value will be overwritten using global parameter
  double  h_vel_z_score_min_;      // Minimum number of horizontal velocity standard deviations
  bool h_vel_z_score_min_enabled_; // True if value has been set independently. Otherwise,
  // value will be overwritten using global parameter
  double  h_vel_z_score_max_;      // Maximum number of horizontal velocity standard deviations
  bool h_vel_z_score_max_enabled_; // True if value has been set independently. Otherwise,
  // value will be overwritten using global parameter
  double  h_vel_z_distance_;       // Distance at which h_vel_z_score scales from min to max as range decreases
  std::string  h_vel_z_distance_units_;    // Distance at which h_vel_z_score scales from min to max as range decreases
  bool h_vel_z_distance_enabled_;  // True if value has been set independently. Otherwise,
  // value will be overwritten using global parameter
  double  v_pos_z_score_;          // Number of vertical position standard deviations
  bool v_pos_z_score_enabled_;     // True if value has been set independently. Otherwise,
  // value will be overwritten using global parameter
  double  v_vel_z_score_;          // Number of vertical velocity standard deviations
  bool v_vel_z_score_enabled_;     // True if value has been set independently. Otherwise,
  // value will be overwritten using global parameter

  static double MinError;

  void initSUM();

  void copyFrom(const WCV_TAUMOD_SUM& wcv);

  bool sumof(const Vect2& v1, const Vect2& v2, const Vect2& w) const;

  Vect2 average_direction(const Vect2& v1, const Vect2& v2) const;

  std::pair<Vect2,Vect2> optimal_pair(const Vect2& v1, const Vect2& v2, const Vect2& w1, const Vect2& w2) const;

  std::pair<Vect2,Vect2> optimal_wcv_pair_comp_init(const Vect2& v1, const Vect2& v2, double s_err, double v_err) const;

  std::pair<Vect2,Vect2> optimal_wcv_pair(const Vect2& s, const Vect2& v, double s_err, double v_err, int eps1, int eps2) const;

  bool horizontal_wcv_taumod_uncertain(const Vect2& s, const Vect2& v, double s_err, double v_err) const;

  bool vertical_WCV_uncertain(double sz, double vz, double sz_err, double vz_err) const;

  double horizontal_wcv_taumod_uncertain_entry(const Vect2& s, const Vect2& v, double s_err, double v_err, double T) const;

  double Theta_D_uncertain(const Vect2& s, const Vect2& v, double s_err, double v_err, int eps) const;

  double horizontal_wcv_taumod_uncertain_exit(const Vect2& s, const Vect2& v,double s_err, double v_err, double T) const;

  LossData horizontal_wcv_taumod_uncertain_interval(const Vect2& s, const Vect2& v,double s_err, double v_err, double T) const;

  LossData vertical_WCV_uncertain_full_interval_szpos_vzpos(double T, double minsz/*,double maxsz*/, double minvz/*, double maxvz*/) const;

  LossData vertical_WCV_uncertain_full_interval_szpos_vzneg(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_full_interval_szpos(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_full_interval_split(double T, double minsz,double maxsz, double minvz, double maxvz) const;

  LossData vertical_WCV_uncertain_interval(double B, double T, double sz, double vz, double sz_err, double vz_err) const;

  bool containsSUM(WCV_TAUMOD_SUM* wcv) const;

  double relativeHorizontalPositionError(const TrafficState& own, const TrafficState& ac) const;

  double relativeVerticalPositionError(const TrafficState& own, const TrafficState& ac) const;

  double weighted_z_score(double range) const;

  double relativeHorizontalSpeedError(const TrafficState& own, const TrafficState& ac, double s_err) const;

  double relativeVerticalSpeedError(const TrafficState& own, const TrafficState& ac) const;

public:

  WCV_TAUMOD_SUM & operator=(const WCV_TAUMOD_SUM& wcv);

  virtual Detection3D* make() const;

  /**
   * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.
   */
  virtual Detection3D* copy() const;

  virtual std::string getSimpleClassName() const;

  bool WCV_taumod_uncertain(const Vect3& s, const Vect3& v, double s_err, double sz_err, double v_err, double vz_err) const;

  LossData WCV_taumod_uncertain_interval(double B, double T, const Vect3& s, const Vect3& v,
      double s_err, double sz_err, double v_err, double vz_err) const;

  bool WCV_taumod_uncertain_detection(double B, double T, const Vect3& s, const Vect3& v,
      double s_err, double sz_err, double v_err, double vz_err) const;

  virtual bool contains(const Detection3D* cd) const;

  void updateParameterData(ParameterData& p) const;

  void setParameters(const ParameterData& p);

  /**
   * @return get z-score (number of standard deviations) for horizontal position
   */
  double getHorizontalPositionZScore() const;

  /**
   * @return set z-score (number of standard deviations) for horizontal position (non-negative value)
   */
  void setHorizontalPositionZScore(double val);

  /**
   * @return get min z-score (number of standard deviations) for horizontal velocity
   */
  double getHorizontalVelocityZScoreMin() const;

  /**
   * @return set min z-score (number of standard deviations) for horizontal velocity (non-negative value)
   */
  void setHorizontalVelocityZScoreMin(double val);

  /**
   * @return get max z-score (number of standard deviations) for horizontal velocity
   */
  double getHorizontalVelocityZScoreMax() const;

  /**
   * @return set max z-score (number of standard deviations) for horizontal velocity (non-negative value)
   */
  void setHorizontalVelocityZScoreMax(double val);

  /**
   * @return Distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
   */
  double getHorizontalVelocityZDistance() const;

  /**
   * @return Set distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
   */
  void setHorizontalVelocityZDistance(double val);

  /**
   * @return Distance (in given units) at which h_vel_z_score scales from min to max as range decreases
   */
  double getHorizontalVelocityZDistance(const std::string& u) const;

  /**
   * @return Set distance (in given units) at which h_vel_z_score scales from min to max as range decreases
   */
  void setHorizontalVelocityZDistance(double val, const std::string& u);

  /**
   * @return get z-score (number of standard deviations) for vertical position
   */
  double getVerticalPositionZScore() const;

  /**
   * @return set z-score (number of standard deviations) for vertical position (non-negative value)
   */
  void setVerticalPositionZScore(double val);

  /**
   * @return get z-score (number of standard deviations) for vertical velocity
   */
  double getVerticalSpeedZScore() const;

  /**
   * @return set z-score (number of standard deviations) for vertical velocity (non-negative value)
   */
  void setVerticalSpeedZScore(double val);

  std::string toString() const;

  std::string toPVS() const;

  void set_global_SUM_parameters(const DaidalusParameters& p);

};
}
#endif
