/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.List;

import gov.nasa.larcfm.Util.ConfigReader;
import gov.nasa.larcfm.Util.GeneralSequenceReader;
import gov.nasa.larcfm.Util.GeneralState;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.f;

public class Detection_batch {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		ParameterData argParams = new ParameterData();
		List<String> list = argParams.parseArguments("-", args);

		if (argParams.contains("configfile")) {
			ConfigReader cfgRdr = new ConfigReader();
			System.out.println("Reading config file "+argParams.getString("configfile"));
			cfgRdr.open(argParams.getString("configfile"));
			argParams = cfgRdr.getParameters();
		}

		if (args.length > 0 && list.size() > 0) {
			for (String file : list) {
				System.out.println("--- File: "+file+" start ---");
				GeneralSequenceReader reader = new GeneralSequenceReader();
				reader.setWindowSize(1);
				reader.open(file);
				ParameterData fileParams = reader.getParameters();

				// accumulate parameters for this run
				ParameterData pd = new ParameterData();
				pd.copy(argParams, true);
				pd.copy(fileParams, true);
				CDSSGeneral cdss = new CDSSGeneral();

				// set up all readers
				ArrayList<Detection3D> coreList = DetectionParameterReader.readCoreDetection(pd).first;
				ArrayList<DetectionPolygon> polyList = DetectionParameterReader.readPolygonDetection(pd).first;
				if (coreList.size() == 0) {
					coreList.add(new CDCylinder());
				}
				if (polyList.size() == 0) {
					polyList.add(new CDPolyIter());
				}

				String ownship = reader.getDefaultOwnship();
				if (pd.contains("ownship")) {
					ownship = pd.getString("ownship");
				}

				boolean doDetection = true;
				boolean doViolation = true;
				if (pd.contains("onlyViolations")) {
					doDetection = !pd.getBool("onlyViolations");
				}
				if (pd.contains("onlyConflicts")) {
					doViolation = !pd.getBool("onlyConflicts");
				}

				double B = 0;
				double T = 300;
				if (pd.contains("B")) {
					B = pd.getValue("B");
				}
				if (pd.contains("T")) {
					T = pd.getValue("T");
				}

				System.out.println("Parameters: "+pd);
				for (Detection3D cd : coreList) {
					System.out.println("Detection3D: "+cd);
				}				
				for (DetectionPolygon cd : polyList) {
					System.out.println("DetectionPolygon: "+cd);
				}				
				System.out.println("--- Data start ---");

				for (double time : reader.sequenceKeys()) {
					reader.setActive(time);
					GeneralState own = reader.getSequenceGeneralState(ownship, time);
					if (!own.isInvalid()) {
						for (GeneralState gs : reader.getGeneralStateList()) {
							if (gs.getTime() == time) {
								String line = time+": "+own.getName()+" vs "+gs.getName()+" :";
								boolean println = false;
								boolean vio = false;
								boolean con = false;
								double tin = 0;
								double tout = -1;
								String detstr = "";
								if (!gs.getName().equals(own.getName())) {
									if (gs.hasPointMass()) {
										for (Detection3D cd : coreList) {
											cdss.setCoreDetection(cd);
											vio = cdss.violation(own, gs);
											con = cdss.detection(own, gs, B, T);
											tin = cdss.getTimeIn();
											tout = cdss.getTimeOut();
											String myStr = "";
											if (doViolation && vio) {
												println = true;
												myStr = " violation";
											}
											if (doDetection && con) {
												println = true;
												myStr += " conflict ("+tin+" to "+tout+")";
											}
											if (myStr.length() > 0) {
												detstr += " ["+cd.getIdentifier()+" "+myStr+"]";
											}
										}
									} else if (gs.hasPolygon()) {
										for (DetectionPolygon cd : polyList) {
											cdss.setCorePolygonDetection(cd);
											vio = cdss.violation(own, gs);
											con = cdss.detection(own, gs, B, T);
											tin = cdss.getTimeIn();
											tout = cdss.getTimeOut();
											String myStr = "";
											if (doViolation && vio) {
												println = true;
												myStr = " violation";
											}
											if (doDetection && con) {
												println = true;
												myStr += " conflict ("+tin+" to "+tout+")";
											}
											if (myStr.length() > 0) {
												detstr += " ["+cd.getIdentifier()+" "+myStr+"]";
											}
										}
									}
								}
								line += detstr;

								if (println) {
									System.out.println(line);
								}
							}
						}
					}
				}


				if (reader.hasError()) {
					System.out.println(reader.getMessage());
				}
				System.out.println("--- File: "+file+" done ---");
			}
		} else {
			System.out.println("Pairwise detection program for input files (both aircraft & polygons).");
			System.out.println("Usage: Detection_batch [options] <files>");
			System.out.println("  [options] are parameters preceeed by -, e.g.: -D=5.0[nmi] to assign D to 5 mni");
			System.out.println("  options are as per DetectionParameterReader.java:");
			System.out.println("  Specifically this looks for parameters in the forms:");
			System.out.println("    load_core_detection_XXX, XXX_*, load_polygon_detection_XXX, ");
			System.out.println("    ownship, onlyViolations, onlyConflicts, B, T");
			System.out.println("  All loaded detectors will be run pairwise against the ownship and all traffic");
			System.out.println("  at each time point for which there is data.  If not specified, ownship if first aircraft.");
			System.out.println("  Parameters may also be specified in the input file (these only apply to that file) or via:");
			System.out.println("  -configfile=<filename>");
		}
	}
}
