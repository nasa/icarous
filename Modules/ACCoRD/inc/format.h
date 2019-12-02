/*
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef FORMAT_H_
#define FORMAT_H_

#include <string>
#include <vector>
#include <map>
#include "Vect2.h"
#include "Vect3.h"
#include "Triple.h"

namespace larcfm {
  /** Format an int with 0 decimal places */
  std::string Fmi(int v);
  /** Format an unsigned in with 0 decimal places */
  std::string Fmui(unsigned int v);
  /** Format an unsigned long with 0 decimal places */
  std::string Fmul(unsigned long v);
  /** Format a double with 0 decimal places */
  std::string Fm0(double v);
  /** Format a double with 1 decimal place */
  std::string Fm1(double v);
  /** Format a double with 2 decimal place */
  std::string Fm2(double v);
  /** Format a double with 2 decimal place, filled with zeros */
  std::string Fm2z(double v);
  /** Format a double with 3 decimal place */
  std::string Fm3(double v);
  /** Format a double with 3 decimal place, filled with zeros */
  std::string Fm3z(double v);
  /** Format a double with 4 decimal place */
  std::string Fm4(double v);
  /** Format a double with 6 decimal place */
  std::string Fm6(double v);
  /** Format a double with 8 decimal place */
  std::string Fm8(double v);
  /** Format a double with 12 decimal place */
  std::string Fm12(double v);
  /** Format a double with 16 decimal place */
  std::string Fm16(double v);

  /** Format an integer to have at least minLength digits, prepending zeros as necessary */
  std::string FmLead(int i, int minLength);

  std::string FmPrecision(double v);
  std::string FmPrecision(double v, int precision);

  std::string Fmb(bool b);

  /** Format a vector */
  std::string FmVec(Vect2 v);
  /** Format a vector */
  std::string FmVec(Vect3 v);

    /** Return a string 'n' long with 's' left-justified */
  std::string padLeft(std::string s, int n);
  /** Return a string 'n' long with 's' right-justified */
  std::string padRight(std::string s, int n);

  /** Returns true/false string */
  std::string bool2str(bool b);

  /** send string to the console with a "carriage return" */
  void fpln(const std::string& str);
  /** send a string to the console without a "carriage return" */
  void fp(const std::string& str);
  /** send string to the console with a "carriage return" */
  void fpln(std::ostream* os, const std::string& str);
  /** send a string to the console without a "carriage return" */
  void fp(std::ostream* os, const std::string& str);
  /** send string to the error console with a "carriage return" */
  void fdln(const std::string& str);

  /** Format a position vector as a Euclidean position */
  std::string fvStrE(const Vect3& v);
  
  /** Format a position vector as a Euclidean position */
  std::string fvStrE(const Vect2& v);

  /** Format a position vector */
  std::string fsStr(const Vect2& s);

  /** Format a position vector */
  std::string fsStr(const Vect3& s);


  std::string fsStr8NP(const Vect3& s);

  std::string fsStr15NP(const Vect3& s);

  /** Format a velocity vector as a Euclidean velocity */
  std::string fvStr(const Vect2& s);
  
  /** Format a velocity vector as a Euclidean velocity */
  std::string fvStr(const Vect3& s);

  /** Format a velocity vector as a polar velocity */
  std::string fvStr2(const Vect2& v);
  
  /** Format a velocity vector as a polar velocity */
  std::string fvStr2(const Vect3& v);
  
  std::string FmPair(const std::pair<int,int>& p);
  std::string FmPair(const std::pair<double,double>& p);
  std::string FmTriple(const Triple<int,int,int>& p);
  std::string FmTriple(const Triple<double,double,double>& p);

  std::string Fobj(const std::vector<int>& v);
  std::string Fobj(const std::vector<double>& v);
  std::string Fobj(const std::vector<std::string>& v);
  std::string Fobj(const std::vector<std::pair<int,int> >& v);
  std::string Fobj(const std::vector<std::pair<double,double> >& v);
  std::string Fobj(const std::vector<Triple<int,int,int> >& v);
  std::string Fobj(const std::vector<Triple<double, double, double> >& v);

  std::string Fobj(const std::map<std::string,std::string>& v);
  std::string Fobj(const std::map<int,int>& v);
  std::string Fobj(const std::map<int,std::string>& v);
  std::string Fobj(const std::map<std::string,int>& v);
  std::string Fobj(const std::map<double,double>& v);
  std::string Fobj(const std::map<double,std::string>& v);
  std::string Fobj(const std::map<std::string,double>& v);

  std::string list2str(const std::vector<std::string>& l, const std::string& delimiter);

  std::string Farray(int const v[], int sz);
  std::string Farray(double const v[], int sz);
  std::string Farray(std::string const v[], int sz);

  std::string double2PVS(double val);
  std::string double2PVS(double val, int precision);

}
#endif /* FORMAT_H_ */
