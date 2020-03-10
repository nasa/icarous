/*
 * Const.h
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CONSTS_H_
#define CONSTS_H_

#include <string>

namespace larcfm {

  /** Flag indicating an entry into the protected zone */
  static const int Entry       = -1;
  /** Flag indicating an exit from the protected zone */
  static const int Exit        =  1;
  
  // Type of resolution

  /** Conflict resolution is not needed (0) */
  static const int Unnecessary =  0;
  /** No resolution available (-1) */
  static const int None        =  -1;
  /**  Conflict resolution (1) */
  static const int Conflict    =  1;
  /**  Loss of separation resolution (2) */
  static const int LoS         =  2;
  /**  Loss of separation (Convergent) resolution (2) */
  static const int LoSConv     =  2;
  /**  Loss of separation (Divergent) resolution (2) */
  static const int LoSDivg     =  3;

}


/*  static std::string resolutionStr(int r) {         // for debug purposes
     if (r == -1) return "None";
     else if (r == 0) return "Unnecessary";
     else if (r == 1) return "Conflict";
     else if (r == 2) return "LoS";
     else return "?????";
  }
*/

#endif /* CONSTS_H */
