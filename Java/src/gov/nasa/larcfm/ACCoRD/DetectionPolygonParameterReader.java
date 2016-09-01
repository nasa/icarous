/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Triple;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * This class contains static methods to read in DetectionPolygon instance definitions from a ParameterData object in a format consistent with a ParameterData object created by DetectionPolygonParameterWriter.
 */
public class DetectionPolygonParameterReader {

	/**
	 * This parses parameters involved with DetectionPolygon objects (if present) and returns a list of DetectionPolygon objects loaded, and the ones chosen by set_det_polygon_detection and set_res_polygon_detection, 
	 * if successfully specified.
	 * 
	 * Specifically this looks for parameters:
	 * load_polygon_detection_XXX : create a DetectionPolygon instance of the specified class with label XXX
	 * XXX_* : parameters associated with object with label XXX
	 * set_det_polygon_detection : instance for return.second
	 * set_res_polygon_detection : instance for return.third
	 * 
	 * The user still needs to assign these to the appropriate object(s).
	 * If no alternates are loaded, return the empty list for the first part of the pair. 
	 * If no polygon detection is set, return null for the second part of the pair.
	 * @param params
	 * @return triple of all DetectionPolygon objects loaded, the "det" DetectionPolygon object, and the "res" DetectionPolygon object
	 * Note that the "det" and "res" (if defined) will be references to list entries.
	 * 
	 * If verbose is false (default true), suppress all status messages except exceptions
	 */
	public static Triple<ArrayList<DetectionPolygon>,DetectionPolygon,DetectionPolygon> readPolygonDetection(ParameterData params) {
		return readPolygonDetection(params,false);
	}

	public static Triple<ArrayList<DetectionPolygon>,DetectionPolygon,DetectionPolygon> readPolygonDetection(ParameterData params, boolean verbose) {
		ArrayList<DetectionPolygon> cdlist = new ArrayList<DetectionPolygon>();
		DetectionPolygon chosenD = null;
		DetectionPolygon chosenR = null;
		List<String> mlist = params.matchList("load_polygon_detection_");
		Collections.sort(mlist);
		for (String pname : mlist) {
			String instanceName = pname.substring(23);
			String dname = params.getString(pname);
			try {
				Object obj = Class.forName(dname).newInstance();
				if (obj instanceof DetectionPolygon) {
					DetectionPolygon d = (DetectionPolygon) obj;
					if (verbose) System.out.println(">>>>> Polygon detection "+dname+" ("+instanceName+") loaded <<<<<");
					ParameterData instpd = params.extractPrefix(instanceName+"_");
					if (instpd.size() > 0) {
						d.setParameters(instpd);
						if (verbose) System.out.println(">>>>> Polygon detection parameters for "+instanceName+" set <<<<<");
					}
					// set instance identifier if it was not already set explicitly as a parameter
					if (d.getIdentifier().equals("")) {
						d.setIdentifier(instanceName);
					}
					if (params.contains("set_det_polygon_detection") && params.getString("set_det_polygon_detection").equalsIgnoreCase(instanceName)) {
						chosenD = d;
						//            if (verbose) System.out.println(">>>>> Polygon detection det set to "+instanceName+" <<<<<");

					}
					if (params.contains("set_res_polygon_detection") && params.getString("set_res_polygon_detection").equalsIgnoreCase(instanceName)) {
						chosenR = d;
						//            if (verbose) System.out.println(">>>>> Polygon detection res set to "+instanceName+" <<<<<");
					}
					cdlist.add(d);
				} else {
					throw new InstantiationException("Error: "+dname+" is not a known instance of DetectionPolygon");
				}
			} catch (ClassNotFoundException e) {
				System.out.println("Error loading polygon detection: Class "+dname+" not found on classpath");
			} catch (Exception e) {
				System.out.println("Error loading polygon detection: "+e);                 
			}
		}
		if (params.contains("set_det_polygon_detection")  && chosenD == null) {
			if (verbose) System.out.println(">>>>> Polygon detection "+params.getString("set_det_polygon_detection")+" does not appear to be loaded, det cannot be set <<<<<");
		}
		if (params.contains("set_res_polygon_detection")  && chosenR == null) {
			if (verbose) System.out.println(">>>>> Polygon detection "+params.getString("set_res_polygon_detection")+" does not appear to be loaded, res cannot be set <<<<<");
		}
		return new Triple<ArrayList<DetectionPolygon>,DetectionPolygon,DetectionPolygon>(cdlist,chosenD,chosenR);
	} 

}
