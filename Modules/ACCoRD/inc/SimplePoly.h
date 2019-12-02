/*
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef SIMPLEPOLY_H_
#define SIMPLEPOLY_H_

#include <vector>
#include <string>
#include "Position.h"
#include "EuclideanProjection.h"
#include "BoundingRectangle.h"
#include "ErrorLog.h"
#include "Poly3D.h"
#include "Poly3DLL.h"

namespace larcfm {

/**
 * <p>A basic polygon that describes a volume.  This volume has a flat bottom and top 
 * (specified as altitude values).  Points describe the cross-section area vertices in
 * a consistently clockwise (or consistently counter-clockwise) manner.  The cross-section
 * need not be convex, but an "out of order" listing of the vertices will result in edges 
 * that cross, and will cause several calculations to fail (with no warning).</p>
 * 
 * <p>A SimplePoly sets the altitude for all its points to be the _bottom_ altitude,
 * while the top is stored elsewhere as a single value.  The exact position for "top"
 * vertices is computed on demand.</p>
 * 
 * <p>The cross-section must be a simple polygon, that is it allows for non-convex areas, but
 * vertices and edges may not overlap or cross.  Vertices may be ordered in either a clockwise
 * or counterclockwise manner.</p>
 * 
 * <p>(A vertex-complete polygon allows for vertices and edges to overlap but not cross, while
 * a general polygon allows for edges to cross.)</p>
 * 
 * <p>Point indices are based on the order they are added.</p>
 * 
 * Note: polygon support is experimental and the interface is subject to change!
 *
 */
class SimplePoly {
private:
	bool isLatLon_b;
	Poly3D p3d;
	Poly3DLL p3dll;
//	mutable bool boundingCircleDefined; // = false;
	mutable bool centroidDefined; // = false;
	mutable bool averagePointDefined; // = false;
	Position cPos;         // cached centroid
//	Position bPos;         // cached boundingCircle
	Position aPos;         // cached averagePoint


	mutable double clockwiseSum; // sum of all angles in order (measured in a clockwise manner), if (approx) 2PI then it is clockwise, , if (approx) -2PI, then counterclockwise;

	static const bool move_constant_shape;
	//static const double nullAlt;// = -MAXDOUBLE;

    void init();
    
    void calcCentroid() const;

    /**
	 * Return the area (in m^2 or rad^2) of this SimplePoly.
	 */
	double signedArea(double dx, double dy) const;
	
  public:

	/**
	 * Constructs an empty SimplePoly.
	 */
	SimplePoly();

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude [m]
	 * @param t Top altitude [m]
	 */
	SimplePoly(double b, double t);

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top altitude
	 * @param units units to interpret both altitudes
	 */
	SimplePoly(double b, double t, const std::string& units);

	/**
	 * Create a deep copy of a SimplePoly
	 * 
	 * @param p Source poly.
	 */
	SimplePoly(const SimplePoly& p);



	bool equals(const SimplePoly& p) const;

	/**
	 * Returns the position of the (bottom) point with index n.  This returns an INVALID point if the index is invalid, not the centroid.
	 * 
	 * NOTE: The altitude component of the nth position is ambiguous.  This method returns
	 * the bottom altitude of the 3D object.
	 * 
	 * @param n index
	 * @return position
	 */
	Position getVertex(int n) const;

	std::vector<Position> getVertices() const;

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
	 * @param p3 3D polygon
	 * @return a new SimplePoly
	 */
    static SimplePoly make(const Poly3D& p3);

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
	 * @param p3 3D polygon
	 * @return a new SimplePoly
	 */
    static SimplePoly make(const Poly3DLL& p3);


	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates .
	 * @param p3 3D polygon
	 * @param proj projection to use to interpret the points in the 3D polygon
	 * @return a new SimplePoly
	 */
    static SimplePoly make(const Poly3D& p3, const EuclideanProjection& proj);


	/**
	 * Create a SimplePoly from a List&lt;LatLonAlt&gt;. 
	 * @param pList list of points
	 * @param b bottom altitude
	 * @param t top altitude
	 * @return a new SimplePoly
	 */
	static SimplePoly make(std::vector<LatLonAlt> pList, double b, double t);

	/**
	 * Create a SimplePoly from a List&lt;Position&gt;. 
	 * @param pList list of points
	 * @param b bottom altitude
	 * @param t top altitude
	 * @return a new SimplePoly
	 */
	static SimplePoly mk(std::vector<Position> pList, double b, double t);

	/** return a Poly3D version of this.  proj is ignored if this is Euclidean
	 * @param proj projection
	 * @return a new 3D polygon
	 */
	Poly3D poly3D(const EuclideanProjection& proj) const;


	static std::vector<LatLonAlt> rip(const SimplePoly& sPoly);

	/**
	 * Determine if the polygon contains geodetic points. 
	 * @return True if this SimplePoly contains any geodetic points. If size is 0, returns false.
	 */
	bool isLatLon() const;

    bool isClockwise() const;

    void reverseOrder();

	/**
	 * closest horizontal distance from p to an edge.  
	 * @param p position
	 * @return distance from edge
	 */
	double distanceFromEdge(const Position& p) const;

	/**
	 * return vertical distance from the polygon.  This will be zero if the point is between the polygon's top and bottom.
	 * @param p position
	 * @return distance
	 */
	double distanceV(const Position& p) const;

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * @param p position
	 * @return true, if position is in the polygon
	 */
	bool contains(const Position& p);

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 *
	 * @param p position
	 * @return true, if position is in the polygon
	 */
	bool contains2D(const Position& p);


	/**
	 * Number of points
	 * @return Number of points in this SimplePoly
	 */
	int size() const;

	/**
	 * Add vertices to a polygon until it has a specified number.  This will not remove vertices if there are already more than the desired number.  
	 * Each new vertex will be added the to current longest edge.
	 * @param sz desired number of vertices
	 */
	void increaseVertices(int sz);

		
	/**
	 * Return this centroid of this volume.
	 * Note: if sides are small (less than about 10^-5 in internal units), there may be errors in the centroid calculations
	 * @return The centroid position of this volume.
	 */
	Position centroid() ;
	
  private:

	Position avgPos(const std::vector<Position>& points, const std::vector<double>& wgts) ;

  public:

	/**
	 * Area in square meters.
	 * @return area
	 */
	double area();

	/**
	 * Return the average of all vertices.  Note this is not the same as the centroid!  This will, however, have the nice property of having
	 * a constant velocity when dealing with a morphing polygon. 
	 * @return average of all vertices
	 */
	Position averagePoint()  ;

    double apBoundingRadius();


    /**
     * Returns the center of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
     * @return center of polygon
     */
    Position boundingCircleCenter();

    /** Returns true if this polygon is convex
     * @return true, if convex
     */
    bool isConvex() const;

    /**
     * Returns the radius of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
     * @return radius of bounding circle
     */
    double boundingCircleRadius();

    Position maxDistPair(const Position& p) const;

    /**
     * Returns the max horizontal distance between any vertex and the centroid
     * @return radius
     */
    double maxRadius();

    bool insert(int i, const Position& p);

    /**
     * Add a new point to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner.
     * This currently does NOT set the Z component of the point (unless it is the first point)
     * @param p position to add
     * @return false, if an error is detected (duplicate or invalid vertex), true otherwise.
     */
    bool add(const Position& p);
    bool add(const LatLonAlt& p);
	/** 
	 * Add a new point to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner. 
	 * This currently does NOT set the Z component of the point (unless it is the first point)
	 * @param p position to add
	 * @return false, if an error is detected (duplicate or invalid vertex), true otherwise. 
	 */
    bool add(const Vect2& p);


    /**
     * Remove a point from this SimplePoly.
     * @param n Index (in order added) of the point to be removed.
     */
    void remove(int n);

    /**
     * Change the position of a point already added to the SimplePoly, indicated by its index.
     * This currently does NOT set the Z component of the point.
     *
     * @param n index
     * @param p position
     * @return false, if an invalid vertex is detected, true otherwise
     */
    bool set(int n, Position p);

	/**
	 * Sets the top altitude of this SimplePoly.
	 * @param t New top altitude.
	 */
	void setTop(double t);

	/**
	 * Return the top altitude of this SimplePoly.
	 * @return top altitude
	 */
	double getTop() const;

	/**
	 * Sets the bottom altitude of this SimplePoly.
	 * @param b New bottom altitude.
	 */
	void setBottom(double b);

	/**
	 * Return the bottom altitude of this SimplePoly.
	 * @return bottom altitude
	 */
	double getBottom() const;


	/**
	 * Returns the position of the top point with index n.
	 * If n is not a valid index, this returns the centroid position.
	 * 
	 * @param n index
	 * @return position
	 */
	Position getTopPoint(int n) const;

	/**
	 * Returns a deep copy of this SimplPoly.
	 * 
	 * @return copy
	 */
	SimplePoly copy() const;

	/** return a Poly3D version of this.  proj is ignored if this is Euclidean 
	 * @param proj projection
	 * @return a new 3D polygon
	 */
	Poly3D poly3D(const EuclideanProjection& proj);

	/**
	 * This moves the SimplePoly by the amount determined by the given (Euclidean) offset.
	 * Used in GUI for dragging things.
	 * @param off offset
	 */
	void translate(const Vect3& off);

	SimplePoly linear(const Velocity& v, double t) const;
	SimplePoly linearAllVerts(const Velocity& v, double t) const;
	/** Move a polygon by velocity using vertex 0 and reconstructing geometrically
	 * 
	 * @param v velocity
	 * @param t time
	 * @return polygon
	 */
	SimplePoly linearFixedShape(const Velocity& v, double t) const;


	bool validate() const;

	bool validate(ErrorLog* error) const;

	int maxInRange(const Position& p, double a1, double a2) const;

	BoundingRectangle getBoundingRectangle();

	/**
	 * Return the angle that is perpendicular to the middle of the edge from vertex i to i+1, facing outward.
	 * Return NaN if i is out of bounds or vertex i overlaps vertex i+1.
	 * 
	 * @param i index
	 * @return angle 
	 */
	double perpSide(int i) const;

  private:
	bool vertexConvex(const Position& p0, const Position& p1, const Position& p2);

  public:
	/**
	 * Return the internal angle of vertex i.
	 * Return NaN if i is out of bounds or vertex i overlaps with an adjacent vertex.
	 * 
	 * @param i index
	 * @return angle
	 */
	double vertexAngle(int i);

	std::string toOutput(const std::string& name, int precision, int numberTcpColumns) const;

	/**
	 * String representation of this SimplePoly.
	 */
	std::string toString() const;

	std::vector<std::string> toStringList(int vertex, int precision) const;


}; //class

}// namespace

#endif
