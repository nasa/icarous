import math
import numpy as np

#/
# This file is the units conversion file. Most of this came from the units.java code in the 
# FormalATM methods source code. 

# The file starts with general basic definitions. It then has functions
# to add base units for conversions. Included in this file are almost all of the conversions that will 
# be needed for the aircraft files, so no units should have to be added.
# This file only has conversions for distance, speed, and time. The java code has additional conversions
# for pressure and temperature, but those are not necessary for the files that will access this file.

# Errors:
# If a file that uses the units file returns an error, the error is likely in the data, either it includes
# dashes or some other string that the units class cannot handle. It is also possible that the information inputed
# has not yet been converted to integers, because that step does not happen in the masterReadIn file.
# If this error occurs, it would be helpful to print the data being converted to check for inconsistancies
# before adding a new base unit. 

# This file does not check for unit compatability. It will convert feet to seconds if the user gives it that information.

# Side note: because "min" and "in" are protected words in python, the user might need to enter "minutes" or
# "inches" to work around that error. 

# Because python does not have function polymorphism and keeping up with different function names is too much
# trouble for the user, the "convert_string" function is the most useful function because it allows the user to convert
# within internal units without worrying about entering a string instead of a number in the "to" function. This function
# basically preforms the role of the "to" and "from" functions in the java code
#/

# Below is the definition of base units. Most are self-explanitory
unitless = 1.0
unspecified = 1.0
internal = 1.0

second = 1.0
metre = 1.0
foot = 0.3048
meter_per_second2 = metre / (second * second)
foot_per_second2 = foot / (second * second)
# gn is the unit for gravity 
gn = 8.80665 * meter_per_second2

G = gn
meter = metre 
m = metre
s = second 
ft = foot

radian = 1.0
degree = math.pi / 180 

deg = degree
rad = radian

hertz = 1.0 / second
Hz = hertz

radian_per_second = radian / second 
degree_per_second = degree / second
deg_per_second = degree_per_second 

hertzAngular = 2 * math.pi * radian_per_second
ns = 1.0e-9 * second 
minute = 60.0 * second 
hour = 60.0 * minute

kilometer = 1000.0 * metre 
kilometre = 1000.0 * metre 
km = kilometre
mm = metre / 1000.0

inch = foot / 12.0

mile = 5280.0 * foot
mi = mile

nautical_mile = 1852.0 * metre
NM = nautical_mile

metre2 = metre * metre
meter2 = metre * metre 
foot2 = foot * foot
inch2 = inch * inch 

metre3 = metre * metre * metre 
meter3 = metre * metre * metre
foot3 = foot * foot * foot
inch3 = inch * inch * inch

meter_per_second = meter / second 
kilometer_per_hour = km / hour 
knot = nautical_mile / hour
mph = mile / hour
fpm = foot / minute

# Dictionary that holds a string (key) and a UnitPair (value)
unitTable = {}

# This is a list of compatible units for the "unspecified" unit 
unspecifiedUnits = []

# This is a class that holds a factor (float) and a value (list). It is more of a struct
class UnitPair():
	def __init__(self, factor, compatible):
		self.factor = factor
		# compatible paramater is a list 
		compatible = []
		self.compatible = compatible

# This function an object from UnitPair class given a key from the dictionary unitTable
def getUnitPairInternal(unit):
	if unit == None:
		return "A 'null' was given for a unit name"
	pair = unitTable.get(unit)
	return pair

# This function boolean value of true if the given unit is a valid unit
def isUnit(unit):
	return getUnitPair(unit) != None

# This function up any unit string that may contain brackets or extra space
# This should not be necessary in any of the FormalATM files 
def cleanOnly(unit):
	unit = unit.replace("[", "")
	unit = unit.replace("]", "")
	unit = unit.replace(" ", "")
	return unit

# This if cleanOnly unit is an acutal unit
def clean(unit):
	unit = cleanOnly(unit)
	if isUnit(unit):
		return unit
	else:
		return "Unspecified"

# This function unitPair from given unit
def getUnitPair(unit):
	if unit == None:
		return "Unknown unit"
	pair = getUnitPairInternal(unit)
	return pair

# This function gets the vactor value from an object from UnitPair
def getFactor(unit):
	return getUnitPair(unit).factor

# This function adds a UnitPair to the list of known units. 
# It also preforms various error checking functions
# newPair needs to be created as newPair = UnitPair(factor, compatable) by user
def addUnitPair(name, newPair):
	if newPair.factor <= 0.0:
		return "The conversion factor was not positive, this is invalid"
	if " " in name:
		return "A unit name canot contain spaces"
	if name in unitTable.keys():
		pair = getUnitPair(name)
		if newPair.compatible != pair.compatible:
			return "A unit already exists with the name ", name, " but it has a different dimensionality than the new unit. This is not allowed"
		if newPair.factor != pair.factor:
			print(name + " has already been definied as a unit and the new factor does not equal the old factor. Units cannot be redefined.")
			return False
	unitTable[name] = newPair
	return True

# This function adds a new UnitPair to the list of known units
# This also adds this unit to the list of compatible units for the "unspecified" unit
def addNewUnitPair(name, newPair):
	if addUnitPair(name, newPair):
		unspecifiedUnits.append(name)
		newPair.compatible.append(name)

# This function gives another name to a unit. For example, you could add "ft" to "feet"
def addSimilarUnit(newName, originalName):
	if originalName == "unspecified":
		return "Cannot make any units similar to unspecified"
	addUnitPair(newName, getUnitPair(originalName))

# This function creates a composite unit
# This method will not make the new unit compatible with any other unit
def addUnit(name, factor):
	temp = UnitPair(factor, "")
	addNewUnitPair(name, temp)

# This function creates a composite unit
# This method will not make the new unit compatible with any other unit
# This function expects the user to enter a base unit
def addUnit_base(name, factor, base):
	if base == "unspecified":
		return "Cannot base any unit on the 'unspecified' unit"
	basePair = getUnitPair(base)
	temp = UnitPair(factor*basePair.factor, basePair.compatible)
	addNewUnitPair(name, temp)

# This function adds a fundamental unit, where the supplied factor is the conversion factor
# between the given unit and internal units
def addCoreUnit(name, factor, base):
	if base == "unspecified":
		print("Cannot base any unit on the 'unspecified' unit")
	temp = UnitPair(factor, getUnitPair(base).compatible)
	addNewUnitPair(name, temp)

# This function determines if two units are compatible with one another 
def isCompatible(unit1, unit2):
	p1 = getUnitPair(unit1)
	p2 = getUnitPair(unit2)
	if unit1 == "unspecified" or unit2 == unspecified:
		return True 
	return p1.compatible == p2.compatible

# This function to the units indicated by the parameter "factor" from the internal units
def To(factor, value):
	if factor <= 0.0:
		return "Invalid conversion factor"
	return value / factor

# This function converts to the units indicated by the parameter "factor" from the internal units
# This method is slower since it invovles finding a string in a list
def To_string(unit, value):
	# print(getFactor(unit))
	return To(getFactor(unit), value)

# This function converts from the units indicated by "units" to internal units
def From(factor, value):
	if factor <= 0.0:
		return "Invalid conversion factor"
	return value * factor

# This function converts from the units indicated by the parameter "factor" from the internal units
# This method is slower since it invovles finding a string in a list
def From_string(unit, value):
	return From(getFactor(clean(unit)), value)

# This functions determines if the value (in internal units) was originally in some form of specified units
# If so, then do nothing. If not, use the default units
def fromInternal(defaultUnits, units, value):
	if units == "unspecified":
		return From(getFactor(defaultUnits), value)
	else:
		return value

# This function converts the value from one unit to another unit, using given values in strings
# fromUnit and toUnit are strings, value is a an int or float
def convert_string(fromUnit, toUnit, value):
	# print(From_string(fromUnit, value))
	return To_string(toUnit, From_string(fromUnit, value))

# The function converts the value from one unit to another unit
def convert(fromUnit, toUnit, value):
		return To(toUnit, From(fromUnit, value))


# below are function calls that populate the unitTable dictionary with all the conversions
# that the user should need for calculating airplane trajectory
addUnit("rad", rad)
addCoreUnit("deg", deg, "rad")
addSimilarUnit("radian", "rad")
addUnit("rad/s", radian / second)
addCoreUnit("deg/s", degree / second, "rad/s")
addSimilarUnit("degree/s", "deg/s")
addUnit("rad/s^2", radian / (second * second))
addCoreUnit("deg/s^2", degree / (second * second), "rad/s^2")
addSimilarUnit("radian/s^2", "rad/s^2")
addSimilarUnit("degree/s^2", "deg/s^2")
addUnit("unitless", unitless)
addSimilarUnit("none", "unitless")
addUnit("s", s);
addCoreUnit("ns", ns, "s");
addCoreUnit("min", minute, "s");
addCoreUnit("h", hour, "s");
addSimilarUnit("second", "s");
addSimilarUnit("minute", "min");
addSimilarUnit("hour", "h");
addSimilarUnit("hr", "h");
addUnit_base("ms", 0.001, "s");
addUnit_base("d", 24.0, "hour");
addSimilarUnit("day", "d");
addUnit("m", m);
addCoreUnit("mm", mm, "m");
addCoreUnit("ft", ft, "m");
addCoreUnit("km", km, "m");
addUnit_base("yard", 3.0, "ft");
addCoreUnit("NM", NM, "m");
addSimilarUnit("nm", "NM");  ## Do not add nm, nm means nanometers, not nautical miles
addSimilarUnit("nmi", "NM");
addCoreUnit("mi", mi, "m");
addSimilarUnit("metre", "m");
addSimilarUnit("meter", "m");
addSimilarUnit("foot", "ft");
addSimilarUnit("feet", "ft");
addSimilarUnit("kilometer", "km");
addSimilarUnit("kilometre", "km");
addSimilarUnit("nautical_mile", "nmi");
addSimilarUnit("mile", "mi");
addUnit("m/s", meter_per_second);
addSimilarUnit("meter_per_second", "m/s");
addSimilarUnit("metre_per_second", "m/s");
addCoreUnit("ft/s", foot / second, "m/s");
addSimilarUnit("fps", "ft/s");
addSimilarUnit("foot_per_second", "ft/s");
addSimilarUnit("feet_per_second", "ft/s");
addSimilarUnit("feet/second", "ft/s");
addCoreUnit("knot", knot, "m/s");
addSimilarUnit("kn", "knot");
addSimilarUnit("kts", "knot");
addCoreUnit("mph", mph, "m/s");
addCoreUnit("ft/min", fpm, "m/s");
addSimilarUnit("fpm", "ft/min");
addSimilarUnit("foot_per_minute", "ft/min");
addSimilarUnit("feet_per_minute", "ft/min");
addUnit("m/s^2", meter_per_second2);
addCoreUnit("ft/s^2", foot / (second * second), "m/s^2");
addCoreUnit("G", G, "m/s^2");
addUnit("m^2", meter2);
addCoreUnit("ft^2", foot2, "m^2");
addCoreUnit("in^2", inch2, "m^2");
addSimilarUnit("meter2", "m^2");
addSimilarUnit("metre2", "m^2");
addSimilarUnit("foot2", "ft^2");
addSimilarUnit("inch2", "in^2");
addUnit("m^3", meter3);
addCoreUnit("ft^3", foot3, "m^3");
addCoreUnit("in^3", inch3, "m^3");

