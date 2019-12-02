/* 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef COLOREDVALUE_H_
#define COLOREDVALUE_H_

#include "BandsRange.h"
#include "IntervalSet.h"
#include <string>
#include <vector>

namespace larcfm {

class ColoredValue {
public:
  double val;
  BandsRegion::Region color_left;
  BandsRegion::Region color_right;

  ColoredValue(BandsRegion::Region l, double v, BandsRegion::Region r);
  std::string toString() const;
  static void init(std::vector<ColoredValue>& l, double min, double max, BandsRegion::Region int_color);
  static void init(std::vector<ColoredValue>& l, double min, double max, double min_val, double max_val, BandsRegion::Region int_color);
  static void init(std::vector<ColoredValue>& l, double min, double max, double mod, BandsRegion::Region int_color);
  static void insert(std::vector<ColoredValue>& l, double lb, double ub, BandsRegion::Region int_color);
  static void fromColoredValuestoBandsRanges(std::vector<BandsRange>& ranges, const std::vector<ColoredValue>& l);
  static void insertNoneSetToColoredValues(std::vector<ColoredValue>& l, IntervalSet none_set, BandsRegion::Region bg_color);
//  static void toBands(std::vector<BandsRange>& ranges, const std::vector<ColoredValue>& l);
};

}
#endif
