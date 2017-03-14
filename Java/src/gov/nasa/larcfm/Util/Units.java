/* 
 * Units
 *
 * Contact: Jeff Maddalon 
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

package gov.nasa.larcfm.Util;

import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.SortedSet;
import java.util.Iterator;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

/**
 * This class defines the conversion factors for various units. Base units are
 * defined for the seven base dimensions: length, mass, time, electric current,
 * thermodynamic temperature, amount of substance, and luminous intensity.
 * Together these seven units form a coherent set of units. If all quantities in
 * a program are defined in terms of these seven coherent units, then no unit
 * conversions are required within the program. Unit conversions are only needed
 * when specifying and outputting quantities. This class is designed to
 * facilitate that type of use.
 * <p>
 * 
 * For example, note there are no unit conversions in the equations:
 * <p>
 * 
 * <pre>
 * double one_mile = Units.from(&quot;mi&quot;, 1.0);
 * double one_km = Units.from(&quot;km&quot;, 1.0);
 * double time = Units.from(&quot;hour&quot;, 1.0);
 * 
 * double distance = one_mile + one_km;
 * double speed = distance / time;
 * 
 * System.out.println(&quot;The distance should be 2.6 and in kilometers is &quot; + Units.to(&quot;km&quot;, distance)); 
 * System.out.println(&quot;The distance should be 1.6 and in miles is &quot; + Units.to(&quot;mi&quot;, distance)); 
 * System.out.println(&quot;The distance should be 8560 in feet is &quot; + Units.to(&quot;ft&quot;, distance)); 
 * System.out.println(&quot;The speed should be 1.6 and in miles/hour is &quot; + Units.to(&quot;mph&quot;, speed)); 
 * System.out.println(&quot;The speed should be 0.72 and in m/s is &quot; + Units.to(&quot;m/s&quot;, speed)); 
 * </pre>
 * <p>
 * 
 * One important consideration when using the Units class is that the Units
 * class performs <b>NO</b> consistency checks. The Units class trusts that the
 * user will use units with a consistent dimension. This was a deliberate design
 * decision because of the performance penalty associated with checking
 * consistency. An example of this inconsistency is:
 * <p>
 * 
 * <pre>
 * double one_mile = Units.from(&quot;mile&quot;, 1.0);
 * System.out.println(&quot;One mile in liters is &quot; + Units.to(&quot;liter&quot;, one_mile));
 * </pre>
 * 
 * Obviously this makes no sense, but the Units class will not flag this as an
 * error; be careful.
 * <p>
 * 
 * One can determine if two units are compatible by using the
 * {@link #isCompatible} method. Units are considered compatible if they have
 * the same dimensionality. For instance, the unit "meter" is compatible with
 * the unit "foot" since they are both units of distance.
 * <p>
 * 
 * <pre>
 * if (!isCompatible(&quot;m&quot;, &quot;L&quot;)) {
 * 	System.out.println(&quot;'m' and 'L' are not compatible units!&quot;);
 * }
 * if (isCompatible(&quot;ft&quot;, &quot;m&quot;)) {
 * 	System.out.println(&quot;'ft' and 'm' are compatible units!&quot;);
 * }
 * </pre>
 * <p>
 * 
 * One can always correctly convert between compatible units. The unit
 * "unspecified" is a special unit that is compatible with all units. To get the
 * list of units compatible with a given unit use the
 * {@link #getCompatibleUnits} method.
 * <p>
 * 
 * The user can also create their own composite units with the
 * {@link #addUnit(String, double, String)} method. For example a <i>fathom</i>
 * unit could be defined as:
 * <p>
 * <tt>Units.addUnit("fathom", 6.0, "foot"); </tt>
 * <p>
 * This method means, define the unit called "fathom", where one fathom is equal
 * to 6.0 feet. This method also states that the new "fathom" unit is compatible
 * with the unit of "foot."
 * 
 * To create a new composite unit that has no compatible units, use the
 * {@link #addUnit(String, double)} method. For example a <i>linear density</i>
 * unit could be defined as:
 * <p>
 * <tt>Units.addUnit("kg/m", Units.kg / Units.meter);</tt>
 * <p>
 * 
 * Usage Notes:
 * <p>
 * 
 * <ul>
 * <li>An alternate way to use the units class is to use constant factors
 * instead of string representations. For example:
 * <p>
 * 
 * <pre>
 * double one_mile = Units.from(Units.mile, 1.0);
 * double one_km = Units.from(Units.kilometer, 1.0);
 * double time = Units.from(Units.hour, 1.0);
 * </pre>
 * 
 * The advantage of this approach is that it is slightly more efficient since a
 * lookup of the string value does not need to be performed. Since this
 * operation only occurs when values are input, it is questionable how much time
 * is really saved.
 * 
 * <li>The above examples (using either strings or factors) are the preferred
 * use of the Units class; however, this notation can become
 * cumbersome--especially when specifying values inside code. An alternate use of
 * the Units class is to multiply by the conversion factor when specifying
 * constants. For example:
 * <p>
 * 
 * <pre>
 * double one_mile = 1.0 * Units.mile;
 * System.out.println(&quot;One mile in kilometers is &quot; + Units.to(&quot;km&quot;, one_mile));
 * </pre>
 * 
 * <p>
 * 
 * The "to" and "from" methods are preferred, because some conversions are not
 * simply multiplying a factor, but involve an offset also. Those that involve an
 * offset (currently only degreeC and degreeF), must use the "to" and "from"
 * methods. Forms such as <tt>double temp = 32.0 * degreeF;</tt> are
 * <i>always</i> wrong.
 * 
 * <li>The Units class does not define a "pound". The problem is that in most
 * official standards documents, a pound is a unit of mass; however, in most
 * science and engineering texts, a pound is a unit of force. We avoid the
 * problem by not defining a pound at all, and instead defining a pound_mass
 * (lbm) and a pound_force (lbf).
 * <p>
 * 
 * <li>There are many more units defined as strings in the Units class than the
 * number defined as class fields in the Units class. To get a complete list of
 * all units defined as a strings call the {@link #getAllUnits()} method.
 * <p>
 * 
 * <li>The internal unit for angles is always radians. The SI convention is
 * followed by calling this a dimensionless derived unit, defined as m / m.
 * <p>
 * 
 * Since angles are always represented in radians, a user of the Units class
 * does not need to perform unit conversions to get radians for trigonometric
 * functions. Expressions like
 * <p>
 * <tt>a = Math.cos(Units.from("radian", pi_over_2));</tt>
 * <p>
 * are not necessary, but are still valid. Expressions that use degrees still
 * need units specified, for example:
 * <p>
 * <tt>double right_angle = Units.from("degree", 90.0);</tt><br>
 * <tt>a = Math.cos(right_angle);</tt>
 * <p>
 * 
 * <li>The units for thermodynamic temperature are degreeC, degreeF, K,
 * and degreeR, representing Celsius, Farenheit, Kelvin, and Rankin.
 * <p>
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
 * <p>
 * 
 * <li>Both joules and newton-meters are defined. These units are so similar
 * that they could be considered the same. They are kept separate because joules
 * are used to represent energy and newton-meters are used to represent torque.
 * Energy is a scalar quantity and torque is a vector. It is rare that they are
 * interchanged. The US Customary unit for energy is the foot-pound (ft-lbf),
 * and the unit for torque is the pound-foot (lbf-ft). This last name is our
 * invention.
 * <p>
 * 
 * <li>Both names <i>metre</i> and <i>meter</i> and <i>litre</i> and
 * <i>liter</i> are defined. Each pair is identical. In the approved English
 * language version of the SI standard, only the terms "metre" and "litre" are
 * defined. However, in the US version of this document, prepared by the
 * National Institute of Standards and Technology (Special Pub. 330) using
 * current US government policy, the only defined terms are "meter" and "liter."
 * The Units class defines both; however, the reader will observe a bias in the
 * documentation in favor of the US terms.
 * </ul>
 * 
 * 
 * Special Constants:
 * <p>
 * 
 * <ul>
 * <li>The Units class defines <i>gn</i> as the adopted physical constant of
 * gravity. It is given (out of place) in the Units class, due to the fact that
 * the definition of the fundamental US customary unit of mass (slug) depends on
 * this quantity.
 * <p>
 * 
 * This quantity, Units.gn, is defined in IEEE/ASTM SI 10-1997 as (exactly)
 * 9.80665 m/s^2 [page 25]. The 1962 U.S. Standard Atmosphere [page 4] calls
 * this quantity "standard sea-level gravity" and describes it as the
 * acceleration due to the combined effects of gravity and the Earth's rotation
 * (i.e. the centrifugal relief) at geodetic latitude 45 degrees. This constant
 * was adopted in the US in 1935.
 * <p>
 * 
 * The latitude of 45 degrees was chosen because this is the standard latitude
 * chosen by the World Meteorological Organization to calibrate barometers. As a
 * note, this value is the solution to Lambert's equation (sea-level gravity as
 * a function of latitude), when the latitude is 45 degrees 32 minutes and 33
 * seconds.
 * 
 * <li>The Units class defines Units.P0 as the adopted physical constant of
 * standard atmosphere. This quantity equals 101325 Pa. This definition is added
 * to the Units class because the definition of an atmosphere unit ("atm")
 * depends on it. This definition comes from NIST Special Publication 330, the
 * International System of Units (SI), 1991 edition, p15.
 * </ul>
 * 
 * Implementation Notes:
 * <p>
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
 * <p>
 * 
 * <li>The purpose of this class is geared to modeling large engineered systems
 * such as airplanes. As such, many units useful in other disciplines are not
 * defined (such as the "carat" or a "bushel"). However, they may be defined in
 * future versions of this class.
 * <p>
 * 
 * Please note that since the complete set of seven base units are defined, any
 * other units needed may be defined by the user with the {@link #addUnit
 * addUnit()} method.
 * <p>
 * 
 * <li>Much care was used in defining each of these conversion factors. The full
 * precision from appropriate standards documents was used. However, some units
 * are defined as the arithmetic relationship between 'more fundamental'
 * quantities. In these cases, the conversion factors are limited to the resolution
 * of double precision operations. For most engineering purposes
 * (see the point above) this is more than good enough; however, someone working
 * with very precise quantities (&gt; 12 significant digits), should be aware of
 * the possibility of differences.
 * <p>
 * 
 * <li>The names of many of the conversion factors violate standard naming
 * conventions for class variables. This was deemed acceptable since these names
 * follow the standard capitalization conventions from SI or other standards
 * bodies.
 * <p>
 * 
 * <li>The Units class is not <i>immutable</i>. However modification of this
 * class is limited. Specifically, there is no mechanism to <i>delete</i> a unit from
 * the Units class, nor is there any ability to <i>change</i> a conversion factor of
 * a unit. These restrictions allow the user to be confident that once a unit is
 * in the Units class, its definition will never change. The unit <b>A</b> will
 * always be an ampere, it will never become an angstrom.
 * <p>
 * 
 * </ul>
 * 
 * References:
 * <p>
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

public final class Units {

	public static class UnitException extends RuntimeException {
		static final long serialVersionUID = 0;

		UnitException(String name) {
			super(name);
		}
	};

	// Base units (time, distance, mass, temp)

	/**
	 * a "unit" to indicate this quantity has no dimensionality, it always
	 * equals unity (1.0).
	 */
	public static final double unitless = 1.0;

	/**
	 * a "unit" to indicate this quantity has dimensionality but that it is
	 * unknown, it always equals unity (1.0).
	 */
	public static final double unspecified = 1.0;

	/** a "unit" to indicate that no conversion should take place. */
	public static final double internal = 1.0;

	/** base unit of electric current (SI and US) */
	public static final double ampere;
	/** ampere, base unit of electric current (SI and US) */
	public static final double A;

	/** base unit of amount of substance (SI and US) */
	public static final double mole;

	/** base unit of luminous intensity (SI and US) */
	public static final double candela;
	/** candela, base unit of luminous intensity (SI and US) */
	public static final double cd;

	/** base unit of time (SI and US) */
	public static final double second;
	/** second, base unit of time (SI and US) */
	public static final double s;

	/** base unit of distance (SI) */
	public static final double meter;
	/** base unit of distance (SI) */
	public static final double metre;
	/** meter, base unit of distance (SI) */
	public static final double m;

	/**
	 * base unit of distance (US). This is an international foot, not a US
	 * survey foot. They differ by 2 parts in a million.
	 */
	public static final double foot;
	/**
	 * foot, base unit of distance (US). This is an international foot, not a US
	 * survey foot. They differ by 2 parts in a million.
	 */
	public static final double ft;

	/** base unit of mass (SI) */
	public static final double kilogram;
	/** kilogram, base unit of mass (SI) */
	public static final double kg;

	/** base unit of mass (US) */
	public static final double slug;

	/** base unit of thermodynamic temperature (SI) */
	public static final double kelvin;
	/** kelvin, base unit of thermodynamic temperature (SI) */
	public static final double K;
	/**
	 * Degrees kelvin. This unit
	 * represents the base unit of thermodynamic temperature (SI). <p>  
	 * According to NIST special publication 330 (p29) this unit
	 * should not be called "degrees kelvin", but rather "kelvin". We hope the
	 * reader is not too upset that consistency with other units of
	 * thermodynamic temperature was favored over strict correctness. Note
	 * the unit 'K' is also defined.
	 */
	public static final double degreeK;

	/**
	 * Degrees Rankine, base unit of thermodynamic temperature (US) on an
	 * absolute temperature scale.
	 */
	public static final double degreeR;

	public static final String degreeStr = "\u00B0";
	public static final String degreeCStr = "\u00B0C";
	public static final String degreeFStr = "\u00B0F";
	public static final String degreeRStr = "\u00B0R";


	// These aren't fundamental, but they are useful

	/**
	 * unit of mass. This name violates the convention of using standardized
	 * names for all units. In literature written by standards bodies concerned
	 * with unit definitions, this unit is almost exclusively referred to as the
	 * "pound." However, most literature in science and engineering, refers to
	 * this quantity as "pound-mass" and refers to a similar unit of force as a
	 * "pound." To avoid confusion, the Units class does not define a "pound" at
	 * all; it only defines "pound_mass" and "pound_force" with their respective
	 * abbreviations "lbm" and "lbf."
	 */
	public static final double pound_mass;
	/** pound of mass, a unit of mass. */
	public static final double lbm;
	/**
	 * unit of force. This name violates the convention of using standardized
	 * names for all units. In literature written by standards bodies concerned
	 * with unit definitions this unit is almost exclusively referred to as the
	 * "pound-force." However, most literature in science and engineering,
	 * refers to this quantity as a "pound" and refers to a similar unit of mass
	 * as a "pound-mass." To avoid confusion, the Units class does not define a
	 * "pound" at all; it only defines "pound_mass" and "pound_force" with their
	 * respective abbreviations "lbm" and "lbf."
	 */
	public static final double pound_force;
	/** pound of force, a unit of force. */
	public static final double lbf;

	/** meters per second squared, a unit of acceleration */
	public static final double meter_per_second2;
	private static final double foot_per_second2;

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
	public static final double gn;

	static {

		// Define some of the base units

		ampere = 1.0;
		mole = 1.0;
		candela = 1.0;

		second = 1.0;
		metre = 1.0;
		foot = 0.3048;

		meter_per_second2 = metre / (second * second);
		foot_per_second2 = foot / (second * second);

		// "gn" is the gravitational acceleration at 45 deg lat, sea-level

		gn = 9.80665 * meter_per_second2; // gn in internal units

		// the remaining base units.

		kilogram = 1.0;
		pound_mass = 0.45359237 * kilogram;
		slug = pound_mass * gn / foot_per_second2;
		kelvin = 1.0;
		degreeR = 1.0 / 1.8;

		pound_force = slug * foot_per_second2;

		// Define some abbreviations

		A = ampere;
		cd = candela;
		meter = metre;
		m = metre;
		s = second;
		ft = foot;
		kg = kilogram;
		lbm = pound_mass;
		lbf = pound_force;
		K = kelvin;
		degreeK = kelvin;

	}

	// Derived units

	/** derived unit for angles */
	public static final double radian = 1.0;
	/** unit for angles */
	public static final double degree = Math.PI / 180.;
	/** unit for angles */
	public static final double deg = degree;
	/** radian, derived unit for angles */
	public static final double rad = radian;

	/**
	 * unit of frequency (cycles per second). This unit is <b>NOT</b> compatible
	 * with the unit {@link #radian_per_second} and they should not be used
	 * together. Use {@link #hertzAngular Units.hertzAngular} instead
	 */
	public static final double hertz = 1.0 / second;
	/**
	 * hertz, unit of frequency (cycles per second). This unit is <b>NOT</b>
	 * compatible with the unit {@link #radian_per_second} and they should not
	 * be used together. Use {@link #hertzAngular Units.hertzAngular} instead
	 */
	public static final double Hz = hertz;
	/**
	 * unit of angular velocity. This unit is <b>NOT</b> the same as
	 * {@link #hertz} and they should not be used interchangably. If you want to
	 * use a quantity that is compatible with radian_per_second and is expressed
	 * in "hertz" use {@link #hertzAngular} instead.
	 */
	public static final double radian_per_second = radian / second;
	/** unit of angular velocity. */
	public static final double degree_per_second = degree / second;
	/** unit of angular velocity. */
	public static final double deg_per_second = degree_per_second;
	/**
	 * unit of a angular velocity, where one "cycle" is a complete rotation of
	 * the circle (2*PI). Therefore, this quantity is defined as 2 * PI *
	 * radian_per_second. This is a very non-standard unit and it is defined
	 * because some people want a unit (that they often, incorrectly, call
	 * "hertz") that is compatible with radians per second. In reality,
	 * hertzAngular is an angular velocity not a frequency.
	 */
	public static final double hertzAngular = 2 * Math.PI * radian_per_second;

	/** nanosecond, a unit of time. Equal to 1.0 x 10^-9 of a second */
	public static final double ns = 1.0e-9 * second;
	/** unit of time, equals 60.0 seconds */
	public static final double minute = 60.0 * second;
	/** minute, unit of time, equals 60.0 seconds */
	public static final double min = minute;

	/** unit of time, equals 60.0 minutes */
	public static final double hour = 60.0 * minute;

	/** unit of distance */
	public static final double kilometer = 1000.0 * metre;
	/** unit of distance */
	public static final double kilometre = 1000.0 * metre;
	/** kilometer, unit of distance */
	public static final double km = kilometre;

	/** millimeter, unit of distance */
	public static final double mm = metre / 1000.0;

	/** unit of distance, equals 1/12.0 of a foot */
	public static final double inch = foot / 12.0;
	/** inch, unit of distance, equals 1/12.0 of a foot */
	public static final double in = inch;

	/**
	 * unit of distance, equals 5280.0 feet. This is the international mile
	 * <i>not</i> a statute mile. The international mile differs from the
	 * statute mile by 2 parts per million. The statute mile is not defined in
	 * the Units class.
	 */
	public static final double mile = 5280.0 * foot;
	/**
	 * a mile, unit of distance, equals 5280.0 feet. This is the international
	 * mile not a statute mile. The international mile differs from the statute
	 * mile by 2 parts per million. The statute mile is not defined in the Units
	 * class.
	 */
	public static final double mi = mile;

	/** unit of distance (International nautical mile), equals 1852.0 meters */
	public static final double nautical_mile = 1852.0 * metre;

	/**
	 * International nautical mile, unit of distance, equals 1852.0 meters.
	 * There is no internationally accepted abbreviation for a nautical
	 * mile. Symbols that have been used include M, NM, Nm, nm, and nmi. NM
	 * seems to be the preferred abbreviation from the International Civil
	 * Aviation Organization (ICAO). The abbreviation nm should not
	 * be used, because it could be confused with nanometers.
	 */
	public static final double NM = nautical_mile;

	/** metres squared, a unit of area */
	public static final double metre2 = metre * metre;
	/** meters squared, a unit of area */
	public static final double meter2 = metre * metre;
	/** feet squared, a unit of area */
	public static final double foot2 = foot * foot;
	/** inches squared, a unit of area */
	public static final double inch2 = inch * inch;

	/** metres cubed, a unit of volume */
	public static final double metre3 = metre * metre * metre;
	/** meters cubed, a unit of volume */
	public static final double meter3 = metre * metre * metre;

	/** a unit of volume, equals 1/1000.0 of a meter cubed */
	public static final double litre = metre3 / 1000.0;
	/** a unit of volume, equals 1/1000.0 of a meter cubed */
	public static final double liter = meter3 / 1000.0;
	/** liters, a unit of volume, equals 1/1000.0 of a meter cubed */
	public static final double L = litre;

	/** feet cubed, a unit of volume */
	public static final double foot3 = foot * foot * foot;
	/** inches cubed, a unit of volume */
	public static final double inch3 = inch * inch * inch;
	/** a unit of volume. The US liquid gallon. */
	public static final double gallon = 231.0 * inch3;
	/** US liquid gallons, a unit of volume */
	public static final double gal = gallon;

	/**
	 * acceleration of gravity, used as a unit of acceleration, see definition
	 * of Units.gn
	 */
	public static final double G = gn;

	/** unit of force */
	public static final double newton = kilogram * meter_per_second2;
	/** Newton, unit of force */
	public static final double N = newton;

	/** unit of energy and work, defined as a Newton * meter */
	public static final double joule = newton * metre;
	/** joule, unit of energy and work, defined as a Newton * meter */
	public static final double J = joule;
	/** unit of energy and work, defined as a foot * pound_force */
	public static final double foot_pound_force = foot * pound_force;

	/** unit of power, defined as a joule per second */
	public static final double watt = joule / second;
	/** watt, unit of power, defined as a joule per second */
	public static final double W = watt;
	/** potential difference or electromotive force */
	public static final double volt = W / A;
	/** Volt, potential difference or electromotive force */
	public static final double V = volt;
	/** electric charge */
	public static final double coulomb = A * s;
	/** Coulomb, electric charge */
	public static final double C = coulomb;
	/** electric resistence */
	public static final double ohm = V / A;
	/** capacitance */
	public static final double farad = A * s / V;
	/** Farad, capacitance */
	public static final double F = farad;

	/** unit of velocity, defined as a meter per second */
	public static final double meter_per_second = meter / second;
	/** unit of velocity, defined as a kilometer per hour */
	public static final double kilometer_per_hour = km / hour;

	/**
	 * unit of velocity, defined as a nautical mile per hour. A widely accepted
	 * abbreviation for knot is "kn", see NIST SP330-2008.
	 */
	public static final double knot = nautical_mile / hour;

	/**
	 * miles per hour, a unit of velocity, defined as a mile per hour
	 */
	public static final double mph = mile / hour;
	/** feet per minute, unit of velocity */
	public static final double fpm = foot / minute;

	/** pounds-force per square inch, a unit of pressure */
	public static final double psi = pound_force / inch2;
	/** pounds-force per square foot, a unit of pressure */
	public static final double psf = pound_force / foot2;
	/** unit of pressure, defined as a newton per meter squared */
	public static final double pascal = newton / meter2;
	/** pascal, unit of pressure, defined as a newton per meter squared */
	public static final double Pa = pascal;
	/**
	 * P0, the adopted standard atmosphere. This quanity equals 101325 Pa. This
	 * definition comes from NIST Special Publication 330, the International
	 * System of Units (SI), 1991 edition, p15.
	 */
	public static final double P0 = 101325.0 * Pa;
	/**
	 * millimeters of mercury (at 32 degrees Fahrenheit), a unit of pressure
	 * equal to 133.3224 Pascals (see NIST publication 811). It approximately
	 * equals 1/760.0 of Units.P0;
	 */
	public static final double mmHg = 133.3224 * Pa;
	/**
	 * inches of mercury (at 32 degrees Fahrenheit), a unit of pressure
	 * approximately equal to 1/29.92 of P0. Actually, it equals 25.4 * mmHg.
	 */
	public static final double inHg = mmHg * in / mm;
	/** atmosphere, a unit of pressure, defined as one P0 */
	public static final double atm = 1.0 * P0;
	/**
	 * bar, a unit of pressure, defined as 100 000.0 Pascals
	 */
	public static final double bar = 100000.0 * Pa;
	/** millibar, a unit of pressure, defined as one thousandth of a bar */
	public static final double mbar = bar / 1000.0;

	// Flags used to indicate a special function is needed for conversion

	/**
	 * degrees Celsius, a unit of temperature, <b>must</b> use the Units.to and
	 * Units.from methods
	 */
	private static final double degreeC = -1.0;

	/**
	 * degrees Fahrenheit, a unit of temperature, <b>must</b> use the Units.to
	 * and Units.from methods
	 */
	private static final double degreeF = -2.0;

	private final static double C_OFFSET = 273.15;
	private final static double F_OFFSET = 273.15 * 1.8 - 32.0;

	// Setup a table for string names for all supported units
	private static Map<String, UnitPair> unitTable;

	// the compatible units for the "unspecified" unit
	// (aka, all units in this class)
	private static SortedSet<String> unspecifiedUnits;

	/**
	 * This class just helps the Units class. It encapsulates the unit
	 * conversion factor and the list of compatible units. This class is
	 * immutable.
	 */
	private static class UnitPair {
		final double factor;
		final SortedSet<String> compatible;

		UnitPair(double factor, SortedSet<String> compatible) {
			this.factor = factor;
			this.compatible = compatible;
		}
	}

	static {

		// Create the table of unit names

		unitTable = new HashMap<String, UnitPair>(150); // A good default size
		unspecifiedUnits = new TreeSet<String>();

		try {
			addUnit("rad", rad);
			addUnit("deg", deg, "rad");
			addSimilarUnit("radian", "rad");
			addSimilarUnit("degree", "deg");
			addSimilarUnit(degreeStr,"deg");
			addUnit("Hz", hertz);
			addSimilarUnit("hertz", "Hz");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("rad/s", radian / second);
			addUnit("deg/s", degree / second, "rad/s");
			addUnit("hertzAngular", hertzAngular, "rad/s");
			addSimilarUnit("degree/s", "deg/s");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("rad/s^2", radian / (second * second));
			addUnit("deg/s^2", degree / (second * second), "rad/s^2");
			addSimilarUnit("radian/s^2", "rad/s^2");
			addSimilarUnit("degree/s^2", "deg/s^2");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("unitless", unitless);
			addSimilarUnit("none", "unitless");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("s", s);
			addUnit("ns", ns, "s");
			addUnit("min", min, "s");
			addUnit("h", hour, "s");
			addSimilarUnit("second", "s");
			addSimilarUnit("minute", "min");
			addSimilarUnit("hour", "h");
			addSimilarUnit("hr", "h");
			addUnit("ms", 0.001, "s");
			addUnit("d", 24.0, "hour");
			addSimilarUnit("day", "d");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("m", m);
			addUnit("mm", mm, "m");
			addUnit("ft", ft, "m");
			addUnit("km", km, "m");
			addUnit("in", in, "m");
			addUnit("yard", 3.0, "ft");
			addUnit("NM", NM, "m");
			addSimilarUnit("nm", "NM");  // Do not add nm, nm means nanometers, not nautical miles
			addSimilarUnit("nmi", "NM");
			addUnit("mi", mi, "m");
			addSimilarUnit("metre", "m");
			addSimilarUnit("meter", "m");
			addSimilarUnit("foot", "ft");
			addSimilarUnit("feet", "ft");
			addSimilarUnit("inch", "in");
			addSimilarUnit("kilometer", "km");
			addSimilarUnit("kilometre", "km");
			addSimilarUnit("nautical_mile", "nmi");
			addSimilarUnit("mile", "mi");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("kg", kg);
			addUnit("slug", slug, "kg");
			addUnit("lbm", lbm, "kg");
			addSimilarUnit("kilogram", "kg");
			addSimilarUnit("pound_mass", "lbm");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("K", K);
			addSimilarUnit("degreeK", "K");

			//
			// Use a different style here to "addSimilarUnit" because the negative
			// factors for degreeC and degreeF will cause exceptions to be thrown
			//

			UnitPair kUnitPair = getUnitPair("K");

			kUnitPair.compatible.add(Units.degreeCStr); //"degreeC");
			unitTable.put("degreeC",
					new UnitPair(degreeC, kUnitPair.compatible));
			unitTable.put(Units.degreeCStr,
					new UnitPair(degreeC, kUnitPair.compatible));

			kUnitPair.compatible.add(Units.degreeFStr); //"degreeF");
			unitTable.put("degreeF",
					new UnitPair(degreeF, kUnitPair.compatible));
			unitTable.put(Units.degreeFStr,
					new UnitPair(degreeF, kUnitPair.compatible));

			//
			// Back to the regular style
			//
			addUnit(Units.degreeRStr, degreeR, "K");
			addSimilarUnit("degreeR", Units.degreeRStr);

		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("m/s", meter_per_second);
			addSimilarUnit("meter_per_second", "m/s");
			addSimilarUnit("metre_per_second", "m/s");
			addUnit("km/h", kilometer_per_hour, "m/s");
			addSimilarUnit("kph", "km/h");
			addSimilarUnit("kilometer_per_hour", "km/h");
			addSimilarUnit("kilometre_per_hour", "km/h");
			addUnit("ft/s", foot / second, "m/s");
			addSimilarUnit("fps", "ft/s");
			addSimilarUnit("foot_per_second", "ft/s");
			addSimilarUnit("feet_per_second", "ft/s");
			addSimilarUnit("feet/second", "ft/s");
			addUnit("knot", knot, "m/s");
			addSimilarUnit("kn", "knot");
			addSimilarUnit("kts", "knot");
			addUnit("mph", mph, "m/s");
			addUnit("ft/min", fpm, "m/s");
			addSimilarUnit("fpm", "ft/min");
			addSimilarUnit("foot_per_minute", "ft/min");
			addSimilarUnit("feet_per_minute", "ft/min");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("m/s^2", meter_per_second2);
			addUnit("ft/s^2", foot / (second * second), "m/s^2");
			addUnit("G", G, "m/s^2");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("m^2", meter2);
			addUnit("ft^2", foot2, "m^2");
			addUnit("in^2", inch2, "m^2");
			addSimilarUnit("meter2", "m^2");
			addSimilarUnit("metre2", "m^2");
			addSimilarUnit("foot2", "ft^2");
			addSimilarUnit("inch2", "in^2");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("m^3", meter3);
			addUnit("ft^3", foot3, "m^3");
			addUnit("in^3", inch3, "m^3");
			addUnit("L", L, "m^3");
			addUnit("gal", gal, "m^3");
			addSimilarUnit("meter3", "m^3");
			addSimilarUnit("metre3", "m^3");
			addSimilarUnit("foot3", "ft^3");
			addSimilarUnit("gallon", "gal");
			addSimilarUnit("liter", "L");
			addSimilarUnit("litre", "L");
			addSimilarUnit("inch3", "in^3");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("N", N);
			addUnit("lbf", lbf, "N");
			addSimilarUnit("newton", "N");
			addSimilarUnit("pound_force", "lbf");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("J", J);
			addSimilarUnit("joule", "J");
			addUnit("ft-lbf", foot * pound_force, "J");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		// Note these are torques, even though they have the same unit
		// as energy.
		try {
			addUnit("N-m", newton * meter);
			addUnit("lbf-ft", pound_force * foot, "N-m");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("kg-m^2", kilogram * meter2);
			addUnit("slug-ft^2", slug * foot2, "kg-m^2");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("A", A);
			addSimilarUnit("ampere", "A");
			addUnit("W", W);
			addSimilarUnit("watt", "W");
			addUnit("V", V);
			addSimilarUnit("volt", "V");
			addUnit("C", C);
			addSimilarUnit("coulomb", "C");
			addUnit("ohm", ohm);
			addUnit("F", F);
			addSimilarUnit("farad", "F");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		try {
			addUnit("Pa", Pa);
			addUnit("psi", psi, "Pa");
			addUnit("psf", psf, "Pa");
			addUnit("atm", atm, "Pa");
			addUnit("mmHg", mmHg, "Pa");
			addUnit("inHg", inHg, "Pa");
			addUnit("bar", bar, "Pa");
			addUnit("mbar", mbar, "Pa");
			addSimilarUnit("pascal", "Pa");

			addUnit("kg/m^3", kilogram / meter3);
			addUnit("slug/ft^3", slug / foot3, "kg/m^3");

			addUnit("kg/m^3-s", kilogram / (meter3 * second));
			addUnit("slug/ft^3-s", slug / foot3, "kg/m^3-s");
		} catch (UnitException e) {
			throw new Error(e.getMessage());
		}

		//
		// Must go last
		//

		Iterator<UnitPair> itr = unitTable.values().iterator();
		while (itr.hasNext()) {
			unspecifiedUnits.addAll((itr.next()).compatible);
		}

		unspecifiedUnits.add("unspecified");
		unitTable.put("unspecified",
				new UnitPair(unspecified, unspecifiedUnits));
		addUnitPair("internal", getUnitPair("unspecified"));
	}

	/** This class should never be instantiated. */
	private Units() {
	}

	/**
	 * Get the UnitPair for a given string unit.
	 */
	private static synchronized UnitPair getUnitPairInternal(String unit) {
		UnitPair pair;

		// if (unit.charAt(0) == '[' && unit.charAt(unit.length() - 1) == ']') {
		// unit = unit.substring(1, unit.length() - 1);
		// }

		pair = unitTable.get(unit);

		return pair;
	}

	/** Determine if the given string is a valid unit
	 * @param unit string representation of a unit
	 * @return true, if this string is known as a unit in the database of units */
	public static boolean isUnit(String unit) {
		return getUnitPairInternal(unit) != null;
	}

	/**
	 * Clean up the unit string that may contain brackets or extra space. For
	 * instance, " [ feet]" becomes "feet". However, if the unit is "[  fred]"
	 * then "fred" will be returned (unlike the clean() method).
	 */
	private static String cleanOnly(String unit) {
		StringBuilder sb = new StringBuilder(unit);
		String ut;
		trimBuilder(sb);


		int start_idx = sb.indexOf("[");
		if (start_idx >= 0) {
			sb.replace(0,start_idx+1,"");		  
		}
		int end_idx = sb.indexOf("]");
		if (end_idx >= 0) {
			sb.replace(end_idx,sb.length()+1,"");		  
		}
		trimBuilder(sb);

		ut = sb.toString();


		//	  if (sb.length() > 2) {
		//		  if (sb.charAt(0) == '[' && sb.charAt(sb.length() - 1) == ']') {
		//			  sb.deleteCharAt(0);
		//			  sb.deleteCharAt(sb.length() - 1);
		//			  trimBuilder(sb);
		//			  ut = sb.toString();
		//		  }
		//	  }
		return ut;
	}

	/**
	 * Clean up the unit string that may contain brackets or extra space. For
	 * instance, " [ feet]" becomes "feet". If the string in brackets is not a
	 * recognized, then "unspecified" is returned.
	 * @param unit a "dirty" string
	 * @return a cleaned-up unit string
	 */
	public static String clean(String unit) {
		String ut = Units.cleanOnly(unit);
		if (isUnit(ut)) {
			return ut;
		} else {
			return "unspecified";
		}
	}

	private static void trimBuilder(StringBuilder sb) {
		while (sb.length() > 0 && Character.isWhitespace(sb.charAt(0))) {
			sb.deleteCharAt(0);
		}
		while (sb.length() > 0
				&& Character.isWhitespace(sb.charAt(sb.length() - 1))) {
			sb.deleteCharAt(sb.length() - 1);
		}
	}

	/**
	 * Get the UnitPair for a given string unit.
	 */
	private static UnitPair getUnitPair(String unit) throws UnitException {
		if (unit == null) {
			throw new UnitException("A 'null' was given for a unit name.");
		}

		UnitPair pair;

		pair = getUnitPairInternal(unit);

		if (pair == null) {
			throw new UnitException("Unknown unit '" + unit + "'");
		}

		return pair;
	}

	/**
	 * Find the conversion factor for the given name. No computation should be
	 * performed on the factor that is returned. Generally, the factor should be
	 * passed directly to a <b>to</b> or <b>from</b> method.
	 * <p>
	 * 
	 * This method is primarily used as a performance enhancement (and so it
	 * should be used carefully, and when in doubt, not at all). The idea is if
	 * one wants to convert several values of the same unit instead of
	 * converting each individually (and incuring the string to value lookup
	 * penality each time), perform the string lookup once (using this method),
	 * then convert each value with the returned factor.
	 * <p>
	 * 
	 * Unexpected behavior may result if the unit is "degreeC" or "degreeF".
	 * <p>
	 * 
	 * @param unit
	 *            the string name for a unit. Name can be one of the elements
	 *            returned from the {@link #getAllUnits()} method.
	 * @return the factor used to convert from the given unit to internal units.
	 *         It can be used as the factor parameter in the <b>to</b> and
	 *         <b>from</b> methods
	 * 
	 * @exception UnitException
	 *                Thrown if the unit name is not known to the Units class or
	 *                if it is invalid.
	 */
	public static double getFactor(String unit) throws UnitException {
		return (getUnitPair(unit)).factor;
	}

	/**
	 * Add a UnitPair to the list of known units. Perform various error checking
	 * functions.
	 * 
	 * @param name
	 *            the new unit
	 * @param factor
	 *            the new conversion factor
	 * @param base
	 *            the UnitPair to which the new unit is compatible. If null,
	 *            then the new unit is not compatible with any other units.
	 * @exception UnitException
	 *                Thrown if the new unit is already defined in the Units
	 *                class, if the base unit is "unspecified", if the base unit
	 *                is not known to the Units class, or if the factor supplied
	 *                is negative.
	 *                <p>
	 * 
	 */
	private static synchronized boolean addUnitPair(String name,
			UnitPair newPair) throws UnitException {

		if (newPair.factor <= 0.0) {
			throw new UnitException("The conversion factor was not "
					+ "positive, this is invalid");
		}

		if (name.matches(".*\\s.*")) {
			throw new UnitException("A unit name cannot contain spaces.");			
		}

		if (unitTable.containsKey(name)) {
			UnitPair pair = getUnitPair(name);

			if (newPair.compatible != pair.compatible) {
				throw new UnitException("A unit already exists with the "
						+ "name '" + name + "' but it has a different "
						+ "dimensionality than the new "
						+ "unit. This is not allowed.");
			}
			// note that the two factors must be identical, not
			// close, but identical.
			if (newPair.factor != pair.factor) {
				throw new UnitException("'" + name
						+ "' has already been defined "
						+ "as a unit and the new factor "
						+ "does not equal the old factor. "
						+ "Units cannot be redefined.");
			}

			return false;
		}

		unitTable.put(name, newPair);
		return true;
	}

	/**
	 * Add a new UnitPair to the list of known units. This also adds this unit
	 * to the list of compatible units for the "unspecified" unit.
	 * 
	 * @param name
	 *            the new unit
	 * @param factor
	 *            the new conversion factor
	 * @param base
	 *            the UnitPair to which the new unit is compatible. If null,
	 *            then the new unit is not compatible with any other units.
	 * @exception UnitException
	 *                Thrown if the new unit is already defined in the Units
	 *                class, if the base unit is "unspecified", if the base unit
	 *                is not known to the Units class, or if the factor supplied
	 *                is negative.
	 *                <p>
	 * 
	 */
	private static synchronized void addNewUnitPair(String name,
			UnitPair newPair) throws UnitException {
		if (addUnitPair(name, newPair)) {
			unspecifiedUnits.add(name);
			newPair.compatible.add(name);
		}
	}

	/**
	 * Give another name to a unit. For instance, "meter" and "metre" or "foot"
	 * and "ft". When two units are truely identical, this method is preferred
	 * over {@link #addUnit(String, double, String)} because less storage will
	 * be allocated and the list of compatible units will only include one name
	 * (which is probably the desired behavior). The convention is to make the
	 * common abbreviation (m, ft, s, etc.) the defined unit, and the expanded
	 * names (meter, foot, second, etc.) "similar" units.
	 * 
	 * @param newName
	 *            the new name for the unit.
	 * @param originalName
	 *            the name of the original unit
	 * @exception UnitException
	 *                Thrown if the originalName is "unspecified", if the
	 *                originalName is not known to the Units class, or if the
	 *                unit indicated by newName already exists.
	 */
	public static void addSimilarUnit(String newName, String originalName)
			throws UnitException {

		if (originalName.equals("unspecified")) {
			throw new UnitException("Cannot make any units similar to "
					+ "'unspecified'");
		}

		addUnitPair(newName, getUnitPair(originalName));
	}

	/**
	 * Create a composite unit. For example a <i>linear density</i> unit could
	 * be defined as:<br>
	 * <tt>
	 * Units.addUnit("kg/m", Units.kg / Units.meter);</tt>
	 * <p>
	 * 
	 * This method will not make the new unit compatible with any other unit.
	 * See the {@link #getCompatibleUnits} method
	 * <p>
	 * 
	 * @param name
	 *            the string name for unit being created.
	 * @param factor
	 *            the conversion factor
	 * @exception UnitException
	 *                Thrown if either the name supplied is already defined, or
	 *                if the factor supplied includes a factor with an offset
	 *                bias (Units.C or Units.F).
	 *                <p>
	 */
	public static void addUnit(String name, double factor) throws UnitException {

		addNewUnitPair(name, new UnitPair(factor, new TreeSet<String>()));
	}

	/**
	 * Create a composite unit. For example a <i>fathom</i> unit could be
	 * defined as:<br>
	 * <tt> Units.addUnit("fathom", 6.0, "foot"); </tt><br>
	 * This method says that one fathom is equal to 6.0 feet and that the new
	 * unit is compatible with "foot".
	 * <p>
	 * An alternate use for this method is to define a new unit that is
	 * compatible with another unit. For instance, <br>
	 * <tt> Units.addUnit("knot/s", Units.knot / Units.s, "m/s^2"); </tt><br>
	 * This example creates a new acceleration unit: knots per second.
	 * In this approach, the base unit <b>must</b> be formed only from the core
	 * SI units (m, s, kg, etc.), no other units and no numeric values; however,
	 * units that are formed from the core SI units (like Newton, Joule, or
	 * Pascal) are allowed.
	 * <p>
	 * In no case should the base unit ever be "degreeC" or "degreeF".
	 * 
	 * @param name
	 *            the string name for unit being created.
	 * @param factor
	 *            the conversion factor
	 * @param base
	 *            unit that the new unit is based on.
	 * @exception UnitException
	 *                Thrown if the new unit is already defined in the Units
	 *                class, if the base unit is "unspecified", if the base unit
	 *                is not known to the Units class, or if the factor supplied
	 *                is negative.
	 *                <p>
	 * 
	 */
	public static void addUnit(String name, double factor, String base)
			throws UnitException {
		if (base.equals("unspecified") || base.equals("degreeC") || base.equals("degreeF")) {
			throw new UnitException("Cannot use unit \""+base+"\" as a base unit.");
		}

		UnitPair basePair = getUnitPair(base);

		addNewUnitPair(name, new UnitPair(factor * basePair.factor,
				basePair.compatible));
	}

	/**
	 * Returns the list of units that have the same dimensionality as the given
	 * unit.
	 * <p>
	 * 
	 * @param unit a unit
	 * @return a list of compatible units
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static synchronized String[] getCompatibleUnits(String unit)
			throws UnitException {
		return getUnitPair(unit).compatible.toArray(new String[0]);
	}

	/**
	 * Determines if these two units are compatible with one another. That is,
	 * if they have the same dimensionality (for instance, both are units of
	 * distance) then true is returned. Note, the unit "unspecified" is
	 * compatible with all units, including "unitless" which is not compatible
	 * with any other units.
	 * 
	 * @param unit1 one unit
	 * @param unit2 another unit
	 * @return true if the given two units are compatible
	 * @exception UnitException
	 *                if the unit is not found.
	 */
	public static boolean isCompatible(String unit1, String unit2)
			throws UnitException {
		UnitPair p1 = getUnitPair(unit1);
		UnitPair p2 = getUnitPair(unit2);

		if (unit1.equals("unspecified") || unit2.equals("unspecified")) {
			return true;
		}

		return p1.compatible == p2.compatible;
	}

	/**
	 * Return the internal (factor 1.0) unit compatable with this unit
	 * @param unit unit to check
	 * @return unit with factor 1.0 that is compatible with this unit.  In the event the unit is recognized but has no factor 1.0 equivalent, this will return the empty string.
	 * This will throw a UnitException if the unit is not recognized.
	 */
	public static String getCompatibleInternalUnit(String unit) {
		String[] compat = getCompatibleUnits(unit);
		for (int i = 0; i < compat.length; i++) {
			if (getFactor(compat[i]) == 1.0) {
				return compat[i];
			}
		}
		return "";
	}

	/**
	 * Returns the list of all units currently registered to the Units class and
	 * returns this list in sorted order.<p>
	 * @return list of units
	 */
	public static String[] getAllUnits() {
		Set<String> set;

		synchronized (Units.class) {
			set = unitTable.keySet();
		}

		List<String> list = new ArrayList<String>(set);

		Collections.sort(list);

		String[] a = new String[list.size()];
		list.toArray(a);
		return a;
	}

	/**
	 * Convert <b>to</b> the units indicated by the parameter "factor" from
	 * internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the value (in internal units) to be converted
	 * @return the parameter "value" converted from internal units to the units
	 *         specficied by factor
	 * @exception UnitException
	 *                Thrown if the conversion factor is invalid.
	 */
	public static double to(double factor, double value) throws UnitException {

		if (factor <= 0.0) {
			if (factor == degreeC) {
				return (value / K - C_OFFSET);
			} else if (factor == degreeF) {
				return (value / degreeR - F_OFFSET);
			} else {
				throw new UnitException("Invalid conversion factor");
			}
		}

		return (value / factor);
	}

	/**
	 * Convert <b>to</b> the units indicated by the parameter "unit" from
	 * internal units. Performance Note: this function is relatively slow since
	 * it involves finding a string in a list.
	 * <p>
	 * 
	 * @param unit
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s".
	 * @param value
	 *            the value (in internal units) to be converted
	 * @return the parameter "value" converted from internal units to the units
	 *         specficied by factor
	 * @exception UnitException
	 *                Thrown if the supplied unit is invalid.
	 */
	public static double to(String unit, double value) throws UnitException {
		return to(getFactor(unit), value);
	}

	/**
	 * Convert the values in the given array <b>to</b> the units indicated by
	 * the parameter "factor" from internal units.
	 * <p>
	 * 
	 * @param unit
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static void to(String unit, double[] value) throws UnitException {
		to(getFactor(unit), value);
	}


	/**
	 * Convert the values in the given array <b>to</b> the units indicated by
	 * the parameter "factor" from internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                if the conversion factor is invalid.
	 */
	public static void to(double factor, double[] value) throws UnitException {

		if (factor <= 0.0) {
			for (int i = 0; i < value.length; i++) {
				value[i] = to(factor, value[i]);
			}
		} else {
			for (int i = 0; i < value.length; i++) {
				value[i] /= factor;
			}
		}
	}

	/**
	 * Convert the values in the given array <b>to</b> the units indicated by
	 * the parameter "factor" from internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static void to(String factor, double[][] value) throws UnitException {
		to(getFactor(factor), value);
	}

	/**
	 * Convert the values in the given array <b>to</b> the units indicated by
	 * the parameter "factor" from internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the conversion factor is invalid.
	 */
	public static void to(double factor, double[][] value) throws UnitException {

		if (factor <= 0.0) {
			for (int i = 0; i < value.length; i++) {
				to(factor, value[i]);
			}
		} else {
			for (int i = 0; i < value.length; i++) {
				for (int j = 0; j < value[i].length; j++) {
					value[i][j] /= factor;
				}
			}
		}
	}

	/**
	 * Convert the values in the given array <b>to</b> the units indicated by
	 * the parameter "factor" from internal units.
	 * <p>
	 * 
	 * @param unit
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static void to(String unit, double[][][] value) throws UnitException {
		to(getFactor(unit), value);
	}

	/**
	 * Convert the values in the given array <b>to</b> the units indicated by
	 * the parameter "factor" from internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the conversion factor is invalid.
	 */
	public static void to(double factor, double[][][] value) throws UnitException {

		if (factor <= 0.0) {
			for (int i = 0; i < value.length; i++) {
				to(factor, value[i]);
			}
		} else {
			for (int i = 0; i < value.length; i++) {
				for (int j = 0; j < value[i].length; j++) {
					for (int k = 0; j < value[i].length; j++) {
						value[i][j][k] /= factor;
					}
				}
			}
		}
	}

	/**
	 * Convert <b>from</b> the units indicated by "units" to internal units.
	 * <p>
	 * 
	 * @param units
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the value (in units specfied by the parameter "units") to be
	 *            converted
	 * @return the parameter "value" converted to internal units
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static double from(String units, double value) throws UnitException {
		return from(getFactor(units), value);
	}  

	/**
	 * Convert <b>from</b> the units indicated by "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the value (in units specfied by the parameter "factor") to be
	 *            converted
	 * @return the parameter "value" converted to internal units
	 * @exception UnitException
	 *                Thrown if the conversion factor is invalid.
	 */
	public static double from(double factor, double value) throws UnitException {

		if (factor <= 0.0) {
			if (factor == degreeC) {
				return ((value + C_OFFSET) * K);
			} else if (factor == degreeF) {
				return ((value + F_OFFSET) * degreeR);
			} else {
				throw new UnitException("Invalid conversion factor");
			}
		}
		return (value * factor);
	}


	/**
	 * Convert the values in the given array <b>from</b> the units indicated by
	 * "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static void from(String factor, double[] value) throws UnitException {
		from(getFactor(factor), value);
	}

	/**
	 * Convert the values in the given array <b>from</b> the units indicated by
	 * "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                if the conversion factor is invalid.
	 */
	public static void from(double factor, double[] value) throws UnitException {

		if (factor <= 0.0) {
			for (int i = 0; i < value.length; i++) {
				value[i] = from(factor, value[i]);
			}
		} else {
			for (int i = 0; i < value.length; i++) {
				value[i] *= factor;
			}
		}
	}

	/**
	 * Convert the values in the given array <b>from</b> the units indicated by
	 * "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static void from(String factor, double[][] value)
			throws UnitException {
		from(getFactor(factor), value);
	}

	/**
	 * Convert the values in the given array <b>from</b> the units indicated by
	 * "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the conversion factor is invalid.
	 */
	public static void from(double factor, double[][] value)
			throws UnitException {

		if (factor <= 0.0) {
			for (int i = 0; i < value.length; i++) {
				from(factor, value[i]);
			}
		} else {
			for (int i = 0; i < value.length; i++) {
				for (int j = 0; j < value[i].length; j++) {
					value[i][j] *= factor;
				}
			}
		}
	}

	/**
	 * Convert the values in the given array <b>from</b> the units indicated by
	 * "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static void from(String factor, double[][][] value) throws UnitException {
		from(getFactor(factor), value);
	}

	/**
	 * Convert the values in the given array <b>from</b> the units indicated by
	 * "factor" to internal units.
	 * <p>
	 * 
	 * @param factor
	 *            the unit conversion factor, for example Units.METER
	 * @param value
	 *            the array of values to be converted
	 * @exception UnitException
	 *                Thrown if the conversion factor is invalid.
	 */
	public static void from(double factor, double[][][] value)
			throws UnitException {

		if (factor <= 0.0) {
			for (int i = 0; i < value.length; i++) {
				from(factor, value[i]);
			}
		} else {
			for (int i = 0; i < value.length; i++) {
				for (int j = 0; j < value[i].length; j++) {
					for (int k = 0; k < value[i].length; k++) {
						value[i][j][k] *= factor;
					}
				}
			}
		}
	}

	/**
	 * Return the value in internal units.  The expectation is that the value is given in 
	 * internal units; however, the given units may be "unspecified" and 
	 * the default units will be used for conversion.  NOTE: This method is fairly specialized
	 * and is only useful in certain circumstances.
	 * <p>
	 * 
	 * @param defaultUnits the units to use if the units parameter is unspecified 
	 * @param units
	 *            the string representation of a unit conversion factor, For
	 *            example "m/s" See {@link #getAllUnits} for allowable strings.
	 * @param value
	 *            the value in internal units that was originally in the units "units".
	 * @return the value converted to internal units
	 * @exception UnitException
	 *                Thrown if the unit was not found.
	 */
	public static double fromInternal(String defaultUnits, String units, double value) throws UnitException {
		if (units.equals("unspecified")) {
			return from(getFactor(defaultUnits), value);			
		} else {
			return value;
		}
	}

	/**
	 * Convert the value from one unit to another unit.
	 * <p>
	 * 
	 * @param fromUnit
	 *            the unit of the original quantity
	 * @param toUnit
	 *            the unit of the new quantity
	 * @param value
	 *            the value to be converted
	 * @return the value in units of "toUnit"
	 * @exception UnitException
	 *                thrown if the units are unknown or incompatible.
	 */
	public static double convert(String fromUnit, String toUnit, double value)
			throws UnitException {
		return to(toUnit, from(fromUnit, value));
	}

	/**
	 * Convert the value from one unit to another unit.
	 * <p>
	 * 
	 * @param fromUnit
	 *            the unit of the original quantity
	 * @param toUnit
	 *            the unit of the new quantity
	 * @param value
	 *            the value to be converted
	 * @return the value in units of "toUnit"
	 * @exception UnitException
	 *                thrown if the units are unknown or incompatible.
	 */
	public static double convert(double fromUnit, double toUnit, double value)
			throws UnitException {
		return to(toUnit, from(fromUnit, value));
	}

	// This regex expression basically says is the string roughly in two parts, something that looks like a 
	// number and something that looks like a unit.
	//private static final Pattern numre = Pattern.compile("\\s*([-+0-9\\.]+)\\s*\\[?\\s*([-/^_a-zA-Z0-9\u00B0]*)\\s*\\]?"); //\\s*$");
	//private static final Pattern numre = Pattern.compile("\\s*([-+0-9\\.]+)\\s*([-\\[\\]\\/^_a-zA-Z0-9\u00B0]*).*"); //\\s*$");
	private static final Pattern numre = Pattern.compile("\\s*([-+0-9\\.]+)\\s*\\[?\\s*([-\\/^_a-zA-Z0-9\\u00B0]*).*");
	/**
	 * Parse a string, representing a value and a unit. 
	 * If the string does not contain a unit, then the unit "unspecified" is returned.
	 * @param s string to parse
	 * @return unit string;
	 */
	public static String parseUnits(String s) {
		String unit = "unspecified";
		Matcher m = numre.matcher(s);
		if (m.matches()) {
			unit = Units.clean(m.group(2));
		}
		//		String[] fields = s.split(" ");
		//		if (fields.length < 1) return unit;
		//		if (fields.length > 1) {
		//			unit = Units.clean(fields[1]);
		//		}
		return unit;
	}

	/**
	 * Parse a string into a value in internal units.  
	 * If the string contains a unit, then the value is understood in terms of that unit.  If no
	 * unit is specified in the string, then the unit in "defautlUnitsFrom" is used.
	 * @param defaultUnitsFrom the default units to convert the value from (if no text unit is specified) 
	 * @param str string to parse
	 * @param defaultValue the default value is the parameter 'value' is invalid. The units used are the defaultUnitsFrom. 
	 * @return converted value
	 */
	public static double parse(String defaultUnitsFrom, String str, double defaultValue) {
		double dbl;
		double dv = from(defaultUnitsFrom,defaultValue);

		String unit;
		Matcher m = numre.matcher(str);
		if (m.matches()) { 
			unit = Units.cleanOnly(m.group(2));
			//f.pln(" $$$ group 1: "+m.group(1)+"  group 2: "+m.group(2)+"  unit="+unit);

			// The logic here can be debated.  What should be returned when 
			// an invalid value or invalid unit is provided?  If parse("ft", "10 fjkdsj", 5)
			// is called, what should be returned? Units.from("ft", 10) or Units.from("ft", 5)?
			// I chose Units.from("ft", 10) because in the degenerate case of 
			// parse("10 jfkdjks", 5)--that is, with an implied default unit of "internal"--
			// returning 10 seems more correct than returning 5.
			//
			//   supplied                returned (aka converted) 
			//   value        unit       value          unit
			//   -----        ----       -----          ----
			//   valid        valid      suppliedvalue  suppliedunit
			//   valid        invalid    suppliedvalue  defunit
			//   invalid      valid      defvalue       defunit
			//   invalid      invalid    defvalue       defunit
			//   illformed               defvalue       defunit

			if (Util.is_double(m.group(1))) {
				dbl = Util.parse_double(m.group(1));
				if (Units.isUnit(unit)) {
					dbl = Units.from(unit, dbl);		   
				} else {
					dbl = Units.from(defaultUnitsFrom, dbl);		
				}
			} else {
				dbl = dv;		
			}

		} else {
			dbl = dv;
		}
		return dbl;
	}

	/**
	 * Parse a string, including an optional units identifier, as a double value.
	 * If the string does not contain a (valid) unit, then the value is interpreted as an "unspecified" unit. This version does not
	 * parse numbers in exponential notation, e.g., "10e-4".  
	 * @param str string to parse
	 * @param default_value if the string is not recognized as a valid value, the result to be returned 
	 * @return value
	 */
	public static double parse(String str, double default_value) {
		return parse("internal", str, default_value);
	}

	/**
	 * Parse a string, including an optional units identifier, as a double value.
	 * If the string does not contain a (valid) unit, then the value is interpreted as an "unspecified" unit. This version does not
	 * parse numbers in exponential notation, e.g., "10e-4".  
	 * @param str string to parse
	 * @return value
	 */
	public static double parse(String str) {
		return parse("internal", str, 0.0);
	}

	/**
	 * Parse a string into a value in internal units.  
	 * If the string contains a unit, then the value is understood in terms of that unit.  If no
	 * (valid) unit is specified in the string, then the unit in "defautlUnitsFrom" is used.
	 * @param defaultUnitsFrom the default units to convert the value from (if no text unit is specified) 
	 * @param str string to parse
	 * @return converted value
	 */
	public static double parse(String defaultUnitsFrom, String str) {
		return parse(defaultUnitsFrom, str, 0.0);
	}

	/**
	 * Parse a string into a value, then convert this value to the given units.
	 * This value is interpreted as the "defautlUnitsFrom" unit by default (unless the string s contains units).
	 * @param defaultUnitsFrom the default units to convert the value from (if no text unit is specified) 
	 * @param unitsTo the units to convert the value to
	 * @param value string to parse
	 * @return converted value
	 */
	public static double parse(String defaultUnitsFrom, String unitsTo, String value) {
		return Units.to(unitsTo, parse(defaultUnitsFrom, value));
	}

	/**
	 * Return a string for the value and unit.
	 * @param value value in internal units
	 * @param unit unit to convert to
	 * @return string with converted value with default decimal places, labeled with the provided unit
	 * @throws UnitException
	 */
	public static String str(String unit, double value) throws UnitException {
		return str(unit,value,Constants.get_output_precision());
	}

	/**
	 * Return a string for the value and unit.
	 * @param value value in internal units
	 * @param unit unit to convert to
	 * @return string with converted value with no formatting restrictions, labeled with the provided unit
	 * @throws UnitException
	 */
	public static String strX(String unit, double value) throws UnitException {
		return Double.toString(to(unit, value)) + " [" + unit + "]";	
	}

	/**
	 * Return a string for the value and unit.
	 * @param value value in internal units
	 * @param unit unit to convert to
	 * @param precision 
	 * @return string with converted value with precision decimal places, labeled with the provided unit
	 * @throws UnitException
	 */
	public static String str(String unit, double value, int precision) throws UnitException {
		return f.FmPrecision(to(unit, value),precision) + " [" + unit + "]";	
	}

	/**
	 * Creates a file called units.html which contains a list of all available
	 * units, their factors, and their compatible units is displayed.
	 * 
	 * @param argv command line arguments
	 * @throws UnitException 
	 */
	public static void main(String[] argv) throws UnitException {

		Iterator<String> itr;
		UnitPair u;
		String[] units = getAllUnits();

		System.out.println();
		System.out.println("<body>");
		System.out.println("<H1>List of Units</H1>");
		System.out.println("Note: this is the list of units when the ");
		System.out.println("Units class was created, the user may have added");
		System.out.println("other units in the interm.<p>");
		System.out.println("<table border>");
		System.out.println("<tr><th>Unit</th>");
		System.out.println("    <th>Factor</th>");
		System.out.println("    <th>Compatible Units</th></tr>");
		for (int i = 0; i < units.length; i++) {
			System.out.print("<tr><td align=center>");
			System.out.print(units[i]);
			System.out.print("</th>");
			u = getUnitPair(units[i]);
			System.out.print("<td align=right>");
			System.out.print(u.factor);
			System.out.print("</td>");
			System.out.print("<td>");
			itr = u.compatible.iterator();
			while (itr.hasNext()) {
				System.out.print(itr.next());
				System.out.print(' ');
			}
			System.out.println("</td>");
			System.out.println("</tr>");
		}
		System.out.println("</table>");
		System.out.println("</body>");
	}
}
