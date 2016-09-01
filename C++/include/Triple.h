/*
 * Triple.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 *
 * a three element tuple
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TRIPLE_H_
#define TRIPLE_H_

namespace larcfm {

  /** A class representing a three member tuple */
template <typename L, typename C, typename R> 
class Triple {
 public:
    /** The left-most element */ 
    L first;
    /** The center element */
    C second;
    /** The rightmost element */
    R third;
    
    /** Construct a Triple */
    Triple(): first(L()), second(C()), third(R()) {}
 
    /** Construct a Triple */
 Triple(const L& left, const C& center, const R& right) : first(left), second(center), third(right) {
    }

    /** The left-most element */ 
    L getLeft() const {
        return first;
    }
 
    /** The center element */
    C getCenter() const {
         return second;
    }
 
    /** The rightmost element */
    R getRight() const {
         return third;
    }


    /** The first (or leftmost) element */
    L getFirst() const {
        return first;
    }
 
    /** The second (or center) element */
    C getSecond() const {
         return second;
    }
 
    /** The third (or rightmost) element */
    R getThird() const {
         return third;
    }

    /** Copy a Triple */
    template <typename L1, typename C1, typename R1> 
    Triple(const Triple<L1,C1, R1>& t): first(t.left), second(t.center), third(t.right) {}
 };

/** Make a Triple */
 template <typename L, typename C, typename R> 
 Triple<L, C, R> make(const L& left, const C& center, const R& right) {
   return new Triple<L, C, R>(left, center, right);
 }

 /** Are these two Triples equal? */
  template <typename L, typename C, typename R> 
  bool operator== (const Triple<L,R,C>& t1, const Triple<L,R,C>& t2) {
    return t1.first == t2.first && t1.second == t2.second && t1.third == t2.third;
  }

  /** Are these two Triples unequal? */
  template <typename L, typename C, typename R> 
  bool operator!= (const Triple<L,R,C>& t1, const Triple<L,R,C>& t2) {
    return !(t1==t2);
  }
  
}


#endif /* TRIPLE_H_ */
