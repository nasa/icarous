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

#ifndef BANDSREGION_H_
#define BANDSREGION_H_

#include <string>

namespace larcfm {

class BandsRegion {
public:
  /*
   * NONE: No band
   * FAR: Far conflict band
   * MID: Mid conflict bands
   * NEAR: Near conflict band
   * RECOVERY: Band for violation recovery
   * UNKNOWN : Invalid band
   */

  enum Region {UNKNOWN,NONE,FAR,MID,NEAR,RECOVERY};

  // Number of conflict bands (NEAR, MID, FAR)
  static const int NUMBER_OF_CONFLICT_BANDS = 3;
  static bool isValidBand(Region region);
  static bool isResolutionBand(Region region);
  static bool isConflictBand(Region region);
  static Region valueOf(const std::string& str);
  static std::string to_string(Region region);
  static int orderOfRegion(Region region);
  static int orderOfConflictRegion(Region region);
  static Region regionFromOrder(int i);

};

}
#endif
