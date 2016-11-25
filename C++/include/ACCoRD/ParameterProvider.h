/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef PARAMETERPROVIDER_H_
#define PARAMETERPROVIDER_H_

#include "ParameterData.h"


namespace larcfm {

class ParameterProvider {

  public:

	virtual ~ParameterProvider() = 0;

    virtual ParameterData getParameters() const = 0;
    virtual void updateParameterData(ParameterData& p) const = 0;

};

inline ParameterProvider::~ParameterProvider(){}

}

#endif //PARAMETERPROVIDER_H_
