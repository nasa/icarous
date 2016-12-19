/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "WCV_HZ.h"
#include "WCV_VMOD.h"

namespace larcfm {

/** Constructor that uses the default TCAS tables. */
WCV_HZ::WCV_HZ() {
  wcv_vertical = new WCV_VMOD();
  id = "";
}

/** Constructor that specifies a particular instance of the TCAS tables. */
WCV_HZ::WCV_HZ(const WCVTable& tab) {
  wcv_vertical = new WCV_VMOD();
  table.copyValues(tab);
  id = "";
}

Detection3D* WCV_HZ::make() const {
  return new WCV_HZ();
}

/**
 * Returns a deep copy of this WCV_HZ object, including any results that have been calculated.
 */
Detection3D* WCV_HZ::copy() const {
  WCV_HZ* ret = new WCV_HZ();
  ret->table.copyValues(table);
  ret->id = id;
  return ret;
}

std::string WCV_HZ::getSimpleClassName() const {
  return "WCV_HZ";
}

bool WCV_HZ::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName())) {
    return containsTable((WCV_tvar*)cd);
  }
  return false;
}

}

