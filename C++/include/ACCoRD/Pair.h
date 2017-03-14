/*
 * Pair.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 *
 * a three element tuple
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PAIR_H_
#define PAIR_H_

namespace larcfm {

   /** A class representing a two member tuple */
template <typename L, typename R> 
class Pair {
 public:
    /** The left-most element */ 
    L left;
    /** The rightmost element */
    R right;
    
    /** Construct a Pair */
    Pair(): left(L()),  right(R()) {}
 
    /** Construct a Pair */
    Pair(const L& left, const R& right) : left(left),  right(right) {}

    /** The left-most element */ 
    L getLeft() const {
        return left;
    }
 
    /** The rightmost element */
    R getRight() const {
         return right;
    }


    /** The first (or leftmost) element */
    L getFirst() const {
        return left;
    }


    /** The first (or leftmost) element */
    R getSecond() const {
        return right;
    }

 
    /** Copy a Pair */
    template <typename L1,  typename R1> 
    Pair(const Pair<L1, R1>& t): left(t.left),  right(t.right) {}
 };

/** Make a Pair */
 template <typename L, typename R> 
 Pair<L, R> make(const L& left,  const R& right) {
   return new Pair<L, R>(left,  right);
 }

 /** Are these two Pairs equal? */
  template <typename L,  typename R> 
  bool operator== (const Pair<L,R>& t1, const Pair<L,R>& t2) {
    return t1.left == t2.left && t1.right == t2.right;
  }

  /** Are these two Pairs unequal? */
  template <typename L,  typename R> 
  bool operator!= (const Pair<L,R>& t1, const Pair<L,R>& t2) {
    return !(t1==t2);
  }
  
}


#endif /* PAIR_H_ */
