/* 
 * PlanReader
 *
 * Contact: George Hagen
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#ifndef PARAMETERREADER_H
#define PARAMETERREADER_H

#include "ParameterData.h"
#include <string>
#include <vector>

namespace larcfm {

/**
 * Interface class for various readers that can read in parameters
 */
class ParameterReader {
  
  public:
  
	virtual ~ParameterReader() {};

    virtual ParameterData& getParametersRef() = 0;

//   /**
//     * Returns true if the parameter key was defined.
//     */
//    virtual bool containsParameter(const std::string& key) const = 0;
//   /**
//     * Returns the string value of the given parameter key.
//     * This may be a space-delimited list.  If the key is not present,
//     * return the empty string.  Parameter keys may be case-sensitive.
//     */
//    virtual std::string getString(const std::string& key) const = 0;
//   /**
//     * Returns the Boolean value of the given parameter key.
//     * If the key is not present, or not representation of "true",
//     * return the empty string.  Parameter keys may be case-sensitive.
//     */
//    virtual bool getParameterBool(const std::string& key) const = 0;
//	/**
//     * Returns the double-precision value of the given parameter key.
//     * If units were specified in the file, this value has been converted into
//     * internal units. If no units were specified, then the value
//     * in the file is returned.  If the key is not present or if the
//     * value is not a numeral, then return 0.
//     * Parameter keys may be case-sensitive.
//     */
//  virtual double getParameterValue(const std::string& key) const = 0;
//   /**
//     * Returns the double-precision value of the given parameter key in internal units.
//     * If no units were specified in the file, then the defaultUnit parameter is used.
//     * If the key is not present or if the
//     * value is not a numeral, then return 0.
//     * Parameter keys may be case-sensitive.
//     */
//    virtual double getParameterValue(const std::string& key, const std::string& defaultUnit) const = 0;
//
//  /**
//   * Returns a vector of parameter strings encountered.
//   */
//  virtual std::vector<std::string> getParameterList() const = 0;
//
//  /**
//   * Checks the parameters against the supplied list, and sets a warning if any input parameters are not listed.
//   */
//  virtual void validateParameters(std::vector<std::string> c) const = 0;
//
//  /**
//   * Copies all parameters from the given object into the current object
//   * @param p source ParameterReader
//   * @param overwrite If set to true, duplicate keys will be overwritten in the local object, if false the local values will be preserved
//   */
//   virtual void copyParameters(const ParameterReader& p, bool overwrite) = 0;
//
//
//   virtual int getParameterInt(const std::string& key) const = 0;
//
//   virtual long getParameterLong(const std::string& key) const = 0;
//
//   virtual std::string getParameterUnit(const std::string& key) const = 0;
//
//   virtual void setParameter(const std::string& key, double value) = 0;
//
//   virtual void setParameter(const std::string& key, double value, const std::string& units) = 0;
//
//   virtual void setParameter(const std::string& key, bool value) = 0;
//
//   virtual void setParameter(const std::string& key, const std::string& value) = 0;
//
//   virtual void setParameter(const std::string& s) = 0;

};

}

#endif //PARAMETERREADER_H
