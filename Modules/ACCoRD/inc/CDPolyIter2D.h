/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef CDPOLYITER2D_H_
#define CDPOLYITER2D_H_

#include <vector>

#include "Poly2D.h"
#include "MovingPolygon2D.h"

namespace larcfm {
  class CDPolyIter2D {

  private:

    int conflicts;
    std::vector<double> timesin;
    std::vector<double> timesout;
    static const double microStepNum;

  public:

    // no conflict: timein = -1 AND timeout = 0;
     //std::vector<double> timesin = new std::vector<double>();
     //std::vector<double> timesout = new std::vector<double>();

     /**
      * Time to loss of separation in internal units.
      *
      * @return the time to loss of separation. If time is negative then there
      * is no conflict.   Note that this is a relative time.
      */

     std::vector<double> getTimesIn() const ;

     /**
      * Time to exit from loss of separation in internal units.
      *
      * @return the time to exit out loss of separation. If timeOut is zero then
      * there is no conflict. If timeOut is negative then, timeOut is infinite.
      * Note that this is a relative time.
      */
     std::vector<double> getTimesOut() const ;

     int getConflicts() const;

     double getTimeIn(int i) const;

     /**
      * Time to exit from loss of separation in internal units.
      *
      * @return the time to exit out loss of separation. If timeOut is zero then
      * there is no conflict. If timeOut is negative then, timeOut is infinite.
      * Note that this is a relative time.
      */
     double getTimeOut(int i) const ;

     double getTimeIn() const ;

     /**
      * Time to exit from loss of separation in internal units.
      *
      * @return the time to exit out loss of separation. If timeOut is zero then
      * there is no conflict. If timeOut is negative then, timeOut is infinite.
      * Note that this is a relative time.
      */
     double getTimeOut() const;


     double poly2D_detection_micro(const MovingPolygon2D& mpi, const Vect2& so, const Vect2& vo, double t0, bool t0in, double t1, bool t1in, double minstep) const;

//      bool poly2D_approx_detection(double T, const Poly2D& pi, const Vect2& vi, const Vect2& so, const Vect2& vo) ;




      //TODO note this may return conflict "blips" (tin=tout) -- may want to filter these out
     /**
      *
      * @param T  lookahead time
      * @param pi polygon
      * @param vi velocity vector for polygon
      * @param so ownship position
      * @param vo ownship velocity
      * @return true if ownship will enter loss with polygon pi within lookahead time T
      */
//     bool poly2D_detection(double T, Poly2D pi, const Vect2& vi, const Vect2& so, const Vect2& vo, double tStep) ;


     // MOVING Polygon Version
     bool polyIter2D_detection(double T, const MovingPolygon2D& mpi, const Vect2& so, const Vect2& vo, double tStep) ;



  };

}

#endif /* CDPOLYITER2D_H_ */
