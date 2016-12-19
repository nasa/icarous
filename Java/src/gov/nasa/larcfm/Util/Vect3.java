/*
 * Vect3.java 
 * 
 * 3-D vectors.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.Arrays;

/**
 * 3-Dimensional mathematical vectors.
 */
public class Vect3 {
  
  /** A zero vector */
  public static final Vect3 ZERO = new Vect3();

  /** An invalid Vect3.  Note that this is not necessarily equal to other invalid Vect3s -- use the isInvalid() test instead. */
  public static final Vect3 INVALID = new Vect3(Double.NaN,Double.NaN,Double.NaN);

  /** The x-component */
  public final double x;

  /** The y-component */
  public final double y;

  /** The z-component */
  public final double z;

  /**
   * Creates a zero vector.
   */
  private Vect3() {
    this(0.0,0.0,0.0);
  }

  /**
   * Creates a vector that is an extension of v
   */
  public Vect3(Vect2 v, double z) {
    this(v.x,v.y,z);
  }

  /**
   * Creates a new vector with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
   * 
   * @param x Real value [internal units]
   * @param y Real value [internal units]
   * @param z Real value [internal units]
   */
  public Vect3(double x, double y, double z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }

  /**
   * Creates a new vector with coordinates (<code>x</code>,<code>y</code>,<code>z</code>) in specified units.
   * 
   * @param x  Real value [ux]
   * @param ux Units x
   * @param y  Real value [uy]
   * @param ux Units y
   * @param z  Real value [uz]
   * @param uz Units z
   */
  public static Vect3 makeXYZ(double x, String ux, double y, String uy, double z, String uz) {
    return new Vect3(Units.from(ux,x),Units.from(uy,y),Units.from(uz,z));
  }

  /**
   * Creates a new vector with coordinates (<code>x</code>,<code>y</code>,<code>z</code>) in internal units.
   * 
   * @param x Real value [internal units]
   * @param y Real value [internal units]
   * @param z Real value [internal units]
   */
  public static Vect3 mkXYZ(double x, double y, double z) {
    return new Vect3(x,y,z);
  }

  public Vect3 mkX(double nx) {
    return mkXYZ(nx, y ,z);
  }

  public Vect3 mkY(double ny) {
    return mkXYZ(x, ny ,z);
  }

  public Vect3 mkZ(double nz) {
    return mkXYZ(x, y , nz);
  }

  /** The x coordinate */
  public double x() {
    return x;
  }

  /** The y coordinate */
  public double y() {
    return y;
  }

  /** The z coordinate */
  public double z() {
    return z;
  }

  /**
   * Zero constant.
   */
  public static Vect3 Zero() {
    return new Vect3();
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    long temp;
    temp = Double.doubleToLongBits(x);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(y);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(z);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    //if (getClass() != obj.getClass())
    if (!(obj instanceof Vect3))
      return false;
    Vect3 other = (Vect3) obj;
    if (Double.doubleToLongBits(x) != Double.doubleToLongBits(other.x))
      return false;
    if (Double.doubleToLongBits(y) != Double.doubleToLongBits(other.y))
      return false;
    if (Double.doubleToLongBits(z) != Double.doubleToLongBits(other.z))
      return false;
    return true;
  }

  /**
   * Checks if vector is zero.
   * 
   * @return <code>true</code>, if <code>this</code> vector is zero.
   */
  public boolean isZero() {
    return x == 0.0 && y == 0.0 && z == 0.0;
  }

  /**
   * Checks if vectors are almost equal.
   * 
   * @param v Vector
   * 
   * @return <code>true</code>, if <code>this</code> vector is almost equal 
   * to <code>v</code>.
   */
  public boolean almostEquals(Vect3 v) {
    return Util.almost_equals(x,v.x) && Util.almost_equals(y,v.y) && 
        Util.almost_equals(z,v.z);
  }

  /**
   * Checks if vectors are almost equal.
   * 
   * @param v Vector
   * @param maxUlps unit of least precision
   * 
   * @return <code>true</code>, if <code>this</code> vector is almost equal 
   * to <code>v</code>.
   */
  public boolean almostEquals(Vect3 v, long maxUlps) {
    return Util.almost_equals(x, v.x, maxUlps) && Util.almost_equals(y, v.y, maxUlps) && 
        Util.almost_equals(z, v.z, maxUlps);
  }

  public boolean within_epsilon(Vect3 v2, double epsilon) {
    //f.pln(" $$$ Vect3.within_epsilon: v1 = "+(new Vect3(x,y,z))+ " v2 = "+v2+" epsilon = "+epsilon );
    //f.pln(" $$$ Vect3.within_epsilon: z = "+z+" v2.z = "+v2.z);
    if (Math.abs(x - v2.x) > epsilon) return false;
    if (Math.abs(y - v2.y) > epsilon) return false;
    if (Math.abs(z - v2.z) > epsilon) return false;
    //f.pln(" $$$ Vect3.within_epsilon: return true");
    return true;  
  }

  /**
   * Dot product.
   * 
   * @param v Vector
   * 
   * @return the dot product of <code>this</code> vector and <code>v</code>.
   */
  public double dot(Vect3 v) {
    return dot(v.x,v.y,v.z);
  }

  /**
   * Dot product.
   * 
   * @param x Real value
   * @param y Real value
   * @param z Real value
   * 
   * @return the dot product of <code>this</code> vector and (<code>x</code>,<code>y</code>,<code>z</code>).
   */
  public double dot(double x, double y, double z) {
    return this.x*x + this.y*y + this.z*z;
  }

  /**
   * Square.
   * 
   * @return the dot product of <code>this</code> vector with itself.
   */
  public double sqv() {
    return dot(x,y,z); // dot product of this vector with itself
  }

  /**
   * Norm.
   * 
   * @return the norm of of <code>this</code> vector.
   */
  public double norm() {
    return Util.sqrt_safe(sqv());
  }

  /**
   * Make a unit vector from the current vector.  If it is a zero vector, then a copy is returned.
   * @return the unit vector
   */
  public Vect3 Hat() {
    double n = norm();
    if (n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
      return this;
    }
    return new Vect3(x/n, y/n, z/n);
  }

  public Vect3 Hat2D() {
	  return mkZ(0.0).Hat();
  }
  
  /**
   * Cross product.
   * 
   * @param v Vector
   * 
   * @return the cross product of <code>this</code> vector and <code>v</code>.
   */
  public Vect3 cross(Vect3 v) {
    return new Vect3(this.y*v.z - this.z*v.y, this.z*v.x - this.x*v.z, this.x*v.y - this.y*v.x);
  }

  public boolean parallel(Vect3 v) {
    return cross(v).almostEquals(Vect3.ZERO); 
  }

  /**
   * 2-Dimensional projection.
   *
   * @return the 2-dimensional projection of <code>this</code>.
   */
  public Vect2 vect2() {
    return new Vect2(x,y);
  }

  /**
   * Vector addition.
   * 
   * @param v Vector
   * 
   * @return the vector addition of <code>this</code> vector and <code>v</code>.
   */
  public Vect3 Add(Vect3 v) {
    return new Vect3(x+v.x, y+v.y, z+v.z);
  }

  /**
   * Vector subtraction.
   * 
   * @param v Vector
   * 
   * @return the vector subtraction of <code>this</code> vector and <code>v</code>.
   */
  public Vect3 Sub(Vect3 v) {
    return new Vect3(x-v.x, y-v.y, z-v.z);
  }

  /**
   * Vector negation.
   * 
   * @return the vector negation of of <code>this</code> vector.
   */
  public Vect3 Neg() {
    return new Vect3(-x,-y,-z);
  }

  /**
   * Scalar multiplication.
   * 
   * @param k Real value
   * 
   * @return the vector scalar multiplication of <code>this</code> vector and <code>k</code>.
   */
  public Vect3 Scal(double k) {
    return new Vect3(k*x, k*y, k*z);
  }

  /**
   * Scalar and addition multiplication. Compute: k*<code>this</code> + v
   * 
   * @param k Real value
   * @param v Vector
   * 
   * @return the scalar multiplication <code>this</code> vector and <code>k</code>, followed by an
   * addition to vector <code>v</code>.
   */
  public Vect3 ScalAdd(double k, Vect3 v) {
    return new Vect3(k*x+v.x, k*y+v.y, k*z+v.z);
  }

  /**
   * Addition and scalar multiplication.  Compute: this + k*<code>v</code>;
   * 
   * @param k real value
   * @param v vector
   * 
   * @return the addition of <code>this</code> vector to <code>v</code> scaled by <code>k</code>.
   */
  public Vect3 AddScal(double k, Vect3 v) {
    return new Vect3(x+k*v.x, y+k*v.y, z+k*v.z);
  }

  /**
   * Right perpendicular, z-component set to 0
   * 
   * @return the right perpendicular of <code>this</code> vector, i.e., (<code>y</code>, <code>-x</code>).
   */
  public Vect3 PerpR() {
    return new Vect3(y,-x,0);
  }

  /**
   * Left perpendicular, z-component set to 0
   * 
   * @return the left perpendicular of <code>this</code> vector, i.e., (<code>-y</code>, <code>x</code>).
   */
  public Vect3 PerpL() {
    return new Vect3(-y,x,0);
  }

  /**
   * Calculates position after t time units in direction and magnitude of velocity v
   * @param v    velocity
   * @param t    time
   * @return the new position
   */
  public Vect3 linear(Vect3 v, double t) {
    return new Vect3(x+v.x*t, y+v.y*t, z+v.z*t);
  }
  
  /**
   * Calculates position after t time units in direction and magnitude of velocity v
   * @param track  
   * @param t       time
   * @return the new position (horizontal only)
   */
  public Vect3 linearByDist(double track, double d) {
	double anySpeed = 100;
	Velocity v = Velocity.mkTrkGsVs(track,anySpeed,0.0);
	double dt = d/anySpeed;
	return linear(v,dt);
  }


  /** 3-D time of closest point of approach 
   * if time is negative or velocities are parallel returns 0
   */
  public static double tcpa (Vect3 so, Vect3 vo, Vect3 si, Vect3 vi) {
    double t;
    Vect3 s = so.Sub(si);
    Vect3 v = vo.Sub(vi);
    double nv = v.sqv();
    if (nv > 0) 
      t = Util.max(0,-s.dot(v)/nv);
    else 
      t = 0;
    return t;
  }// tcpa

  /** 3-D distance at time of closest point of approach
   **/
  public static double dcpa(Vect3 so, Vect3 vo, Vect3 si, Vect3 vi) {
    double t = tcpa(so,vo,si,vi);
    Vect3 s = so.Sub(si);
    Vect3 v = vo.Sub(vi);
    Vect3 st = s.AddScal(t,v);
    return st.norm();
  }// dcpa

  /**
   * Returns true if the current vector has an "invalid" value
   */
  public boolean isInvalid() {
    return Double.isNaN(x) || Double.isNaN(y) || Double.isNaN(z);
  }

  /**
   * Cylindrical norm.
   * @param d Radius of cylinder
   * @param h Half-height of cylinder
   * @return the cylindrical distance of <code>this</code>. The cylindrical distance is
   * 1 when <code>this</code> is at the boundaries of the cylinder. 
   */
  public double cyl_norm(double d, double h) {
    return Util.max(vect2().sqv()/Util.sq(d),Util.sq(z/h));
  }

  /**
   * Compare two vectors: return true iff delta is within specified limits 
   * 
   * @param v a Vect3
   * @param maxX
   * @param maxY
   * @param maxZ
   * @return true iff each component of the vector is within the given bound.
   */
  public boolean compare(Vect3 v, double maxX, double maxY, double maxZ) {
    if (Math.abs(v.x - x) > maxX) return false;
    if (Math.abs(v.y - y) > maxY) return false;
    if (Math.abs(v.z - z) > maxZ) return false;
    return true;
  }

  /** The horizontal distance between this vector and the given vector, essentially same as v.Sub(w).vect2().norm() */
  public double distanceH(Vect3 w) {
    Vect2 v = new Vect2(x,y);
    return v.Sub(w.vect2()).norm(); 
  }

  /** The vertical distance between this vector and the given vector, essentially same as v.z - w.z */
  public double distanceV(Vect3 w) {
    return z - w.z;
  }

  /** A string representation of this vector */
  public String toString() {
    return toString(Constants.get_output_precision());
  }

	/** A string representation of this vector */
  public String toString(int precision) {
    return formatXYZ(precision,"(",", ",")");
  }

  public String toStringNP(String xunit, String yunit, String zunit, int prec) {
    return f.FmPrecision(Units.to(xunit, x), prec) + ", " + f.FmPrecision(Units.to(yunit, y), prec) + ", " 	+ f.FmPrecision(Units.to(zunit, z), prec);
  }

  public String formatXYZ(int prec, String pre, String mid, String post) {
    return pre+f.FmPrecision(x,prec)+mid+f.FmPrecision(y,prec)+mid+f.FmPrecision(z,prec)+post;
  }

  public String toPVS(int prec) {
    return "(# x:= "+f.FmPrecision(x,prec)+", y:= "+f.FmPrecision(y,prec)+", z:= "+f.FmPrecision(z,prec)+" #)";
  }

  /** 
   * This parses a space or comma-separated string as a Vect3 (an inverse to the toString method).  If three 
   * bare values are present, then it is interpreted as internal units.
   * If there are 3 value/unit pairs then each values is interpreted wrt the appropriate unit.  If the string 
   * cannot be parsed, an INVALID value is returned. 
   * */
  public static Vect3 parse(String str) {
    String[] fields = str.split(Constants.wsPatternParens);
    if (fields[0].equals("")) {
      fields = Arrays.copyOfRange(fields,1,fields.length);
    }
    try {
      if (fields.length == 3) {
        return new Vect3(Double.parseDouble(fields[0]),Double.parseDouble(fields[1]),Double.parseDouble(fields[2]));
      } else if (fields.length == 6) {
        return new Vect3(Units.from(Units.clean(fields[1]),Double.parseDouble(fields[0])),
            Units.from(Units.clean(fields[3]),Double.parseDouble(fields[2])),
            Units.from(Units.clean(fields[5]),Double.parseDouble(fields[4])));
      }
    } catch (Exception e) {}
    return Vect3.INVALID;
  }

}
