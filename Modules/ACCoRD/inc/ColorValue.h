/* 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef COLORVALUE_H_
#define COLORVALUE_H_

#include "IntervalSet.h"
#include "BandsRegion.h"
#include <string>
#include <vector>

namespace larcfm {

class ColorValue {
public:
  double val;
  BandsRegion::Region color_left;
  BandsRegion::Region color_right;

  ColorValue(BandsRegion::Region l, double v, BandsRegion::Region r);
  std::string toString() const;
  static std::string listToString(const std::vector<ColorValue>& l);
  static void init(std::vector<ColorValue>& l, double min, double max, BandsRegion::Region int_color);
  static void init(std::vector<ColorValue>& l, double min, double max, double min_val, double max_val, BandsRegion::Region int_color);
  static void init_with_mod(std::vector<ColorValue>& l, double mod, BandsRegion::Region int_color);
  static void init_with_mod(std::vector<ColorValue>& l, double min, double max, double mod, BandsRegion::Region int_color);
  static void insert(std::vector<ColorValue>& l, double lb, double ub, BandsRegion::Region int_color);
  static void insert_with_mod(std::vector<ColorValue>& l, double lb, double ub, double mod, BandsRegion::Region int_color);
  static void insertNoneSetToColorValues(std::vector<ColorValue>& l, const IntervalSet& none_set, BandsRegion::Region bg_color);
  static void insertRecoverySetToColorValues(std::vector<ColorValue>& l, const IntervalSet& recovery_set);
  static BandsRegion::Region region_of(const std::vector<ColorValue>& l, double val);

};

}
#endif
