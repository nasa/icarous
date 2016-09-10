/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ParameterAcceptor;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Optional;

/** 
 * It is assumed that for all alert level i >= 1: detection(i+1) => detection(i) and that there is at least one
 * guidance level whose region is different from NONE. 
 */

public class AlertLevels implements ParameterAcceptor {

	public List<AlertThresholds> alertor_; 
	public int conflict_level_;

	public AlertLevels() {
		alertor_ = new ArrayList<AlertThresholds>();
		conflict_level_ = 0;
	}

	public AlertLevels(AlertLevels alertor) {
		alertor_ = new ArrayList<AlertThresholds>();
		copy(alertor);
	}

	/** 
	 * @return alerting thresholds (unbuffered) as defined by SC-228 MOPS.
	 * Maneuver guidance logic produces multilevel bands:
	 * MID: Corrective
	 * NEAR: Warning
	 * Well-clear volume (unbuffered) is defined by alerting level 2 (conflict_alerting_level)  
	 * 
	 */
	static public AlertLevels WC_SC_228_Thresholds() {
		WCVTable preventive = new WCVTable();
		preventive.setDTHR(0.66,"nmi");
		preventive.setZTHR(700,"ft");
		preventive.setTTHR(35);
		preventive.setTCOA(0);

		WCVTable corrective = new WCVTable();
		corrective.setDTHR(0.66,"nmi");
		corrective.setZTHR(450,"ft");
		corrective.setTTHR(35);
		corrective.setTCOA(0);

		WCVTable warning = new WCVTable();
		warning.setDTHR(0.66,"nmi");
		warning.setZTHR(450,"ft");
		warning.setTTHR(35);
		warning.setTCOA(0);

		AlertLevels alertor = new AlertLevels();
		alertor.setConflictAlertLevel(2);
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(preventive),55,75,BandsRegion.NONE)); 
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(corrective),55,75,BandsRegion.MID)); 
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(warning),25,55,BandsRegion.NEAR));

		return alertor;
	}

	/** 
	 * @return alerting thresholds (unbuffered) as defined by SC-228 MOPS.
	 * Maneuver guidance logic produces single level bands:
	 * NEAR: Corrective
	 * Well-clear volume (unbuffered) is defined by alerting level 2 (conflict_alerting_level)  
	 * 
	 */
	static public AlertLevels SingleBands_WC_SC_228_Thresholds() {
		WCVTable preventive = new WCVTable();
		preventive.setDTHR(0.66,"nmi");
		preventive.setZTHR(700,"ft");
		preventive.setTTHR(35);
		preventive.setTCOA(0);

		WCVTable corrective = new WCVTable();
		corrective.setDTHR(0.66,"nmi");
		corrective.setZTHR(450,"ft");
		corrective.setTTHR(35);
		corrective.setTCOA(0);

		WCVTable warning = new WCVTable();
		warning.setDTHR(0.66,"nmi");
		warning.setZTHR(450,"ft");
		warning.setTTHR(35);
		warning.setTCOA(0);

		AlertLevels alertor = new AlertLevels();
		alertor.setConflictAlertLevel(2);
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(preventive),55,75,BandsRegion.NONE)); 
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(corrective),55,75,BandsRegion.MID)); 
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(warning),25,55,BandsRegion.NONE));

		return alertor;
	}

	/** 
	 * @return alerting thresholds (buffered) as defined by SC-228 MOPS.
	 * Maneuver guidance logic produces multilevel bands:
	 * MID: Corrective
	 * NEAR: Warning
	 * Well-clear volume (buffered) is defined by alerting level 2 (conflict_alerting_level)  
	 * 
	 */
	static public AlertLevels Buffered_WC_SC_228_Thresholds() {
		WCVTable preventive = new WCVTable();
		preventive.setDTHR(1.0,"nmi");
		preventive.setZTHR(750,"ft");
		preventive.setTTHR(35);
		preventive.setTCOA(20);

		WCVTable corrective = new WCVTable();
		corrective.setDTHR(1.0,"nmi");
		corrective.setZTHR(450,"ft");
		corrective.setTTHR(35);
		corrective.setTCOA(20);

		WCVTable warning = new WCVTable();
		warning.setDTHR(1.0,"nmi");
		warning.setZTHR(450,"ft");
		warning.setTTHR(35);
		warning.setTCOA(20);

		AlertLevels alertor = new AlertLevels();
		alertor.setConflictAlertLevel(2);
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(preventive),60,75,BandsRegion.NONE));
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(corrective),60,75,BandsRegion.MID));
		alertor.addLevel(new AlertThresholds(new WCV_TAUMOD(warning),30,55,BandsRegion.NEAR));

		return alertor;
	}

	/** 
	 * @return alerting thresholds for single bands given by detector,
	 * alerting time, and lookahead time. 
	 */
	static public AlertLevels SingleBands(Detection3D detector, 
			double alerting_time, double lookahead_time) {
		AlertLevels alertor = new AlertLevels();
		alertor.setConflictAlertLevel(1);
		alertor.addLevel(new AlertThresholds(detector,
				alerting_time,lookahead_time,BandsRegion.NEAR));
		return alertor;
	}
	
	/** 
	 * Clears alert levels
	 **/
	public void clear() {
		alertor_.clear();
		conflict_level_ = 0;
	}

	/**
	 * @return numerical type of most severe alert level.
	 */
	public int mostSevereAlertLevel() {
		return alertor_.size();
	}

	/**
	 * @return first guidance level whose region is different from NONE
	 */   
	public int firstGuidanceLevel() {  
		for (int i=0; i < alertor_.size(); ++i) {
			if (alertor_.get(i).getRegion().isConflictBand()) {
				return i+1;
			}
		}
		return 0;
	}

	/**
	 * @return last guidance level whose region is different from NONE
	 */   
	public int lastGuidanceLevel() {  
		for (int i=alertor_.size(); i > 0; --i) {
			if (alertor_.get(i-1).getRegion().isConflictBand()) {
				return i;
			}
		}
		return 0;
	}

	/**
	 * @return conflict alert level. When the numerical is value is 0, first guidance level
	 */   
	public int conflictAlertLevel() {  
		if (1 <= conflict_level_ && conflict_level_ <= mostSevereAlertLevel()) {
			return conflict_level_;
		} else {
			return firstGuidanceLevel();
		}
	}

	/**
	 * @return detector for given alert level
	 */   
	public Optional<Detection3D> detector(int alert_level) {
		if (alert_level == 0) {
			alert_level = conflictAlertLevel();
		}
		if (alert_level > 0) {
			return Optional.of(alertor_.get(alert_level-1).getDetector());
		} else {
			return Detection3D.NoDetector;
		}
	}

	/**
	 * @return conflict detector for conflict alert level  
	 */   
	public Optional<Detection3D> conflictDetector() {
		return detector(0);
	}

	/**
	 * Set conflict alert level
	 */   
	public void setConflictAlertLevel(int alert_level) {
		if (0 <= alert_level) {
			conflict_level_ = alert_level; 
		}
	}

	/**
	 * Set the threshold values of a given alert level. 
	 */
	public void setLevel(int level, AlertThresholds thresholds) {
		if (1 <= level && level <= alertor_.size()) {
			alertor_.set(level-1,new AlertThresholds(thresholds));
		} 
	}

	/**
	 * Add an alert level and returns its numerical type, which is a positive number.
	 */
	public int addLevel(AlertThresholds thresholds) {
		alertor_.add(new AlertThresholds(thresholds));
		return alertor_.size();
	}

	/** 
	 * @return threshold values of a given alert level
	 */
	public AlertThresholds getLevel(int level) {
		if (1 <= level && level <= alertor_.size()) {
			return alertor_.get(level-1);
		} else {
			return AlertThresholds.INVALID;
		}
	}

	/**
	 * Copy alertor to the same values as the given parameter.
	 * Detectors are deeply copied.
	 */
	public void copy(AlertLevels alertor) {
		alertor_.clear();
		conflict_level_ = alertor.conflict_level_;
		for (int i=1; i <= alertor.mostSevereAlertLevel(); ++i) {
			alertor_.add(new AlertThresholds(alertor.getLevel(i)));
		}
	}

	@Override
	public ParameterData getParameters() {
		ParameterData p = new ParameterData();
		updateParameterData(p);
		return p;
	}

	@Override
	public void updateParameterData(ParameterData p) {
		p.set("conflict_level",f.Fmi(conflict_level_));
		// get list of detectors that are in the alerts
		List<Detection3D> dlist = new ArrayList<Detection3D>();
		List<String> idlist = new ArrayList<String>(); 
		for (int i = 0; i < alertor_.size(); i++) {
			Detection3D det = alertor_.get(i).getDetector();
			det.setIdentifier("det_"+f.Fmi(i+1));
			idlist.add(det.getIdentifier()+"_id");
			dlist.add(det);
		}
		// create the base parameterdata object storing the detector information
		// this also ensures they each have a unique identifier
		ParameterData pdmain = Detection3DParameterWriter.writeCoreDetection(dlist, null, null);
		// remove detection3d identifier keys from pdmain
		for (String idkey:idlist) {
			pdmain.remove(idkey);
		}
		int precision = Constants.get_output_precision();
		// add parameters for each alerter, ensuring they have an ordered set of identifiers
		for (int i = 1; i <= mostSevereAlertLevel(); i++) {
			ParameterData pd = new ParameterData();
			pd.set("detector", getLevel(i).getDetector().getIdentifier());
			pd.setInternal("alerting_time", getLevel(i).getAlertingTime(), "s", precision);
			pd.setInternal("early_alerting_time", getLevel(i).getEarlyAlertingTime(), "s", precision);
			pd.set("region", getLevel(i).getRegion().name());
			pd.setInternal("spread_trk", getLevel(i).getTrackSpread(), "deg", precision);
			pd.setInternal("spread_gs", getLevel(i).getGroundSpeedSpread(), "knot", precision);
			pd.setInternal("spread_vs", getLevel(i).getVerticalSpeedSpread(), "fpm", precision);
			pd.setInternal("spread_alt", getLevel(i).getAltitudeSpread(), "ft", precision);
			//make sure each instance has a unique, ordered name
			String prefix = "alert_"+f.Fmi(i)+"_";
			pdmain.copy(pd.copyWithPrefix(prefix), true);
		}
		p.copy(pdmain, true);
	}

	@Override
	public void setParameters(ParameterData p) {
		if (p.contains("conflict_level")) conflict_level_ = p.getInt("conflict_level");
		// read in all detector information
		List<Detection3D> dlist = Detection3DParameterReader.readCoreDetection(p).first;
		// put in map for easy lookup
		HashMap<String,Detection3D> dmap = new HashMap<String,Detection3D>();
		dlist.stream().forEach(x -> dmap.put(x.getIdentifier(),x));
		// extract parameters for each alertlevel:
		int counter = 1;
		String prefix = "alert_"+f.Fmi(counter)+"_";
		ParameterData pdsub = p.extractPrefix(prefix);
		if (pdsub.size() > 0) {
			alertor_.clear();
		}
		while (pdsub.size() > 0) {
			// build the alertlevel
			Detection3D det = dmap.get(pdsub.getString("detector"));
			double alertingTime = pdsub.getValue("alerting_time");
			double earlyAlertingTime = pdsub.getValue("early_alerting_time");
			BandsRegion br = BandsRegion.valueOf(pdsub.getString("region"));
			AlertThresholds al = new AlertThresholds(det,alertingTime,earlyAlertingTime,br);
			al.setTrackSpread(pdsub.getValue("spread_trk"));
			al.setGroundSpeedSpread(pdsub.getValue("spread_gs"));
			al.setVerticalSpeedSpread(pdsub.getValue("spread_vs"));
			al.setAltitudeSpread(pdsub.getValue("spread_alt"));
			// modify or add the alertlevel (this cannot remove levels)
			if (counter <= alertor_.size()) {
				setLevel(counter,al);
			} else {
				addLevel(al);
			}
			// next set
			counter++;
			prefix = "alert_"+f.Fmi(counter)+"_";
			pdsub = p.extractPrefix(prefix);
		}
	}
	
	public String toString() {
		String s = "Conflict Level: "+conflict_level_+" ("+conflictAlertLevel()+")\n";
		for (int i=0; i < alertor_.size(); ++i) {
			s += "Level "+(i+1)+": "+alertor_.get(i).toString()+"\n";
		}
		return s;
	}

	public String toPVS(int prec) {
		String s = "("+conflictAlertLevel()+", (: ";
		boolean first = true;
		for (int i=0; i < alertor_.size(); ++i) {
			if (first) {
				first = false;
			} else {
				s += ",";
			}
			s += alertor_.get(i).toPVS(prec);
		}
		return s+" :))";
	}

}
