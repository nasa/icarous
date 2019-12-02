/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef SUMDATA_H_
#define SUMDATA_H_

#include <string>

namespace larcfm {

/** 
 * It is assumed that for all alert level i >= 1: detection(i+1) => detection(i) and that there is at least one
 * guidance level whose region is different from NONE. 
 */
class SUMData {

private:
  double s_EW_std_;     // East/West position standard deviation in internal units
  double s_NS_std_;     // North/South position standard deviation in internal units
  double s_EN_std_;     // East/North position standard deviation in internal units
  double sz_std_;       // Vertical position standard deviation in internal units
  double v_EW_std_;     // East/West velocity standard deviation in internal units
  double v_NS_std_;     // North/South velocity standard deviation in internal units
  double v_EN_std_;     // East/North velocity standard deviation in internal units
  double vz_std_;       // Vertical velocity standard deviation in internal units
  // The following errors are not multiplied by z-score yet
  double s_err_;        // Uncertainty error in the horizontal position
  double v_err_;        // Uncertainty error in the horizontal velocity

public:
  SUMData();

  SUMData(const SUMData& sum);

  static const SUMData& EMPTY();

  double get_s_EW_std() const;

  double get_s_EW_std(const std::string& u) const;

  double get_s_NS_std() const;

  double get_s_NS_std(const std::string& u) const;

  double get_s_EN_std() const;

  double get_s_EN_std(const std::string& u) const;

  double get_sz_std() const;

  double get_sz_std(const std::string& u) const;

  double get_v_EW_std() const;

  double get_v_EW_std(const std::string& u) const;

  double get_v_NS_std() const;

  double get_v_NS_std(const std::string& u) const;

  double get_v_EN_std() const;

  double get_v_EN_std(const std::string& u) const;

  double get_vz_std() const;

  double get_vz_std(const std::string& u) const;

  void set(const SUMData& sum);

  static double eigen_value_bound(double var1, double var2, double cov);

  /**
   * In PVS: covariance@h_pos_uncertainty and covariance@h_vel_uncertainty, but in Java they are not multiplied by the z-score yet.
   */
  static double horizontal_uncertainty(double x_std, double y_std, double xy_std);

  /**
   * s_EW_std: East/West position standard deviation in internal units
   * s_NS_std: North/South position standard deviation in internal units
   * s_EN_std: East/North position standard deviation in internal units
   */
  void setHorizontalPositionUncertainty(double s_EW_std, double s_NS_std, double s_EN_std);

  /**
   * sz_std : Vertical position standard deviation in internal units
   */
  void setVerticalPositionUncertainty(double sz_std);

  /**
   * v_EW_std: East/West velocity standard deviation in internal units
   * v_NS_std: North/South velocity standard deviation in internal units
   * v_EN_std: East/North velocity standard deviation in internal units
   */
  void setHorizontalVelocityUncertainty(double v_EW_std, double v_NS_std,  double v_EN_std);

  /**
   * vz_std : Vertical speed standard deviation in internal units
   */
  void setVerticalSpeedUncertainty(double vz_std);

  /**
   * Set all uncertainties to 0
   */
  void resetUncertainty();

  /**
   * @return Horizontal position error
   */
  double getHorizontalPositionError() const;

  /**
   * @return Vertical position error
   */
  double getVerticalPositionError() const;

  /**
     Set Horizontal speed error
   */
  double getHorizontalSpeedError() const;

  /**
   * @return Vertical speed error
   */
  double getVerticalSpeedError() const;

  /**
   * Check if aircraft is using sensor uncertainty mitigation
   */
  bool is_SUM() const;

};
}
#endif
