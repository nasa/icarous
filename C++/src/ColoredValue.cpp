/* 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ColoredValue.h"
#include "format.h"

namespace larcfm {

ColoredValue::ColoredValue(double v, BandsRegion::Region c) {
  val = v;
  color = c;
}

std::string ColoredValue::toString() const {
  std::string s = "("+Fm2(val)+", "+BandsRegion::to_string(color)+")";
  return s;
}

void ColoredValue::insert(std::vector<ColoredValue>& l, const Interval& ii,
    BandsRegion::Region lb_color, BandsRegion::Region ub_color) {
  int i;
  for (i=0; i < (int) l.size() && Util::almost_less(l[i].val,ii.low); ++i);
  if (i == (int) l.size()) {
    l.push_back(ColoredValue(ii.low,lb_color));
  } else if (!Util::almost_equals(l[i].val,ii.low)) {
    l.insert(l.begin()+i,ColoredValue(ii.low,lb_color));
  }
  for (;i < (int) l.size() && Util::almost_less(l[i].val,ii.up); ++i);
  if (i == (int) l.size()) {
    l.push_back(ColoredValue(ii.up,ub_color));
  } else if (Util::almost_equals(l[i].val,ii.up)) {
    l[i].color = ub_color;
  } else {
    l.insert(l.begin()+i,ColoredValue(ii.up,ub_color));
  }
}

void ColoredValue::toBands(std::vector<BandsRange>& ranges, const std::vector<ColoredValue>& l) {
  for (int i = 1; i < (int) l.size(); ++i) {
    ranges.push_back(BandsRange(Interval(l[i-1].val,l[i].val),l[i].color));
  }
}

}
