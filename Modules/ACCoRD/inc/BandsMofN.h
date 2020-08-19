/*
 * BandsMofN.h
 *
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BANDSMOFN_H_
#define BANDSMOFN_H_

#include <vector>
#include "MofN.h"
#include "BandsRegion.h"
#include "ColorValue.h"

namespace larcfm {

class BandsMofN {

public:
  double val;
  MofN colors_left;
  MofN colors_right;
  BandsMofN(const ColorValue& cv, int m, int n);
  BandsMofN(double value, const MofN& mofn);
  BandsRegion::Region left_m_of_n(BandsRegion::Region region);
  BandsRegion::Region right_m_of_n(BandsRegion::Region region);
  bool same_colors() const;
  std::string toString() const;
  static std::string listToString(const std::vector<BandsMofN>& l);

  virtual ~BandsMofN() {};

private:

};

}

#endif /* BANDSMOFN_H_ */
