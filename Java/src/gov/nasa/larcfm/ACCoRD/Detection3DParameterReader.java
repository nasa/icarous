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
 * This class contains static methods to read in Detection3D instance definitions from a ParameterData object in a format consistent with a ParameterData object created by DetectionParameterWriter.
 */
public class Detection3DParameterReader {

	/**
	 * This parses parameters involved with Detection3D objects (if present) and returns a list of Detection3D objects loaded, and the one chosen by set_det_core_detection and set_res_core_detection, 
	 * if successfully specified.  The list will be sorted by the instance identifiers.
	 * 
	 * Specifically this looks for parameters:
	 * load_core_detection_XXX : create a Detection3D instance of the specified class with label XXX
	 * XXX_* : parameters associated with object with label XXX
	 * set_det_core_detection : instance for return.second
	 * set_res_core_detection : instance for return.third
	 * 
	 * The user still needs to assign these to the appropriate object(s).
	 * If no alternates are loaded, return the empty list for the first part of the pair. 
	 * If no det or res core detection is set, return null for the second and/or third part of the pair.
	 * @param params
	 * @return triple of all Detection3D objects loaded, the "det" Detection3D object, and the "res" Detection3D object
	 * Note that the "det" and "res" (if defined) will be references to list entries.
	 * 
	 * If verbose is false (default true), suppress all status messages except exceptions
	 */
	public static Triple<ArrayList<Detection3D>,Detection3D,Detection3D> readCoreDetection(ParameterData params) {
		return readCoreDetection(params,false);
	}

	public static Triple<ArrayList<Detection3D>,Detection3D,Detection3D> readCoreDetection(ParameterData params, boolean verbose) {
		ArrayList<Detection3D> cdlist = new ArrayList<Detection3D>();
		Detection3D chosenD = null;
		Detection3D chosenR = null;
		List<String> mlist = params.matchList("load_core_detection_");
		Collections.sort(mlist);
		for (String pname : mlist) {
			String instanceName = pname.substring(20);
			String dname = params.getString(pname);
			try {
				Object obj = Class.forName(dname).newInstance();
				if (obj instanceof Detection3D) {
					Detection3D d = (Detection3D) obj;
					if (verbose) System.out.println(">>>>> Core detection "+dname+" ("+instanceName+") loaded <<<<<");
					ParameterData instpd = params.extractPrefix(instanceName+"_");
					if (instpd.size() > 0) {
						d.setParameters(instpd);
						if (verbose) System.out.println(">>>>> Core detection parameters for "+instanceName+" set <<<<<");
					}
					// set instance identifier if it was not already set explicitly as a parameter
					if (d.getIdentifier().equals("")) {
						d.setIdentifier(instanceName);
					}
					if (params.contains("set_det_core_detection") && params.getString("set_det_core_detection").equalsIgnoreCase(instanceName)) {
						chosenD = d;
						//            if (verbose) System.out.println(">>>>> Core detection det set to "+pname+" <<<<<");

					}
					if (params.contains("set_res_core_detection") && params.getString("set_res_core_detection").equalsIgnoreCase(instanceName)) {
						chosenR = d;
						//            if (verbose) System.out.println(">>>>> Core detection res set to "+pname+" <<<<<");
					}
					cdlist.add(d);
				} else {
					throw new InstantiationException("Error: "+dname+" is not a known instance of Detection3D");
				}
			} catch (ClassNotFoundException e) {
				System.out.println("Error loading core detection: Class "+dname+" not found on classpath");
			} catch (Exception e) {
				System.out.println("Error loading core detection: "+e);                 
			}
		}
		if (params.contains("set_det_core_detection")  && chosenD == null) {
			if (verbose) System.out.println(">>>>> Core detection "+params.getString("set_det_core_detection")+" does not appear to be loaded, det cannot be set <<<<<");
		}
		if (params.contains("set_res_core_detection")  && chosenR == null) {
			if (verbose) System.out.println(">>>>> Core detection "+params.getString("set_res_core_detection")+" does not appear to be loaded, res cannot be set <<<<<");
		}
		return new Triple<ArrayList<Detection3D>,Detection3D,Detection3D>(cdlist,chosenD,chosenR);
	}

}
