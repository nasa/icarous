/*
 * Tuple6.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 *
 * a four element tuple
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#ifndef Tuple6_H_
#define Tuple6_H_

namespace larcfm {

/** A 6-dimensional tuple 
 * @param <T1> type of first element
 * @param <T2> type of second element
 * @param <T3> type of third element
 * @param <T4> type of fourth element
 * @param <T5> type of fifth element
 * @param <T6> type of sixth element
 * */
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class Tuple6 {

public:
 T1 first;
 T2 second;
 T3 third;
 T4 fourth;
 T5 fifth;
 T6 sixth;

 /** Construct a new Tuple6 object */
  Tuple6(): first(T1()), second(T2()), third(T3()), fourth(T4()), fifth(T5()), sixth(T6()) {}
  
  /** Construct a new Tuple6 object */
  Tuple6(const T1& first, const T2& second, const T3& third, const T4& fourth, const T5& fifth, const T6& sixth) :
	  first(first), second(second), third(third), fourth(fourth), fifth(fifth), sixth(sixth) {
  }
 
    /** Return the first element of this Tuple6 
     * @return element */
  T1 getFirst() const {
    return first;
    }
 
    /** Return the second element of this Tuple6 
     * @return element */
  T2 getSecond() const {
    return second;
    }
 
    /** Return the third element of this Tuple6 
     * @return element */
  T3 getThird() const {
    return third;
  }
 
    /** Return the fourth element of this Tuple6 
     * @return element */
  T4 getFourth() const {
    return fourth;
  }

    /** Return the fifth element of this Tuple6 
     * @return element */
  T5 getFifth() const {
    return fifth;
  }

    /** Return the fifth element of this Tuple6 
     * @return element */
  T6 getSixth() const {
    return sixth;
  }


  /** Copy a Tuple6 object */
  template<typename U1, typename U2, typename U3, typename U4, typename U5, typename U6>
  Tuple6(const Tuple6<U1,U2,U3,U4,U5,U6>& q): first(q.first), second(q.second), third(q.third), fourth(q.fourth), fifth(q.fifth) , sixth(q.sixth){}
 
  /** Make a new Tuple6 object */
  static Tuple6<T1,T2,T3,T4,T5, T6> make(const T1& e1, const T2& e2, const T3& e3, const T4& e4, const T5& e5, const T6& e6) {
    return Tuple6<T1,T2,T3,T4, T5, T6>(e1, e2, e3, e4, e5, e6);
  }
};

  /** Are these two Tuple6 objects equal? */
  template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  bool operator== (const Tuple6<T1,T2,T3,T4,T5,T6>& q1, const Tuple6<T1,T2,T3,T4,T5,T6>& q2) {
    return q1.first == q2.first && q1.second == q2.second && q1.third == q2.third && q1.fourth == q2.fourth && q1.fifth == q2.fifth && q1.sixth == q2.sixth;
  }

  /** Are these two Tuple6 objects unequal? */
  template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  bool operator!= (const Tuple6<T1,T2,T3,T4,T5,T6>& q1, const Tuple6<T1,T2,T3,T4,T5,T6>& q2) {
    return !(q1 == q2);
  }

  /** ordering */
  template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  bool operator< (const Tuple6<T1,T2,T3,T4,T5,T6>& q1, const Tuple6<T1,T2,T3,T4,T5,T6>& q2) {
	  return q1 != q2 && !(q1 < q2);
  }


}

#endif /* Tuple6_H_ */
