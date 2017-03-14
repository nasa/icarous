/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

/**
 * An interface for any object that is able to use a Detection3D object
 */
public interface DetectionPolygonResAcceptor {
  /**
   * Apply a deep copy of this Detection3D object to this object at the lowest level.
   */
  public void setResPolygonDetection(DetectionPolygon cd);
  
  /**
   * Retrieve a reference to this object's associated Detection3D object. 
   */
  public DetectionPolygon getResPolygonDetection();
  
}
