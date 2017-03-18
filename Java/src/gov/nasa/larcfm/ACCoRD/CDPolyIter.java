/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */package gov.nasa.larcfm.ACCoRD;

 import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

 import gov.nasa.larcfm.Util.MovingPolygon2D;
import gov.nasa.larcfm.Util.MovingPolygon3D;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.Poly3D;
import gov.nasa.larcfm.Util.f;

 /**
  * Polygon detection.
  * Based on UNVERIFIED PVS code.
  */

 public class CDPolyIter implements DetectionPolygon {
	 ArrayList<Double> timesin = new ArrayList<Double>();
	 ArrayList<Double> timesout = new ArrayList<Double>();
	 ArrayList<Double> timestca = new ArrayList<Double>();
	 ArrayList<Double> diststca = new ArrayList<Double>();

	 private String id = "";
	 private double timeStep = 1.0; //TODO: make this visible

	 // this is not thread-safe!  Use local instances instead.
	 //  static private CDPolyIter det = new CDPolyIter();

	 static final Pair<Double,Double> noDetection = new Pair<Double,Double>(0.0,-1.0);

	 //  **RWB**  TEMPORARY
	 //  boolean conflict_polygon_3D_cd(double B, double T, MovingPolygon3D mp3D, Vect3 s, Velocity v) throws MovingPolygon2D.BadPolygonException {
	 //    if (B > T || v.vect2().isZero()) {
	 //      return false;
	 //    } else {
	 //      Pair<Double,Double> tp = polygon_alt_inside_time(B,T,mp3D.vspeed,mp3D.minalt,mp3D.maxalt,s.z,v.z);
	 //      double tin = tp.first;
	 //      double tout = tp.second;
	 //      MovingPolygon2D start2dpoly = new MovingPolygon2D();
	 //      start2dpoly.polystart = CDPoly2D.polygon_2D_at(mp3D.horizpoly,tin);
	 //      start2dpoly.polyvel = mp3D.horizpoly.polyvel;
	 //      start2dpoly.tend = mp3D.horizpoly.tend - tin;
	 //      
	 //      boolean ret = CDPoly2D.conflict_polygon_2D_cd(tout-tin,start2dpoly,s.vect2().Add(v.vect2().Scal(tin)),v.vect2());
	 //      return ret;
	 //    }
	 //  }

	 public double getTimeStep() {
		 return timeStep;
	 }

	 public void setTimeStep(double val) {
		 if (timeStep > 0) {
			 timeStep = val;
		 }
	 }

	 static public ArrayList<Vect2> polygon_2D_at(MovingPolygon2D mp, double tr) {
		 ArrayList<Vect2> p = new ArrayList<Vect2>();
		 for (int ii = 0; ii < mp.polystart.size(); ii++) {
			 p.add(mp.polystart.get(ii).Add(mp.polyvel.get(ii).Scal(tr)));
		 }
		 return p;
	 }



	 Pair<Double,Double> polygon_alt_inside_time(double B, double T, double vspeed, double minalt, double maxalt, double sz, double vz) {
		 if (B > T) {
			 return noDetection; 
		 }
		 if (minalt>=maxalt) {
			 return noDetection;
		 } else if (Util.almost_equals(vz-vspeed, 0.0) || Util.almost_equals(B, T)) {
			 if (within_polygon_altitude(minalt+B*vspeed,maxalt+B*vspeed,sz+B*vz)) {
				 return new Pair<Double,Double>(B,T);
			 } else {
				 return noDetection;
			 }
		 } else {
			 double thin  = Vertical.Theta_H(sz-(minalt+maxalt)/2,vz-vspeed,-1,(maxalt-minalt)/2);
			 double thout = Vertical.Theta_H(sz-(minalt+maxalt)/2,vz-vspeed,1,(maxalt-minalt)/2);
			 if (thout < B || thin > T) {
				 return noDetection;
			 } else {
				 return new Pair<Double,Double>(Util.max(B,Util.min(T,thin)), Util.max(B,Util.min(T,thout)));
			 }
		 }
	 }

	 boolean within_polygon_altitude(double minalt, double maxalt, double sz) {
		 return minalt <= sz && sz <= maxalt;
	 }

	 boolean contains(Poly3D p3d, Vect3 s) {
		 return p3d.poly2D().contains(s.vect2()) && within_polygon_altitude(p3d.getBottom(), p3d.getTop(), s.z);
	 }

	 public ParameterData getParameters() {
		 ParameterData p = new ParameterData();
		 updateParameterData(p);
		 return p;
	 }

	 public void updateParameterData(ParameterData p) {
		 p.set("id", id);
		 p.setInternal("timeStep", timeStep,"s");
	 }

	 public void setParameters(ParameterData p) {
		 if (p.contains("id")) {
			 id = p.getString("id");
		 }
		 if (p.contains("timeStep")) {
			 timeStep = p.getValue("timeStep");
		 }
	 }


	 //  @Override
	 public CDPolyIter make() {
		 return new CDPolyIter();
	 }


	 @SuppressWarnings("unchecked")
	 //  @Override
	 public CDPolyIter copy() {
		 CDPolyIter ret = new CDPolyIter();
		 ret.id = id;
		 ret.timesin = new ArrayList<Double>(timesin);
		 ret.timesout = new ArrayList<Double>(timesout);
		 ret.timestca = new ArrayList<Double>(timestca);
		 ret.diststca = new ArrayList<Double>(diststca);
		 return ret; 
	 }

	 boolean polyIter_detection(double B, double T, MovingPolygon3D mp3D, Vect3 s, Velocity v) {
		 timesin = new ArrayList<Double>();
		 timesout = new ArrayList<Double>();
		 timestca = new ArrayList<Double>();
		 diststca = new ArrayList<Double>();
		 if (T < 0.0) {
			 T = 36000.0;
		 }
		 if (B >= T || v.vect2().isZero()) { //TODO: false if v horiz == 0?
//f.pln("polyIter_detection 1");
			 return false;
		 } else {
//f.pln(" $$ CDPolyIter.poly3D_detection: B= "+B+" T ="+T+" (mp3D = "+mp3D.toString()+")    s = "+s+" v = "+v);
			 // shortcut
			 Pair<Double,Double> tp = polygon_alt_inside_time(B,T,mp3D.vspeed,mp3D.minalt,mp3D.maxalt,s.z,v.z);
			 if (tp == noDetection) {
//f.pln("polyIter_detection 2");
				 return false;
			 }
			 // times where we are in possible vertical conflict within [B,T]:
			 double tin = tp.first;
			 double tout = tp.second;
//f.pln("polyIter_detection B="+B+" T="+T+" tin="+tin+" tout="+tout);      
			 MovingPolygon2D start2dpoly = new MovingPolygon2D();
			 start2dpoly.polystart = polygon_2D_at(mp3D.horizpoly,tin);
			 start2dpoly.polyvel = mp3D.horizpoly.polyvel;
//f.pln(" $$$$ polyIter_detection:  start2dpoly.polyvel = "+start2dpoly.polyvel.toString());
			 start2dpoly.tend = mp3D.horizpoly.tend - tin;      
			 //public boolean poly2D_detection(double T, Poly2D pi, Vect2 vi, Vect2 so, Vect2 vo) {    
			 CDPolyIter2D cdp2 = new CDPolyIter2D();
			 // check detection starting from the tin time, 
			 boolean ret = cdp2.poly2DIter_detection(tout-tin, start2dpoly,s.vect2().Add(v.vect2().Scal(tin)),v.vect2(),timeStep);
			 if (!ret) {
//f.pln("polyIter_detection 3");
				 return false;
			 }
			 for(int i = 0; i < cdp2.conflicts; i++) {
				 // return to original time frame for this detection and final result
				 double mytin = cdp2.getTimesIn().get(i)+tin;
				 double mytout = cdp2.getTimesOut().get(i)+tin;
				 tp = polygon_alt_inside_time(mytin,mytout,mp3D.vspeed,mp3D.minalt,mp3D.maxalt,s.z,v.z);
//f.pln("mytin="+mytin+" mytout="+mytout+" tp="+tp);        
				 if (tp != noDetection) {
					 timesin.add(tp.first);
					 timesout.add(tp.second);
					 double tca = (tp.first+tp.second)/2.0;
					 timestca.add(tca);  //TODO FIX ME!!! NOT CORRECT!!!
					 Poly3D pos = mp3D.position(tca); 
					 double dist = pos.centroid().distanceH(s.AddScal(tca, v)); 
					 diststca.add(dist);
//f.pln("ping! "+tp);

				 }
			 }
//f.pln("polyIter_detection 4 " + timesin.size());
			 return timesin.size() > 0;
		 }
	 }

//	 boolean polyIter_containment(double B, double T, MovingPolygon3D mp3D, Vect3 s, Velocity v) {
//		 boolean ret = polyIter_detection(B,T,mp3D,s,v);
//		 if (!ret) {
//			 timesin.add(T);
//			 timesout.add(B);
//		 } else {
//			 ArrayList<Double> times = new ArrayList<Double>();
//			 times.addAll(timesin);
//			 times.addAll(timesout);
//			 Collections.sort(times);
//			 if (times.size() > 0) {
//				 if (times.get(0) == B) {
//					 times.remove(0);
//					 timestca.remove(0);
//					 diststca.remove(0);
//				 } else {
//					 times.add(0,B);
//					 double tca = (B+times.get(1))/2;
//					 timestca.add(0,tca);
//					 Poly3D pos = mp3D.position(tca); 
//					 double dist = pos.centroid().distanceH(s.AddScal(tca, v)); 
//					 diststca.add(0,dist);
//				 }
//				 if (times.get(times.size()-1) == T) {
//					 times.remove(times.size()-1);
//					 timestca.remove(timestca.size()-1);
//					 diststca.remove(diststca.size()-1);
//				 } else {
//					 times.add(T);
//					 double tca = (T+times.get(times.size()-2))/2;
//					 timestca.add(tca);
//					 Poly3D pos = mp3D.position(tca); 
//					 double dist = pos.centroid().distanceH(s.AddScal(tca, v)); 
//					 diststca.add(dist);
//				 }
//			 } else {
//				 times.add(B);
//				 times.add(T);
//			 }
//			 timesin.clear();
//			 timesout.clear();
//			 for (int i = 0; i < times.size()-1; i = i+2) {
//				 timesout.add(times.get(i));
//				 timesin.add(times.get(i+1));
//			 }
//		 }
//		 return !ret;
//	 }

	 
	 @Override
	 public boolean violation(Vect3 so, Velocity vo, Poly3D si) {
		 //f.pln("CDPolyIter.violation: so="+so+" vo="+vo+" si=("+si+")");
		 CDPolyIter det = new CDPolyIter();
		 return det.contains(si, so);
	 }



	 @Override
	 public boolean conflict(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
		 //f.pln("CDPolyIter.conflict: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
		 CDPolyIter det = new CDPolyIter();
		 return det.polyIter_detection(B,T,si,so,vo);

		 //	  return CDPoly3DStable.detection(so, vo, si.position(0), si.velocity(0), 0.1, B, T);
	 }


	 @Override
	 public boolean conflictDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
		 //f.pln("===============  CDPolyIter.conflictDetection: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
		 return polyIter_detection(B,T,si,so,vo);
	 }

//	 @Override
//	 public boolean inside(Vect3 so, Velocity vo, Poly3D si) {
//		 //f.pln("CDPolyIter.violation: so="+so+" vo="+vo+" si=("+si+")");
//		 CDPolyIter det = new CDPolyIter();
//		 return det.contains(si, so);
//	 }
//
//	 @Override
//	 public boolean outside(Vect3 so, Velocity vo, Poly3D si) {
//		 //f.pln("CDPolyIter.violation: so="+so+" vo="+vo+" si=("+si+")");
//		 CDPolyIter det = new CDPolyIter();
//		 return !det.contains(si, so);
//	 }
//
//
//	 @Override
//	 public boolean willEnter(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		 //f.pln("CDPolyIter.conflict: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
//		 CDPolyIter det = new CDPolyIter();
//		 return det.polyIter_detection(B,T,si,so,vo);
//
//		 //	  return CDPoly3DStable.detection(so, vo, si.position(0), si.velocity(0), 0.1, B, T);
//	 }
//
//	 @Override
//	 public boolean willExit(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		 CDPolyIter det = new CDPolyIter();
//		 return !det.polyIter_detection(B,T,si,so,vo);		 
//	 }
//
//
//	 //  //TODO REMOVE ME!!!
//	 //  public static boolean conflict2(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//	 ////	  return CDPoly3DStable.detection(so, vo, si.position(0), si.velocity(0), 0.1, B, T); // "stable" call
//	 //	  return CDPoly3DStable.detection(so, vo, si, 0.1, B, T); // "morphing" call
//	 //  }
//
//	 @Override
//	 public boolean entranceDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		 //f.pln("===============  CDPolyIter.conflictDetection: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
//		 return polyIter_detection(B,T,si,so,vo);
//	 }
//
//	 @Override
//	 public boolean exitDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T) {
//		 //f.pln("===============  CDPolyIter.conflictDetection: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
//		 return polyIter_containment(B,T,si,so,vo);
//	 }


	 //  @Override
	 public List<Double> getTimesIn() {
		 return timesin;
	 }


	 //  @Override
	 public List<Double> getTimesOut() {
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


	 //  @Override
	 public List<Double> getCriticalTimesOfConflict() {
		 return timestca;
	 }


	 //  @Override
	 public List<Double> getDistancesAtCriticalTimes() {
		 return diststca;
	 }

	 public String getSimpleClassName() {
		 return getClass().getSimpleName();
	 }


	 //  @Override
	 public String getClassName() {
		 return getClass().getCanonicalName(); // "gov.nasa.larcfm.ACCoRD.CDPolyIter"
	 }



	 //  @Override
	 public String getIdentifier() {
		 return id;
	 }



	 //  @Override
	 public void setIdentifier(String s) {
		 id = s;
	 }

	 public String toString() {
		 return "CDPolyIter "+id+" timeStep="+f.Fm2(timeStep);
	 }

 }
