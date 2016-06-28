/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterAcceptor;

public interface ParameterTable extends ParameterAcceptor {
  public ParameterTable copy();
  
  public ParameterData getParameters();
  public void updateParameterData(ParameterData p);
  public void setParameters(ParameterData p);
  
  public String toString();
}
