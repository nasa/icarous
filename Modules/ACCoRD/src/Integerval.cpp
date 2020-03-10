/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Integerval.h"
#include "format.h"

namespace larcfm {

Integerval::Integerval(int l, int u) {
  lb = l;
  ub = u;
}

std::string Integerval::toString() const {
  return "[" + Fmi(lb) + "," + Fmi(ub) + "]";
}

std::string Integerval::FmVector(const std::vector<Integerval>& l) {
  std::string s = "[";
  bool space = false;
  for (std::vector<Integerval>::size_type i=0; i < l.size(); ++i) {
    if (space) {
      s += " ";
    } else {
      space = true;
    }
    s += l[i].toString();
  }
  s += "]";
  return s;
}


}
