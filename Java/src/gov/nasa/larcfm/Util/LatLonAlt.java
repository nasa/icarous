/*
 * LatLonAlt.java - container to hold a geodesic position 
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

/**
 * Container to hold a latitude/longitude/altitude position 
 */
public final class LatLonAlt {

	/** The latitude */
	private final double lati;

	/** The longitude */
	private final double longi;

	/** The altitude */
	private final double alti;

	/**
	 * Creates a new position 
	 */
	private LatLonAlt(double lat, double lon, double alt) {
		lati = lat;
		longi = lon;
		alti = alt;
	}

	public static final LatLonAlt ZERO = new LatLonAlt(0.0,0.0,0.0);
	
	/** An invalid LatLonAlt.  Note that this is not necessarily equal to other invalid LatLonAlts -- use the isInvalid() instead */
	public static final LatLonAlt INVALID = new LatLonAlt(Double.NaN,Double.NaN,Double.NaN);

	public static final LatLonAlt NORTHPOLE = new LatLonAlt(Math.PI, 0.0, 0.0);
	public static final LatLonAlt SOUTHPOLE = new LatLonAlt(-Math.PI, 0.0, 0.0);
	
	/**
	 * Creates a new position that is a copy of <code>v</code>.
	 * 
	 * @param v position object
	 */
	public static LatLonAlt mk(LatLonAlt v) {
		return new LatLonAlt(v.lati, v.longi, v.alti);
	}

	/**
	 * Creates a new position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude [deg north latitude]
	 * @param lon longitude [deg east longitude]
	 * @param alt altitude [ft]
	 */
	public static LatLonAlt make(double lat, double lon, double alt) {
		return new LatLonAlt(Units.from("deg", lat),
				Units.from("deg", lon),
				Units.from("ft", alt));
	}

	/**
	 * Creates a new position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude [lat_unit north latitude]
	 * @param lat_unit units of latitude
	 * @param lon longitude [lon_unit east longitude]
	 * @param lon_unit units of longitude
	 * @param alt altitude [alt_unit]
	 * @param alt_unit units of altitude
	 */
	public static LatLonAlt make(double lat, String lat_unit, double lon, String lon_unit, double alt, String alt_unit) {
		return new LatLonAlt(Units.from(lat_unit, lat),
				Units.from(lon_unit, lon),
				Units.from(alt_unit, alt));
	}

	/**
	 * Creates a new position with given values
	 * 
	 * @param lat latitude [internal]
	 * @param lon longitude [internal]
	 * @param alt altitude [internal]
	 */
	public static LatLonAlt mk(double lat, double lon, double alt) {
		return new LatLonAlt(lat, lon, alt);
	}
	
	/**
	 * Creates a new LatLonAlt with only altitude changed
	 * 
	 * @param alt altitude [internal]
	 */
	public LatLonAlt mkAlt(double alt) {
		return new LatLonAlt(lati, longi, alt);
	}
	
	/**
	 * Creates a new LatLonAlt with only altitude changed
	 * 
	 * @param alt altitude [feet]
	 */
	public LatLonAlt makeAlt(double alt) {
		return new LatLonAlt(lati, longi, Units.from("ft",alt));
	}

//	public boolean equals(LatLonAlt a) {
//		return a.lati == lati && a.longi == longi && a.alti == alti;
//	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(alti);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(lati);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(longi);
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
		LatLonAlt other = (LatLonAlt) obj;
		if (Double.doubleToLongBits(alti) != Double
				.doubleToLongBits(other.alti))
			return false;
		if (Double.doubleToLongBits(lati) != Double
				.doubleToLongBits(other.lati))
			return false;
		if (Double.doubleToLongBits(longi) != Double
				.doubleToLongBits(other.longi))
			return false;
		return true;
	}

	
	
	/** Are these two LatLonAlt almost equal? */
	public boolean almostEquals(LatLonAlt a) {
		return GreatCircle.almost_equals(this.lat(), this.lon(), a.lat(), a.lon())
				&& Constants.almost_equals_alt(this.alt(), a.alt());
	}

	/** Are these two LatLonAlt almost equal, where 'almost' is defined by the given distances [m] */
	public boolean almostEquals(LatLonAlt a, double horizdist, double vertdist) {
		return GreatCircle.almost_equals(this.lat(), this.lon(), a.lat(), a.lon(), horizdist)
				&& Util.within_epsilon(this.alt(), a.alt(), vertdist);
	}

	/** Are these two LatLonAlt almost equal horizontally? */
	public boolean almostEqualsHoriz(LatLonAlt a) {
		return GreatCircle.almost_equals(this.lat(), this.lon(), a.lat(), a.lon());
	}

	/**
	 * Return a copy of the current LatLonAlt with a zero altitude.  This is useful for creating projections that need to preserve altitude 
	 */
	public LatLonAlt zeroAlt() {
		return new LatLonAlt(lati,longi,0.0);
	}
	
	/** Return latitude in degrees north */
	public double latitude() {
		return Util.to_180(Units.to(Units.deg, lati));
	}

	/** Return latitude in the given units in a northerly direction */
	public double latitude(String units) {
		return Units.to(units, lati);
	}

	/** Return longitude in degrees east */
	public double longitude() {
		return Util.to_180(Units.to(Units.deg, longi));
	}

	/** Return longitude in the given units in an easterly direction */
	public double longitude(String units) {
		return Units.to(units, longi);
	}

	/** Return altitude in [ft] */
	public double altitude() {
		return Units.to(Units.ft, alti);
	}	
	
	/** Return altitude in the given units */
	public double altitude(String units) {
		return Units.to(units, alti);
	}	
	
	/** Compute a new lat/lon that is offset by dn meters north and de meters east.
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * 
	 * @param so  original lat/lon position
	 * @param dn  offset in north direction (m)
	 * @param de  offset in east direction  (m)
	 * @return
	 */
	public LatLonAlt linearEst(double dn, double de) {
		//f.pln(" lat = "+Units.str("deg",lati)+" lon = "+Units.str("deg",longi));
		double R = 6378137;                   // diameter earth in meters
       //TODO:  switch to		 R = GreatCircle.spherical_earth_radius;
		double nLat = lati + dn/R;
		double nLon = longi + de/(R*Math.cos(lati));
		//f.pln(" nLat = "+Units.str("deg",nLat)+" nLon = "+Units.str("deg",nLon));
		return LatLonAlt.mk(nLat,nLon,alti);
	}

	/** Compute a new lat/lon that is obtained by moving with velocity vo for tm secs
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * 
	 * @param so   original lat/lon position
	 * @param vo   velocity away from original position
	 * @param tm   time of relocation
	 * @return new lat/lon position in direction v0
	 */
	public LatLonAlt linearEst(Velocity vo, double tm) {
		double dn = vo.Scal(tm).y();
		double de = vo.Scal(tm).x();
		double nAlt = alti + vo.z*tm;
		//f.pln(" %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% linearEst: nAlt = "+Units.str("ft",nAlt));
		return linearEst(dn,de).mkAlt(nAlt);   
	}

	public double distanceH(LatLonAlt lla2) {
		return GreatCircle.distance(this,lla2);
	}
	
  /**
    * Returns true if the current LatLonAlt has an "invalid" value
    */
	  public boolean isInvalid() {
		  return Double.isNaN(lati) || Double.isNaN(longi) || Double.isNaN(alti);
	  }

	  /** return the antipodal point corresponding to this LatLonAlt */
	public LatLonAlt antipode() {
		return LatLonAlt.mk(-lati, Util.to_pi(longi+Math.PI), alti);
	}
	
	/** Return latitude in internal units */
	public double lat() {
		return lati;
	}
	/** Return longitude in internal units */
	public double lon() {
		return longi;
	}
	/** Return altitude in internal units */
	public double alt() {
		return alti;
	}
	
    /** String representation with units of [deg,deg,ft] */
	public String toString() {
		return toString(Constants.get_output_precision());
	}

    /** String representation with units of [deg,deg,ft] */
	public String toString(int precision) {
		StringBuffer sb = new StringBuffer(30);
		sb.append('(');
		sb.append(f.FmPrecision(latitude(), precision));
		sb.append(", ");
		sb.append(f.FmPrecision(longitude(), precision));
		sb.append(", ");
		sb.append(f.FmPrecision(altitude(), precision));
		sb.append(')');
		return sb.toString();
	}

	public String toStringUnicode(int precision) {
		StringBuffer sb = new StringBuffer(30);
		sb.append(f.FmPrecision(Math.abs(latitude()), precision));
		if (latitude() > 0) {
			sb.append("\u00B0N");
		} else {
			sb.append("\u00B0S");
		}
		sb.append(", ");
		sb.append(f.FmPrecision(Math.abs(longitude()), precision));
		if (longitude() > 0) {
			sb.append("\u00B0E");
		} else {
			sb.append("\u00B0W");
		}
		sb.append(", ");
		sb.append(Units.str("ft", alt(), precision));
		return sb.toString();
	}
	
    /** String representation */
	public List<String> toStringList(int precision) {
		ArrayList<String> ret = new ArrayList<String>(3);
		ret.add(f.FmPrecision(latitude(), precision));
		ret.add(f.FmPrecision(longitude(), precision));
		ret.add(f.FmPrecision(altitude(), precision));
		return ret;
	}

	/** Return a string representation consistent with StateReader or PlanReader with user-specified units and precision */
	public String toStringNP(String latunit, String lonunit, String zunit, int precision) {
		return f.FmPrecision(f.fm_nz(Units.to(latunit, lati), precision+1),precision) + ", " + f.FmPrecision(f.fm_nz(Units.to(lonunit, longi),precision+1), precision) + ", " 	+ f.FmPrecision(Units.to(zunit, alti), precision);
	}
	
	/** Return a string representation consistent with StateReader or PlanReader with user-specified precision */
	public String toStringNP(int p) {
		return toStringNP("deg", "deg", "ft", p);
	}
	
	/** Return a string representation consistent with StateReader or PlanReader with the global default precision */
	public String toStringNP() {
		return toStringNP("deg", "deg", "ft", Constants.get_output_precision());
	}

	/** Return a string representation consistent with StateReader or PlanReader with the global default precision */
	
	
	/**
	 * Provide a string representation of this object in the 'Degree, Decimal Minutes' convention.
	 * Example: 39 degrees, 20.01 minutes north latitude, 85 degrees, 10.53 minutes west longitude is represented as N392001W0851053 (with a precision = 2)
	 * 
	 * @param precision number of digits of precision for the decimal minutes. 
	 * @return the string representation
	 */
	public String toStringDecimalMinutes(int precision) {
		if (precision < 0 || precision > 10) {
			precision = 0; // decimal minutes
		}
		
		StringBuilder sb = new StringBuilder(100);
	
		double deg_lat = latitude();
		double deg_lon = longitude();
		
		if (deg_lat >= 0.0) {
			sb.append("N");
		} else {
			sb.append("S");
		}

		deg_lat = Math.abs(deg_lat);
		sb.append(f.FmLead((int)Math.floor(deg_lat),2));

		double min_lat = Math.round((deg_lat - Math.floor(deg_lat)) * 60 * Math.pow(10.0, precision));
		sb.append(f.FmLead((int)Math.floor(min_lat),2+precision));

		
		
		if (deg_lon >= 0.0) {
			sb.append("E");
		} else {
			sb.append("W");
		}

		deg_lon = Math.abs(deg_lon);
		sb.append(f.FmLead((int)Math.floor(deg_lon),3));

		double min_lon = Math.round((deg_lon - Math.floor(deg_lon)) * 60 * Math.pow(10.0, precision));
		sb.append(f.FmLead((int)Math.floor(min_lon),2+precision));

		return sb.toString();
	}

	
	/**
	 * parse the lat/lon from the name assuming the degrees, decimal minutes representation
	 * @param name
	 * @return
	 */
	public static LatLonAlt parseDecimalMinutes(String name) {
		int indexToLon;
		String lat;
		char latDirection;
		double latDegrees = 0.0;
		double latMinutes = 0.0;
		double latDecimalMinutes = 0.0;
		String lon;
		char lonDirection;
		double lonDegrees = 0.0;
		double lonMinutes = 0.0;
		double lonDecimalMinutes = 0.0;
		
		// return invalid LatLon if the input is not in the expected form (latitude first, both lat and lon, minimum degrees defined)
		if ( (name.charAt(0)!='N' && name.charAt(0)!='S') || (name.indexOf('W')==-1 && name.indexOf('E')==-1) || name.length()<7 ) {
		//	f.pln(" $$$$$$$$$$$ "+name.charAt(0)+" index = "+name.indexOf('W')); 
			return LatLonAlt.INVALID;
		}
		
		// find the starting position for the longitude definition
		indexToLon = name.indexOf('W');
		if (indexToLon==-1) {
			indexToLon = name.indexOf('E');
		}
		
		// parse the latitude
		lat = name.substring(0, indexToLon);
		latDirection = lat.charAt(0);
		latDegrees = Util.parse_double(lat.substring(1, 3));
		if (lat.length()>3)  latMinutes = Util.parse_double(lat.substring(3, 5));
		if (lat.length()>5)  latDecimalMinutes = Util.parse_double(lat.substring(5, lat.length()))*Math.pow(10, -(lat.length()-5));
		latDegrees += (latMinutes + latDecimalMinutes) / 60.0;		// add the decimal degrees from the minutes data
		if (latDirection=='S') latDegrees = -1.0 * latDegrees;
		
		//parse the longitude
		lon = name.substring(indexToLon, name.length());
		lonDirection = lon.charAt(0);
		lonDegrees = Util.parse_double(lon.substring(1, 4));
		if (lon.length()>4)  lonMinutes = Util.parse_double(lon.substring(4, 6));
		if (lon.length()>6)  lonDecimalMinutes = Util.parse_double(lon.substring(6, lon.length()))*Math.pow(10, -(lon.length()-6));
		lonDegrees += (lonMinutes + lonDecimalMinutes) / 60.0;		// add the decimal degrees from the minutes data
		if (lonDirection=='W') lonDegrees = -1.0 * lonDegrees;
				
		// compose the location and return
		return LatLonAlt.make(latDegrees, lonDegrees, 0.0);
	}
	
	  /** 
	   * This parses a space or comma-separated string as a LatLonAlt (an inverse 
	   * to the toString method).  If three bare values are present, then it is interpreted as deg/deg/ft.
	   * If there are 3 value/unit pairs then each values is interpreted with regard 
	   * to the appropriate unit.  If the string cannot be parsed, an INVALID value is
	   * returned. 
	   * */
	  public static LatLonAlt parse(String str) {
			String[] fields = str.split(Constants.wsPatternParens);
			if (fields[0].equals("")) {
				fields = Arrays.copyOfRange(fields,1,fields.length);
			}
			try {
				if (fields.length == 3) {
					return LatLonAlt.make(Double.parseDouble(fields[0]),Double.parseDouble(fields[1]),Double.parseDouble(fields[2]));
				} else if (fields.length == 6) {
					return LatLonAlt.mk(Units.from(Units.clean(fields[1]),Double.parseDouble(fields[0])),
							Units.from(Units.clean(fields[3]),Double.parseDouble(fields[2])),
							Units.from(Units.clean(fields[5]),Double.parseDouble(fields[4])));
				}
			} catch (Exception e) {}
			return LatLonAlt.INVALID;
	  }
	  
	  
	  /**
	   * Normalizes the given latitude and longitude values to conventional spherical angles.  Thus
	   * values over the pole (95 degrees of latitude) convert to 85 degrees and the longitude 180 degrees different.
	   * 
	   * @param lat latitude
	   * @param lon longitude
	   * @param alt altitude
	   * @return normalized LatLonAlt value
	   */
	  public static LatLonAlt normalize(double lat, double lon, double alt) {
		  double nlat, nlon;
		  nlon = Util.to_pi(lon);
		  lat = Util.to_pi(lat);
		  nlat = Util.to_pi2_cont(lat);
		  if (lat != nlat) {
			  nlon = Util.to_pi(nlon + Math.PI);
		  }
		  return LatLonAlt.mk(nlat, nlon, alt);
	  }

	  /**
	   * Normalizes the given latitude and longitude values to conventional spherical angles.  Thus
	   * values over the pole (95 degrees of latitude) convert to 85 degrees and the longitude 180 degrees different.
	   * The altitude is assumed to be zero.
	   * 
	   * @param lat latitude
	   * @param lon longitude
	   * @return normalized LatLonAlt value
	   */
	  public static LatLonAlt normalize(double lat, double lon) {
		  return normalize(lat, lon, 0.0);
	  }

	  /**
	   * Creates a new LatLonAlt object from the current LatLonAlt object so that latitude and longitude values are 
	   * conventional spherical angles.  Thus
	   * values over the pole (95 degrees of latitude) convert to 85 degrees and the longitude 180 degrees different.
	   * 
	   * @return normalized LatLonAlt value
	   */
	  public LatLonAlt normalize() {
		  return normalize(lat(), lon(), alt());
	  }

	  /**
	   * Return true if this point is (locally) west of the given point.
	   * @param a reference point
	   * @return true if this point is to the west of the reference point within a hemisphere they share.
	   */
	  public boolean isWest(LatLonAlt a) {
		  // this uses the same calculations as Util.clockwise:
		  return Util.clockwise(a.lon(), lon());
	  }

}
