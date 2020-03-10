/* 
 * OutputList
 *
 * Contact: Jeff Maddalon
 *
 * Copyright (c) 2014-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#ifndef OUTPUTLIST_H
#define OUTPUTLIST_H

#include <string>
#include <vector>

namespace larcfm {

/**
 * Interface class for various readers that can read in parameters
 */
class OutputList {
  
  public:
  
	virtual ~OutputList() {};

    /**
     * Output key values from this object as a list of strings.
     */
	virtual std::vector<std::string> toStringList() const = 0;
};

}

#endif //OUTPUTLIST_H
