/* 
 * ErrorReporter
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef ERRORREPORTER_H_
#define ERRORREPORTER_H_

#include <string>

namespace larcfm {

class ErrorReporter {

 public:
  /** 
   * Does this object have an error?
   * @return true if there is an error.
   */
  virtual bool hasError() const = 0;

  /** 
   * Does this object have an error or a warning?
   * @return true if there is an error or warning.
   */
  virtual bool hasMessage() const = 0;

  /**
   * Return a string representation of any errors or warnings.
   * Calling this method will clear any messages and reset both the
   * error and warning status to none.
   * 
   * @return error and warning messages. If there are no messages, an empty string is returned. 
   */
  virtual std::string getMessage() = 0;

  /**
   * Return a string representation of any errors or warnings. Calling this
   * method will not clear the error or warning status (i.e., hasError() will
   * return the same value before and after this call.)
   * 
   * @return error and warning messages.  If there are 
   * no messages, an empty string is returned. 
   */
  virtual std::string getMessageNoClear() const = 0;

  virtual ~ErrorReporter() { /* empty */ };
};

}


#endif /* ERRORREPORTER_H_ */
