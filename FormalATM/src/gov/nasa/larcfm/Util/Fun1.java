/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * Generic interface for a function of type f:A->B with one parameter that has a return value.
 */
public interface Fun1<A,B> {
	public B f(A a);
}
