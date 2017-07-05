/* PolyCARP Example implementation:
 *   - Using Polycarp for geofence containment checks.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <iostream>
#include <vector>
#include "SimplePoly.h"
#include "CDPolycarp.h"
#include "PolycarpResolution.h"
#include "Projection.h"
#include "Constants.h"

using namespace larcfm;

int main(int argc,char* argv[]) {

  std::cout << "##" << std::endl;
  std::cout << "## PolyCARP++@FormalATM" << Constants::version << std::endl;
  std::cout << "##" << std::endl << std::endl;

  // Position and velocity of ownship
  Position so = Position::makeLatLonAlt(37.102456,"deg", -76.387094,"deg", 16.4,"ft");
  Velocity vo = Velocity::makeTrkGsVs(90.0,"deg", 2.5,"kts", 0.0,"fpm");

  // Add geofence
  double floor     = 0;  // 0 m
  double ceiling   = 10; // 10 m
  
  // Make a geofence with 4 vertices (keep in fence)
  SimplePoly geoPolyLLA1(floor,ceiling);
  geoPolyLLA1.addVertex(Position::makeLatLonAlt(37.102545,"deg",-76.387213,"deg",0,"m"));
  geoPolyLLA1.addVertex(Position::makeLatLonAlt(37.102344,"deg",-76.387163,"deg",0,"m"));
  geoPolyLLA1.addVertex(Position::makeLatLonAlt(37.102351,"deg",-76.386844,"deg",0,"m"));
  geoPolyLLA1.addVertex(Position::makeLatLonAlt(37.102575,"deg",-76.386962,"deg",0,"m"));

  CDPolycarp geoPolyCarp;
  PolycarpResolution geoRes;
  
  // Project geofence vertices to a local euclidean coordinate system to use with polycarp functions
  EuclideanProjection proj  = Projection::createProjection(geoPolyLLA1.getVertex(0));
  Poly3D geoPoly3D1          = geoPolyLLA1.poly3D(proj);

  Vect3 so_3 = proj.project(so); // Project ownship position into local euclidean frame
  if (geoPolyCarp.definitelyInside(so_3,geoPoly3D1)) {
    std::cout << "Definitely inside of keep in fence\n";
  }
  if (geoPolyCarp.definitelyOutside(so_3,geoPoly3D1)) {
    std::cout << "Definitely outside of keep in fence\n";
  }

  std::vector<Vect2> fenceVertices;
  fenceVertices.reserve(4);

  for(int i=0;i<4;i++) {
    fenceVertices.push_back(geoPoly3D1.getVertex(i));
  }

  //Check if ownship is near any edge (nearness is defined based on horizontal and vertical thresholds)
  double hthreshold = 1; // 1 m
  double vthreshold = 1; // 1 m	
  if (geoPolyCarp.nearEdge(so_3,geoPoly3D1,hthreshold,vthreshold)) {
    std::cout << "Ownship is near geofence edge";
    
    // Compute a safe point to goto
    double BUFF = 0.1; // Buffer used for numerical stability within polycarp
    Vect2 so_2  = so_3.vect2(); // 2D projection of ownship position        
    
    // Compute safe point
    Vect2 recpoint = geoRes.inside_recovery_point(BUFF,hthreshold,fenceVertices,so_2);
    
    // Convert safe point from euclidean coordinates to Lat, Lon and Alt
    LatLonAlt LLA = proj.inverse(recpoint,so.alt());
    Position RecoveryPoint = Position::makeLatLonAlt(LLA.latitude(),LLA.longitude(),LLA.altitude());
    std::cout << RecoveryPoint.toString();
  }
}
