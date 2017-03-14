/*
 * Tuple5.java
 *
 * Contact: Aaron Dutle (aaron.m.dutle@nasa.gov)
 * NASA LaRC
 *
 * a five element tuple
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/** A 5-dimensional tuple */
public class Tuple5<T1, T2, T3, T4, T5> {
    public final T1 first;
    public final T2 second;
    public final T3 third;
    public final T4 fourth;
    public final T5 fifth;
   
 
    /** Construct a new Tuple5 object */
    public Tuple5(final T1 t1, final T2 t2, final T3 t3, final T4 t4, final T5 t5) {
        this.first = t1;
        this.second = t2;
        this.third = t3;
        this.fourth = t4;
        this.fifth = t5;
    }
 
    /** Return the first element of this Tuple5 */
    public T1 getFirst() {
        return first;
    }
 
    /** Return the second element of this Tuple5 */
    public T2 getSecond() {
        return second;
    }
 
    /** Return the third element of this Tuple5 */
    public T3 getThird() {
        return third;
    }
 
    /** Return the fourth element of this Tuple5 */
    public T4 getFourth() {
        return fourth;
    }
 
    /** Return the fifth element of this Tuple5 */
    public T5 getFifth() {
        return fifth;
    }
    /** Make a new Tuple5 object */
    public static <T1, T2, T3, T4, T5> Tuple5<T1, T2, T3, T4, T5> make(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
        return new Tuple5<T1, T2, T3, T4, T5>(t1, t2, t3, t4, t5);
    }
 
    /** Are these two Tuple5 objects equal? */
    public final boolean equals(Object o) {
        if (!(o instanceof Tuple5<?,?,?,?,?>))
            return false;
 
        final Tuple5<?, ?, ?, ?, ?> other = (Tuple5<?,?,?,?,?>) o;
        return equal(getFirst(), other.getFirst()) 
        	&& equal(getSecond(), other.getSecond())
        	&& equal(getThird(), other.getThird())
        	&& equal(getFourth(), other.getFourth())
        	&& equal(getFifth(), other.getFifth());
    }
    
    /** Are these two Tuple5 objects equal? */
    private static final boolean equal(Object o1, Object o2) {
        if (o1 == null) {
            return o2 == null;
        }
        return o1.equals(o2);
    }
 
    public String toString() {
    	return "("+first.toString()+","+second.toString()+","+third.toString()+","+fourth.toString()+","+fifth.toString()+")";
    }
    
    
    
    public int hashCode()
    {
        int h1 = getFirst() == null ? 0 : getFirst().hashCode();
        int h2 = getSecond() == null ? 0 : getSecond().hashCode();
        int h3 = getThird() == null ? 0 : getThird().hashCode();
        int h4 = getFourth() == null ? 0 : getFourth().hashCode();
        int h5 = getFifth() == null ? 0 : getFifth().hashCode();
        
        return (((h1*37 + h2)*37 + h3)*37 + h4)*37+h5;
    }
}