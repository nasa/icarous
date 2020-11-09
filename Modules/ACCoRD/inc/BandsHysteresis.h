/*
 * Implementation of bands hysteresis logic that includes MofN and persistence.
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BANDSHYSTERESIS_H_
#define BANDSHYSTERESIS_H_

#include "BandsRegion.h"
#include "BandsRange.h"
#include "BandsMofN.h"

namespace larcfm {

class BandsHysteresis {
public:

  /*
   * Creates an empty object
   */
  BandsHysteresis();

  void setMod(double mod);

  void initialize(double hysteresis_time,
      double persistence_time, bool bands_persistence,
      int m, int n);

  double getLastTime() const;

  bool getLastPreferredDirection() const;

  double getLastResolutionLow() const;

  double getLastResolutionUp() const;

  double getRawResolutionLow() const;

  double getRawResolutionUp() const;

  int getLastNFactorLow() const;

  int getLastNFactorUp() const;

  const std::vector<BandsMofN>& bands_mofn() const;

  /*
   * Reset object
   */
  void reset();

  /*
   * Reset object if current time is older than hysteresis time with
   * respect to last time
   */
  void resetIfCurrentTime(double current_time);

  // Implement MofN logic for bands
  void m_of_n(std::vector<ColorValue>& lcvs);

  // Implement persistence logic for bands. Return index in ranges of current value
  int bandsPersistence(std::vector<BandsRange>& ranges, std::vector<ColorValue>& lcvs, bool recovery, double val);

  void resolutionsHysteresis(const std::vector<BandsRange>& ranges,
      BandsRegion::Region corrective_region,
      double delta, int nfactor,
      double val, int idx_l, int idx_u);

  void preferredDirectionHysteresis(double delta, double nfactor, double val);

  void bandsHysteresis(const std::vector<BandsRange>& ranges,
      BandsRegion::Region corrective_region,
      double delta, int nfactor, double val, int idx);

  std::string toString() const;

  virtual ~BandsHysteresis() {};

private:
  double  mod_;
  double  mod2_;
  double hysteresis_time_;
  double persistence_time_;
  bool   bands_persistence_;
  double last_time_;
  int    m_;
  int    n_;
  std::vector<BandsMofN> bands_mofn_; // Color values for computation of MofN bands logic

  /* Parameters for hysteresis of preferred direction */
  bool    preferred_dir_; // Returned preferred direction after hysteresis
  double  time_of_dir_;   // Time of current preferred direction

  // Following variables are used in bands persistence
  BandsRegion::Region conflict_region_;  // Last conflict region
  double conflict_region_low_; // If last region is conflict band, lower bound of conflict band
  double conflict_region_up_;  // If last region is conflict band, upper bound of conflict band
  double time_of_conflict_region_; // Time of last region

  /*
   * resolution_up_,resolution_low_ are the resolution interval computed from all regions that are at least
   * as severe as the corrective region. Negative/positive infinity means no possible resolutions
   * NaN means no resolutions are computed (either because there is no conflict or
   * because of invalid inputs)
   */
  double resolution_up_;
  double resolution_low_;
  double raw_up_; // Resolution up without hysteresis
  double raw_low_; // Resolution low without hysteresis
  // nfactor >= 0 means recovery bands. It's the reduction factor for the internal cylinder
  int    nfactor_up_;
  int    nfactor_low_;

  void conflict_region_persistence(const std::vector<BandsRange>& ranges, int idx);

  // Returns true if a is to the left of b (modulo mod). If mod is 0, this is the same a < b
  // Note that, by definition, if a is almost equal to b, then a is to the left of b.
  bool to_the_left(double a, double b) const;

  // check if region is below corrective region
  static bool is_below_corrective_region(BandsRegion::Region corrective_region, BandsRegion::Region region);

  // check if regions from idx_from to idx_to are resolutions
  bool contiguous_resolution_region(const std::vector<BandsRange>& ranges,
      BandsRegion::Region corrective_region,
      bool dir, int idx_from, int idx_to) const;

  // check if region is above corrective region (corrective or above)
  static bool is_up_from_corrective_region(BandsRegion::Region corrective_region, BandsRegion::Region region);

  void switch_dir(bool dir, double nfactor);

};

} /* namespace larcfm */

#endif /* BANDSHYSTERESIS_H_ */
