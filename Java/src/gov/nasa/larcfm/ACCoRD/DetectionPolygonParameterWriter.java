/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.f;

import java.util.HashSet;
import java.util.List;

/**
 * This class contains static methods to create ParameterData objects that contain information about DetectionPolygon 
 * instance definitions in a format consistent with DetectionPolygonParameterReader.
 */
public class DetectionPolygonParameterWriter {


	/**
	 * Return a ParameterData suitable to be read by readPolygonDetection() based on the supplied DetectionPolygon instances. 
	 * @param dlist list of DetectionPolygon instances -- this may be empty.  det and res instances will be automatically added to the list (if they are not already in it)
	 * @param det detection instance -- this may be null
	 * @param res resolution instance -- this may be null
	 * Note: this may modify the instance identifiers if they are not already unique.
	 */
	public static ParameterData writePolygonDetection(List<DetectionPolygon> dlist, DetectionPolygon det, DetectionPolygon res) {
		return writePolygonDetection(dlist, det, res, false);
	}
	
	/**
	 * Return a ParameterData suitable to be read by readPolygonDetection() based on the supplied DetectionPolygon instances. 
	 * @param dlist list of DetectionPolygon instances -- this may be empty.  det and res instances will be automatically added to the list (if they are not already in it)
	 * @param det detection instance -- this may be null
	 * @param res resolution instance -- this may be null
	 * @param ordered true to modify detection identifiers to ensure they retain the input list's ordering when decoded, false will only modify identifiers if they are not unique
	 */
	public static ParameterData writePolygonDetection(List<DetectionPolygon> dlist, DetectionPolygon det, DetectionPolygon res, boolean ordered) {
		// make sure det and res are in the list, if necessary
		if (det != null && !dlist.contains(det)) {
			dlist.add(det);
		}
		if (res != null && !dlist.contains(res)) {
			dlist.add(res);
		}
		ParameterData p = new ParameterData();
		HashSet<String> names = new HashSet<String>();
		int counter = 1;
		for (DetectionPolygon cd : dlist) {
			//make sure each instance has a unique name
			if (ordered) {
				if (cd.getIdentifier().equals("")) {
					cd.setIdentifier("polydet_"+f.FmLead(counter, 4)+"_"+cd.getClassName());
				} else {
					cd.setIdentifier("polydet_"+f.FmLead(counter, 4)+"_"+cd.getIdentifier());
				}
				counter++;
			} else {
				while (cd.getIdentifier().equals("") || names.contains(cd.getIdentifier())) {
					cd.setIdentifier(cd.getClassName()+"_instance_"+counter);
					counter++;
				}
			}
			String id = cd.getIdentifier();
			names.add(id);
			p.set("load_polygon_detection_"+id+" = "+cd.getClassName());
			p.copy(cd.getParameters().copyWithPrefix(id+"_"),true);
		}
		if (det != null) { 
			p.set("set_det_polygon_detection = "+det.getIdentifier());
		}
		if (res != null) { 
			p.set("set_res_polygon_detection = "+res.getIdentifier());
		}
		return p;
	}
}
