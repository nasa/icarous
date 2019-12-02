/* 
 * Categories of Regions
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
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
  enum Region {UNKNOWN,NONE,RECOVERY,NEAR,MID,FAR};
  static const int NUMBER_OF_CONFLICT_BANDS = 3;
  static bool isValidBand(Region rt);
  static bool isResolutionBand(Region rt);
  static bool isConflictBand(Region rt);
  static Region valueOf(const std::string& str);
  static std::string to_string(Region rt);
  static int order(Region rt);
  static int orderOfConflictRegion(Region rt);
  static Region conflictRegionFromOrder(int i);

};

}
#endif
