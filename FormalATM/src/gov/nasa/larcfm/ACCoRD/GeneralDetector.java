/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.Optional;

import gov.nasa.larcfm.Util.General3DState;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

/**
 * An object that can detect pairwise violations and conflicts.
 * This object allows traffic information to be about either aircraft or polygons.
 * If this object does not contain an appropriate sub-detector for a given traffic object, then it returns "false" or "no conflict" 
 * -- for example, if no polygon detector is defined, polygon traffic will be ignored.
 */
public class GeneralDetector implements Detection3DAcceptor, DetectionPolygonAcceptor {
	Detection3D cd = null;
	DetectionPolygon cdp = null;

	
	public static final Optional<GeneralDetector> NoDetector = Optional.empty();

	/**
	 * A "standard" detector that holds a CDCylinder and CDPolyIter, both with default parameters.
	 */
	public GeneralDetector() {
		cd = new CDCylinder();
		cdp = new CDPolyIter();
	}
	
	/**
	 * An aircraft-only detector, that ignores polygons.
	 * @param aircraftDetector
	 */
	public GeneralDetector(Detection3D aircraftDetector) {
		cd = aircraftDetector.copy();
	}
	
	/**
	 * A polygon-only detector that ignores aircraft.
	 * @param polyDetector
	 */
	public GeneralDetector(DetectionPolygon polyDetector) {
		cdp = polyDetector.copy();
	}

	/**
	 * A detector that accommodates both aircraft and polygon traffic.
	 * @param aircraftDetector
	 * @param polyDetector
	 */
	public GeneralDetector(Detection3D aircraftDetector, DetectionPolygon polyDetector) {
		cd = aircraftDetector.copy();
		cdp = polyDetector.copy();
	}

	public GeneralDetector copy() {
		return new GeneralDetector(cd == null?null:cd.copy(),cdp==null?null:cdp.copy());
	}
	
	/**
	 * Return true if there is a violation detected.
	 * @param so
	 * @param vo
	 * @param traff
	 * @return
	 */
	  public boolean violation(Vect3 so, Velocity vo, General3DState traff) {
		  if (traff.hasPointMass() && cd != null) return cd.violation(so, vo, traff.getVect3(), traff.getVelocity());
		  if (traff.hasPolygon() && cdp != null) return cdp.violation(so, vo, traff.getPolygon().position(0));
		  return false;
	  }

	  public boolean violation(General3DState own, General3DState traff) {
		  if (own.hasPointMass()) {
			  return violation(own.getVect3(), own.getVelocity(), traff);
		  }
		  return false;
	  }

	  
	  /**
	   * Return true if there is a conflict detected within the given lookahead range.
	   * @param so
	   * @param vo
	   * @param traff
	   * @param B
	   * @param T
	   * @return
	   */
	  public boolean conflict(Vect3 so, Velocity vo, General3DState traff, double B, double T) {
		  if (traff.hasPointMass() && cd != null) return cd.conflict(so, vo, traff.getVect3(), traff.getVelocity(), B, T);
		  if (traff.hasPolygon() && cdp != null) return cdp.conflict(so, vo, traff.getPolygon(), B, T);
		  return false;
		  
	  }

	  public boolean conflict(General3DState own, General3DState traff, double B, double T) {
		  if (own.hasPointMass()) {
			  return conflict(own.getVect3(), own.getVelocity(), traff, B, T);
		  }		  
		  return false;
	  }

	  /**
	   * Return a ConflictData object containing the related conflict information.
	   * If the pair produce more than one conflict, this returns a ConflictData that includes all conflicts (lowest time in, greatest time out).
	   * @param so
	   * @param vo
	   * @param traff
	   * @param B
	   * @param T
	   * @return
	   */
	  public ConflictData conflictDetection(Vect3 so, Velocity vo, General3DState traff, double B, double T) {
	    Vect3 si = traff.getVect3();
	    Velocity vi = traff.getVelocity();
		  if (traff.hasPointMass() && cd != null) return cd.conflictDetection(so, vo, si, vi, B, T);
		  if (traff.hasPolygon() && cdp != null) {
			  boolean det = cdp.conflictDetection(so, vo, traff.getPolygon(), B, T);
			  if (det) {
				  double in = T+1.0;
				  double out = B-1.0;
				  double ctime = -1.0;
				  double cdist = Double.MAX_VALUE;
				  for (int i = 0; i < cdp.getTimesIn().size(); i++) {
					  in = Math.min(in, cdp.getTimesIn().get(i));
					  out = Math.max(in, cdp.getTimesOut().get(i));
					  if (cdp.getDistancesAtCriticalTimes().get(i) < cdist) {
						  cdist = cdp.getDistancesAtCriticalTimes().get(i);
						  ctime = cdp.getCriticalTimesOfConflict().get(i);
					  }
				  }
				  return new ConflictData(in,out,ctime,cdist,so.Sub(si),vo.Sub(vi));
			  }
		  }
		  return new ConflictData();		  
	  }

	  public ConflictData conflictDetection(General3DState own, General3DState traff, double B, double T) {
		  if (own.hasPointMass()) {
			  return conflictDetection(own.getVect3(), own.getVelocity(), traff, B, T);
		  }		  
		  return new ConflictData();
	  }

	  
	@Override
	public void setCorePolygonDetection(DetectionPolygon cd) {
		cdp = cd.copy();
	}

	@Override
	public DetectionPolygon getCorePolygonDetection() {
		return cdp;
	}

	@Override
	public void setCoreDetection(Detection3D cd) {
		this.cd = cd.copy();
	}

	@Override
	public Detection3D getCoreDetection() {
		return cd;
	}

}
