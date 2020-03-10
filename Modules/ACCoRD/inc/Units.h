/*
 * Units.h
 *
 * Contact: Jeff Maddalon
 * NASA LaRC
 *
 * Conversion to internal units: meters, seconds, radians.
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef UNITS_H_
#define UNITS_H_

#include "Util.h"
#include <string>

namespace larcfm {

//  double _FormalATM_m();

/** See Units::gn */
double _FormalATM_gn();

/** See Units::P0 */
double _FormalATM_P0();



/**
 * <p>This class defines the conversion factors for various units. Base units are
 * defined for the seven base dimensions: length, mass, time, electric current,
 * thermodynamic temperature, amount of substance, and luminous intensity.
 * Together these seven units form a coherent set of units. If all quantities in
 * a program are defined in terms of these seven coherent units, then no unit
 * conversions are required within the program. Unit conversions are only needed
 * when specifying and outputting quantities. This class is designed to
 * facilitate that type of use.
 * </p>
 * 
 * <p>
 * For example, note there are no unit conversions in the equations:
 * </p>
 * 
 * <pre>
 * double one_mile = Units.from(&quot;mi&quot;, 1.0);
 * double one_km = Units.from(&quot;km&quot;, 1.0);
 * double time = Units.from(&quot;hour&quot;, 1.0);
 * 
 * double distance = one_mile + one_km;
 * double speed = distance / time;
 * 
 * System.out.println(&quot;The distance should be 2.6 in kilometers and is &quot; + Units.to(&quot;km&quot;, distance)); 
 * System.out.println(&quot;The distance should be 1.6 in miles and is &quot; + Units.to(&quot;mi&quot;, distance)); 
 * System.out.println(&quot;The distance should be 8560 in feet and is &quot; + Units.to(&quot;ft&quot;, distance)); 
 * System.out.println(&quot;The speed should be 1.6 in miles/hour and is &quot; + Units.to(&quot;mph&quot;, speed)); 
 * System.out.println(&quot;The speed should be 0.72 in m/s and is &quot; + Units.to(&quot;m/s&quot;, speed)); 
 * </pre>
 * 
 * <p>
 *  * One important consideration when using the Units class is that the Units
 * class performs <b>NO</b> consistency checks. The Units class trusts that the
 * user will use units with a consistent dimension. This was a deliberate design
 * decision because of the performance penalty associated with checking
 * consistency. An example of this inconsistency is:
 * </p>
 * 
 * <pre>
 * double one_mile = Units.from(&quot;mile&quot;, 1.0);
 * System.out.println(&quot;One mile in liters is &quot; + Units.to(&quot;liter&quot;, one_mile));
 * </pre>
 * 
 * <p>
 * Obviously this makes no sense, but the Units class will not flag this as an
 * error; be careful.
 * </p>
 * 
 * <p>
 * One can determine if two units are compatible by using the
 * {@link #isCompatible} method. Units are considered compatible if they have
 * the same dimensionality. For instance, the unit "meter" is compatible with
 * the unit "foot" since they are both units of distance.
 * </p>
 * 
 * <pre>
 * if (!isCompatible(&quot;m&quot;, &quot;L&quot;)) {
 * 	System.out.println(&quot;'m' and 'L' are not compatible units!&quot;);
 * }
 * if (isCompatible(&quot;ft&quot;, &quot;m&quot;)) {
 * 	System.out.println(&quot;'ft' and 'm' are compatible units!&quot;);
 * }
 * </pre>
 * 
 * <p>
 *  * One can always correctly convert between compatible units. To get the
 * list of units compatible with a given unit use the
 * {@link #getCompatibleUnits} method.
 * </p>
 * 
 * <p>
 * The user can also create their own composite units with the
 * {@link #addUnit(String, double, String)} method. For example a <i>fathom</i>
 * unit could be defined as:
 * </p>
 * <code>Units.addUnit("fathom", 6.0, "foot"); </code>
 * 
 * <p>
 * This method means, define the unit called "fathom", where one fathom is equal
 * to 6.0 feet. This method also states that the new "fathom" unit is compatible
 * with the unit of "foot."
 * </p>
 * 
 * <p>
 * To create a new composite unit that has no compatible units, use the
 * {@link #addUnit(String, double)} method. For example a <i>linear density</i>
 * unit could be defined as:
 * </p>
 * <code>Units.addUnit("kg/m", Units.kg / Units.meter);</code>
 * 
 * 
 * <h2>Special Units:</h2>
 * 
 * <ul>
 * <li> unitless - a name to represent that the quantity has no units.  Ratios and 
 * percentages are typical examples of unitless quantities.</li>
 * 
 * <li> unspecified - a name to represent that the units of the quantity that are unknown.
 * The real distinction between a <i>unitless</i> quantity and an <i>unspecified</i> quantity is the 
 * notion of units compatibility.  Unitless is not compatible with any other unit, and
 * unspecified is is compatible with all units. </li>
 * </ul>
 * 
 * 
 * <h2>Usage Notes:</h2>
 * 
 * <ul>
 * <li><p>An alternate way to use the units class is to use constant factors
 * instead of string representations. For example:
 * </p>
 * 
 * <pre>
 * double one_mile = Units.from(Units.mile, 1.0);
 * double one_km = Units.from(Units.kilometer, 1.0);
 * double time = Units.from(Units.hour, 1.0);
 * </pre>
 * 
 * <p>The advantage of this approach is that it is slightly more efficient since a
 * lookup of the string value does not need to be performed. Since this
 * operation only occurs when values are input, it is questionable how much time
 * is really saved.</p></li>
 * 
 * <li><p>The above examples (using either strings or factors) are the preferred
 * use of the Units class; however, this notation can become
 * cumbersome--especially when specifying values inside code. An alternate use of
 * the Units class is to multiply by the conversion factor when specifying
 * constants. For example:
 * </p>
 * 
 * <pre>
 * double one_mile = 1.0 * Units.mile;
 * System.out.println(&quot;One mile in kilometers is &quot; + Units.to(&quot;km&quot;, one_mile));
 * </pre>
 * 
 * <p>
 * The &quot;to&quot; and &quot;from&quot; methods are preferred, because some conversions are not
 * simply multiplying a factor, but involve an offset also. Those that involve an
 * offset (currently only degreeC and degreeF), must use the "to" and "from"
 * methods. Forms such as <code>double temp = 32.0 * degreeF;</code> are
 * <i>always</i> wrong.</p></li>
 * 
 * <li>The Units class does not define a "pound". The problem is that in most
 * official standards documents, a pound is a unit of mass; however, in most
 * science and engineering texts, a pound is a unit of force. We avoid the
 * problem by not defining a pound at all, and instead defining a pound_mass
 * (lbm) and a pound_force (lbf).
 * </li>
 * 
 * <li>There are many more units defined as strings in the Units class than the
 * number defined as class fields in the Units class. To get a complete list of
 * all units defined as a strings call the {@link #getAllUnits()} method.
 * </li>
 * 
 * <li><p>The internal unit for angles is always radians. The SI convention is
 * followed by calling this a dimensionless derived unit, defined as m / m.
 * </p>
 * 
 * <p>
 * Since angles are always represented in radians, a user of the Units class
 * does not need to perform unit conversions to get radians for trigonometric
 * functions. Expressions like
 * </p>
 * <code>a = Math.cos(Units.from("radian", pi_over_2));</code>
 * <p>
 * are not necessary, but are still valid. Expressions that use degrees still
 * need units specified, for example:
 * </p>
 * <code>double right_angle = Units.from("degree", 90.0);</code><br>
 * <code>a = Math.cos(right_angle);</code>
 * </li>
 * 
 * <li>The units for thermodynamic temperature are degreeC, degreeF, K,
 * and degreeR, representing Celsius, Farenheit, Kelvin, and Rankin.
 * </li>
 * 
 * <li>SI defines the unit of frequency as the reciprocal second and gives this
 * quantity the name <i>hertz</i> and the symbol <i>Hz</i>. The Units class
 * defines these quantities as "hertz" and "Hz". Some disciplines use the term
 * <i>frequency</i> to indicate an angular velocity. Specifically, the notion in
 * dynamic systems of <i>natural frequency</i>, often abbreviated with the Greek
 * symbol lower-case omega (&#969;), is actually an angular velocity. In the
 * Units class, the appropriate symbol for angular velocity is
 * "radian_per_second" or "degree_per_second". However, since quantities like
 * <i>natural frequency</i> have frequency in their name, they are often,
 * incorrectly, expressed in "hertz." To accomodate this type of usage, the
 * Units class defines a unit "hertzAngular". "hertz" and "hertzAngular" are
 * <b>NOT</b> compatible with each other, the first is a frequency and the
 * second is an angular velocity.
 * </li>
 * 
 * <li>Both joules and newton-meters are defined. These units are so similar
 * that they could be considered the same. They are kept separate because joules
 * are used to represent energy and newton-meters are used to represent torque.
 * Energy is a scalar quantity and torque is a vector. It is rare that they are
 * interchanged. The US Customary unit for energy is the foot-pound (ft-lbf),
 * and the unit for torque is the pound-foot (lbf-ft). This last name is our
 * invention.
 * </li>
 * 
 * <li>Both names <i>metre</i> and <i>meter</i> and <i>litre</i> and
 * <i>liter</i> are defined. Each pair is identical. In the approved English
 * language version of the SI standard, only the terms "metre" and "litre" are
 * defined. However, in the US version of this document, prepared by the
 * National Institute of Standards and Technology (Special Pub. 330) using
 * current US government policy, the only defined terms are "meter" and "liter."
 * The Units class defines both; however, the reader will observe a bias in the
 * documentation in favor of the US terms.</li>
 * </ul>
 * 
 * 
 * <h2>Special Constants:</h2>
 * 
 * <ul>
 * <li><p>The Units class defines <i>gn</i> as the adopted physical constant of
 * gravity. It is given (out of place) in the Units class, due to the fact that
 * the definition of the fundamental US customary unit of mass (slug) depends on
 * this quantity.
 * </p>
 * 
 * <p>This quantity, Units.gn, is defined in IEEE/ASTM SI 10-1997 as (exactly)
 * 9.80665 m/s^2 [page 25]. The 1962 U.S. Standard Atmosphere [page 4] calls
 * this quantity "standard sea-level gravity" and describes it as the
 * acceleration due to the combined effects of gravity and the Earth's rotation
 * (i.e. the centrifugal relief) at geodetic latitude 45 degrees. This constant
 * was adopted in the US in 1935.
 * </p>
 * 
 * <p>The latitude of 45 degrees was chosen because this is the standard latitude
 * chosen by the World Meteorological Organization to calibrate barometers. As a
 * note, this value is the solution to Lambert's equation (sea-level gravity as
 * a function of latitude), when the latitude is 45 degrees 32 minutes and 33
 * seconds.</p></li>
 * 
 * <li>The Units class defines Units.P0 as the adopted physical constant of
 * standard atmosphere. This quantity equals 101325 Pa. This definition is added
 * to the Units class because the definition of an atmosphere unit ("atm")
 * depends on it. This definition comes from NIST Special Publication 330, the
 * International System of Units (SI), 1991 edition, p15.</li>
 * </ul>
 * 
 * <h2>Implementation Notes:</h2>
 * 
 * <ul>
 * <li>The Units class uses the SI system (with base units: meter [distance],
 * kilogram [mass], second [time interval], ampere [electric current], kelvin
 * [thermodynamic temperature], mole [amount of substance], and candela
 * [luminous intensity]) as a coherent set of base units (internal units).
 * <p>
 * 
 * Please remember the user of the Units class should never <i>need</i> to know
 * how the variables are stored internally. The user need only remember to
 * specifically state the units on input and output and any conversions will
 * occur automatically.
 * </li>
 * 
 * <li>The purpose of this class is geared to modeling large engineered systems
 * such as airplanes. As such, many units useful in other disciplines are not
 * defined (such as the "carat" or a "bushel"). However, they may be defined in
 * future versions of this class.
 * <p>
 * 
 * Please note that since the complete set of seven base units are defined, any
 * other units needed may be defined by the user with the {@link #addUnit
 * addUnit()} method.</li>
 * 
 * <li>Much care was used in defining each of these conversion factors. The full
 * precision from appropriate standards documents was used. However, some units
 * are defined as the arithmetic relationship between 'more fundamental'
 * quantities. In these cases, the conversion factors are limited to the resolution
 * of double precision operations. For most engineering purposes
 * (see the point above) this is more than good enough; however, someone working
 * with very precise quantities (&gt; 12 significant digits), should be aware of
 * the possibility of differences.</li>
 * 
 * 
 * <li>The names of many of the conversion factors violate standard naming
 * conventions for class variables. This was deemed acceptable since these names
 * follow the standard capitalization conventions from SI or other standards
 * bodies.
 * </li>
 * 
 * <li>The Units class is not <i>immutable</i>. However modification of this
 * class is limited. Specifically, there is no mechanism to <i>delete</i> a unit from
 * the Units class, nor is there any ability to <i>change</i> a conversion factor of
 * a unit. These restrictions allow the user to be confident that once a unit is
 * in the Units class, its definition will never change. The unit <b>A</b> will
 * always be an ampere, it will never become an angstrom.</li>
 * 
 * </ul>
 * 
 * <h2>References:</h2>
 * 
 * <ul>
 * <li>National Institute of Standards and Technology (NIST) Special Publication
 * 330, 1991 Edition, The International System of Units (SI).
 * 
 * <li>National Institute of Standards and Technology (NIST) Special Publication
 * 811, 1995 Edition, Guide for the Use of the International System of Units
 * (SI).
 * 
 * <li>IEEE Standards Coordinating Committee 14 on Quantities, Units, and Letter
 * Symbols, IEEE/ASTM SI 10-1997, Standard for Use of the International System
 * of Units (SI): The Modern Metric System.
 * 
 * <li>1962 Standard Atmosphere.
 * 
 * <li>Bureau International des Poids et Mesures, The International System of
 * Units (SI), 7th edition, 1998.
 * 
 * </ul>
 * 
 * 
 */
class Units {
public:

	/** Units were not specified */
	static const double unspecified;
	/** Quantity without units */
	static const double unitless;
	/** Use the internal representation for this quantity */
	static const double internal;

	/** meter */
	static const double m;
	/** kilometer */
	static const double km;
	/** nautical mile */
	static const double nmi;
	/** nautical mile */
	static const double NM;
	/** foot */
	static const double foot;
	/** foot */
	static const double ft;
	/** millimeter */
	static const double mm;

	/** meter squared, a unit of area */
	static const double meter2;

	/** seconds */
	static const double s;
	/** seconds */
	static const double sec;
	/** minutes */
	static const double min;
	/** hours */
	static const double hour;
	/** days */
	static const double day;
	/** milliseconds */
	static const double ms;

	/** radians */
	static const double rad;
	/** degrees */
	static const double deg;
	/** degrees */
	static const double degree;

	/** kilograms */
	static const double kg;
	/** pounds of mass */
	static const double pound_mass;

	/** Meters per second */
	static const double mps;
	static const double meter_per_second;
	/** kilometers per hour */
	static const double kph;
	static const double kilometer_per_hour;
	/** knots, (nautical miles per hour) */
	static const double knot;
	static const double kn;
	static const double kts;
	/** feet per minute */
	static const double fpm;
	/** feet per second */
	static const double foot_per_second;
	/** nautical mile per second */
	static const double nautical_mile_per_second;
	/** mile per hour */
	static const double mph;

	static const double degreeC;
	static const double degreeF;
	static const double K;
	static const double degreeK;
	static const double degreeR;


	
	/**
	 * gn is the adopted physical constant of gravity. It is given (out of
	 * place) here, due to the fact that the definition of the fundamental US
	 * customary units of mass (slug and pound-mass) depend on this quantity.
	 * <p>
	 *
	 * This quantity, Units.gn, is defined in IEEE/ASTM SI 10-1997 as (exactly)
	 * 9.80665 m/s^2 [page 25]. The 1962 U.S. Standard Atmosphere [page 4] calls
	 * this quantity "standard sea-level gravity" and describes it as the
	 * acceleration due to the combined effects of gravity and the Earth's
	 * rotation (i.e. the centrifugal relief) at geodetic latitude 45 degrees.
	 * This constant was adopted long before 1962.
	 * <p>
	 *
	 * This definition is also in NIST Special Publication 330, the
	 * International System of Units (SI), 1991 edition, p17.
	 * <p>
	 */
	static const double gn;

	static const double G;

	/** meters per second^2 */
	static const double meter_per_second2;

	/** unit of force */
	static const double newton;

	/** pascal, a unit of pressure */
	static const double pascal;
	/** pascal, unit of pressure, defined as a newton per meter squared */
	static const double Pa;
	/**
	 * P0, the adopted standard atmosphere. This quanity equals 101325 Pa. This
	 * definition comes from NIST Special Publication 330, the International
	 * System of Units (SI), 1991 edition, p15.
	 */
	static const double P0;
	/** atmosphere, a unit of pressure, defined as one P0 */
	static const double atm;


	/** Convert the value in internal units to the given units */
	static double to(const double symbol, const double value);
	/** Convert the value in internal units to the given units */
	static double to(const std::string& symbol, double value);
	/** Convert the value from the given units into internal units */
	static double from(const double symbol, const double value);
	/** Convert the value from the given units into internal units */
	static double from(const std::string& units, double value);
	static double fromInternal(const std::string& defaultUnits, const std::string& units, double value);

	/** Get the unit conversion factor for the given string unit */
	static double getFactor(const std::string& unit);
  /** Determine if the given string is a valid unit
   * @param unit string representation of a unit
   * @return true, if this string is known as a unit in the database of units */
	static bool isUnit(const std::string& unit);

	/**
	 * Determines if these two units are compatible with one another. That is,
	 * if they have the same dimensionality (for instance, both are units of
	 * distance) then true is returned. Note, the unit "unspecified" is
	 * compatible with all units, including "unitless" which is not compatible
	 * with any other units.
	 */
	static bool isCompatible(const std::string& unit1, const std::string& unit2);

	/** Make a string representation of the value in internal units to the given units */
	static std::string str(const std::string& symbol, double value);

	/** Make a string representation of the value in internal units to the given units. */
	static std::string strX(const std::string& symbol, double value);

	/** Make a string representation of the value in internal units to the given units */
	static std::string str(const std::string& symbol, double value, int precision);

  /**
   * Clean up the unit string that may contain brackets or extra space. For
   * instance, " [ feet]" becomes "feet". If the string in brackets is not a
   * recognized, then "unspecified" is returned.
   * @param unit a "dirty" string
   * @return a cleaned-up unit string
   */
	static std::string clean(const std::string& unit);

private:
  /**
   * Clean up the unit string that may contain brackets or extra space. For
   * instance, " [ feet]" becomes "feet". However, if the unit is "[  fred]"
   * then "fred" will be returned (unlike the clean() method).
   */
	static std::string cleanOnly(const std::string& unit);
public:

	static double parse(const std::string& defaultUnitsFrom, const std::string& s, double default_val);

	static double parse(const std::string& s);

  /**
   * Parse a string, including an optional units identifier, as a double value.
   * If the string does not contain a (valid) unit, then the value is interpreted as an "unspecified" unit.
   * This version does not parse numbers in exponential notation, e.g., "10e-4".  (Java does)
   * @param str string to parse
   * @param default_value if the string is not recognized as a valid value, the result to be returned 
   * @return value
   */
	static double parse(const std::string& str, double default_value);

	static double parse(const std::string&, const std::string& str);

	/**
	 * Parse a string, representing a value and a unit.
	 * If the string does not contain a unit, then the unit "unspecified" is returned.
     * This version does not parse numbers in exponential notation, e.g., "10e-4".  (Java does)
	 * @param s string to parse
	 * @return unit string;
	 */
	static std::string parseUnits(const std::string& s);



	/**
	 * Return the canonical name for this unit.  For instance, "meter" will return "m" and "m" will return "m".
	 */
	static const std::string canonical(const std::string& unit);

};

}

#endif /* UNITS_H_ */
