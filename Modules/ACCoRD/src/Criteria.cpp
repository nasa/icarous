/*
 * Criteria.cpp - determine implicit coordination criteria
 *
 * Contact: Cesar Munoz
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Criteria.h"
#include "CriteriaCore.h"
//#include "UnitSymbols.h"

using namespace larcfm;

  bool Criteria::check(double sox, double soy, double soz,
          double vox, double voy, double voz,
          double six, double siy, double siz,
          double vix, double viy, double viz,
          double nvox, double nvoy, double nvoz,
          double minRelVs,
          double D, double H, int epsh, int epsv) {
      Position so = Position::makeXYZ(sox,soy,soz);
      Velocity vo = Velocity::makeVxyz(vox, voy, "kn", voz, "fpm");
      Position si = Position::makeXYZ(six,siy,siz);
      Velocity vi = Velocity::makeVxyz(vix, viy, "kn", viz, "fpm");
      Velocity nvo = Velocity::makeVxyz(nvox, nvoy, "kn", nvoz, "fpm");
    return check(so, vo, si, vi, nvo, minRelVs, D, H,epsh,epsv);
  }


  bool Criteria::checkLL(
            double lato, double lono, double alto,
            double trko, double gso, double vso,
            double lati, double loni, double alti,
            double trki, double gsi, double vsi,
            double ntrko, double ngso, double nvso,
            double minRelVs,double D, double H, int epsh, int epsv) {
        Position po(LatLonAlt::make(lato,lono,alto));
        Velocity vo = Velocity::makeTrkGsVs(trko,"deg", gso,"kn", vso,"fpm");
        Position pi(LatLonAlt::make(lati,loni,alti));
        Velocity vi = Velocity::makeTrkGsVs(trki,"deg", gsi,"kn", vsi,"fpm");
        Velocity nvo = Velocity::makeTrkGsVs(ntrko,"deg", ngso,"kn", nvso,"fpm");
        return check(po, vo, pi, vi, nvo, minRelVs, D, H,epsh,epsv);
    }

   bool Criteria::check(
            const Position& so, const Velocity& vo,
            const Position& si, const Velocity& vi,
            const Velocity& nvo,
            double minRelVs, double D, double H, int epsh, int epsv) {
        if (so.isLatLon() != si.isLatLon()) {
            return false; // if the dimensionality of the two positions does not agree, then always return false
        }
        double D_i = Units::from("nm",D);
        double H_i = Units::from("ft",H);
        if (so.isLatLon()) {
            EuclideanProjection sp = Projection::createProjection(so.lla());
            std::pair<Vect3,Velocity> po = sp.project(so,vo);
            std::pair<Vect3,Velocity> pi = sp.project(si,vi);
            Velocity nvop = sp.projectVelocity(so,nvo);
            return CriteriaCore::criteria(po.first.Sub(pi.first), po.second, pi.second, nvop,
            		minRelVs, D_i, H_i,epsh,epsv);
        } else {
            return CriteriaCore::criteria(so.vect3().Sub(si.vect3()), vo, vi, nvo,
            		minRelVs, D_i, H_i,epsh,epsv);
        }
    }

