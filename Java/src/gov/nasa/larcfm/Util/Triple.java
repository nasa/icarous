/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/** A class representing a three member tuple */
public class Triple<L, C, R> {
    /** The leftmost element */
    public final L first;
    /** The centermost element */
    public final C second;
    /** The rightmost element */
    public final R third;



 
    /** Construct a Triple */
    public Triple(final L left, final C center, final R right) {
        this.first = left;
        this.second = center;
        this.third = right;
    }
    
    public Triple(final Pair<L,C> p, final R right) {
        this.first = p.first;
        this.second = p.second;
        this.third = right;
    }
 
    /** The left-most element */ 
    public L getLeft() {
        return first;
    }
 
    /** The center element */
    public C getCenter() {
         return second;
    }
 
    /** The rightmost element */
    public R getRight() {
         return third;
    }
    
    /** The first (or leftmost) element */
    public L getFirst() {
        return first;
    }
 
    /** The second (or center) element */
    public C getSecond() {
         return second;
    }
 
    /** The third (or rightmost) element */
    public R getThird() {
         return third;
    }
    
 
    /** Make a Triple */
    public static <L, C, R> Triple<L, C, R> make(L left, C center, R right) {
        return new Triple<L, C, R>(left, center, right);
    }
 
    /** Are these two Triples equal? */
    public final boolean equals(Object o) {
        if (!(o instanceof Triple<?,?,?>))
            return false;
 
        final Triple<?, ?, ?> other = (Triple<?,?,?>) o;
        return equal(getLeft(), other.getLeft()) 
        	&& equal(getCenter(), other.getCenter())
        	&& equal(getRight(), other.getRight());
    }
    
    /** Are these two Triples equal? */
    private static final boolean equal(Object o1, Object o2) {
        if (o1 == null) {
            return o2 == null;
        }
        return o1.equals(o2);
    }
 
    
    public String toString() {
    	return "("+first.toString()+"; "+second.toString()+"; "+third.toString()+")";
    }
    
    public int hashCode()
    {
        int hLeft = getLeft() == null ? 0 : getLeft().hashCode();
        int hCenter = getCenter() == null ? 0 : getCenter().hashCode();
        int hRight = getRight() == null ? 0 : getRight().hashCode();
        
        return (hLeft*37 + hCenter)*37 + hRight;
    }
}
