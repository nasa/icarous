/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

/**
 * An interface for any object that is able to use a Detection3D object
 */
public interface Detection3DResAcceptor {
  /**
   * Apply a deep copy of this Detection3D object to this object at the lowest resolution level.
   */
  public void setResDetection(Detection3D cd);
  
  /**
   * Retrieve a reference to this object's associated Detection3D resolution object. 
   */
  public Detection3D getResDetection();
  
}
