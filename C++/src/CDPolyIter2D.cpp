/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include <vector>
#include "format.h"
#include "CD2D.h"
#include "CDPolyIter2D.h"
#include "MovingPolygon2D.h"
#include "MovingPolygon3D.h"

namespace larcfm {

//  const double CDPolyIter2D::accuracy = 0.01;

     // no conflict: timein = -1 AND timeout = 0;
	  //std::vector<double> timesin = std::vector<double>();
	  //std::vector<double> timesout = std::vector<double>();
	  
	  /**
	   * Time to loss of separation in internal units.
	   * 
	   * @return the time to loss of separation. If time is negative then there 
	   * is no conflict.   Note that this is a relative time.
	   */

	  std::vector<double> CDPolyIter2D::getTimesIn() const {
	       return timesin;
	  }

	  /**
	   * Time to exit from loss of separation in internal units.
	   * 
	   * @return the time to exit out loss of separation. If timeOut is zero then 
	   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
	   * Note that this is a relative time.
	   */
	  std::vector<double> CDPolyIter2D::getTimesOut() const {
	       return timesout;
	  }

	    int CDPolyIter2D::getConflicts() const {
	      return conflicts;
	    }

	  double CDPolyIter2D::getTimeIn(int i) const {
	    if (timesin.size() == 0) return -1;
	    if (i > (int) timesin.size()) {
	      fpln(" $$ getTimeIn error, index out of range");
	      i = timesin.size()-1;
	    }
	    return timesin[i];
	  }

	  /**
	   * Time to exit from loss of separation in internal units.
	   * 
	   * @return the time to exit out loss of separation. If timeOut is zero then 
	   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
	   * Note that this is a relative time.
	   */
	  double CDPolyIter2D::getTimeOut(int i) const {
	    if (timesin.size() == 0) return 0;
	    if (i > (int) timesin.size()) {
	      fpln(" $$ getTimeIn error, index out of range");
	      i = timesin.size()-1;
	    }
	    return timesout[i];
	  }

	  double CDPolyIter2D::getTimeIn() const {
	    if (timesin.size() == 0) return -1;
	    return timesin[0];
	  }

	  /**
	   * Time to exit from loss of separation in internal units.
	   * 
	   * @return the time to exit out loss of separation. If timeOut is zero then 
	   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
	   * Note that this is a relative time.
	   */
	  double CDPolyIter2D::getTimeOut() const {
	    if (timesin.size() == 0) return 0;
	    return timesout[timesout.size()-1];
	  }


	
//	   bool CDPolyIter2D::poly2D_approx_detection(double T, const Poly2D& pi, const Vect2& vi, const Vect2& so, const Vect2& vo) {
//	        Vect2 center = pi.centroid();
//	        //double innerDiam = pi.innerDiameter();
//	        double outerDiam = pi.outerDiameter();
//	        //fpln(" $$$$$$$$$$$ poly2D_approx_detection: outerDiam = "+outerDiam+" so = "+so+" vo = "+vo+" center = "+center+" vi = "+vi);
//	        CD2D cd = CD2D();
//	        bool det = cd.detection(so.Sub(center), vo, vi, outerDiam, T);
//	        timesin = std::vector<double>();
//	        timesout = std::vector<double>();
//	        conflicts = 0;
//	        if (det) {
//	          conflicts = 1;
//	          timesin.push_back(cd.getTimeIn());
//	          timesout.push_back(cd.getTimeOut());
//	        }
//	        return det;
//	       }


double CDPolyIter2D::poly2D_detection_micro(const MovingPolygon2D& mpi, const Vect2& so, const Vect2& vo, double t0, bool t0in, double t1, bool t1in, double minstep) const {
	         // base case: can't differentiate begin from end or reach minstep time slice
	         if (t0in == t1in || t1-t0 < minstep) {
	           return t0;
	         }
	         Poly2D mpiStart = Poly2D(mpi.polystart);
	         double tmid = (t1+t0)/2.0;
	         Vect2 soAtTm = so.linear(vo,tmid);
	         Poly2D npi = mpiStart.linear(mpi.polyvel,tmid);
	         bool inside = npi.contains(soAtTm);
	         if (inside == t0in) {
	           return poly2D_detection_micro(mpi,so,vo,tmid,inside,t1,t1in,minstep);
	         } else {
	           return poly2D_detection_micro(mpi,so,vo,t0,t0in,tmid,inside,minstep);
	         }
	       }

	  
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
      bool CDPolyIter2D::polyIter2D_detection(double T, const MovingPolygon2D& mpi, const Vect2& so, const Vect2& vo, double tStep) {
//fpln(" $$$ poly2DIter_detection T="+Fm4(T)+" ENTER: so = "+so.toString()+" vo="+vo.toString()+" tStep="+Fm4(tStep));
        Poly2D mpiStart = Poly2D(mpi.polystart);
        double tend = mpi.tend;
        double tSeg = std::min(tend, T);
//fpln(" CDPolyIter2D::poly2D_detection: tSeg = "+Fm1(tSeg)+" mpi = "+mpi.toString());
        Poly2D mpiEnd = mpiStart.linear(mpi.polyvel,tSeg);
        timesin = std::vector<double>();
        timesout = std::vector<double>();
        conflicts = 0;
        Vect2 centerStart = mpiStart.averagePoint();
        Vect2 centerEnd   = mpiEnd.averagePoint();
        Vect2 vi = centerEnd.Sub(centerStart).Scal(1/tSeg);
        double outerRadStart = mpiStart.apBoundingRadius();
        double outerRadEnd = mpiEnd.apBoundingRadius();
        double outerRadius = std::max(outerRadStart,outerRadEnd);
        CD2D cd = CD2D();
        LossData det = cd.detection(so.Sub(centerStart), vo, vi, outerRadius, T);
        if (!det.conflict()) {
          return false;
        }
        double tmin = det.getTimeIn();
        double tmout = det.getTimeOut();
//fpln("poly2D_detection tmin="+Fm4(tmin)+" tmout="+Fm4(tmout));
        bool prevInside = false;
        for (double t = tmin; t <= tmout; t = t + tStep) { 
          Vect2 soAtTm = so.linear(vo,t);
          Poly2D npi = mpiStart.linear(mpi.polyvel,t);
          //fpln(" $$$ poly2D_detection: npi = "+npi);
          bool inside = npi.contains(soAtTm);
          if (inside) {
            if (!prevInside) {
//fpln(" poly2D_detection 1: t= "+Fm2(t)+" inside = "+bool2str(inside)+" prevInside="+bool2str(prevInside)+" soAtTm="+soAtTm.toString()+" npi = "+npi.toString());
              double tin = t;
              if (t > tmin) { // if not immediately in violation, get a better estimate of the time
                tin = poly2D_detection_micro(mpi, so, vo, t-tStep, false, t, true, tStep/10.0);
              }
              conflicts++;
              timesin.push_back(tin);
            }
            prevInside = true;
            // end of detection range, if still in conflict, end it here
            if (t+tStep > tmout) {
              timesout.push_back(tmout);
            }
          } else {
//          for (int i = 0; i < npi.size(); i++) fpln("ui_reference_point_D"+Util::nextCount()+" = "+Position(Vect3(npi.getVertex(i),0)).toString8()); 
            if (prevInside) {   // going from inside to outside
//fpln(" poly2D_detection 2: t= "+Fm2(t)+" inside = "+bool2str(inside)+" prevInside="+bool2str(prevInside)+" soAtTm="+soAtTm.toString()+" npi = "+npi.toString());
              double tout = t-tStep;
              tout = poly2D_detection_micro(mpi, so, vo, t-tStep, true, t, false, tStep/10.0); // get a better estimate of the time
              timesout.push_back(tout);
            }
            prevInside = false;
          }
        }
        //pplans.add(sPlan);
        //DebugSupport.dumpPlanList(pplans,"pplans");
//fpln(" %%>> poly2D_detection: tmin = "+Fm1(tmin)+" timein = "+Fm1(getTimeIn())+" timeout = "+Fm1(getTimeOut())+" tmout = "+Fm1(tmout)+" conflicts = "+Fmi(conflicts));
        //if (getTimeIn() < 0) DebugSupport.halt();

        int i = 0;
        while (i < (int) timesin.size()) {
          if (timesout[i] - timesin[i] < tStep) {
//fpln("CDPolyIter2D.poly2D_detection dropping short conflict "+Fm4(timesin[i])+" "+Fm4(timesout[i]));
            timesout.erase(timesout.begin()+i);
            timesin.erase(timesin.begin()+i);
            conflicts--;
          } else {
//fpln("CDPolyIter2D.poly2D_detection keeping long conflict "+Fm4(timesin[i])+" "+Fm4(timesout[i]));
            i++;
          }
        }

        return conflicts > 0;
      }


//	  // MOVING Polygon Version
//	  bool CDPolyIter2D::poly2D_detection(double T, const MovingPolygon2D& mpi, const Vect2& so, const Vect2& vo, double tStep) {
//	    Poly2D pi = Poly2D(mpi.getPolystart());
//	    Vect2 vi = mpi.getPolyvel()[0];
//	    return poly2D_detection(T,pi,vi,so,vo, tStep);
//	  }


}
