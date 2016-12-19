/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.LossData;
import gov.nasa.larcfm.Util.Poly2D;
import gov.nasa.larcfm.Util.MovingPolygon2D;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;

public class CDPolyIter2D {

  // no conflict: timein = -1 AND timeout = 0;
	  private ArrayList<Double> timesin = new ArrayList<Double>();
	  private ArrayList<Double> timesout = new ArrayList<Double>();

	  private double microStepNum = 10.0; //TODO: make this visible
	  
	  public int conflicts;
	  
	  /**
	   * Time to loss of separation in internal units.
	   * @return the time to loss of separation. If time is negative then there 
	   * is no conflict.   Note that this is a relative time.
	   */

	  public ArrayList<Double> getTimesIn() {
	       return timesin;
	  }

	  /**
	   * Time to exit from loss of separation in internal units.
	   * 
	   * @return the time to exit out loss of separation. If timeOut is zero then 
	   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
	   * Note that this is a relative time.
	   */
	  public ArrayList<Double> getTimesOut() {
	       return timesout;
	  }
	  
	  public double getTimeIn(int i) {
	    if (timesin.size() == 0) return -1;
	    if (i > timesin.size()) {
	      f.pln(" $$ getTimeIn error, index out of range");
	      i = timesin.size()-1;
	    }
	    return timesin.get(i);
	  }

	  /**
	   * Time to exit from loss of separation in internal units.
	   * 
	   * @return the time to exit out loss of separation. If timeOut is zero then 
	   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
	   * Note that this is a relative time.
	   */
	  public double getTimeOut(int i) {
	    if (timesin.size() == 0) return 0;
	    if (i > timesin.size()) {
	      f.pln(" $$ getTimeIn error, index out of range");
	      i = timesin.size()-1;
	    }
	    return timesout.get(i);
	  }

	  public double getTimeIn() {
	    if (timesin.size() == 0) return -1;
	    return timesin.get(0);
	  }

	  /**
	   * Time to exit from loss of separation in internal units.
	   * 
	   * @return the time to exit out loss of separation. If timeOut is zero then 
	   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
	   * Note that this is a relative time.
	   */
	  public double getTimeOut() {
	    if (timesin.size() == 0) return 0;
	    return timesout.get(timesout.size()-1);
	  }

	  
//	  public boolean poly2D_approx_detection(double T, Poly2D pi, Vect2 vi, Vect2 so, Vect2 vo) {    
//	    Vect2 center = pi.getCentroid();
//	    //double innerDiam = pi.innerDiameter();
//	    double outerDiam = pi.outerDiameter();
//	    //f.pln(" $$$$$$$$$$$ poly2D_approx_conflict: outerDiam = "+outerDiam+" so = "+so+" vo = "+vo+" center = "+center+" vi = "+vi);
//	    CD2D cd = new CD2D();
//	    boolean det = cd.detection(so.Sub(center), vo, vi, outerDiam, T);
//	    timesin = new ArrayList<Double>();
//	    timesout = new ArrayList<Double>();
//	    conflicts = 0;
//        if (det) {
//          conflicts = 1;
//          timesin.add(cd.getTimeIn());
//          timesout.add(cd.getTimeOut());
//        }
//	    return det;
//	   }

	
//	   public boolean poly2D_approx_detection(double T, Poly2D pi, Vect2 vi, Vect2 so, Vect2 vo) {    
//	        Vect2 center = pi.centroid();
//	        //double innerDiam = pi.innerDiameter();
//	        double outerDiam = pi.outerDiameter();
//	        //f.pln(" $$$$$$$$$$$ poly2D_approx_detection: outerDiam = "+outerDiam+" so = "+so+" vo = "+vo+" center = "+center+" vi = "+vi);
//	        CD2D cd = new CD2D();
//	        boolean det = cd.detection(so.Sub(center), vo, vi, outerDiam, T);
//	        timesin = new ArrayList<Double>();
//	        timesout = new ArrayList<Double>();
//	        conflicts = 0;
//	        if (det) {
//	          conflicts = 1;
//	          timesin.add(cd.getTimeIn());
//	          timesout.add(cd.getTimeOut());
//	        }
//	        return det;
//	       }

	  
	   // get a more exact reading for time in/time out via binary search
	   double poly2D_detection_micro(MovingPolygon2D mpi, Vect2 so, Vect2 vo, double t0, boolean t0in, double t1, boolean t1in, double minstep) {
	     // base case: can't differentiate begin from end or reach minstep time slice
	     if (t0in == t1in || t1-t0 < minstep) {
	       return t0;
	     }
         Poly2D mpiStart = new Poly2D(mpi.polystart);    
         double tmid = (t1+t0)/2.0;
         Vect2 soAtTm = so.linear(vo,tmid);
         Poly2D npi = mpiStart.linear(mpi.polyvel,tmid);
         boolean inside = npi.contains(soAtTm);
         if (inside == t0in) {
           return poly2D_detection_micro(mpi,so,vo,tmid,inside,t1,t1in,minstep);
         } else {
           return poly2D_detection_micro(mpi,so,vo,t0,t0in,tmid,inside,minstep);
         }
	   }
	   
	   

//	      // MOVING Polygon Version
//	      public boolean poly2D_detection(double T, MovingPolygon2D mpi, Vect2 so, Vect2 vo) {
//	//f.pln("CDPolyIter2D.poly2D_detection T="+T+" mpi="+mpi+" so="+so+" vo="+vo);    
//	        Poly2D pi = new Poly2D(mpi.polystart);    
//	        Vect2 vi = mpi.polyvel.get(0);    
//	        return poly2D_detection(T,pi,vi,so,vo);
//	      }
//   
  
	   
	   
	   
	  
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
      public boolean poly2DIter_detection(double T, MovingPolygon2D mpi, Vect2 so, Vect2 vo, double tStep) {  
//f.pln(" $$$ poly2DIter_detection T="+T+" ENTER: so = "+so+" vo="+vo+" tStep="+tStep);
        
        Poly2D mpiStart = new Poly2D(mpi.polystart);    
        //Vect2 vi = mpi.polyvel.get(0);    
        double tend = mpi.tend;
        double tSeg = Util.min(tend, T); 
//f.pln(" CDPolyIter2D::poly2D_detection: tSeg = "+f.Fm1(tSeg)+" mpi = "+mpi.toString());
        Poly2D mpiEnd = mpiStart.linear(mpi.polyvel,tSeg);                
        timesin = new ArrayList<Double>();
        timesout = new ArrayList<Double>();
        conflicts = 0;
        Vect2 centerStart = mpiStart.averagePoint();
        Vect2 centerEnd   = mpiEnd.averagePoint();       
        Vect2 vi = centerEnd.Sub(centerStart).Scal(1/tSeg);      
        double outerRadStart = mpiStart.apBoundingRadius();
        double outerRadEnd = mpiEnd.apBoundingRadius();
        double outerRadius = Util.max(outerRadStart,outerRadEnd);
//f.pln("poly2DIter_detection D = "+outerRadius+" so="+so+" si="+centerStart+" T="+T);        
        CD2D cd = new CD2D();
        LossData det = cd.detection(so.Sub(centerStart), vo, vi, outerRadius, T);
        if (!det.conflict()) {
//f.pln("poly2DIter_detection: no cd det");        	
          return false;
        }
        double tmin = det.getTimeIn();
        double tmout = det.getTimeOut();
        boolean prevInside = false;
//        double tStep2 = Util.max(tStep, (tmout-tmin)/100.0);
//f.pln("poly2D_detection tmin="+tmin+" tmout="+tmout);
        for (double t = tmin; t <= tmout; t = t + tStep) { 
          Vect2 soAtTm = so.linear(vo,t);
          Poly2D npi = mpiStart.linear(mpi.polyvel,t);
          boolean inside = npi.contains(soAtTm);
          if (inside) {
            if (!prevInside) {
//f.pln(" $$$ poly2D_detection 1: t = "+t+" "+inside+" prevInside="+prevInside+" soAtTm="+soAtTm+" npi = "+npi);          
              double tin = t;
              if (t > tmin) { // if not immediately in violation, get a better estimate of the time
                tin = poly2D_detection_micro(mpi, so, vo, t-tStep, false, t, true, tStep/microStepNum);
              }
//f.pln("!!!!!!!!start conflict!!!!!!");              
              conflicts++;
              timesin.add(tin);
            }
            prevInside = true;
            // end of detection range, if still in conflict, end it here
            if (t+tStep > tmout) {
              timesout.add(tmout);
            }
          } else {
//          for (int i = 0; i < npi.size(); i++) f.pln("ui_reference_point_D"+Util.nextCount()+" = "+new Position(new Vect3(npi.getVertex(i),0)).toString8()); 
            if (prevInside) {   // going from inside to outside
//f.pln(" $$$ poly2D_detection 2: t = "+t+" "+inside+" prevInside="+prevInside+" soAtTm="+soAtTm+" npi = "+npi);          
              double tout = t-tStep;
              tout = poly2D_detection_micro(mpi, so, vo, t-tStep, true, t, false, tStep/microStepNum); // get a better estimate of the time
              timesout.add(tout);
            }
            prevInside = false;
          }
        }
        //pplans.add(sPlan);
        //DebugSupport.dumpPlanList(pplans,"pplans");
//      f.pln(" %%>> poly2D_detection: tmin = "+f.Fm1(tmin)+" timein = "+f.Fm1(getTimeIn())+" timeout = "+f.Fm1(getTimeOut())+" tmout = "+f.Fm1(tmout)+" conflicts = "+conflicts);
        //if (getTimeIn() < 0) DebugSupport.halt();
        
        // strip out short conflicts
        int i = 0;
        while (i < timesin.size()) {
          if (timesout.get(i) - timesin.get(i) < tStep) {
//f.pln("CDPolyIter2D.poly2D_detection dropping short conflict "+timesin.get(i)+" "+timesout.get(i));            
            timesout.remove(i);
            timesin.remove(i);
            conflicts--;
          } else {
//f.pln("CDPolyIter2D.poly2D_detection keeping long conflict "+timesin.get(i)+" "+timesout.get(i));            
            i++;
          }
        }
        

//f.pln();        
        return conflicts > 0;
      }



	
}
