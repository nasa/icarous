/*
 * Vect2.java 
 * 
 * 2-D vectors.
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/**
 * 2-Dimensional mathematical vectors.
 */
public /*final*/ class Vect2 {

  /** The x-component */
  public final double  x;

  /** The y-component */
  public final double  y;

  public static final Vect2 ZERO = new Vect2();

  /** An invalid Vect2.  Note that this is not necessarily equal to other invalid Vect2s -- use the isInvalid() test instead. */
  public static final Vect2 INVALID = new Vect2(Double.NaN,Double.NaN);

  /**
   * Creates a zero vector.
   */
  private Vect2() {
    this(0.0,0.0);
  }

  /**
   * Creates a new vector with coordinates (<code>x</code>,<code>y</code>).
   * 
   * @param x Real value
   * @param y Real value
   */
  public Vect2(double x, double y) {
    this.x = x;
    this.y = y;
  }

  /**
   * Zero constant.
   * @return zero vector
   */
  public static Vect2 Zero() {
    return new Vect2();
  }

  /** The x component 
   * @return x component
   * */
  public double x() {
    return x;
  }

  /** the y component 
   * @return y component
   * */
  public double y() {
    return y;
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
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (getClass() != obj.getClass())
      return false;
    Vect2 other = (Vect2) obj;
    if (Double.doubleToLongBits(x) != Double.doubleToLongBits(other.x))
      return false;
    if (Double.doubleToLongBits(y) != Double.doubleToLongBits(other.y))
      return false;
    return true;
  }

  /**
   * Checks if vector is zero.
   * 
   * @return <code>true</code>, if <code>this</code> vector is zero.
   */
  public boolean isZero() {
    return x == 0.0 && y == 0.0;
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
  public boolean almostEquals(Vect2 v, long maxUlps) {
    return Util.almost_equals(x, v.x, maxUlps) && Util.almost_equals(y, v.y, maxUlps);
  }

  /**
   * Checks if vectors are almost equal.
   * 
   * @param v Vector
   * 
   * @return <code>true</code>, if <code>this</code> vector is almost equal 
   * to <code>v</code>.
   */
  public boolean almostEquals(Vect2 v) {
    return Util.almost_equals(x,v.x) && Util.almost_equals(y,v.y);
  }




  /**
   * Dot product.
   * 
   * @param v Vector
   * 
   * @return the dot product of <code>this</code> vector and <code>v</code>.
   */
  public double dot(Vect2 v) {
    return dot(v.x,v.y);
  }

  /**
   * Dot product.
   * 
   * @param x Real value
   * @param y Real value
   * 
   * @return the dot product of <code>this</code> vector and (<code>x</code>,<code>y</code>).
   */
  public double dot(double x, double y) {
    return this.x*x + this.y*y;
  }

  /**
   * Determinant.
   * 
   * @param v Vector
   * 
   * @return the determinant of <code>this</code> vector and <code>v</code>.
   */
  public double det(Vect2 v) {
    return det(v.x,v.y);
  }

  /**
   * Determinant.
   * 
   * @param x Real value
   * @param y Real value
   * 
   * @return the determinant of <code>this</code> vector and (<code>x</code>,<code>y</code>).
   */
  public double det(double x, double y) {
    return this.x*y - this.y*x;
  }


  /**
   * Square.
   * 
   * @return the dot product of <code>this</code> vector with itself.
   */
  public double sqv() {
    //This implementation
    //return dot(x,y);
    //was replaced with this implementation
    return x*x+y*y; 
    //for performance reasons.
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
   * Angle.
   * 
   * @return the angle of <code>this</code> vector in the range 
   * (-<code>pi</code>, <code>pi</code>]. Convention is counter-clockwise 
   * with respect to east.
   */
  public double angle() {
    return Util.atan2_safe(y, x);
  }

  /**
   * Track angle.
   * 
   * @return the track angle of <code>this</code> vector in the range 
   * (-<code>pi</code>, <code>pi</code>]. The track angle is the clockwise angle
   * with respect to north, i.e., <code>v.track() = pi/2 - v.angle()</code>.
   */
  public double trk() {
    return Util.atan2_safe(x,y);
  }

  /**
   * Compass angle.
   * 
   * @return the track angle of <code>this</code> vector in the range 
   * [<code>0</code>, <code>2*pi</code>). Convention is clockwise with respect to north.
   */
  public double compassAngle() {
    return Util.to_2pi(trk());
  }

  /**
   * Compass angle from this position to v2
   * 
   * @param v2 vector
   * @return the track angle of <code>v2-this</code> vector in the range 
   * [<code>0</code>, <code>2*Math.PI</code>). Convention is clockwise with respect to north.
   */
  public double compassAngle(Vect2 v2) {
    return v2.Sub(this).compassAngle();
  }

  /**
   * Construct a Vect2 from a track angle and ground speed.  This is
   * a vector of length gs pointed in the direction of track angle of trk.
   *
   * @param trk    track angle
   * @param gs     ground speed
   * @return a vector in with the given length and with the direction of track angle
   *
   */
  public static Vect2 mkTrkGs(double trk, double gs) {
    return new Vect2(gs*Math.sin(trk),gs*Math.cos(trk));
  }

  /**
   * Construct a Vect2 from the math angle and length.  
   *
   * @param length length of the vector
   * @param angle a mathematical angle (0 degrees is to the right, angles increase counter-clockwise)
   * @return a vector in with the given length and with the direction of "angle"
   */
  public static Vect2 mkLengthAngle(double length, double angle) {
    return new Vect2(length*Math.cos(angle),length*Math.sin(angle));
  }

  /**
   * Determine if vop is to the right or to the left of vo.
   * Returns 1 if vop is to the right, -1 if vop is to the left.
   * 
   * @param vo   first vector
   * @param vop  second vector
   * @return 1 if vop is to the right, -1 if vop is to the left
   */
  public static int right_or_left(Vect2 vo, Vect2 vop) {
    return Util.sign(vop.det(vo));
  }  

  private double sqRel(Vect2 v) {
    return Util.sq(x-v.x) + Util.sq(y-v.y);
  }

  /**
   * Relative less or equal.
   * 
   * @param v  Vector
   * @param vo Vector
   * 
   * @return <code>true</code>, if the norm of <code>Sub(vo)</code> is less or equal than 
   * the norm of <code>v.Sub(vo)</code>.
   */
  public boolean leq(Vect2 v, Vect2 vo) {
    return sqRel(vo) <= v.sqRel(vo);
  }

  /**
   * Make a unit vector from the current vector.  If it is a zero vector, then a copy is returned.
   * @return the unit vector
   */
  public Vect2 Hat() {
    //Vect2 v = new Vect2(this);
    //v.hat();
    //return v;
    double n = norm();
    if ( n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
      return ZERO;
    }
    return new Vect2(x/n, y/n);
  }

  /**
   * 3-Dimensional extension.
   * 
   * @param z Real value
   * 
   * @return the 3-dimensional extension of <code>this</code> with <code>z</code>.
   */
  public Vect3 vect3(double z) {
    return new Vect3(x,y,z);
  }

  /**
   * Vector addition.
   * 
   * @param v Vector
   * 
   * @return the vector addition of <code>this</code> vector and <code>v</code>.
   */
  public Vect2 Add(Vect2 v) {
    //    Vect2 u = new Vect2(x,y);
    //    u.add(v);
    //    return u;
    return new Vect2(x+v.x,y+v.y);
  }

  /**
   * Vector subtraction.
   * 
   * @param v Vector
   * 
   * @return the vector subtraction of <code>this</code> vector and <code>v</code>.
   */
  public Vect2 Sub(Vect2 v) {
    //    Vect2 u = new Vect2(x,y);
    //    u.sub(v);
    //    return u;
    return new Vect2(x-v.x,y-v.y);
  }

  /**
   * Vector negation.
   * 
   * @return the vector negation of of <code>this</code> vector.
   */
  public Vect2 Neg() {
    return new Vect2(-x,-y);
  }

  /**
   * Scalar multiplication.
   * 
   * @param k Real value
   * 
   * @return the vector scalar multiplication of <code>this</code> vector and <code>k</code>.
   */
  public Vect2 Scal(double k) {
    //    Vect2 u = new Vect2(x,y);
    //    u.scal(k);
    //    return u;
    return new Vect2(k*x,k*y);
  }

  /**
   * Scalar and addition multiplication.  Compute: k*<code>this</code> + v
   * 
   * @param k real value
   * @param v vector
   * 
   * @return the scalar multiplication <code>this</code> vector and <code>k</code>, followed by an
   * addition to vector <code>v</code>.
   */
  public Vect2 ScalAdd(double k, Vect2 v) {
    //    Vect2 u = new Vect2(x,y);
    //    u.scal(k);
    //    u.add(v);
    //    return u;
    return new Vect2(k*x+v.x,k*y+v.y);
  }

  /**
   * Addition and scalar multiplication.  Compute: this + k*<code>v</code>;
   * 
   * @param k real value
   * @param v vector
   * 
   * @return the addition of <code>this</code> vector to <code>v</code> scaled by <code>k</code>.
   */
  public Vect2 AddScal(double k, Vect2 v) {
    //Vect2 u = new Vect2(x,y);
    //    Vect2 vv = new Vect2(v.x,v.y);
    //    vv.scal(k);
    //    vv.add(this);
    //    return vv;
    return new Vect2(x+k*v.x,y+k*v.y);
  }

  /**
   * Right perpendicular.
   * 
   * @return the right perpendicular of <code>this</code> vector, i.e., (<code>y</code>, <code>-x</code>).
   */
  public Vect2 PerpR() {
    return new Vect2(y,-x);
  }

  /**
   * Left perpendicular.
   * 
   * @return the left perpendicular of <code>this</code> vector, i.e., (<code>-y</code>, <code>x</code>).
   */
  public Vect2 PerpL() {
    return new Vect2(-y,x);
  }

	/**
	 * Calculates position after t time units in direction and magnitude of velocity v
	 * @param v    velocity
	 * @param t    time
	 * @return the new position
	 */
  public Vect2 linear(Vect2 v, double t) {
    return new Vect2(x + v.x*t,y + v.y*t);
  }

  public double distance(Vect2 s) {
    return   Util.sqrt_safe(Util.sq(s.x - x) + Util.sq(s.y - y));
  }

  /**
   * Returns true if the current vector has an "invalid" value
   * @return true if invalid
   */
  public boolean isInvalid() {
    return Double.isNaN(x) || Double.isNaN(y);
  }

  /** 
   * Return actual time of closest point approach (return negative infinity if parallel)
   * 
   * @param so position of ownship
   * @param vo velocity of ownship
   * @param si position of intruder
   * @param vi velocity of intruder
   * @return time of closest point of approach
   */
  public static double actual_tcpa (Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
    double rtn;
    Vect2 s = so.Sub(si);
    Vect2 v = vo.Sub(vi);
    double nv = v.norm();
    if (nv > 0) {
      rtn = -s.dot(v)/(nv*nv);
    } else {
      rtn = Double.NEGATIVE_INFINITY;
    }
    return rtn;
  }

  /** Return time to closest point of approach 
   * if time is negative or velocities are parallel returns 0
   * 
   * @param so position of ownship
   * @param vo velocity of ownship
   * @param si position of intruder
   * @param vi velocity of intruder
   * @return time of closest point of approach
   */
  public static double tcpa(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
    double t;
    Vect2 s = so.Sub(si);
    Vect2 v = vo.Sub(vi);
    double nv = v.sqv();
    if (nv > 0) 
      t = Util.max(0,-s.dot(v)/nv);
    else 
      t = 0;
    return t;
  }

  /** distance at time of closest point of approach
   * 
   * @param so position of ownship
   * @param vo velocity of ownship
   * @param si position of intruder
   * @param vi velocity of intruder
   * @return distance at time of closest point of approach
   */
  public static double dcpa(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
    double t = tcpa(so,vo,si,vi);
    Vect2 s = so.Sub(si);
    Vect2 v = vo.Sub(vi);
    Vect2 st = s.AddScal(t,v);
    return st.norm();
  }

  /**
   * distance at time of closest approach within time bound [0,T]
   * 
   * @param so position of ownship
   * @param vo velocity of ownship
   * @param si position of intruder
   * @param vi velocity of intruder
   * @param T time bound
   * @return distance at time of closest point of approach
   */
  public static double dcpa (Vect2 so, Vect2 vo, Vect2 si, Vect2 vi, double T) {
    double t = Util.min(tcpa(so,vo,si,vi),T);
    Vect2 s = so.Sub(si);
    Vect2 v = vo.Sub(vi);
    Vect2 st = s.AddScal(t,v);
    return st.norm();
  }

  /**
   * returns the perpendicular distance between line defined by s,v and point q.
   * @param s point on line
   * @param v direction vector of line
   * @param q a point not on the line
   * @return perpendicular distance
   */
  public static double distPerp(Vect2 s, Vect2 v, Vect2 q) {
    double tp = q.Sub(s).dot(v)/v.sqv();
    return s.Add(v.Scal(tp)).Sub(q).norm();
  }

  /**
   * returns the longitudinal distance between line defined by s,v and point q.
   * @param s point on line
   * @param v direction vector of line
   * @param q a point not on the line
   * @return longitudinal distance
   */
  public static double distAlong(Vect2 s, Vect2 v, Vect2 q) {
    double tp = q.Sub(s).dot(v)/v.sqv();
    //return s.Add(v.Scal(tp)).Sub(s).norm();
    //f.pln(" $$$ distAlong: tp = "+tp);
    return Util.sign(tp)*v.Scal(tp).norm();
  }


  /**
   * Computes intersection point of two lines
   * @param s0 starting point of line 1
   * @param v0 direction vector for line 1
   * @param s1 starting point of line 2
   * @param v1 direction vector of line 2
   * @return 2-dimensional point of intersection, or an invalid point if they are parallel
   */
  public static Vect2 intersect_pt(Vect2 s0, Vect2 v0, Vect2 s1, Vect2 v1) {
    if (Util.almost_equals(v0.det(v1),0.0)) { // parallel: no intersection point
      //		  f.pln(" Vect2.intersect_pt: $$$$$$$$ ERROR $$$$$$$$$");
      return Vect2.INVALID;
    } else {
      Vect2 delta = s1.Sub(s0);
      double ss = delta.det(v1)/v0.det(v1);
      return s0.Add(v0.Scal(ss));
    }
  }

  public static Vect2 midPoint (Vect2 v1, Vect2 v2) {
    return new Vect2((v1.x + v2.x)/2, (v1.y+v2.y)/2    );
  }

  /** A string representation of this vector */
  public String toString() {
    return toString(Constants.get_output_precision());
  }

	/** A string representation of this vector 
	 * @param precision number of digits of precision
	 * @return string representation
	 * */
  public String toString(int precision) {
    return formatXY(precision,"(",", ",")");
  }

  public String toStringNP(String xunit, String yunit, int prec) {
    return f.FmPrecision(Units.to(xunit, x), prec) + ", " + f.FmPrecision(Units.to(yunit, y), prec);
  } 

  public String formatXY(int prec, String pre, String mid, String post) {
    return pre+f.FmPrecision(x,prec)+mid+f.FmPrecision(y,prec)+post;
  }

  public String toPVS(int prec) {
    return "(# x:= "+f.FmPrecision(x,prec)+", y:= "+f.FmPrecision(y,prec)+" #)";
  }

  /**
   * Returns true if x components and y components of both vectors are within the given bounds
   * @param v2
   * @param epsilon
   * @return true if vector is within bounds
   */
  public boolean within_epsilon(Vect2 v2, double epsilon) {
    if (Math.abs(x - v2.x) > epsilon) return false;
    if (Math.abs(y - v2.y) > epsilon) return false;
    return true;  
  }

}
