/*
 * Tuple5.h
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

#ifndef Tuple5_H_
#define Tuple5_H_

namespace larcfm {

/** A 5-dimensional tuple */
template <typename T1, typename T2, typename T3, typename T4, typename T5>
class Tuple5 {

public:
 T1 first;
 T2 second;
 T3 third;
 T4 fourth;
 T5 fifth;

 /** Construct a new Tuple5 object */
  Tuple5(): first(T1()), second(T2()), third(T3()), fourth(T4()), fifth(T5()) {}
  
  /** Construct a new Tuple5 object */
  Tuple5(const T1& first, const T2& second, const T3& third, const T4& fourth, const T5& fifth) :
	  first(first), second(second), third(third), fourth(fourth), fifth(fifth) {
  }
 
  /** Return the first element of this Tuple5 */
  T1 getFirst() const {
    return first;
    }
 
  /** Return the second element of this Tuple5 */
  T2 getSecond() const {
    return second;
    }
 
  /** Return the third element of this Tuple5 */
  T3 getThird() const {
    return third;
  }
 
  /** Return the fourth element of this Tuple5 */
  T4 getFourth() const {
    return fourth;
  }

  /** Return the fifth element of this Tuple5 */
  T5 getFifth() const {
    return fifth;
  }


  /** Copy a Tuple5 object */
  template<typename U1, typename U2, typename U3, typename U4, typename U5>
  Tuple5(const Tuple5<U1,U2,U3,U4,U5>& q): first(q.first), second(q.second), third(q.third), fourth(q.fourth), fifth(q.fifth) {}
 
  /** Make a new Tuple5 object */
  static Tuple5<T1,T2,T3,T4,T5> make(const T1& e1, const T2& e2, const T3& e3, const T4& e4, const T5& e5) {
    return Tuple5<T1,T2,T3,T4, T5>(e1, e2, e3, e4, e5);
  }
};

  /** Are these two Tuple5 objects equal? */
  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  bool operator== (const Tuple5<T1,T2,T3,T4,T5>& q1, const Tuple5<T1,T2,T3,T4,T5>& q2) {
    return q1.first == q2.first && q1.second == q2.second && q1.third == q2.third && q1.fourth == q2.fourth && q1.fifth == q2.fifth;
  }

  /** Are these two Tuple5 objects unequal? */
  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  bool operator!= (const Tuple5<T1,T2,T3,T4,T5>& q1, const Tuple5<T1,T2,T3,T4,T5>& q2) {
    return !(q1 == q2);
  }


}

#endif /* Tuple5_H_ */
