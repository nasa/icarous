/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.General3DState;
import gov.nasa.larcfm.Util.GeneralState;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class CDSSGeneral implements Detection3DAcceptor,
DetectionPolygonAcceptor {

	/** CD3D object */
	private GeneralDetector cd;

	private ConflictData data;

	public CDSSGeneral() {
		cd = new GeneralDetector();
	}

	public CDSSGeneral(Detection3D cd1, DetectionPolygon cd2) {
		cd = new GeneralDetector();
		cd.setCoreDetection(cd1);
		cd.setCorePolygonDetection(cd2);
	}

	public CDSSGeneral(GeneralDetector cd1) {
		cd = new GeneralDetector();
		cd.setCoreDetection(cd1.getCoreDetection());
		cd.setCorePolygonDetection(cd1.getCorePolygonDetection());		
	}

	public boolean violation(Vect3 so, Velocity vo, General3DState si) {
		boolean rtn = cd.violation(so, vo, si);
		return rtn;
	}

	public boolean violation(Position sop, Velocity vop, GeneralState gs, EuclideanProjection proj) {
		if (sop.isLatLon()) {
			Vect3 so = proj.projectPoint(sop);
			Velocity vo = proj.projectVelocity(sop, vop);
			General3DState si = gs.get3DState(proj);
			return violation(so, vo, si);
		} else {
			return violation(sop.point(), vop, gs.get3DState(null));
		}
	}

	public boolean violation(Position sop, Velocity vop, GeneralState si) {
		EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
		return violation(sop,vop,si,proj);
	}

	public boolean violation(GeneralState so, GeneralState si) {
		if (so.hasPointMass()) {
			return violation(so.getPosition(),so.getVelocity(),si);
		} else {
			return false;
		}
	}


	public boolean conflict(Vect3 so, Velocity vo, General3DState si, double B, double T) {
		boolean rtn = cd.conflict(so, vo, si, B, T);
		return rtn;
	}

	public boolean conflict(Position sop, Velocity vop, GeneralState gs, EuclideanProjection proj, double B, double T) {
		if (sop.isLatLon()) {
			Vect3 so = proj.projectPoint(sop);
			Velocity vo = proj.projectVelocity(sop, vop);
			General3DState si = gs.get3DState(proj);
			return conflict(so, vo, si, B, T);
		} else {
			return conflict(sop.point(), vop, gs.get3DState(null), B, T);
		}
	}

	public boolean conflict(Position sop, Velocity vop, GeneralState si, double B, double T) {
		EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
		return conflict(sop,vop,si,proj, B, T);
	}

	public boolean conflict(GeneralState so, GeneralState si, double B, double T) {
		if (so.hasPointMass()) {
			return conflict(so.getPosition(),so.getVelocity(),si, B, T);
		} else {
			return false;
		}
	}


	public boolean detection(Vect3 so, Velocity vo, General3DState si, double B, double T) {
		data = cd.conflictDetection(so, vo, si, B, T);
		return data.conflict();
	}

	public boolean detection(Position sop, Velocity vop, GeneralState gs, EuclideanProjection proj, double B, double T) {
		if (sop.isLatLon()) {
			Vect3 so = proj.projectPoint(sop);
			Velocity vo = proj.projectVelocity(sop, vop);
			General3DState si = gs.get3DState(proj);
			return detection(so, vo, si, B, T);
		} else {
			return detection(sop.point(), vop, gs.get3DState(null), B, T);
		}
	}

	public boolean detection(Position sop, Velocity vop, GeneralState si, double B, double T) {
		EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
		return detection(sop,vop,si,proj, B, T);
	}

	public boolean detection(GeneralState so, GeneralState si, double B, double T) {
		if (so.hasPointMass()) {
			return detection(so.getPosition(),so.getVelocity(),si, B, T);
		} else {
			return false;
		}
	}


	public double getTimeIn() {
		return data.getTimeIn();
	}

	public double getTimeIn(String ut) {
		return Units.to(ut,getTimeIn());
	}

	public double getTimeOut() {
		return data.getTimeOut();
	}

	public double getTimeOut(String ut) {
		return Units.to(ut,getTimeOut());
	}

	public double getCriticalTimeOfConflict() {
		return data.getCriticalTimeOfConflict();
	}

	public double getCriticalTimeOfConflict(String ut) {
		return Units.to(ut,getCriticalTimeOfConflict());
	}


	@Override
	public void setCorePolygonDetection(DetectionPolygon d) {
		cd.setCorePolygonDetection(d);
	}

	@Override
	public DetectionPolygon getCorePolygonDetection() {
		return cd.getCorePolygonDetection();
	}

	@Override
	public void setCoreDetection(Detection3D d) {
		cd.setCoreDetection(d);
	}

	@Override
	public Detection3D getCoreDetection() {
		return cd.getCoreDetection();
	}

}
