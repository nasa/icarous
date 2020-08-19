/* 
 * Bands interface
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef GENERICDHSTATEBANDS_H_
#define GENERICDHSTATEBANDS_H_

#include "GenericStateBands.h"
#include <string>

namespace larcfm {

/**
 * This provides an interface to classes for various conflict
 * prevention bands. The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
class GenericDHStateBands : public GenericStateBands {

 public:

  virtual ~GenericDHStateBands() {};

    /** Sets the minimum horizontal separation distance [nmi].  When
     * this parameter is set any existing band information is
     * cleared. */
    virtual void setDistance(double d, const std::string& unit) = 0;
    /** Returns the minimum horizontal separation distance [nmi] */
    virtual double getDistance(const std::string& uni) const = 0;
  
    /** Sets the minimum vertical separation distance [feet]. When this
     * parameter is set any existing band information is cleared. */
    virtual void setHeight(double h, const std::string& unit) = 0;
    /** Returns the minimum vertical separation distance. [feet] */
    virtual double getHeight(const std::string& unit) const = 0;

};

}
#endif
