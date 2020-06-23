/* 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ColorValue.h"

#include "DaidalusParameters.h"
#include "format.h"

namespace larcfm {


ColorValue::ColorValue(BandsRegion::Region l, double v, BandsRegion::Region r) {
  val = v;
  color_left = l;
  color_right = r;
}

std::string ColorValue::toString() const {
  std::string s = "<"+BandsRegion::to_string(color_left)+", "+FmPrecision(val)+", "+BandsRegion::to_string(color_right)+">";
  return s;
}

std::string ColorValue::listToString(const std::vector<ColorValue>& l) {
  std::string s = "[";
  bool comma = false;
  for (std::vector<ColorValue>::const_iterator i_ptr = l.begin(); i_ptr != l.end(); ++i_ptr) {
    if (comma) {
      s+= ", ";
    } else {
      comma = true;
    }
    s += i_ptr->toString();
  }
  s += "]";
  return s;
}

/*
 * Initialize a list of color values, with min < max values, and interior color.
 * Initial list is has two color values: (unknown,min,int_color) and
 * (int_color,max,unknown).
 */
void ColorValue::init(std::vector<ColorValue>& l, double min, double max, BandsRegion::Region int_color) {
  init(l,min,max,min,max,int_color);
}

/*
 * Initialize a list of color values, with min <= min_val < max_val <= max values, and interior color.
 * Initial list is has at most four color values: (unknown,min,unknown),
 * (unknown,min_val,int_color),(int_color,max_val,unknown), and
 * (unknown,max,unknown).
 */
void ColorValue::init(std::vector<ColorValue>& l, double min, double max, double min_val, double max_val, BandsRegion::Region int_color) {
  l.clear();
  if (Util::almost_less(min,min_val,DaidalusParameters::ALMOST_)) {
    l.push_back(ColorValue(BandsRegion::UNKNOWN,min,BandsRegion::UNKNOWN));
  }
  l.push_back(ColorValue(BandsRegion::UNKNOWN,min_val,int_color));
  l.push_back(ColorValue(int_color,max_val,BandsRegion::UNKNOWN));
  if (Util::almost_less(max_val,max,DaidalusParameters::ALMOST_)) {
    l.push_back(ColorValue(BandsRegion::UNKNOWN,max,BandsRegion::UNKNOWN));
  }
}

/*
 * Initialize a list of color values with two color values:
 * (int_color,0,int_color), (int_color,mod,int_color)
 */
void ColorValue::init_with_mod(std::vector<ColorValue>& l, double mod, BandsRegion::Region int_color) {
  l.clear();
  l.push_back(ColorValue(int_color,0,int_color));
  l.push_back(ColorValue(int_color,mod,int_color));
}

/*
 * Initialize a list of color values, with min != max, and 0 <= min < max <= mod values,
 * and interior color.
 * If min = max: Initial list has two color values (unknown,0,int_color) and  (int_color,max,unknown)
 * If min < max: Initial list has four color values: (unknown,0,unknown),
 * (unknown,min,int_color), (int_color,max,unknown), and (unknown,mod,unknown).
 * If min > max: Initial list is has four color values: (unknown,0,int_color),
 * (int_color,max,unknown), (unknown,min,int_color), and (int_color,mod,unknown).
 */
void ColorValue::init_with_mod(std::vector<ColorValue>& l, double min, double max, double mod, BandsRegion::Region int_color) {
  if (mod > 0 && Util::almost_equals(Util::modulo(max-min,mod),0,DaidalusParameters::ALMOST_)) {
    init_with_mod(l,mod,int_color);
  } else {
    l.clear();
    if (Util::almost_equals(min,max,DaidalusParameters::ALMOST_)) {
      l.push_back( ColorValue(BandsRegion::UNKNOWN,0,int_color));
      l.push_back( ColorValue(int_color,mod,BandsRegion::UNKNOWN));
    } else if (min < max) {
      if (!Util::almost_equals(0,min,DaidalusParameters::ALMOST_)) {
        l.push_back( ColorValue(BandsRegion::UNKNOWN,0,BandsRegion::UNKNOWN));
      }
      l.push_back( ColorValue(BandsRegion::UNKNOWN,min,int_color));
      l.push_back( ColorValue(int_color,max,BandsRegion::UNKNOWN));
      if (!Util::almost_equals(max,mod,DaidalusParameters::ALMOST_)) {
        l.push_back( ColorValue(BandsRegion::UNKNOWN,mod,BandsRegion::UNKNOWN));
      }
    } else {
      l.push_back( ColorValue(int_color,0,int_color));
      l.push_back( ColorValue(int_color,max,BandsRegion::UNKNOWN));
      l.push_back( ColorValue(BandsRegion::UNKNOWN,min,int_color));
      l.push_back( ColorValue(int_color,mod,int_color));
    }
  }
}

/*
 * List l has been initialized and it's not empty. The bound l(0).val <= lb < ub <= l(n-1).val, where
 * n is the length of l. This function inserts (lb,ub) with the interior color int_color.
 */
void ColorValue::insert(std::vector<ColorValue>& l, double lb, double ub, BandsRegion::Region int_color) {
  if (l.empty() || Util::almost_geq(lb,ub,DaidalusParameters::ALMOST_) ||
      Util::almost_leq(ub,l.at(0).val,DaidalusParameters::ALMOST_) ||
      Util::almost_geq(lb,l.at(l.size()-1).val,DaidalusParameters::ALMOST_)) {
    return;
  }
  lb = Util::max(lb,l.at(0).val);
  ub = Util::min(ub,l.at(l.size()-1).val);
  int pivotl,pivotr;
  // Find a place to insert the lower bound of the interval
  pivotl = 0;
  pivotr = l.size()-1;
  while (pivotl+1 < pivotr) {
    int mid = (pivotl+pivotr)/2;
    if (Util::almost_less(lb,l.at(mid).val,DaidalusParameters::ALMOST_)) {
      pivotr = mid;
    } else if (Util::almost_greater(lb,l.at(mid).val,DaidalusParameters::ALMOST_)) {
      pivotl = mid;
    } else {
      pivotl = mid;
      pivotr = mid+1;
    }
  }
  int i = pivotl;
  // Insert lower bound as the color value (color,lb,int_color)
  if (i < static_cast<int>(l.size()-1) && !Util::almost_equals(l.at(i).val,lb,DaidalusParameters::ALMOST_) &&
      l.at(i).color_right != BandsRegion::UNKNOWN &&
      BandsRegion::orderOfRegion(l.at(i).color_right) < BandsRegion::orderOfRegion(int_color)) {
    // Insert the color value (ext_color,ii.low,color) to the right of the i-th point
    BandsRegion::Region ext_color = l.at(i).color_right;
    l.insert(l.begin()+i+1,ColorValue(ext_color,lb,ext_color));
    // The right color of the lb is set to ext_color to avoid breaking the color invariant.
    // This color will be repainted in the next loop.
    ++i;
  }
  // Find a place j where to insert the upper bound of the interval
  // Everything from the right of i to the left of j that can be overridden
  // by ext_color is re-painted
  for (; i < static_cast<int>(l.size()-1) && Util::almost_leq(l.at(i+1).val,ub,DaidalusParameters::ALMOST_); ++i) {
    if (l.at(i).color_right != BandsRegion::UNKNOWN &&
        BandsRegion::orderOfRegion(l.at(i).color_right) < BandsRegion::orderOfRegion(int_color)) {
      l[i].color_right = int_color;
      l[i+1].color_left = int_color;
    }
  }
  // Insert upper bound as the color value (int_color,ub,color)
  if (i < static_cast<int>(l.size()-1) && !Util::almost_equals(l.at(i).val, ub, DaidalusParameters::ALMOST_) &&
      l.at(i).color_right != BandsRegion::UNKNOWN &&
      BandsRegion::orderOfRegion(l.at(i).color_right) < BandsRegion::orderOfRegion(int_color)) {
    // Insert the color value (color,ii.up,ext_color) to the right of the i-th point
    BandsRegion::Region ext_color = l.at(i).color_right;
    l[i].color_right = int_color;
    l.insert(l.begin()+i+1,ColorValue(int_color,ub,ext_color));
    ++i;
  }
  // Take care of circular bands, e.g., those that do not have UNKNOWN in
  // the extremes.
  if (l[0].color_left != BandsRegion::UNKNOWN) {
    l[0].color_left = l[l.size()-1].color_left;
  }
  if (l[l.size()-1].color_right != BandsRegion::UNKNOWN) {
    l[l.size()-1].color_right = l[0].color_right;
  }
}

/*
 * List l has been initialized and it's not empty. Insert with mod logic. lb doesn't have to be
 * less than up
 */
void ColorValue::insert_with_mod(std::vector<ColorValue>& l, double lb, double ub, double mod, BandsRegion::Region int_color) {
  if (lb < ub) {
    insert(l,lb,ub,int_color);
  } else if (mod > 0){
    insert(l,0,ub,int_color);
    insert(l,lb,mod,int_color);
  }
}

/*
 *  Insert a "none set" of intervals into consistent list of color values. A "none set" is a
 *  sorted list of intervals, where values OUTSIDE the intervals have a given background color.
 *  The color inside the intervals is "transparent".
 *  The DAIDALUS core banding algorithm computes a none set for each bands region.
 */
void ColorValue::insertNoneSetToColorValues(std::vector<ColorValue>& l, const IntervalSet& none_set, BandsRegion::Region bg_color) {
  double min = l.at(0).val;
  double max = l.at(l.size()-1).val;
  for (int i=0; i < none_set.size();i++) {
    insert(l, min, none_set.getInterval(i).low, bg_color);
    min = none_set.getInterval(i).up;
  }
  insert(l,min,max,bg_color);
}

/*
 *  Insert a "recovery set" of intervals into consistent list of recovery color values. A "none set" is a
 *  sorted list of intervals, where values INSIDE the intervals have a RECOVERY color.
 *  The color outside the intervals is "transparent".
 *  The DAIDALUS core recovery banding algorithm computes a recovery set for the corrective region.
 */
void ColorValue::insertRecoverySetToColorValues(std::vector<ColorValue>& l, const IntervalSet& recovery_set) {
  for (int i=0; i < recovery_set.size();i++) {
    insert(l,recovery_set.getInterval(i).low,recovery_set.getInterval(i).up,BandsRegion::RECOVERY);
  }
}

/*
 * Return region given value in a list of color values. If the value is one of the color points,
 * return the smallest color according to orderOfConflictRegion.
 */
BandsRegion::Region ColorValue::region_of(const std::vector<ColorValue>& l, double val) {
  // In this function is important that RECOVERY and NONE both have the order 0,
  // since they represent close intervals.
  // For that reason, orderOfConflictRegion is used instead of orderOfRegion
  int i;
  for (i=0; i < static_cast<int>(l.size()) && Util::almost_less(l[i].val,val,DaidalusParameters::ALMOST_); ++i);
  if (i < static_cast<int>(l.size())) {
    if (Util::almost_equals(l[i].val,val,DaidalusParameters::ALMOST_)) {
      if (BandsRegion::isResolutionBand(l[i].color_right) || !BandsRegion::isValidBand(l[i].color_left)) {
        return l[i].color_right;
      } else if (BandsRegion::isResolutionBand(l[i].color_left) || !BandsRegion::isValidBand(l[i].color_right) ||
          BandsRegion::orderOfConflictRegion(l[i].color_left) <
          BandsRegion::orderOfConflictRegion(l[i].color_right)) {
        return l[i].color_left;
      } else {
        return l[i].color_right;
      }
    } else {
      return l[i].color_left;
    }
  }
  return BandsRegion::UNKNOWN;
}


}
