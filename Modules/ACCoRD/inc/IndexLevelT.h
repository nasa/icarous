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
  int index_; // Aircraft 0-Index
  int level_; // Alert level
  double T_;  // Lookahead time

  IndexLevelT(int index, int level, double T);

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
