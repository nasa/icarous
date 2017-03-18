/*
 * Quad.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 *
 * a four element tuple
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#ifndef QUAD_H_
#define QUAD_H_

namespace larcfm {

/** A 4-dimensional tuple 
 * 
 * @param <T1> generic type, first element
 * @param <T2> generic type, second element
 * @param <T3> generic type, third element
 * @param <T4> generic type, fourth element
 */
template <typename T1, typename T2, typename T3, typename T4> 
class Quad {

public:
 T1 first;
 T2 second;
 T3 third;
 T4 fourth;

 /** Construct a new Quad object */
  Quad(): first(T1()), second(T2()), third(T3()), fourth(T4()) {}
  
  /** Construct a new Quad object */
  Quad(const T1& first, const T2& second, const T3& third, const T4& fourth) : first(first), second(second), third(third), fourth(fourth) {
  }
 
    /** Return the first element of this Quad 
     * @return first element
     * */
  T1 getFirst() const {
    return first;
    }
 
    /** Return the second element of this Quad 
     * @return second element
     * */
  T2 getSecond() const {
    return second;
    }
 
    /** Return the third element of this Quad 
     * @return third element
     * */
  T3 getThird() const {
    return third;
  }
 
    /** Return the fourth element of this Quad 
     * @return forth element
     * */
  T4 getFourth() const {
    return fourth;
  }

  /** Copy a Quad object */
  template<typename U1, typename U2, typename U3, typename U4>
  Quad(const Quad<U1,U2,U3,U4>& q): first(q.first), second(q.second), third(q.third), fourth(q.fourth) {}
 
  /** Make a new Quad object */
  static Quad<T1,T2,T3,T4> make(const T1& e1, const T2& e2, const T3& e3, const T4& e4) {
    return Quad<T1,T2,T3,T4>(e1, e2, e3, e4);
  }
};

  /** Are these two Quad objects equal? */
  template<typename T1, typename T2, typename T3, typename T4>
  bool operator== (const Quad<T1,T2,T3,T4>& q1, const Quad<T1,T2,T3,T4>& q2) {
    return q1.first == q2.first && q1.second == q2.second && q1.third == q2.third && q1.fourth == q2.fourth;
  }

  /** Are these two Quad objects unequal? */
  template<typename T1, typename T2, typename T3, typename T4>
  bool operator!= (const Quad<T1,T2,T3,T4>& q1, const Quad<T1,T2,T3,T4>& q2) {
    return !(q1 == q2);
  }


}

#endif /* QUAD_H_ */
