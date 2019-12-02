/*
 * Copyright (c) 2013-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * Detection3D.h
 *
 *  Created on: Dec 12, 2012
 *      Author: ghagen
 */

#ifndef PARAMETERTABLE_H_
#define PARAMETERTABLE_H_

#include "ParameterAcceptor.h"
#include "ParameterData.h"

namespace larcfm {

class ParameterTable : public ParameterAcceptor {
public:
  virtual ~ParameterTable() = 0;
  virtual std::string toString() const = 0;
};

inline ParameterTable::~ParameterTable(){}

} /* namespace larcfm */
#endif /* DETECTION3D_H_ */
