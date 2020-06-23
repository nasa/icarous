/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef INDEXLEVELT_H_
#define INDEXLEVELT_H_

#include <vector>
#include <string>
#include "TrafficState.h"

namespace larcfm {

class IndexLevelT {
public:
  int index; // Aircraft 0-Index
  int level; // Alert level
  double time_horizon;
  // Time horizon for computation of bands, either lookahead time for conflict bands
  // or alerting time for peripheral bands

  IndexLevelT(int idx, int lvl, double th);

  std::string toString() const;

  /**
   * @return acs the list of aircraft identifiers from list of IndexLevelTs
   */
  static void toStringList(std::vector<std::string>& acs, const std::vector<IndexLevelT>& idxs, const std::vector<TrafficState>& traffic);

  /**
   * @return string representation of a list of IndexLevelTs
   */
  static std::string toString(const std::vector<IndexLevelT>& ilts);

};

}

#endif
