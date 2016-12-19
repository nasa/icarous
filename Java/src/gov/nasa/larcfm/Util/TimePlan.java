/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.Kinematics;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.ProjectedKinematics;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Constants;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

public class TimePlan {

	public int numMidAccelPoints = 2; // when converting to a plan add up to this number of mid-accel points to the accel zone.  Number of points added = min(numMidAccelPoints, floor(accelTime/5))
	
	public enum PointTypeT {NONE,BOT,EOT,EOTBOT,TRKFIX}; // TRKFIX is a correction point for track, see vin
	public enum PointTypeG {NONE,BGS,EGS,EGSBGS, GSFIX}; // GSFIX is a correction point for gs, see vin
	public enum PointTypeV {NONE,BVS,EVS,EVSBVS, VSFIX}; // VSFIX is a correction point for vs, see vsin

	public class TimePoint {
		public Position pos = Position.INVALID;
		public Velocity vel = Velocity.INVALID;
		public double signedRadius = 0.0;
		public double g_accel = 0.0;
		public double v_accel = 0.0;
		public PointTypeT t_type = PointTypeT.NONE;
		public PointTypeG g_type = PointTypeG.NONE;
		public PointTypeV v_type = PointTypeV.NONE;
		public Position h_srcPos = Position.INVALID;
		public double h_srcTime = Double.NaN;
		public Position v_srcPos = Position.INVALID;
		public double v_srcTime = Double.NaN;
		public String label = "";
		@Override
		public String toString() {
			return "TimePoint [pos=" + pos + ", vel=" + vel + ", signedRadius="
					+ f.Fm4(signedRadius) + ", g_accel=" + f.Fm4(g_accel) + ", v_accel=" + f.Fm4(v_accel) + ", t_type=" + t_type
					+ ", gtype=" + g_type
					+ ", v_type=" + v_type + ", h_srcPos=" + h_srcPos
					+ ", h_srcTime=" + h_srcTime + ", v_srcPos=" + v_srcPos
					+ ", v_srcTime=" + v_srcTime + ", label=" + label + "]";
		}
	}

	public class htype {
		public Position pos = Position.INVALID;
		public double signedRadius = Double.NaN;
		public double g_accel = Double.NaN;
		public Velocity vin = Velocity.INVALID;
		public PointTypeT ttype = PointTypeT.NONE;
		public PointTypeG gtype = PointTypeG.NONE;
		public Position srcPos = Position.INVALID;
		public double srcTime = Double.NaN;
		public String label = "";

		public htype() {}

		public htype(Position pos, double signedRadius, double gaccel, Velocity vin, PointTypeT typet, PointTypeG typeg,
				Position srcPos, double srcTime, String label) {
			super();
			this.pos = pos;
			this.signedRadius = signedRadius;
			this.g_accel = gaccel;
			this.vin = vin;
			this.ttype = typet;
			this.gtype = typeg;
			this.srcPos = srcPos;
			this.srcTime = srcTime;
			this.label = label;
		}

		@Override
		public String toString() {
			return "htype [pos=" + pos + ", signedRadius=" + signedRadius + ", g_accel=" + g_accel + ", vin=" + vin
					+ ", ttype=" + ttype + ", gtype=" + gtype + ", srcPos=" + srcPos + ", srcTime="
					+ srcTime + ", label=" + label + "]";
		}
	}

//	public class gtype {
//		public Position pos = Position.INVALID;
//		public double accel = Double.NaN;
//		public Velocity vin = Velocity.INVALID;
//		public PointTypeG gtype = PointTypeG.NONE;
//		public Position srcPos = Position.INVALID;
//		public double srcTime = Double.NaN;
//		public String label = "";
//
//		public gtype() {}
//
//		public gtype(Position pos, double accel, Velocity vin, PointTypeG typeg,
//				Position srcPos, double srcTime, String label) {
//			super();
//			this.pos = pos;
//			this.accel = accel;
//			this.vin = vin;
//			this.gtype = typeg;
//			this.srcPos = srcPos;
//			this.srcTime = srcTime;
//			this.label = label;
//		}
//
//		@Override
//		public String toString() {
//			return "gtype [pos=" + pos + ", accel=" + accel + ", vin=" + vin
//					+ ", gtype=" + gtype + ", srcPos=" + srcPos + ", srcTime="
//					+ srcTime + ", label=" + label + "]";
//		}
//	}
	
	public class vtype {
		public double alt = Double.NaN;
		public double accel = Double.NaN;
		public double vsin = Double.NaN;
		public PointTypeV vtype = PointTypeV.NONE;
		public Position srcPos = Position.INVALID;
		public double srcTime = Double.NaN;
		public String label = "";

		public vtype() {}

		public vtype(double alt, double accel, double vsin, PointTypeV type,
				Position srcPos, double srcTime, String label) {
			super();
			this.alt = alt;
			this.accel = accel;
			this.vsin = vsin;
			this.vtype = type;
			this.srcPos = srcPos;
			this.srcTime = srcTime;
			this.label = label;
		}

		@Override
		public String toString() {
			return "vtype [alt=" + Units.to("ft", alt) + ", accel=" + accel + ", vsin=" + vsin
					+ ", vtype=" + vtype + ", srcPos=" + srcPos + ", srcTime="
					+ srcTime + ", label=" + label + "]";
		}

	}

	// time -> (position, accel/radius, gsin, type)
	private TreeMap<Double,htype> hmap = new TreeMap<Double,htype>();
//	// time -> (position, accel, gsin, type)
//	private TreeMap<Double,gtype> gmap = new TreeMap<Double,gtype>();
	// time -> (alt, accel, vsin, type)
	private TreeMap<Double,vtype> vmap = new TreeMap<Double,vtype>();

	/** Returns null if there is already an incompatible point type at that time */
	private PointTypeT tPointType(double time, PointTypeT ty) {
		if (hmap.get(time) == null) return ty;
		PointTypeT ty1 = hmap.get(time).ttype;
		if (ty == PointTypeT.NONE || ty == ty1) return ty1;
		if (ty1 == PointTypeT.NONE) return ty;
		if (ty == PointTypeT.TRKFIX && (ty1 != PointTypeT.NONE)) return ty1;
		if ((ty == PointTypeT.BOT && ty1 == PointTypeT.EOT) || (ty == PointTypeT.EOT && ty1 == PointTypeT.BOT)) return PointTypeT.EOTBOT;
		f.pln("TimePlan ERROR: incompatable tPointTypes "+ty+" "+ty1+" at "+time);
		Debug.halt(toString());
		return null;
	}

	/** Returns null if there is already an incompatible point type at that time */
	private PointTypeG gPointType(double time, PointTypeG ty) {
		if (hmap.get(time) == null) return ty;
		PointTypeG ty1 = hmap.get(time).gtype;
		if (ty == PointTypeG.NONE || ty == ty1) return ty1;
		if (ty1 == PointTypeG.NONE) return ty;
		if (ty == PointTypeG.GSFIX && (ty1 != PointTypeG.NONE)) return ty1;
		if ((ty == PointTypeG.BGS && ty1 == PointTypeG.EGS) || (ty == PointTypeG.EGS && ty1 == PointTypeG.BGS)) return PointTypeG.EGSBGS;
		f.pln("TimePlan ERROR: incompatable gPointTypes "+ty+" "+ty1+" at "+time);
		Debug.halt(toString());
		return null;
	}

	/** Returns null if there is already an incompatible point type at that time */
	private PointTypeV vPointType(double time, PointTypeV ty) {
		if (vmap.get(time) == null) return ty;
		PointTypeV ty1 = vmap.get(time).vtype;
//f.pln("vPointType time="+time+" newty="+ty+" oldty="+ty1);		
		if (ty == PointTypeV.NONE || ty == ty1) return ty1;
		if (ty1 == PointTypeV.NONE) return ty;
		if (ty == PointTypeV.VSFIX && (ty1 != PointTypeV.NONE)) return ty1;
		if ((ty == PointTypeV.BVS && ty1 == PointTypeV.EVS) || (ty == PointTypeV.EVS && ty1 == PointTypeV.BVS)) return PointTypeV.EVSBVS;
		f.pln("TimePlan ERROR: incompatable vPointTypes "+ty+" "+ty1+" at "+time);
		Debug.halt(toString());
		return null;
	}

	private void hadd(double time, Position p, double ta, double ga, Velocity vin, PointTypeT tyt, PointTypeG tyg, Position srcP, double srcT, String lab) {
		//f.pln(" $$>>   hadd: tyt = "+tyt+" tyg ="+tyg+" time = "+time+" p = "+p+" lab = "+lab+" vin="+vin);
		if (time == 0.0) time = 0.0; // prevent negative zero
		htype prev = hmap.get(time);
		if (prev != null && !ttypeNone(prev.ttype) && !gtypeNone(prev.gtype)) {
			double dist = prev.pos.distanceH(p);
			if (dist > 0.1) {
				f.pln("TIMEPLAN WARNING: old horizontal point at time "+time+ " has a position "+dist+" m away from new point!");
				f.pln("old point="+hmap.get(time));
                Debug.halt();
			}
			double vdist = prev.vin.vect2().distance(vin.vect2());
			if (!prev.vin.isInvalid() && !vin.isInvalid() && vdist > 0.01) {
				f.pln("TIMEPLAN WARNING: old horizontal vin at time "+time+ " has a velocity "+vdist+" m/s away from new vin!");
				f.pln("old point="+hmap.get(time));
				if ((prev.ttype == PointTypeT.BOT && tyt == PointTypeT.BOT) || (prev.gtype == PointTypeG.BGS && tyg == PointTypeG.BGS)) {
					Debug.halt();
				}
			}
		}	
		// preserve old vin
		if (prev != null && ((prev.ttype == PointTypeT.BOT && tyt != PointTypeT.BOT) || (prev.gtype == PointTypeG.BGS && tyg != PointTypeG.BGS))) {
			vin = prev.vin;
		}
		// preserve accel values
		ta = (ta == 0.0 && prev != null ? prev.signedRadius : ta);
		ga = (ga == 0.0 && prev != null ? prev.g_accel : ga);
		
		PointTypeT newtypet = tPointType(time,tyt);
		PointTypeG newtypeg = gPointType(time,tyg);
		if (newtypet != tyt && newtypeg != tyg && ttypeNone(tyt)) {
			f.pln("hadd: track-defined point already exists at time="+time);
		} else if (newtypet != tyt && newtypeg != tyg && gtypeNone(tyg)) {
			f.pln("hadd: gs-defined point already exists at time="+time);
		} else {
			hmap.put(time, new htype(p,ta,ga,vin,tPointType(time,tyt),gPointType(time,tyg),srcP,srcT,lab));
		}
	}

	private void vadd(double time, double alt, double a, double vsin, PointTypeV ty, Position srcP, double srcT, String lab) {
        //f.pln(" $$$>>>>> vadd: ty = "+ty+" time = "+time+" lab = "+lab+" a = "+a+" vsin="+vsin+" alt = "+Units.str("ft", alt));
		if (time < 0.0 || Double.isNaN(time)) Debug.halt();
		if (time == 0.0) time = 0.0; // prevent negative zero
		vtype prev = vmap.get(time);
		if (prev != null && !vtypeNone(prev.vtype)) {
			double dist = Math.abs(prev.alt - alt);
			if (dist > 0.1) {
				f.pln("TIMEPLAN WARNING: old vertical point at time "+time+ " has an alt "+dist+" m away from new point!");
				f.pln("old point="+vmap.get(time));
                Debug.halt();
			}
			double vsdiff = Math.abs(prev.vsin - vsin);
			if (!Double.isNaN(vsdiff) && vsdiff > 2.0) {
				f.pln("TIMEPLAN WARNING: old vertical vsin at time "+time+ " has a velocity "+vsdiff+" m/s away from new vsin!");
				f.pln("old point="+vmap.get(time));
				if (prev.vtype == PointTypeV.BVS && ty == PointTypeV.BVS) {
					Debug.halt();
				}
			}
		}
		// preserve old vsin
		if (prev != null && (prev.vtype == PointTypeV.BVS && ty != PointTypeV.BVS)) {
			vsin = prev.vsin;
		}
		PointTypeV newtype = vPointType(time,ty);
		if (newtype != ty && vtypeNone(ty)) {
			//f.pln(" $$>> vadd: vs-defined point already exists at time="+time);
		} else {
			vmap.put(time, new vtype(alt,a,vsin,newtype,srcP,srcT,lab));
		}
	}

	public void addStartHoriz(double time, Position p, Velocity vin) {
		//f.pln(" $$>> addStartHoriz: time = "+time+" vin = "+vin);
		hadd(time, p, 0.0, 0.0, vin, PointTypeT.NONE, PointTypeG.NONE,p, time, "START");
	}

	public void addStartVert(double time, double alt, double vs) {
		//f.pln(" $$>> addStartVert: time = "+time+" vs = "+vs);
		vadd(time, alt, 0.0, vs, PointTypeV.NONE, Position.ZERO_XYZ.mkAlt(alt), time, "START");
	}

	public void addStart(double time, Position p, Velocity vin) {
		//f.pln(" $$>> addStart: time = "+time+" vin = "+vin);
		hadd(time, p, 0.0, 0.0, vin, PointTypeT.NONE, PointTypeG.NONE,p, time, "START");
		vadd(time, p.alt(), 0.0, vin.vs(), PointTypeV.NONE, p, time, "START");
	}

	public void addEndHoriz(double time, Position p, Velocity vout) {
		//f.pln(" $$>> addEnd: time = "+time+" vout = "+vout);
		hadd(time, p, 0.0, 0.0, vout, PointTypeT.NONE, PointTypeG.NONE, p, time, "END");
	}

	public void addEndVert(double time, double alt, double vz) {
		//f.pln(" $$>> addEnd: time = "+time+" vout = "+vout);
		vadd(time, alt, 0.0, vz, PointTypeV.NONE, Position.ZERO_XYZ.mkAlt(alt), time, "END");
	}

	public void addEnd(double time, Position p, Velocity vout) {
		//f.pln(" $$>> addEnd: time = "+time+" vout = "+vout);
		hadd(time, p, 0.0, 0.0, vout, PointTypeT.NONE, PointTypeG.NONE, p, time, "END");
		vadd(time, p.alt(), 0.0, vout.vs(), PointTypeV.NONE, p, time, "END");
	}

	public void addPeriodPoint(double time, Position p) {
		//f.pln(" $$$$$$$$ ADD PERIOD POINT time = "+time);
		haddNone(time, p,  "PERIOD");
		vaddNone(time, p.alt(), "PERIOD");
	}
	
//	public void addHorizonPoint(double time, Position p) {
//		//f.pln(" $$>> addHorizonPoint: time = "+time+" vin = "+vin);
//		hadd(time, p, 0.0, Velocity.INVALID, PointTypeT.NONE, PointTypeG.NONE,p, time, "HORIZON");
//		vadd(time, p.alt(), 0.0, Double.NaN, PointTypeV.NONE, p, time, "HORIZON");
//	}
	
	public void addCurrentPos(double time, Position p, Velocity vin) {
		//f.pln(" $$>> addCurrentPos: time = "+time+" vin = "+vin);
		hadd(time, p, 0.0, 0.0, vin, PointTypeT.NONE, PointTypeG.NONE,p, time, "CURRENT_POS");
		vadd(time, p.alt(), 0.0, vin.vs(), PointTypeV.NONE, p, time, "CURRENT_POS");
	}


	public void addHorizonH(double time, Position p) {
		hadd(time, p, 0.0, 0.0, Velocity.INVALID, PointTypeT.NONE, PointTypeG.NONE,p, time, "H_HORIZON");
	}
	
	public void addHorizonV(double time, Position p) {
		vadd(time, p.alt(), 0.0, Double.NaN, PointTypeV.NONE, p, time, "V_HORIZON");
	}
	
	public void haddNone(double time, Position p, String label) {
		hadd(time,p, 0.0, 0.0, Velocity.INVALID,PointTypeT.NONE, PointTypeG.NONE, p, time, label);
	}

	public void vaddNone(double time, double alt, String label) {
		vadd(time,alt,Double.NaN,Double.NaN,PointTypeV.NONE, Position.ZERO_XYZ.mkAlt(alt), time, label);
	}


	//	public void addBaseHorizPoint(Position p, double time, String label) {
	//		hadd(time, p, 0, Velocity.INVALID, PointType.NONE);
	//	}

	public void addBOT(double time, Position p, double omega, Velocity vin, Position srcP, double srcT, String label) {
		//f.pln(" $$+++++++++++= addBOT time = "+time+" omega = "+omega+" vin = "+vin);
		if (vin.isInvalid()) Debug.halt();
		if (turnBegin(time) >= 0.0) f.pln("TIMEPLAN WARNING: Adding a BOT at time "+time+" that appears to overlap with an existing turn region starting at "+turnBegin(time));
		hadd(time, p, omega, 0.0, vin, PointTypeT.BOT, PointTypeG.NONE, srcP, srcT, label);
	}

	public void addEOT(double time, Position p, double omega, Velocity vout, Position srcP, double srcT, String label) {
		hadd(time, p, 0.0, 0.0, vout, PointTypeT.EOT, PointTypeG.NONE, srcP, srcT, label);
	}

	public void addBGS(double time, Position p, double acc, Velocity vin, Position srcP, double srcT, String label) {
		if (vin.isInvalid()) Debug.halt();
		if (gsBegin(time) >= 0.0) f.pln("TIMEPLAN WARNING: Adding a BGS at time "+time+" that appears to overlap with an existing GS region starting at "+gsBegin(time));
		hadd(time, p, 0.0, acc, vin, PointTypeT.NONE,PointTypeG.BGS, srcP, srcT, label);
	}

	public void addEGS(double time, Position p, double acc, Velocity vout, Position srcP, double srcT, String label) {
		hadd(time, p, 0.0, 0.0, vout, PointTypeT.NONE,PointTypeG.EGS, srcP, srcT, label);
	}

	//	public void addBaseVertPoint(double alt, double time, String label) {
	//		vadd(time, alt, 0, 0.0, PointType.NONE, label);
	//	}

	public void addBVS(double time, double alt, double acc, double vsin, Position srcP, double srcT, String label) {
		//f.pln(" $$ TimePlan.addBVS:  time = "+time+"  alt = "+Units.str("ft", alt)+" acc = "+acc+" vsin = "+vsin);
		if (vsBegin(time) >= 0.0) f.pln("TIMEPLAN WARNING: Adding a BVS at time "+time+" that appears to overlap with an existing VS region starting at "+vsBegin(time));
		vadd(time, alt, acc, vsin, PointTypeV.BVS, srcP, srcT, label);
	}

	public void addEVS(double time, double alt, double acc, double vsout, Position srcP, double srcT, String label) {
		//f.pln(" $$ TimePlan.addEVS:  time = "+time+"  alt = "+Units.str("ft", alt)+" acc = "+acc);
		vadd(time, alt, acc, vsout, PointTypeV.EVS, srcP, srcT, label);
	}
	//
	
	//-------------
	// These 3 are currently functionally the same as haddNona and vaddNone, but are intended to potentially store known velocity fragments:

	// add a TRK-specified point (no known acceleration)
	public void addTRKFIX(double time, Position p, Velocity vin, String label) {
//		hadd(time,p,Double.NaN,vin,PointTypeT.TRKFIX, PointTypeG.NONE, p, time, label);
		hadd(time,p,0.0,0.0,Velocity.INVALID,PointTypeT.TRKFIX, PointTypeG.NONE, p, time, label);
	}

	// add a GS-specified point (no known acceleration)
	public void addGSFIX(double time, Position p, Velocity vin, String label) {
//		hadd(time,p,Double.NaN,vin,PointTypeT.NONE, PointTypeG.GSFIX, p, time, label);
		hadd(time,p,0.0,0.0,Velocity.INVALID,PointTypeT.NONE, PointTypeG.GSFIX, p, time, label);
	}

	// add a VS-specified point (no known acceleration)
	public void addVSFIX(double time, double alt, double vz, String label) {
//		vadd(time,alt,Double.NaN,vz,PointTypeV.VSFIX, Position.ZERO_XYZ.mkAlt(alt), time, label);
		vadd(time,alt,Double.NaN,Double.NaN,PointTypeV.VSFIX, Position.ZERO_XYZ.mkAlt(alt), time, label);
	}

	//-------------
	
	public void hremove(double time) {
		hmap.remove(time);
	}

	public void vremove(double time) {
		vmap.remove(time);
	}

	// return null if time not found
	public htype hget(double time) {
		return hmap.get(time);
	}

	public Set<Double> htimes() {
		return hmap.keySet();
	}

	// return null if time not found
	public vtype vget(double time) {
		return vmap.get(time);
	}

	public Set<Double> vtimes() {
		return vmap.keySet();
	}


	public double getFirstTime() {
		if (hmap.size() == 0) {
			if (vmap.size() == 0) return 0.0;
			return vmap.firstKey();
		}
		if (vmap.size() == 0) return hmap.firstKey();
		return Util.min(vmap.firstKey(), hmap.firstKey());
	}

	public double getLastTime() {
		//f.pln(" ###$$$ getLastTime: vmap = "+vmap);
		//f.pln(" ###$$$ getLastTime: "+vmap.size()+" "+hmap.size()+" "+vmap.lastKey()+" "+ hmap.lastKey());
		if (hmap.size() == 0) {
			if (vmap.size() == 0) return 0.0;
			return vmap.lastKey();
		}
		if (vmap.size() == 0) return hmap.lastKey();
		return Util.max(vmap.lastKey(), hmap.lastKey());
	}

	public double prevHTime(double t) {
		Set<Double> hts = htimes();
		double prev = -1.0;
		for (Double htm: hts) {
			if (htm < t) prev = htm;
			else break;
		}
		return prev;
	}

	public double nextHTime(double t) {
		Set<Double> hts = htimes();
		for (Double htm: hts) {
			if (htm > t) return htm;
		}
		return -1.0;
	}

	public double prevVTime(double t) {
		Set<Double> vts = vtimes();
		double prev = -1.0;
		for (Double vtm: vts) {
			if (vtm < t) prev = vtm;
			else break;
		}
		return prev;
	}

	public double nextVTime(double t) {
		Set<Double> vts = vtimes();
		for (Double vtm: vts) {
			if (vtm > t) return vtm;
		}
		return -1.0;
	}

	public double prevTime(double t) {
		Set<Double> ts = allTimes();
		double prev = -1.0;
		for (Double tm: ts) {
			if (tm < t) prev = tm;
			else break;
		}
		return prev;
	}

	public double nextTime(double t) {
		Set<Double> ts = allTimes();
		for (Double tm: ts) {
			if (tm > t) return tm;
		}
		return -1.0;
	}

	//	/**
	//	 * This finds the last previous index where the TCP is of type tcp1 or tcp2
	//	 */
	//	public int prevTCP(int current, NavPoint.TCPType tcp1, NavPoint.TCPType tcp2) {
	//		if (current < 0) {
	//			addWarning("prevTCP invalid starting index "+current+" with TCP types "+tcp1+" and "+tcp2);
	//			return -1;
	//		}
	//		for (double time : allTimes()) {
	//
	//		for (int j = current; j >=0; j--) {
	//			if (points.get(j).tcp==tcp1 || points.get(j).tcp==tcp2) {
	//				return j;
	//			}
	//		}
	//		return -1;
	//	}


	//	/**
	//	 * Returns the point time <= time, or -1 if before the first
	//	 */
	//	public double prevHTime(double t) {
	//		double prev = -1;
	//		for (Double key : hmap.keySet()) {
	//			if (key > t) break;
	//			prev = key;
	//		}
	//		return prev;
	//	}
	//	
	//	/**
	//	 * Returns the point time > time, or -1 if after last
	//	 */
	//	public double nextHTime(double t) {
	//		for (Double key : hmap.keySet()) {
	//			if (key > t) return key;
	//		}
	//		return -1;		
	//	}

	/** Return turn begin time or negative time if not in turn.  Used to see if in a (possibly not closed) accel zone or not. */
	public double turnBegin(double t) {
		double ret = -1;
		for (Double key : hmap.keySet()) {
			if (key > t) break;
//			if (t == 2590) {
//				f.pln(" hmap = "+hmap);
//				f.pln(" hmap.get(key).ttype = "+hmap.get(key).ttype);
//			}
			if (hmap.get(key).ttype == PointTypeT.BOT || hmap.get(key).ttype == PointTypeT.EOTBOT) ret = key;
			if (hmap.get(key).ttype == PointTypeT.EOT) ret = -1;
		}
		return ret;
	}

	/** Return gs begin time or negative time if not in gs accel.  Used to see if in a (possibly not closed) accel zone or not. */
	public double gsBegin(double t) {
		double ret = -1;
		for (Double key : hmap.keySet()) {
			if (key > t) break;
			if (hmap.get(key).gtype == PointTypeG.BGS || hmap.get(key).gtype == PointTypeG.EGSBGS) ret = key;
			if (hmap.get(key).gtype == PointTypeG.EGS) ret = -1;
		}		
		return ret;
	}

	/** Return vs begin time or negative time if not in vs accel.  Used to see if in a (possibly not closed) accel zone or not. */
	public double vsBegin(double t) {
		double ret = -1;
		for (Double key : vmap.keySet()) {
			if (key > t) break;
			if (vmap.get(key).vtype == PointTypeV.BVS || vmap.get(key).vtype == PointTypeV.EVSBVS) ret = key;
			if (vmap.get(key).vtype == PointTypeV.EVS) ret = -1;
		}		
		return ret;
	}

	public Position position(double time) {
		//f.pln("..... TimePlan.position: ENTER time="+time+" getFirstTime = "+getFirstTime()+" getLastTime = "+getLastTime());
		if (!timeValid(time)) {
			f.pln("..... TimePlan.position: OUT OF TIME RANGE! "+getFirstTime()+" <? time = "+time+" <? "+getLastTime());
			//f.pln(f.Fobj(allTimes()));			
			Debug.halt();
			return Position.INVALID;
		}
		Position pos;
		if (hmap.get(time) == null) {
			//f.pln("..... TimePlan.position: hMAP == null");
			double tbegin = turnBegin(time);
			double gsbegin = gsBegin(time);
			if (tbegin >= 0.0) {
				double dt = time-tbegin;
				pos = ProjectedKinematics.turnOmega(hmap.get(tbegin).pos, hmap.get(tbegin).vin, dt, hmap.get(tbegin).signedRadius).first;
				//f.pln("..... TimePlan.position: time="+time+" pos="+pos+" case 1");				
			} else if (gsbegin >= 0.0) {
				double dt = time-gsbegin;
				pos = ProjectedKinematics.gsAccel(hmap.get(gsbegin).pos, hmap.get(gsbegin).vin, dt, hmap.get(gsbegin).g_accel).first;
				//f.pln("..... TimePlan.position: time="+time+" pos="+pos+" case 2");				
			} else {
				double prevT = -1.0;
				double nextT = -1.0;
				for (Double key : hmap.keySet()) {
					if (key <= time) prevT = key;
					if (key > time) {
						nextT = key;
						break;
					}
				}
				if (prevT < 0) {
					pos = Position.INVALID;
					//f.pln("..... TimePlan.position: time="+time+" pos="+pos+" case 3");				
				} else if (nextT < 0) {
					pos = hmap.get(prevT).pos;
					//f.pln("..... TimePlan.position: time="+time+" pos="+pos+" case 4");				
				} else {
					//					Velocity v = hmap.get(prevT).pos.initialVelocity(hmap.get(nextT).pos, nextT-prevT);
					pos = new Position(GreatCircle.interpolate(hmap.get(prevT).pos.lla(), hmap.get(nextT).pos.lla(), (time-prevT)/(nextT-prevT)));
					//					Velocity v2 = pos.initialVelocity(hmap.get(nextT).pos, nextT-time);
				}
			}
		} else {
			pos = hmap.get(time).pos;
			//f.pln(" $$..... TimePlan.position: time="+time+" pos="+pos+" case 6 = ELSE");				
		}
		Double alt;
		if (vmap.get(time) == null) {
			double vsbegin = vsBegin(time);
			if (vsbegin >= 0.0) {
				double dt = time-vsbegin;
				Vect3 so = Vect3.mkXYZ(0,0,vmap.get(vsbegin).alt);
				double vsin = vmap.get(vsbegin).vsin;
				Velocity vo = Velocity.mkVxyz(0, 0, vsin );
				double a = vmap.get(vsbegin).accel;
				//f.pln(" $$>>>>>>..... TimePlan.position: so = "+so+" vo = "+vo+" a = "+a+" vsbegin = "+vsbegin+" vsin = "+vsin);
				alt = Kinematics.vsAccel(so, vo, dt, a).first.z();
				//f.pln("..... TimePlan.position: time="+time+" alt="+Units.to("ft", alt)+" case 1");				
			} else {
				double prevT = -1.0;
				double nextT = -1.0;
				for (Double key : vmap.keySet()) {
					if (key <= time) prevT = key;
					if (key > time) {
						nextT = key;
						break;
					}
				} if (prevT < 0) {
					alt = Double.NaN;					
					//f.pln("..... TimePlan.position: time="+time+" alt="+Units.to("ft", alt)+" case 2");				
				} else if (nextT < 0) {
					alt = vmap.get(prevT).alt;
					//f.pln("..... TimePlan.position: time="+time+" alt="+Units.to("ft", alt)+" case 3");				
				} else {
					double alt1 = vmap.get(prevT).alt;
					double alt2 = vmap.get(nextT).alt;
					double vs = (alt2-alt1)/(nextT-prevT);
					alt = alt1 + vs *(time-prevT);
					//f.pln("prev alt = "+Units.to("ft",vmap.get(prevT).alt));
					//f.pln("next alt = "+Units.to("ft",vmap.get(nextT).alt));
					//f.pln("vs = "+Units.to("fpm", vs));
					//f.pln("..... TimePlan.position: time="+time+" alt="+Units.to("ft", alt)+" case 4");				
				}
			}
		} else {
			alt = vmap.get(time).alt;
			//f.pln("..... TimePlan.position:time="+time+" alt="+Units.to("ft", alt)+" case 5");				
		}
		return new Position(LatLonAlt.mk(pos.lat(), pos.lon(), alt));
	}

    // time = absolute time of extrapolated point
	public Position positionExtrapolate(double time) {
		//f.pln("..... TimePlan.positionExtrapolate: ENTER time="+time); //+" TimePlan = "+this.toString());
		if (timeValid(time)) {
			return position(time);
		}

		Position posH;
		double prevT = -1.0;
		for (Double key : hmap.keySet()) {
			if (key <= time) prevT = key;
		}
		if (prevT < 0) {
			posH = Position.INVALID;
			//f.pln(" $$$ TimePlan.positionExtrapolate: time="+time+" posH="+posH+" case 3");				
		} else {
			Velocity vin = velocity(prevT);
			htype h = hmap.get(prevT);
			//f.pln(" $$$ TimePlan.positionExtrapolate1: time = "+time+" prevT = "+prevT+" h.pos = "+h.pos+" vin="+vin);
			posH = new Position(GreatCircle.linear_initial(h.pos.lla(), vin, time - prevT));
			//f.pln(" $$$ TimePlan.positionExtrapolate2: pos = "+posH);
		}
		
		double altV;
		prevT = -1.0;
		for (Double key : vmap.keySet()) {
			if (key <= time) prevT = key;
		}
		if (prevT < 0) {
			altV = Double.NaN;
			//f.pln(" $$$ TimePlan.positionExtrapolate: time="+time+" altV="+Units.str("ft", altV)+" case 3");				
		} else {
			//f.pln(" $$$$$$ positionExtrapolate: prevT = "+prevT);
			Velocity vin = velocity(prevT);
			vtype v = vmap.get(prevT);
			//f.pln(" $$$ TimePlan.positionExtrapolate3: time = "+time+" prevT = "+prevT+" v.alt = "+Units.str("ft", v.alt)+" vin="+vin+" v = "+v);
			altV = v.alt + vin.vs()*(time - prevT);
			//f.pln(" $$$ TimePlan.positionExtrapolate4: altV = "+Units.str("ft", altV));
		}
		//f.pln(" $$$$$$$$$$$$ ..... TimePlan.positionExtrapolate: EXIT posH = "+posH+" altV = "+Units.str("ft", altV));
		return posH.mkAlt(altV);
	}

	/**
	 * 
	 * Note: This assumes that the Time plan has beem constructed such that there exists at least one point (a start point) at or before the time queried
	 * 
	 * @param time
	 * @return
	 */
	public Velocity velocity(double time) {
		//f.pln(" $$$$$$$$$$$$$$$$$$$$$ TimePlan.velocity time="+time);
		if (time < getFirstTime() || time > getLastTime()) return Velocity.INVALID;
		Velocity vel;
		if (hmap.get(time) == null || hmap.get(time).vin.isInvalid()) {
			double tbegin = turnBegin(time);
			double gsbegin = gsBegin(time);
			if (tbegin >= 0.0) {
				double dt = time-tbegin;
				vel = ProjectedKinematics.turnOmega(hmap.get(tbegin).pos, hmap.get(tbegin).vin, dt, hmap.get(tbegin).signedRadius).second;
			} else if (gsbegin >= 0.0) {
				double dt = time-gsbegin;
				vel = ProjectedKinematics.gsAccel(hmap.get(gsbegin).pos, hmap.get(gsbegin).vin, dt, hmap.get(gsbegin).g_accel).second;
			} else {
				double prevT = -1.0;
				double nextT = -1.0;
				for (Double key : hmap.keySet()) {
					if (key < time) prevT = key;
					if (key > time) {
						nextT = key;
						break;
					}
				}
				//f.pln(" $$$$$$$$$$$$$ velocity here0 time="+time+" prevT="+prevT+"  nextT="+nextT+" hmap.get="+hmap.get(prevT)+" hmap.get(time)="+hmap.get(time));
				if (prevT >= 0) {
					if (hmap.get(time) != null) {
						vel = GreatCircle.velocity_final(hmap.get(prevT).pos.lla(), hmap.get(time).pos.lla(), time-prevT);
					} else if (nextT >= 0) {
						vel = GreatCircle.interpolate_velocity(hmap.get(prevT).pos.lla(), hmap.get(nextT).pos.lla(), nextT-prevT, (time-prevT)/(nextT-prevT));
					} else if (!hmap.get(prevT).vin.isInvalid()) {
						vel = hmap.get(prevT).vin;
					} else {
						vel = Velocity.INVALID;
					}
				} else if (nextT >= 0) {
					if (hmap.get(time) != null) { //first point
						vel = GreatCircle.velocity_initial(hmap.get(time).pos.lla(), hmap.get(nextT).pos.lla(), nextT-time);
					} else { // before the first point
						vel = Velocity.INVALID;						
					}
				} else {// no clue what's going on -- single point?
					vel = Velocity.INVALID;
				}
			}
		} else {
			vel = hmap.get(time).vin;
		}
		
		// fix the track of the very first point if it is not a turn-special point
		if (time == hmap.firstKey() && hmap.get(time).ttype == PointTypeT.NONE) {
			double trk = hmap.get(time).pos.initialVelocity(hmap.get(nextHTime(time)).pos, 100).trk();
			vel = vel.mkTrk(trk);
		}

		//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ TimePlan.velocity1: time = "+time+" vel = "+vel);
		Double vz;
		if (vmap.get(time) == null || Double.isNaN(vmap.get(time).vsin)) {
			double vsbegin = vsBegin(time);
			if (vsbegin >= 0.0) {
				double dt = time-vsbegin;
				Vect3 so = Vect3.mkXYZ(0,0,vmap.get(vsbegin).alt);
				Velocity vo = Velocity.mkVxyz(0, 0, vmap.get(vsbegin).vsin);
				double a = vmap.get(vsbegin).accel;
				vz = Kinematics.vsAccel(so, vo, dt, a).second.z();
			} else {
				double prevT = -1.0;
				double nextT = -1.0;
				for (Double key : vmap.keySet()) {
					if (key < time) prevT = key;
					if (key > time) {
						nextT = key;
						break;
					}
				}
				//f.pln(" $$$$ TimePlan.velocity: time = "+time+" prevT = "+prevT+" nextT = "+nextT);
				if (prevT >= 0) {
					if (vmap.get(time) != null) {
						vz = (vmap.get(time).alt-vmap.get(prevT).alt)/(time-prevT);
					} else if (nextT >= 0) {
						vz = (vmap.get(nextT).alt-vmap.get(prevT).alt)/(nextT-prevT);
					} else if (!Double.isNaN(vmap.get(prevT).vsin)) {
						vz = vmap.get(prevT).vsin;
					} else {
						vz = Double.NaN;
					}
				} else if (nextT >= 0) {
					if (vmap.get(time) != null) { //first point
						//f.pln(" $$$$ vmap.get(nextT) = "+vmap.get(nextT));
						vz = (vmap.get(time).alt - vmap.get(nextT).alt)/(nextT-time);
					} else { // before the first point
						//f.pln(" $$$$ TimePlan.velocity:: before the first point, vmap.get("+f.Fm1(time)+") = "+vmap.get(time));
						vz = Double.NaN;			
					}
				} else {// no clue what's going on -- single point?
					vz = Double.NaN;
				}
			}
		} else {
			vz = vmap.get(time).vsin;
		}
		//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ TimePlan.velocity2: time = "+time+" vz = "+vz);
		return vel.mkVs(vz);
	}

	public int removeRedundantVertPoints(double minAltDiff) {
		double prevTime = -1;
		double currTime = -1;
		double nextTime = -1;
		int count = 0;
		ArrayList<Double> vkillTimes = new ArrayList<Double>();
		for(double time : allTimes()) {
			if (vmap.get(time) != null) {
				if (prevTime < 0) {
					prevTime = time;
				} else if (currTime < 0) {
					currTime = time;
				} else {
					nextTime = time;
					double prevAlt = vmap.get(prevTime).alt;
					double currAlt = vmap.get(currTime).alt;
					double nextAlt = vmap.get(nextTime).alt;
					if (vmap.get(currTime).vtype == PointTypeV.NONE && Math.abs(prevAlt-currAlt) < minAltDiff && Math.abs(currAlt-nextAlt) < minAltDiff) {
						vkillTimes.add(currTime);
						count++;
					}
					prevTime = currTime;
					currTime = nextTime;
				}
			}
		}
		for(int i = 0; i < vkillTimes.size(); i++) {
			vmap.remove(vkillTimes.get(i));
		}
		return count;
	}

	public int removeRedundantHorizPoints(double minDist) {
		double prevTime = -1;
		double currTime = -1;
		double nextTime = -1;
		int count = 0;
		ArrayList<Double> hkillTimes = new ArrayList<Double>();
		for(double time : allTimes()) {
			if (hmap.get(time) != null) {
				if (prevTime < 0) {
					prevTime = time;
				} else if (currTime < 0) {
					currTime = time;
				} else {
					nextTime = time;
					Position prevPos = hmap.get(prevTime).pos;
					Position currPos = hmap.get(currTime).pos;
					Position nextPos = hmap.get(nextTime).pos;

					Velocity v = prevPos.initialVelocity(nextPos, nextTime-prevTime);
					Position inline = prevPos.linear(v, currTime-prevTime);

					if (hmap.get(currTime).ttype == PointTypeT.NONE && hmap.get(currTime).gtype == PointTypeG.NONE && inline.distanceH(currPos) < minDist) {
						hkillTimes.add(currTime);
						count++;
					}
					prevTime = currTime;
					currTime = nextTime;
				}
			}
		}
		for(int i = 0; i < hkillTimes.size(); i++) {
			hmap.remove(hkillTimes.get(i));
		}
		return count;
	}
	
	/**
	 * delete any points at the given time
	 * @param time
	 */
	public void remove(double time) {
		hremove(time);
		vremove(time);
	}
	
	/**
	 * Return true if there is an explicit data point at the given time
	 * @param time
	 * @return
	 */
	public boolean hasPoint(double time) {
		return hmap.containsKey(time) || vmap.containsKey(time);
	}

	/**
	 * Return true if a point exists at the given time and it is a TCP point.
	 * @param time
	 * @return
	 */
	public boolean isTCP(double time) {
		htype h = hmap.get(time);
		vtype v = vmap.get(time);
		return (h != null && (!ttypeNone(h.ttype)  || !gtypeNone(h.gtype))) || (v != null && !vtypeNone(v.vtype));  
	}
	
	/**
	 * Return a sorted set of all defined time points
	 */
	public Set<Double>allTimes() {
		TreeSet<Double> s = new TreeSet<Double>();
		s.addAll(htimes());
		s.addAll(vtimes());
		return s;
	}

	public boolean timeValid(double time) {
		return time >= getFirstTime() && time <= getLastTime();		
	}

	public int size() {
		return allTimes().size();
	}


	/**
	 * Return a TimePoint for a defined point, or null if not on a defined point 
	 */
	public TimePoint getPoint(double time) {
		if (hmap.containsKey(time) || vmap.containsKey(time)) {
			TimePoint p = new TimePoint();
			p.pos = position(time);			
			p.vel = velocity(time);
			//f.pln(" $$$>>> TimePlan.getPoint: p.pos = "+p.pos+" p.vel = "+p.vel);
			p.label = ""; // f.Fm1(time);
			if (htimes().contains(time)) {
				p.signedRadius = hmap.get(time).signedRadius;
				p.g_accel = hmap.get(time).g_accel;
				p.t_type = hmap.get(time).ttype;
				p.g_type = hmap.get(time).gtype;
				p.h_srcPos = hmap.get(time).srcPos;
				p.h_srcTime = hmap.get(time).srcTime;
				p.label += ","+hmap.get(time).label;
			}
			if (vtimes().contains(time)) {
				p.v_accel = vmap.get(time).accel;
				p.v_type = vmap.get(time).vtype;
				p.v_srcPos = vmap.get(time).srcPos;
				p.v_srcTime = vmap.get(time).srcTime;
				p.label += ","+vmap.get(time).label;
			}
			return p;
		}
		return null;
	}
	
//	/**
//	 * Returns the closest timepoint before the given time, or null if this before the first time
//	 * @param time
//	 * @return
//	 */
//	public TimePoint getPrevPoint(double time) {
//		ArrayList<Double> at = new ArrayList(allTimes());
//		for (int i = 0; i < at.size(); i++) {
//			if (at.get(i) >= time) {
//				return getPoint(at.get(i-1));
//			}
//		}
//		return getPoint(getLastTime());
//	}
//
//	/**
//	 * Return the closest point after the given time, or null if this is after the last time
//	 * @param time
//	 * @return
//	 */
//	public TimePoint getNextPoint(double time) {
//		ArrayList<Double> at = new ArrayList(allTimes());
//		for (int i = 0; i < at.size(); i++) {
//			if (at.get(i) > time) {
//				return getPoint(at.get(i));
//			}
//		}
//		return null;		
//	}

	private boolean ttypeNone(PointTypeT ptty) {
		return ptty == PointTypeT.NONE || ptty == PointTypeT.TRKFIX;
	}

	private boolean gtypeNone(PointTypeG ptty) {
		return ptty == PointTypeG.NONE || ptty == PointTypeG.GSFIX;
	}

	private boolean vtypeNone(PointTypeV ptty) {
		return ptty == PointTypeV.NONE || ptty == PointTypeV.VSFIX;
	}

	/**
	 * add midpoints to accel zones of a complete plan
	 * @param p plan
	 * @param num max number of points
	 * @param timeBuffer min time needed to add any points
	 */
	private void addMidPoints(Plan p, int num, double timeBuffer) {
		if (num == 0) return;
		int i = 0;
		while (i < p.size()) {
			NavPoint np = p.point(i);
			if (np.isEndTCP()) {
				double t = np.time();
				int j = i-1;
				if (np.isEOT()) j = Util.min(p.prevBOT(i),j);//fixed
				if (np.isEGS()) j = Util.min(p.prevBGS(i),j);//fixed
				if (np.isEVS()) j = Util.min(p.prevBVS(i),j);//fixed
				double prevT = p.getTime(j);
				double dt = t-prevT;
				double step = dt/(1+Util.min(num, Math.floor(dt/timeBuffer)));
				if (step > 0) {
//					/f.pln(" $$## addMidPoints: num = "+num+" step = "+step+" t = "+t+" prevT+step = "+(prevT+step));
					for (double ct = prevT+step; ct + step*0.1 < t; ct = ct + step) {
						NavPoint mid = new NavPoint(p.position(ct),ct).makeLabel("AccelMidPoint");
						if (p.add(mid) > 0) {
							i++;
						}
					}
				}
			}
			i++;
		}
	}
	
	public Plan makePlan(String name) {
		Plan p = new Plan(name);
//f.pln("tplan.makePlan()="+toString());		
		for (double time : allTimes()) {
			if (Double.isNaN(time)) continue;                       // @@@@@@@@@@@@@@@ RWB @@@@@@@@@@@@@@@@@@@ ????  see 0tr10.vel.csv
			TimePoint pt = getPoint(time);
			//f.pln(" $$$ makePlan: time = "+time+" pt = "+pt);
			NavPoint np = new NavPoint(pt.pos,time);
			if ((!ttypeNone(pt.t_type) || !gtypeNone(pt.g_type)) &&  !vtypeNone(pt.v_type)) {
				f.pln("TimePlan.makeplan WARNING: point "+time+" has both horizontal and vertical component (nonfatal)");
			}
			if (!ttypeNone(pt.t_type) && !gtypeNone(pt.g_type)) {
				f.pln("TimePlan.makeplan WARNING: point "+time+" has both turn and gs component (nonfatal)");
			}

			//			if ((pt.t_type != PointTypeT.NONE || pt.g_type != PointTypeG.NONE) && !Double.isNaN(pt.h_srcPos.lat()) && !Double.isNaN(pt.h_srcTime)) {
			//				src = new NavPoint(pt.h_srcPos,pt.h_srcTime);
			//			}
			//			if (pt.v_type != PointTypeV.NONE && !Double.isNaN(pt.v_srcPos.alt()) && !Double.isNaN(pt.v_srcTime)) {
			//				src = src.mkAlt(pt.v_srcPos.alt()).makeTime(pt.v_srcTime);
			//			}

			//f.pln(" $$$$$............. TimePlan.makePlan0: np = "+np.toStringFull());
			boolean hashsrc = (!ttypeNone(pt.t_type) || !gtypeNone(pt.g_type)) && !Double.isNaN(pt.h_srcPos.lat()) && !Double.isNaN(pt.h_srcTime);
			boolean hasvsrc = !vtypeNone(pt.v_type) && !Double.isNaN(pt.v_srcPos.alt()) && !Double.isNaN(pt.v_srcTime);
			if (hashsrc && ! hasvsrc) {
				np = new NavPoint(pt.h_srcPos,pt.h_srcTime);
			} else if ( ! hashsrc && hasvsrc) {
				np = new NavPoint(pt.v_srcPos,pt.v_srcTime);
			} else if (hashsrc && hasvsrc) {
				np = new NavPoint(pt.h_srcPos,pt.h_srcTime).mkAlt(pt.v_srcPos.alt());
			}
			//f.pln(" $$$$$............... TimePlan.makePlan1: np = "+np.toStringFull());
			switch (pt.t_type) {
			case BOT: np = np.makeBOT(pt.pos, time,  pt.vel, pt.signedRadius,-1); break;
			case EOT: np = np.makeEOT(pt.pos, time, pt.vel,-1); break;
			case EOTBOT: np = np.makeEOTBOT(pt.pos, time,  pt.vel, pt.signedRadius,-1); break;
			default:
			}
			
			switch (pt.g_type) {
			case BGS: np = np.makeBGS(pt.pos, time, pt.g_accel, pt.vel,-1); break;
			case EGS: np = np.makeEGS(pt.pos, time, pt.vel,-1); break;
			case EGSBGS: np = np.makeEGSBGS(pt.pos, time, pt.g_accel, pt.vel,-1); break;
			default:
			}			
			switch (pt.v_type) {
			case BVS: {
				//f.pln(" $$$$$ TimePlan: case BVS: time = "+time+"   pt.vel (velocityIn) = "+pt.vel);  
				np = np.makeBVS(pt.pos, time, pt.v_accel, pt.vel,-1); break;
			}
			case EVS: np = np.makeEVS(pt.pos, time, pt.vel,-1); break;
			case EVSBVS: np = np.makeEVSBVS(pt.pos, time, pt.v_accel, pt.vel,-1); break;
			default:
			}
			np = np.makeLabel(pt.label.replaceAll(Constants.wsPatternBase, "_"));
			//f.pln(" $$$...TimePlan.makePlan_2: np = "+np.toStringFull());
			int index = p.add(np);
			if (index < 0) {
				f.pln("TimePlan.makePlan WARNING: Possible overlapping point at time "+time);
			}
		}
		//f.pln(" $$>> TimePlan.makePlan result: "+p.toStringShort());
		addMidPoints(p,numMidAccelPoints,5.0);
		return p;
	}

	public String toString() {
		String s = "TimePlan.toString:\n";
		s = s + "hmap = "+hmap.toString()+"\n";
		s = s + "vmap = "+vmap.toString()+"\n";
		s = s + "==================== Merged Points ====================\n";
		for (double time : allTimes()) {
			s = s + f.Fm1(time)+" : "+getPoint(time).toString()+"\n";
		}
		return s;
	}

}
