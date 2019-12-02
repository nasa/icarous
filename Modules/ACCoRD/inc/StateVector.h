/*
 * StateVector
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 *
 * a three element tuple
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef STATEVECTOR_H_
#define STATEVECTOR_H_

namespace larcfm {

/**
 * Captures the position and velocity at a certain point in time.  This class requires 
 * the position and velocity are already in a Cartesian frame.
 *
 */
class StateVector {
public:

	Vect3     ss;    // s ;
	Velocity  vv;    // v ;
	double    tt ;   // t;



	StateVector(const Vect3& ss=Vect3::ZERO(), const Velocity& vv=Velocity::ZEROV(), const double tt=0);

    StateVector(const std::pair<Vect3,Velocity>& sv, double t );

	static StateVector makeXYZ(double x, double y, double z, double trk, double gs, double vs, double t);

	std::pair<Vect3,Velocity> pair() const;

	Vect3 s() const;

	Velocity v() const;

	double t() const;

   };

}
#endif /* STATEVECTOR_H_ */
