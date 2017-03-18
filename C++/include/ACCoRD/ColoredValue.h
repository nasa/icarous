/* 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef COLOREDVALUE_H_
#define COLOREDVALUE_H_

#include "BandsRange.h"
#include <string>
#include <vector>

namespace larcfm {

class ColoredValue {
public:
  double val;
  BandsRegion::Region color;

  ColoredValue(double v, BandsRegion::Region c);
  std::string toString() const;
  static void insert(std::vector<ColoredValue>& l, const Interval& ii,
      BandsRegion::Region lb_color, BandsRegion::Region ub_color);
  static void toBands(std::vector<BandsRange>& ranges, const std::vector<ColoredValue>& l);
};

}
#endif
