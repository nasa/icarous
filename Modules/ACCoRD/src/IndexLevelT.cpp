/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "IndexLevelT.h"
#include "format.h"

#include <vector>
#include <string>
#include "TrafficState.h"

namespace larcfm {


IndexLevelT::IndexLevelT(int index, int level, double T) {
  index_ = index;
  level_ = level;
  T_ = T;
}

std::string IndexLevelT::toString() const {
  std::string s="(index: "+Fmi(index_)+", level: "+Fmi(level_)+", T: "+FmPrecision(T_)+")";
  return s;
}

/**
 * @return acs the list of aircraft identifiers from list of IndexLevelTs
 */
void IndexLevelT::toStringList(std::vector<std::string>& acs, const std::vector<IndexLevelT>& idxs, const std::vector<TrafficState>& traffic) {
  acs.clear();
  for (int i = 0; i < (int) idxs.size(); ++i) {
    IndexLevelT ilt = idxs[i];
    acs.push_back(traffic[ilt.index_].getId());
  }
}

/**
 * @return string representation of a list of IndexLevelTs
 */
std::string IndexLevelT::toString(const std::vector<IndexLevelT>& ilts) {
  std::string s = "{";
  bool comma=false;
  for (int i = 0; i < (int) ilts.size(); ++i) {
    IndexLevelT ilt = ilts[i];
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += ilt.toString();
  }
  s += "}";
  return s;
}

}
