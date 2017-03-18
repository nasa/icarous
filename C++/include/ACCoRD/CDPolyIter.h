/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef CDPOLYITER_H_
#define CDPOLYITER_H_

#include <vector>

#include "Poly2D.h"
#include "Vect2.h"
#include "DetectionPolygon.h"
#include "Horizontal.h"
#include "ParameterData.h"
#include "MovingPolygon2D.h"
#include "MovingPolygon3D.h"

namespace larcfm {
   /**
  * Polygon detection.
  * Based on UNVERIFIED PVS code.
  */
class CDPolyIter : public DetectionPolygon {
  private:

    std::vector<double> timesin;
    std::vector<double> timesout;
    std::vector<double> timestca;
    std::vector<double> diststca;

    std::string id;
    double timeStep;


  public:

    CDPolyIter();
//
//    ~CDPolyIter();
//    CDPolyIter(const CDPolyIter& d);
//    CDPolyIter& operator= (const CDPolyIter& d);

    double getTimeStep() const;
    void setTimeStep(double val);


    static const std::pair<double,double> noDetection;

    static std::vector<Vect2> polygon_2D_at(const MovingPolygon2D& mp, double tr);



    std::pair<double,double> polygon_alt_inside_time(double B, double T, double vspeed, double minalt, double maxalt, double sz, double vz) ;

    static bool within_polygon_altitude(double minalt, double maxalt, double sz);

    bool contains(Poly3D& p3d, const Vect3& s);

    ParameterData getParameters() const;
    void updateParameterData(ParameterData& p) const;
    void setParameters(const ParameterData& p);

    bool polyIter_detection(double B, double T, const MovingPolygon3D& mp3D, const Vect3& s, const Velocity& v);


    bool violation(const Vect3& so, const Velocity& vo, const Poly3D& si) const;



    bool conflict(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) const;



    bool conflictDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) ;



    std::vector<double> getTimesIn() const ;



    std::vector<double> getTimesOut() const ;

    double getTimeIn(int i) const ;

    /**
     * Time to exit from loss of separation in internal units.
     *
     * @return the time to exit out loss of separation. If timeOut is zero then
     * there is no conflict. If timeOut is negative then, timeOut is infinite.
     * Note that this is a relative time.
     */
    double getTimeOut(int i) const ;

    double getTimeIn() const;

    /**
     * Time to exit from loss of separation in internal units.
     *
     * @return the time to exit out loss of separation. If timeOut is zero then
     * there is no conflict. If timeOut is negative then, timeOut is infinite.
     * Note that this is a relative time.
     */
    double getTimeOut() const ;

    std::vector<double> getCriticalTimesOfConflict() const ;

    std::vector<double> getDistancesAtCriticalTimes() const ;

    DetectionPolygon* make() const;
    DetectionPolygon* copy() const;

    std::string toString() const;

    std::string getClassName() const;
    std::string getIdentifier() const;
    void setIdentifier(const std::string& s);

    bool equals(DetectionPolygon* d) const;

    std::string strVelocities(const std::vector<Vect2>& polyvel) const;


  };

}

#endif /* CDPOLYITER_H_ */
