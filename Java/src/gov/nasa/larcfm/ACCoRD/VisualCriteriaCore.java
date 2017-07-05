/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.VectFuns;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.IO.ViewPort;

import java.awt.Color;

public class VisualCriteriaCore {

	private VisualCriteriaCore() {
		// Do not make one of these.
	}

	// For displaying criteria in visualizations
    // plus is green, minus is blue
	
	public final static Color nonCriteria = Color.red;
	public final static Color conflict = Color.red;
	public final static Color horiz_los_crit_minus = ViewPort.royalBlue;    // in los, with horiz recovery -
	public final static Color horiz_los_crit_plus = ViewPort.medGreen;    // in los, with horiz recovery +
	public final static Color vert_los_crit_minus = ViewPort.medPurple;    // in los, with vert recovery -
	public final static Color vert_los_crit_plus = ViewPort.kellyGreen;    // in los, with vert recovery +
    public final static Color combined_los_crit_minus = new Color(50,150,250);    // in los, with horiz & vert recovery -
    public final static Color combined_los_crit_plus = new Color(50,250,150);    // in los, with horiz & vert recovery +
	public final static Color horizPlus = ViewPort.kellyGreen;                // horiz +
	public final static Color horizMinus = ViewPort.darkBlue;               // horiz -
	public final static Color horizOther = Color.lightGray;                    // non-chosen horiz criteria
	public final static Color vertOther = ViewPort.brown;             // non-chosen vert criteria -- how can a track angle show up brown? (see T001)
	public final static Color horizBoth = Color.cyan;            // in both horiz+ and horiz-
    public final static Color vertMinusWithHLos = ViewPort.orange;
	public final static Color vertPlusWithHLos = ViewPort.purple;
	public final static Color combinedHorizAndVert = ViewPort.brown;     // a special combined horiz+vertical maneuver is coordinated with horizontal criteria
	public final static Color vertLeftOver = Color.gray;          // everything that satisfies the vertical, except what is in combinedHorizAndVert

	// Chorus 1 version
	public static Color horizDisplayColorOld(Vect3 so3, Vect3 si3, Velocity vo3, Velocity vi3, Velocity nvo3, 
	    double D, double H, double minRelHorizSpeed, int epsh, int epsv) {
	  Vect2 so = so3.vect2(); 
	  Vect2 si = si3.vect2(); 
	  Vect2 vi = vi3.vect2();
	  Vect2 s = so.Sub(si);
	  //Vect2 v = vo.Sub(vi);
	  Vect3 s3 = so3.Sub(si3);    
	  Vect2 nvo = nvo3.vect2();
	  Vect2 nv = nvo.Sub(vi);
	  Vect3 v3  = vo3.Sub(vi3);
	  Color dispColor = nonCriteria;
	  // OLD CRITERIA
	  if (CD3D.lossOfSep(so3,si3,D,H)) {
	      if (CriteriaCore.horizontalRepulsiveCriterion(s3, vo3, vi3, nvo3, epsh)) {  
	        if (epsh == -1) dispColor = horiz_los_crit_minus;
	        if (epsh ==  1) dispColor = horiz_los_crit_plus;
	      }
	      if (dispColor != nonCriteria && VectFuns.divergentHorizGt(s,nvo,vi,minRelHorizSpeed)) dispColor = ViewPort.gold;
	      if (dispColor == nonCriteria && VectFuns.divergentHorizGt(s,nvo,vi,minRelHorizSpeed)) dispColor = ViewPort.lightTan;
	  } else {
	    Vect3 nv3 = nvo3.Sub(vi3);
	    if (CriteriaCore.horizontal_criterion(s,nv,D,epsh)) {
	      if (epsh == -1) dispColor = horizMinus;
	      if (epsh ==  1) dispColor = horizPlus;
	      if (CriteriaCore.horizontal_criterion(s,nv,D,-epsh)) dispColor = horizBoth;
	    } else if (CriteriaCore.vertical_criterion(epsv, s3,v3,nv3,D,H)) {
	      if (epsv == -1) dispColor = vertMinusWithHLos; 
	      if (epsv == +1) dispColor = vertPlusWithHLos; 
	    }
	  }
	  return dispColor;
	}

	// chorus 2 version
    public static Color horizDisplayColorNew(Vect3 so3, Vect3 si3, Velocity vo3, Velocity vi3, Velocity nvo3, int epsh) {
      Vect2 so = so3.vect2(); 
      Vect2 vo = vo3.vect2();
      Vect2 si = si3.vect2(); 
      Vect2 vi = vi3.vect2();
      Vect2 s = so.Sub(si);
      Vect2 nvo = nvo3.vect2();
      Color dispColor = nonCriteria;
      if (CriteriaCore.horizontal_new_repulsive_criterion(s, vo, vi, nvo, epsh)) {
        if (epsh == -1) dispColor = horiz_los_crit_minus;
        if (epsh ==  1) dispColor = horiz_los_crit_plus;
      } else {
        dispColor = Color.red;
      }
      return dispColor;
    }

    // Chorus 1 version
	public static Color simpleCriteriaColorOld(Vect3 so3, Vect3 si3, Velocity vo3, Velocity vi3, Velocity nvo3, 
	    double D, double H, double minRelHorizSpeed, int epsh, int epsv) {
	  Vect2 so = so3.vect2(); 
	  Vect2 si = si3.vect2(); 
	  Vect2 vi = vi3.vect2();
	  Vect2 s = so.Sub(si);
	  Vect3 s3 = so3.Sub(si3);    
	  Vect2 nvo = nvo3.vect2();
	  Color dispColor = nonCriteria;
	  if (CD3D.lossOfSep(so3,si3,D,H)) {
	    if (CriteriaCore.horizontalRepulsiveCriterion(s3, vo3, vi3, nvo3, epsh)) {  
	      if (epsh == -1) dispColor = ViewPort.medGreen;
	      if (epsh ==  1) dispColor = ViewPort.medBlue;
	    }
	    if (dispColor != nonCriteria && VectFuns.divergentHorizGt(s,nvo,vi,minRelHorizSpeed)) dispColor = ViewPort.gold;
	    if (dispColor == nonCriteria && VectFuns.divergentHorizGt(s,nvo,vi,minRelHorizSpeed)) dispColor = ViewPort.lightTan;
	  } else {
        Velocity nv3 = Velocity.make(nvo3.Sub(vi3));
        Velocity v3  = Velocity.make(vo3.Sub(vi3));
	    if (CriteriaCore.criterion_3D(s3,v3,epsh,epsv,nv3,D,H)) {
           if (epsh == -1) dispColor = ViewPort.medGreen;
           if (epsh ==  1) dispColor = ViewPort.medBlue;
	    }
	  }
	  return dispColor;
	}


    // Chorus 1 version
	public static Color vertDisplayColorOld(Vect3 so3, Vect3 si3, Velocity vo3, Velocity vi3, Velocity nvo3, double D, double H,
	    double minVertExitSpeed, int epsh, int epsv) {
	  Vect3 s3 = so3.Sub(si3);    
	  Color dispColor = nonCriteria;
	  if (CriteriaCore.criteria(s3, vo3, vi3, nvo3, minVertExitSpeed, D, H, epsh, epsv)) {
	    if (epsv == -1) dispColor = ViewPort.medGreen;  // vertMinusWithHLos; 
	    if (epsv == +1) dispColor = ViewPort.medBlue;   //vertPlusWithHLos; 
	  } else
	    dispColor = Color.red;
	  return dispColor;
	}
	

    // Chorus 2 version
    public static Color vertDisplayColorNew(Vect3 so3, Vect3 si3, Velocity vo3, Velocity vi3, Velocity nvo3, double minVertExitSpeed, int epsv) {
      Vect3 s3 = so3.Sub(si3);    
      Color dispColor = nonCriteria;
      if (CriteriaCore.vertical_new_repulsive_criterion(s3, vo3, vi3, nvo3, epsv)) {
        if (epsv == -1) dispColor = ViewPort.medGreen;  // vertMinusWithHLos; 
        if (epsv == +1) dispColor = ViewPort.medBlue;   //vertPlusWithHLos; 
      } else {
        dispColor = Color.red;
      }
      return dispColor;
    }

	
}
