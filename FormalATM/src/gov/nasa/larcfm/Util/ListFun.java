/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

/**
 * A generic class that implements some functional list operations.
 * This relies on the Fun1 and Fun2 interfaces.
 * @param <A> Type of elements of input list
 * @param <B> Type of elements of output list or accumulated result.
 */
public class ListFun {
	
	/**
	 * Apply function fun to each element in list as and add the result to the new returned list 
	 * @param fun class containing the 1-parameter function f to apply
	 * @param as starting list
	 */
	public static <A,B> List<B> map(Fun1<A,B> fun, List<A> as) {
		ArrayList<B> bs = new ArrayList<B>();
		for(A a : as) {
			bs.add(fun.f(a));
		}
		return bs;
	}
	
	/**
	 * Fold left function fun across list as starting with base value b.
	 * @param fun class containing the 2-parameter function f to apply
	 * @param b starting value
	 * @param as list to traverse
	 * @return accumulated values of f(...f(f(f(b,a0),a1),a2)...,an)
	 */
	public static <A,B> B fold(Fun2<B,A,B> fun, B b, List<A> as) {
		for(A a: as) {
			b = fun.f(b,a);
		}
		return b;
	}
}
