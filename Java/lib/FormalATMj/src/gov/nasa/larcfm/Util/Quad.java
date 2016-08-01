/*
 * Quad.java
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 *
 * a four element tuple
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/** A 4-dimensional tuple */
public class Quad<T1, T2, T3, T4> {
    public final T1 first;
    public final T2 second;
    public final T3 third;
    public final T4 fourth;
 
    /** Construct a new Quad object */
    public Quad(final T1 t1, final T2 t2, final T3 t3, final T4 t4) {
        this.first = t1;
        this.second = t2;
        this.third = t3;
        this.fourth = t4;
    }
 
    /** Return the first element of this Quad */
    public T1 getFirst() {
        return first;
    }
 
    /** Return the second element of this Quad */
    public T2 getSecond() {
        return second;
    }
 
    /** Return the third element of this Quad */
    public T3 getThird() {
        return third;
    }
 
    /** Return the fourth element of this Quad */
    public T4 getFourth() {
        return fourth;
    }
 
    /** Make a new Quad object */
    public static <T1, T2, T3, T4> Quad<T1, T2, T3, T4> make(T1 t1, T2 t2, T3 t3, T4 t4) {
        return new Quad<T1, T2, T3, T4>(t1, t2, t3, t4);
    }
 
    /** Are these two Quad objects equal? */
    public final boolean equals(Object o) {
        if (!(o instanceof Quad<?, ?,?,?>))
            return false;
 
        final Quad<?, ?, ?, ?> other = (Quad<?,?,?,?>) o;
        return equal(getFirst(), other.getFirst()) 
        	&& equal(getSecond(), other.getSecond())
        	&& equal(getThird(), other.getThird())
        	&& equal(getFourth(), other.getFourth());
    }
    
    /** Are these two Quad objects equal? */
    private static final boolean equal(Object o1, Object o2) {
        if (o1 == null) {
            return o2 == null;
        }
        return o1.equals(o2);
    }
 
    public String toString() {
    	return "("+first.toString()+", "+second.toString()+", "+third.toString()+", "+fourth.toString()+")";
    }
    
    
    
    public int hashCode()
    {
        int h1 = getFirst() == null ? 0 : getFirst().hashCode();
        int h2 = getSecond() == null ? 0 : getSecond().hashCode();
        int h3 = getThird() == null ? 0 : getThird().hashCode();
        int h4 = getFourth() == null ? 0 : getFourth().hashCode();
        
        return ((h1*37 + h2)*37 + h3)*37 + h4;
    }
}