/*
 *  Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

//import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.Util.*;

class CRSSConfig {

  //public static final boolean debugPrintTime = false;         // ************* bCRSS has an option "-t" to print the time
  public final static double D_default = Units.from("NM",    5.0);
  public final static double H_default = Units.from("ft", 1000.0);
  public final static double T_default =  Units.from("s",  300.0);
  
  public static void setDefaultsForParameters(ParameterReader sRdr) {
    sRdr.getParametersRef().setInternal("D",     D_default, "NM");
    sRdr.getParametersRef().setInternal("H",     H_default, "ft");
    sRdr.getParametersRef().setInternal("T",     T_default,  "s");
  }
  
  public static void load_parameters(ParameterReader sRdr, CDSS cdss, CRSS crss) {
    crss.setDistance(sRdr.getParametersRef().getValue("D", "NM"));    // update if the parameter is in the file
    crss.setHeight(sRdr.getParametersRef().getValue("H", "ft"));      // update if the parameter is in the file
    cdss.setDistance(sRdr.getParametersRef().getValue("D", "NM"));    // update if the parameter is in the file
    cdss.setHeight(sRdr.getParametersRef().getValue("H", "ft"));      // update if the parameter is in the file

  }
}
