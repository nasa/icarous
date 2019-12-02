/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef PARAMETERACCEPTOR_H_
#define PARAMETERACCEPTOR_H_

#include "ParameterData.h"
#include "ParameterProvider.h"


namespace larcfm {

class ParameterAcceptor : public ParameterProvider {

  public:

	virtual ~ParameterAcceptor() = 0;

//    virtual ParameterData getParameters() const = 0; // from ParameterProvider
//    virtual void updateParameterData(ParameterData& p) const = 0;
    virtual void setParameters(const ParameterData& p) = 0;


};

inline ParameterAcceptor::~ParameterAcceptor(){}

}

#endif //PARAMETERACCEPTOR_H_
