/* 
 * Categories of Regions
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BandsRegion.h"
#include "format.h"
#include "string_util.h"

namespace larcfm {

BandsRegion::Region BandsRegion::valueOf(const std::string& s) {
  if (equals(s,"NONE")) return NONE;
  if (equals(s,"FAR")) return FAR;
  if (equals(s,"MID")) return MID;
  if (equals(s,"NEAR")) return NEAR;
  if (equals(s,"RECOVERY")) return RECOVERY;
  return UNKNOWN;
}

std::string BandsRegion::to_string(Region region) {
  if (region == NONE) return "NONE";
  if (region == FAR) return "FAR";
  if (region == MID) return "MID";
  if (region == NEAR) return "NEAR";
  if (region == RECOVERY) return "RECOVERY";
  return "UNKNOWN";
}

bool BandsRegion::isValidBand(Region region) {
  return region != UNKNOWN;
}

bool BandsRegion::isResolutionBand(Region region) {
  return region == NONE || region == RECOVERY;
}

bool BandsRegion::isConflictBand(Region region) {
  return isValidBand(region) && !isResolutionBand(region);
}

/**
 * @return NONE: 0, FAR: 1, MID: 2, NEAR: 3, RECOVERY: 4, UNKNOWN: -1
 */
int BandsRegion::orderOfRegion(Region region) {
  switch (region) {
  case NONE: return 0;
  case FAR: return 1;
  case MID: return 2;
  case NEAR: return 3;
  case RECOVERY: return 4;
  default: return -1;
  }
}

/**
 * @return NONE/RECOVERY: 0, FAR: 1, MID: 2, NEAR: 3, UNKNOWN: -1
 */
int BandsRegion::orderOfConflictRegion(Region region) {
  if (isConflictBand(region)) {
    return orderOfRegion(region);
  }
  if (isResolutionBand(region)) {
    return 0;
  }
  return -1;
}

/**
 * @return 0: NONE, 1: FAR, 2: MID, 3: NEAR, 4: RECOVERY, otherwise: UNKNOWN
 */
BandsRegion::Region BandsRegion::regionFromOrder(int i) {
  switch (i) {
  case 0: return NONE;
  case 1: return FAR;
  case 2: return MID;
  case 3: return NEAR;
  case 4: return RECOVERY;
  default: return UNKNOWN;
  }
}

}
