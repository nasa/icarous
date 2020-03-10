/*
 * SimpleProjection.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef EUCLIDEANPROJECTION_H_
#define EUCLIDEANPROJECTION_H_

 /*
  * WARNING! The string "EuclideanProjection" is the name of a MACRO
  * representing the actual type of the chosen projection.
  *
  * Anything starting with this string will be replaced with SOMETHING
  * ELSE at compile time.  You may not notice a difference.  At first.
  *
  */


// Yes, I know this is a bad idea, but this was the only way to both avoid dynamic memory woes and (syntactically) "appear" to have inheritance...

// pick which projection to use here...  undefine all other ones that may be defined
#if defined ENU_PROJECTION_
#undef AZIEQUI_PROJECTION_

#elif defined AZIEQUI_PROJECTION_
#undef ORTHO_PROJECTION_

#elif defined ORTHO_PROJECTION_
#undef SIMPLE_PROJECTION_

#elif defined SIMPLE_PROJECTION_
#undef SIMPLE_NO_POLAR_PROJECTION_

#elif defined SIMPLE_NO_POLAR_PROJECTION_
// no other possibilities
#else
// if none are defined, use this as the default
#define ENU_PROJECTION_
#endif

 /* Not used in C++ in a functional way. */
 namespace larcfm {
  enum ProjectionType {UNKNOWN_PROJECTION, SIMPLE, SIMPLE_NO_POLAR, ENU, AZIEQUI, ORTHO};
 }


#ifdef SIMPLE_PROJECTION_
#define EuclideanProjection SimpleProjection
#define projection_type_value__ SIMPLE
#include "SimpleProjection.h"
#endif

#ifdef SIMPLE_NO_POLAR_PROJECTION_
#define EuclideanProjection SimpleNoPolarProjection
#define projection_type_value__ SIMPLE_NO_POLAR
#include "SimpleNoPolarProjection.h"
#endif

#ifdef ENU_PROJECTION_
#define EuclideanProjection ENUProjection
#define projection_type_value__ ENU
#include "ENUProjection.h"
#endif

#ifdef AZIEQUI_PROJECTION_
#define EuclideanProjection AziEquiProjection
#define projection_type_value__ AZIEQUI
#include "AziEquiProjection.h"
#endif

#ifdef ORTHO_PROJECTION_
#define EuclideanProjection OrthographicProjection
#define projection_type_value__ ORTHO
#include "OrthographicProjection.h"
#endif


#endif /* EUCLIDEANPROJECTION_H_ */
