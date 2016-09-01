/* 
 * Categories of Regions
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BandsRegion.h"
#include "format.h"
#include "string_util.h"

namespace larcfm {

BandsRegion::Region BandsRegion::valueOf(const std::string& s) {
  if (equals(s, "NONE")) return NONE;
  if (equals(s, "FAR")) return FAR;
  if (equals(s, "MID")) return MID;
  if (equals(s, "NEAR")) return NEAR;
  if (equals(s, "RECOVERY")) return RECOVERY;
  return UNKNOWN;
}

std::string BandsRegion::to_string(Region rt) {
    if (rt == NONE) return "NONE";
    if (rt == FAR) return "FAR";
    if (rt == MID) return "MID";
    if (rt == NEAR) return "NEAR";
    if (rt == RECOVERY) return "RECOVERY";
    return "UNKNOWN";
}

bool BandsRegion::isValidBand(Region rt) {
  return rt != UNKNOWN;;
}

bool BandsRegion::isResolutionBand(Region rt) {
  return rt == NONE || rt == RECOVERY;
}

bool BandsRegion::isConflictBand(Region rt) {
  return isValidBand(rt) && !isResolutionBand(rt);
}

}
