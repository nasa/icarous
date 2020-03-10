/*
 * BandsMofN.cpp
 *
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 */

#include "BandsMofN.h"
#include "format.h"
#include "DaidalusParameters.h"

namespace larcfm {

BandsMofN::BandsMofN(const ColorValue& cv, int m, int n) :
                                    val(cv.val),
                                    colors_left(m,n,BandsRegion::orderOfRegion(cv.color_left)),
                                    colors_right(m,n,BandsRegion::orderOfRegion(cv.color_right))
{}

BandsMofN::BandsMofN(double value, const MofN& mofn) :
                val(value),
                colors_left(mofn),
                colors_right(mofn)
{}

BandsRegion::Region BandsMofN::left_m_of_n(BandsRegion::Region region) {
  int code = colors_left.m_of_n(BandsRegion::orderOfRegion(region));
  return BandsRegion::regionFromOrder(code);

}

BandsRegion::Region BandsMofN::right_m_of_n(BandsRegion::Region region) {
  int code = colors_right.m_of_n(BandsRegion::orderOfRegion(region));
  return BandsRegion::regionFromOrder(code);
}

bool BandsMofN::same_colors() const {
  return colors_left.sameAs(colors_right);
}

std::string BandsMofN::toString() const {
  std::string s = "<"+colors_left.toString()+", "+FmPrecision(val)+", "+colors_right.toString()+">";
  return s;
}

std::string BandsMofN::listToString(const std::vector<BandsMofN>& l) {
  std::string s = "[";
  bool comma = false;
  for (std::vector<BandsMofN>::const_iterator i_ptr = l.begin(); i_ptr != l.end(); ++i_ptr) {
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

} /* namespace larcfm */


