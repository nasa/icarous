/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/** A class representing a two member tuple */
public class Pair<L, R> {
    public final L first;
    public final R second;
 
    public Pair(final L left, final R right) {
        this.first = left;
        this.second = right;
    }
 
    /** The first (or leftmost) element */
    public L getFirst() {
        return first;
    }
 
    /** The first (or leftmost) element */
    public R getSecond() {
         return second;
    }
 
    /** Make a Pair */
    public static <L, R> Pair<L, R> make(L left, R right) {
        return new Pair<L, R>(left, right);
    }
 
    public final boolean equals(Object o) {
        if (!(o instanceof Pair<?,?>))
            return false;
 
        final Pair<?, ?> other = (Pair<?,?>) o;
        return equal(first, other.first) 
        	&& equal(second, other.second);
    }
    
    private static final boolean equal(Object o1, Object o2) {
        if (o1 == null) {
            return o2 == null;
        }
        return o1.equals(o2);
    }
     
    public int hashCode()
    {
        int hLeft = first == null ? 0 : first.hashCode();
        int hRight = second == null ? 0 : second.hashCode();
        
        return hLeft*37 + hRight;
    }
    
    public String toString() {
    	return "("+first.toString()+", "+second.toString()+")";
    }
}

