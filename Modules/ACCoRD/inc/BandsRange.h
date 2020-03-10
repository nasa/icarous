/* 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BANDSRANGE_H_
#define BANDSRANGE_H_

#include "Interval.h"
#include "BandsRegion.h"
#include "ColorValue.h"

namespace larcfm {

class BandsRange {
public:

  // This numeric type is used for index variables over vectors of BandsRange
  typedef std::vector<BandsRange>::size_type nat;

  Interval    interval;
  BandsRegion::Region region;
  BandsRange(const Interval& i, BandsRegion::Region r);

  /*
   * Transforms a list of color values into a list of BandRanges.
   * This function avoids adding color points where the left and right colors are the same.
   */
  static void makeRangesFromColorValues(std::vector<BandsRange>& ranges, const std::vector<ColorValue>& l, bool recovery);

  /**
   * Return index in ranges_ where val is found, -1 if invalid input or not found. Notice that values at
   * the limits may be included or not depending on the regions.
   */
  static int index_of(const std::vector<BandsRange>& ranges, double val, double mod);

  std::string toString() const;
  std::string toPVS() const;

private:
  // NONE's when in recovery, become RECOVERY
  static BandsRegion::Region resolution_region(BandsRegion::Region region, bool recovery);

};

}
#endif
