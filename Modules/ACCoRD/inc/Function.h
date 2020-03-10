/*
 * Function.h
 *
 * Higher order functions
 * Author: cmunoz
 * Copyright (c) 2014-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

namespace larcfm {

template <typename inputtype, typename outputtype> class Function {
 public:
   virtual outputtype apply(inputtype) = 0;
   virtual ~Function() {}
};

}
#endif /* FUNCTION_H_ */
