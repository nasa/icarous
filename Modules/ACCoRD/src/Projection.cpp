/**  Projection from Spherical Earth to Euclidean Plane
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *           Anthony Narkawicz         NASA Langley Research Center
 *
 * Holding area for universal projection information.  All projection objects should be retrieved using these functions.
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Projection.h"
#include "SimpleProjection.h"
#include "SimpleNoPolarProjection.h"
#include "ENUProjection.h"
#include "AziEquiProjection.h"
#include "OrthographicProjection.h"
#include "LatLonAlt.h"
#include "string_util.h"
#include <string>

namespace larcfm {

  // the default!!!
  EuclideanProjection Projection::projection = EuclideanProjection();
  ProjectionType Projection::ptype = projection_type_value__;

  EuclideanProjection Projection::createProjection(double lat, double lon, double alt) {
    return projection.makeNew(lat, lon, alt);
  }

  EuclideanProjection Projection::createProjection(const LatLonAlt& lla) {
    return projection.makeNew(lla);
  }

   EuclideanProjection Projection::createProjection(const Position& pos) {
	  LatLonAlt lla = pos.lla().zeroAlt();
	  if (!pos.isLatLon()) lla = LatLonAlt::ZERO();
	  return projection.makeNew(lla);
   }


  double Projection::projectionConflictRange(double lat, double accuracy) {
    return projection.conflictRange(lat,accuracy);
  }
  
  double Projection::projectionMaxRange() {
    return projection.maxRange();
  }

  // void in C++
  void Projection::setProjectionType(ProjectionType t) {  }


  ProjectionType Projection::getProjectionTypeFromString(std::string s) {
	  ProjectionType p = UNKNOWN_PROJECTION;
	  if (toLowerCase(s).compare("simple") == 0) {
		  p = SIMPLE;
	  } else if (toLowerCase(s).compare("simple_no_polar") == 0) {
		  p = SIMPLE_NO_POLAR;
	  } else if (toLowerCase(s).compare("enu") == 0) {
		  p = ENU;
	  } else if (toLowerCase(s).compare("aziequi") == 0) {
		  p = AZIEQUI;
	  } else if (toLowerCase(s).compare("ortho") == 0) {
		  p = ORTHO;
	  }
	  return p;
  }

  ProjectionType Projection::getProjectionType() {
	  return ptype;
  }


  //***********************************
  // deprecated functions:

//  EuclideanProjection projection = EuclideanProjection();
//  ProjectionType ptype = projection_type_value__;

  EuclideanProjection getProjection(double lat, double lon, double alt) {
	  return Projection::createProjection(lat, lon, alt);
  }

  EuclideanProjection getProjection(const LatLonAlt& lla) {
	  return Projection::createProjection(lla);
  }

  double projectionConflictRange(double lat, double accuracy) {
	  return Projection::projectionConflictRange(lat,accuracy);
  }

  double projectionMaxRange() {
	  return Projection::projectionMaxRange();
  }

  // void in C++
  void setProjectionType(ProjectionType t) {  }


  ProjectionType getProjectionTypeFromString(std::string s) {
	  return Projection::getProjectionTypeFromString(s);
  }

  ProjectionType getProjectionType() {
	  return Projection::getProjectionType();
  }


}
