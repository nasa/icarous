/*
 * Implementation of bands hysteresis logic that includes MofN and persistence.
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BandsHysteresis.h"
#include "DaidalusParameters.h"

namespace larcfm {

/*
 * Creates an empty object
 */
BandsHysteresis::BandsHysteresis() {
  mod_ = 0.0;
  mod2_ = 0.0;
  hysteresis_time_ = 0;
  persistence_time_ = 0;
  m_ = 0;
  n_ = 0;
  reset();
}

void BandsHysteresis::setMod(double mod) {
  mod_ = mod;
  mod2_ = mod/2.0;
}

/*
 * Sets hysteresis and persistence time
 */
void BandsHysteresis::initialize(double hysteresis_time,
    double persistence_time, bool bands_persistence,
    int m, int n) {
  hysteresis_time_ = hysteresis_time;
  persistence_time_ = persistence_time;
  bands_persistence_ = bands_persistence;
  m_ = m;
  n_ = n;
}

double BandsHysteresis::getLastTime() const {
  return last_time_;
}

bool BandsHysteresis::getLastPreferredDirection() const {
  return preferred_dir_;
}

double BandsHysteresis::getLastResolutionLow() const {
  return resolution_low_;
}

double BandsHysteresis::getLastResolutionUp() const {
  return resolution_up_;
}

int BandsHysteresis::getLastNFactorLow() const {
  return nfactor_low_;
}

int BandsHysteresis::getLastNFactorUp() const {
  return nfactor_up_;
}

const std::vector<BandsMofN>& BandsHysteresis::bands_mofn() const {
  return bands_mofn_;
}

/*
 * Reset object
 */
void BandsHysteresis::reset() {
  bands_mofn_.clear();
  last_time_ = NAN;

  preferred_dir_ = false;
  time_of_dir_ = NAN;

  conflict_region_ = BandsRegion::UNKNOWN;
  conflict_region_low_ = NAN;
  conflict_region_up_ = NAN;
  time_of_conflict_region_ = NAN;

  resolution_up_ = NAN;
  resolution_low_ = NAN;
  nfactor_up_ = 0;
  nfactor_low_ = 0;
}

/*
 * Reset object if current time is older than hysteresis time with
 * respect to last time. This method update last_time
 */
void BandsHysteresis::resetIfCurrentTime(double current_time) {
  if (ISNAN(last_time_) ||
      current_time <= last_time_ ||
      current_time-last_time_ > hysteresis_time_) {
    reset();
  }
  last_time_ = current_time;
}

// Implement MofN logic for bands
void BandsHysteresis::m_of_n(std::vector<ColorValue>& lcvs) {
  if (hysteresis_time_ > 0 && m_ > 0 && m_ <= n_) {
    if (bands_mofn_.empty()) {
      // Initialize list of MofN values
      std::vector<ColorValue>::const_iterator cv_ptr;
      for(cv_ptr = lcvs.begin(); cv_ptr != lcvs.end(); ++cv_ptr) {
        bands_mofn_.push_back(BandsMofN(*cv_ptr,m_,n_));
      }
    }
    // Produce new list of color values in place
    std::vector<BandsMofN>::iterator i_ptr = bands_mofn_.begin();
    std::vector<ColorValue>::iterator j_ptr = lcvs.begin();
    while (i_ptr != bands_mofn_.end() && j_ptr != lcvs.end()) {
      if (Util::almost_equals(i_ptr->val,j_ptr->val,DaidalusParameters::ALMOST_)) {
        j_ptr->color_left=i_ptr->left_m_of_n(j_ptr->color_left);
        j_ptr->color_right=i_ptr->right_m_of_n(j_ptr->color_right);
        ++i_ptr;
        ++j_ptr;
      } else if (Util::almost_less(i_ptr->val,j_ptr->val,DaidalusParameters::ALMOST_)) {
        j_ptr = lcvs.insert(j_ptr,ColorValue(j_ptr->color_left,i_ptr->val,
            j_ptr->color_left));
      } else {
        i_ptr = bands_mofn_.insert(i_ptr,BandsMofN(j_ptr->val,i_ptr->colors_left));
      }
    }

    // Remove values whose of mofns queues are the same both sides
    i_ptr = bands_mofn_.begin();
    while (i_ptr != bands_mofn_.end()) {
      if (i_ptr != bands_mofn_.begin() && (i_ptr+1) != bands_mofn_.end() &&
          i_ptr->same_colors()) {
        i_ptr = bands_mofn_.erase(i_ptr);
      } else {
        ++i_ptr;
      }
    }
  }
}

void BandsHysteresis::conflict_region_persistence(const std::vector<BandsRange>& ranges, int idx) {
  // Set conflict region for bands persistence
  if (0 <= idx && idx < static_cast<int>(ranges.size()) && BandsRegion::isConflictBand(ranges[idx].region)) {
    conflict_region_ = ranges[idx].region;
    if (mod_ == 0 || (0 < idx && idx < static_cast<int>(ranges.size()-1))) {
      conflict_region_low_ = ranges[idx].interval.low;
      conflict_region_up_ = ranges[idx].interval.up;
    } else if (idx == 0) {
      // In this case mod > 0
      conflict_region_up_ = ranges[0].interval.up;
      if (ranges[0].region ==
          ranges[ranges.size()-1].region) {
        conflict_region_low_ = ranges[ranges.size()-1].interval.low;
      } else {
        conflict_region_low_ = ranges[0].interval.low;
      }
    } else {
      // In this case mod > 0 && idx == ranges.size()-1
      conflict_region_low_ = ranges[idx].interval.low;
      if (ranges[0].region == ranges[idx].region) {
        conflict_region_up_ = ranges[0].interval.up;
      } else {
        conflict_region_up_ = ranges[idx].interval.up;
      }
    }
  } else {
    time_of_conflict_region_ = NaN;
    conflict_region_ = BandsRegion::UNKNOWN;
    conflict_region_low_ = NaN;
    conflict_region_up_ = NaN;
  }
}

// Implement persistence logic for bands
int BandsHysteresis::bandsPersistence(std::vector<BandsRange>& ranges, std::vector<ColorValue>& lcvs, bool recovery, double val) {
  if (hysteresis_time_ > 0 && persistence_time_ > 0 && bands_persistence_) {
    // last_time is never NaN
    if (ISFINITE(conflict_region_low_) && ISFINITE(conflict_region_up_)) {
      // current val is still between conflict_region_low and conflict_region_up
      BandsRegion::Region current_region = ColorValue::region_of(lcvs,val);
      // In the following condition is important that RECOVERY is the higher value,
      // since it overwrites all other colors.
      // Therefore, orderOfRegion is used instead of orderOfConflictRegion.
      if (BandsRegion::isValidBand(current_region) && BandsRegion::isConflictBand(conflict_region_)) {
        if (!ISNAN(time_of_conflict_region_) &&
            BandsRegion::orderOfRegion(current_region) < BandsRegion::orderOfRegion(conflict_region_) &&
            last_time_ >= time_of_conflict_region_ &&
            last_time_-time_of_conflict_region_ < persistence_time_) {
          // Keep the previous conflict band (persistence logic prevails)
          ColorValue::insert_with_mod(lcvs,conflict_region_low_,conflict_region_up_,mod_,conflict_region_);
        } else {
          if (BandsRegion::isConflictBand(current_region) &&
              BandsRegion::orderOfRegion(current_region) > BandsRegion::orderOfRegion(conflict_region_)) {
            time_of_conflict_region_ = last_time_;
          }
        }
      }
    } else {
      time_of_conflict_region_ = last_time_;
    }
  }
  // Compute Color bands
  BandsRange::makeRangesFromColorValues(ranges,lcvs,recovery);
  int idx = BandsRange::index_of(ranges,val,mod_);
  if (!ISNAN(time_of_conflict_region_)) {
    // Persistence is enabled
    conflict_region_persistence(ranges,idx);
  }
  return idx;
}

// check if region is below corrective region
bool BandsHysteresis::is_below_corrective_region(BandsRegion::Region corrective_region, BandsRegion::Region region) {
  // In the following core is important that RECOVERY and NONE both have the order 0,
  // since this function is used to find a resolution.
  // For that reason, orderOfConflictRegion is used instead of orderOfRegion
  return BandsRegion::isValidBand(region) &&
      BandsRegion::orderOfConflictRegion(region) <
      BandsRegion::orderOfConflictRegion(corrective_region);
}

// check if regions from idx_from to idx_to are resolutions
bool BandsHysteresis::contiguous_resolution_region(const std::vector<BandsRange>& ranges,
    BandsRegion::Region corrective_region, bool dir, int idx_from, int idx_to) const {
  int idx=idx_from;
  while (idx != idx_to && 0 <= idx && idx < static_cast<int>(ranges.size()) &&
      is_below_corrective_region(corrective_region,ranges[idx].region)) {
    if (dir) {
      ++idx;
      if (mod_ > 0 &&  idx == static_cast<int>(ranges.size())) {
        idx=0;
      }
    } else {
      if (mod_ > 0 && idx == 0) {
        idx = ranges.size()-1;
      }
      --idx;
    }
  }
  return 0 <= idx && idx < static_cast<int>(ranges.size()) &&
      is_below_corrective_region(corrective_region,ranges[idx].region);
}

// Returns true if a is to the left of b (modulo mod). If mod is 0, this is the same a < b
// Note that, by definition, if a is almost equal to b, then a is to the left of b.
bool BandsHysteresis::to_the_left(double a, double b) const {
  if (Util::almost_equals(a,b,DaidalusParameters::ALMOST_)) {
    return true;
  }
  if (mod_ == 0) {
    return Util::almost_less(a,b,DaidalusParameters::ALMOST_);
  } else {
    return Util::almost_less(Util::modulo(b-a,mod_),mod2_,DaidalusParameters::ALMOST_);
  }
}

void BandsHysteresis::resolutionsHysteresis(const std::vector<BandsRange>& ranges,
    BandsRegion::Region corrective_region, double delta, int nfactor,
    double val, int idx_l, double res_l, int idx_u, double res_u) {
  // last_time is never NaN
  if (hysteresis_time_ <= 0 || ISNAN(time_of_dir_) ||
      last_time_ <= time_of_dir_ || delta <= 0) {
    // No hysteresis at this time
    resolution_low_ = res_l;
    nfactor_low_ = nfactor;
    resolution_up_ = res_u;
    nfactor_up_ = nfactor;
    time_of_dir_ = NaN;
  } else {
    // Make sure that old resolutions are still valid. If not, reset them.
    // persistence of up/right resolutions
    int idx = -1;
    if (ISFINITE(res_u) && // Exists a current resolution
        ISFINITE(resolution_up_) && // Previous resolution exists
        // either up/right was the preferred direction or a up/right was a recovery resolution
        (preferred_dir_ || (nfactor_up_ >= 0 && nfactor_up_ < nfactor)) &&
        // Previous resolution is in the same direction as new one
        to_the_left(val,resolution_up_) &&
        // Before persistence time or within delta of new resolution
        (last_time_-time_of_dir_ < persistence_time_ ||
            nfactor_up_ < nfactor ||
            Util::almost_less(Util::safe_modulo(resolution_up_-res_u,mod_),delta,
                DaidalusParameters::ALMOST_))) {
      idx=BandsRange::index_of(ranges,resolution_up_,mod_);
    }
    if (0 <= idx && idx < static_cast<int>(ranges.size()) &&
        contiguous_resolution_region(ranges,corrective_region,preferred_dir_,
            idx_u,idx)) {
      // Do nothing: keep old up/right resolution
    } else {
      resolution_up_ = res_u;
      nfactor_up_ = nfactor;
    }
    // persistence of low/left resolutions
    idx = -1;
    if (ISFINITE(res_l) && // Exists a current resolution
        ISFINITE(resolution_low_) && // Previous resolution exists
        // either low/left was the preferred direction or a low/left was a recovery resolution
        (!preferred_dir_ || (nfactor_low_ >= 0 && nfactor_low_ < nfactor)) &&
        // Previous resolution is in the same direction as new one
        to_the_left(resolution_low_,val) &&
        // Before persistence time or within delta of new resolution
        (last_time_-time_of_dir_ < persistence_time_ ||
            nfactor_low_ < nfactor ||
            Util::almost_less(Util::safe_modulo(res_l-resolution_low_,mod_),delta,
                DaidalusParameters::ALMOST_))) {
      idx=BandsRange::index_of(ranges,resolution_low_,mod_);
    }
    if (0 <= idx && idx < static_cast<int>(ranges.size()) &&
        contiguous_resolution_region(ranges,corrective_region,preferred_dir_,
            idx_l,idx)) {
      // Do nothing: keep old low/left resolution
    } else {
      resolution_low_ = res_l;
      nfactor_low_ = nfactor;
    }
  }
}

void BandsHysteresis::preferredDirectionHysteresis(double delta, double val, double low, double up) {
  if (!ISFINITE(up) && !ISFINITE(low)) {
    time_of_dir_ = NaN;
    preferred_dir_ = false;
  } else if (!ISFINITE(up)) {
    time_of_dir_ = last_time_;
    preferred_dir_ = false;
  } else if (!ISFINITE(low)) {
    time_of_dir_ = last_time_;
    preferred_dir_ = true;
  } else {
    double mod_up = Util::safe_modulo(up-val,mod_);
    double mod_down = Util::safe_modulo(val-low,mod_);
    bool actual_dir = Util::almost_leq(mod_up,mod_down,DaidalusParameters::ALMOST_);
    if (hysteresis_time_ <= 0 || ISNAN(time_of_dir_) ||
        last_time_ < time_of_dir_ || delta <= 0) {
      time_of_dir_ = last_time_;
      preferred_dir_ = actual_dir;
    } else if (last_time_-time_of_dir_ < persistence_time_ ||
        std::abs(mod_up-mod_down) < delta) {
      // Keep the previous direction (persistence logic prevails)
    } else if ((preferred_dir_ && nfactor_low_ > nfactor_up_) ||
        (!preferred_dir_ && nfactor_up_ > nfactor_low_)) {
      // Keep the previous direction (do not change to a greater nfactor)
    } else if (preferred_dir_ != actual_dir) {
      // Change direction, update time_of_dir
      preferred_dir_ = actual_dir;
      time_of_dir_ = last_time_;
    }
  }
}

// check if region is above corrective region (corrective or above)
bool BandsHysteresis::is_up_from_corrective_region(BandsRegion::Region corrective_region, BandsRegion::Region region) {
  // In the following core is important that RECOVERY and NONE both have the order 0,
  // since this function is used to find a resolution.
  // For that reason, orderOfConflictRegion is used instead of orderOfRegion
  return BandsRegion::isConflictBand(region) &&
      BandsRegion::orderOfConflictRegion(region) >=
      BandsRegion::orderOfConflictRegion(corrective_region);
}

void BandsHysteresis::bandsHysteresis(const std::vector<BandsRange>& ranges,
    BandsRegion::Region corrective_region, double delta, int nfactor, double val, int idx) {
  double res_l = NaN;
  double res_u = NaN;
  int idx_l = idx;
  int idx_u = idx;
  // Find actual resolutions closest to current value
  if (0 <= idx && idx < static_cast<int>(ranges.size()) && is_up_from_corrective_region(corrective_region,ranges[idx].region)) {
    // There is a conflict
    int last_index = ranges.size()-1;
    // Find low/left resolution
    res_l = NINFINITY;
    while (idx_l >= 0 && is_up_from_corrective_region(corrective_region,ranges[idx_l].region)) {
      if (to_the_left(ranges[idx_l].interval.low,val)) {
        //if (Util.almost_less(Util.safe_modulo(val-ranges.get(idx_l).interval.low,mod),min_relative,
        //      DaidalusParameters.ALMOST_)) {
        if (idx_l == 0 && mod_ > 0) {
          idx_l = last_index;
        } else {
          --idx_l;
        }
      } else {
        idx_l = -1;
      }
      if (idx_l == idx) {
        // Already went around
        idx_l = -1;
      }
    }
    if (idx_l >= 0 && BandsRegion::isValidBand(ranges[idx_l].region)) {
      if (idx_l == last_index && mod_ > 0) {
        res_l = 0;
      } else {
        res_l = ranges[idx_l].interval.up;
      }
    }
    // Find up/right resolution
    res_u = PINFINITY;
    while (idx_u <= last_index && is_up_from_corrective_region(corrective_region,ranges[idx_u].region)) {
      if (to_the_left(val,ranges[idx_u].interval.up)) {
        //if (Util.almost_less(Util.safe_modulo(ranges.get(idx_u).interval.up-val,mod),max_relative,
        //      DaidalusParameters.ALMOST_)) {
        if (idx_u == last_index && mod_ > 0) {
          idx_u = 0;
        } else {
          ++idx_u;
        }
      } else {
        idx_u = last_index+1;
      }
      if (idx_u == idx) {
        // Already went around
        idx_u = last_index+1;
      }
    }
    if (idx_u <= last_index && BandsRegion::isValidBand(ranges[idx_u].region)) {
      res_u = ranges[idx_u].interval.low;
    }
  }
  resolutionsHysteresis(ranges,corrective_region,delta,nfactor,val,idx_l,res_l,idx_u,res_u);
  preferredDirectionHysteresis(delta,val,res_l,res_u);
}

std::string BandsHysteresis::toString() const {
  std::string s = "# Hysteresis variables\n";
  s += "hysteresis_time_ = "+FmPrecision(hysteresis_time_)+"\n";
  s += "persistence_time_ = "+FmPrecision(persistence_time_)+"\n";
  s += "bands_persistence_ = "+Fmb(bands_persistence_)+"\n";
  s += "last_time_ = "+FmPrecision(last_time_)+"\n";
  s += "m_ = "+Fmi(m_)+"\n";
  s += "n_ = "+Fmi(n_)+"\n";
  for (int i = 0; i < static_cast<int>(bands_mofn_.size()); ++i) {
    s+="bands_mofn_["+Fmi(i)+"] = ";
    s+=bands_mofn_[i].toString()+"\n";
  }
  s += "preferred_dir_ = "+Fmb(preferred_dir_)+"\n";
  s += "time_of_dir_ = "+FmPrecision(time_of_dir_)+"\n";
  s += "resolution_low_ = "+FmPrecision(resolution_low_)+"\n";
  s += "resolution_up_ = "+FmPrecision(resolution_up_)+"\n";
  s += "nfactor_low_ = "+Fmi(nfactor_low_)+"\n";
  s += "nfactor_up_ = "+Fmi(nfactor_up_)+"\n";
  s += "conflict_region_ = "+BandsRegion::to_string(conflict_region_)+"\n";
  s += "conflict_region_low_ = "+FmPrecision(conflict_region_low_)+"\n";
  s += "conflict_region_up_ = "+FmPrecision(conflict_region_up_)+"\n";
  s += "time_of_conflict_region_ = "+FmPrecision(time_of_conflict_region_)+"\n";
  return s;
}

} /* namespace larcfm */

