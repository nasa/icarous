/*
 * Copyright (c) 2013-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.ListIterator;
import java.util.Collection;
import java.util.Map;
import java.util.TreeMap;
import java.util.regex.Pattern;

/**
 * This class stores a database of parameters. In addition, it performs various
 * operations parameter strings, including parsing some more complicated
 * parameter strings into various data structures.<p>
 * 
 * All key accesses are case insensitive, however the actual key values are stored with case information intact.  Methods returning the 
 * key values (getList, getListFull) will reflect the capitalization scheme used for the initial assignment to a key.
 * 
 */
public class ParameterData {

	public static final String parenPattern = "[()]+";
	public static final String defaultEntrySeparator = "?";
	private boolean preserveUnits;
	private boolean unitCompatibility;
	private Map<String, ParameterEntry> parameters; 
	private String patternStr;

	/** A database of parameters.  A parameter can be a string, double value, or a boolean value.
	 * Units are also stored with each parameter.
	 */
	public ParameterData() {
		preserveUnits = false;
		unitCompatibility = true;
		patternStr = Constants.wsPatternBase;
		parameters = new TreeMap<String, ParameterEntry>(String.CASE_INSENSITIVE_ORDER);
	}

	/**
	 * Copy constructor.
	 * @param p
	 */
	public ParameterData(ParameterData p) {
		preserveUnits = p.preserveUnits;
		unitCompatibility = p.unitCompatibility;
		patternStr = p.patternStr;
		parameters = new TreeMap<String, ParameterEntry>(String.CASE_INSENSITIVE_ORDER);
		copy(p,true);
	}

	/**
	 * Return a copy of this ParameterData object, with all key values being prepended with the string prefix.
	 * This is intended to create a unique ParameterData object representing the parameters of a particular 
	 * instance (of many) of an object, that can then be collected along with others into a larger ParameterData object representing the containing object.
	 * @param prefix
	 * @return copy of ParameterData with changes
	 */
	public ParameterData copyWithPrefix(String prefix) {
		ParameterData p = new ParameterData();
		p.preserveUnits = preserveUnits;
		p.unitCompatibility = unitCompatibility;
		p.patternStr = patternStr;
		for (String key : parameters.keySet()) {
			p.parameters.put(prefix+key, parameters.get(key));
		}
		return p;
	}

	/**
	 * Return a copy of this ParameterData, with the string prefix removed from the beginning of all key values.
	 * If any key value in this object does not start with the given prefix, do not include it in the returned object.
	 * This is intended to filter out the parameters of a particular instance of a group of objects.
	 * The resulting ParameterData object will include an empty string parameter if a key exactly matches the prefix.
	 * 
	 * @param prefix
	 * @return copy of ParameterData with changes
	 */
	public ParameterData extractPrefix(String prefix) {
		String prefixlc = prefix.toLowerCase();
		ParameterData p = new ParameterData();
		p.preserveUnits = preserveUnits;
		p.unitCompatibility = unitCompatibility;
		p.patternStr = patternStr;
		for (String key : parameters.keySet()) {
			String keylc = key.toLowerCase();
			if (keylc.indexOf(prefixlc) == 0) {
				p.parameters.put(key.substring(prefix.length()), parameters.get(key));
			}
		}
		return p;		
	}

	/**
	 * Return a new ParameterData object that is a subset of this object, only containing elements of this object that match those keys in keylist.
	 * If keylist contains keys not in this object, they will not be included in the returned subset.
	 * 
	 * @param keylist list of keys to be included
	 * @return new ParameterData object that is a subset of this object
	 */
	public ParameterData subset(Collection<String> keylist) {
		ParameterData p = new ParameterData();
		p.preserveUnits = preserveUnits;
		p.unitCompatibility = unitCompatibility;
		p.patternStr = patternStr;
		for (String key : keylist) {
			if (contains(key)) {
				p.parameters.put(key, parameters.get(key));
			}
		}
		return p;		
	}

	/**
	 * Return a new ParameterData object that is a subset of this object, only containing the element of this object that matches key
	 * If key is not in this object, they will not be included in the returned subset.
	 * @param key the key to be included
	 * @return new ParameterData object that is a subset of this object
	 */
	public ParameterData subset(String key) {
		ParameterData p = new ParameterData();
		p.preserveUnits = preserveUnits;
		p.unitCompatibility = unitCompatibility;
		p.patternStr = patternStr;
		if (contains(key)) {
			p.parameters.put(key, parameters.get(key));
		}
		return p;		
	}


	/**
	 * Return a list of parameters that have the same values in both this object and p
	 * @param p ParameterData object to compare this object to.
	 * @return list of Parameter keys that have the same values in both objects.
	 */
	public List<String> intersection(ParameterData p) {
		List<String> s = new ArrayList<String>();
		List<String> l1 = getList();
		for (String key : l1) {
			if (p.contains(key)) {
				if (isNumber(key) && getValue(key) == p.getValue(key)) {
					s.add(key);
				} else if (isBoolean(key) && getBool(key) == p.getBool(key)) {
					s.add(key);
				} else if (getString(key).equals(p.getString(key))) {
					s.add(key);					
				}
			}
		}
		return s;
	}

	/** 
	 * Will set methods update the units in the database, or will the units
	 * in the database be preserved?
	 * @return true, if the units in the database will be preserved when a set method is called.
	 */
	public boolean isPreserveUnits() {
		return preserveUnits;
	}

	/**
	 * If true, then all subsequent calls to "set.." methods will not update 
	 * the units in the database to the units supplied through a "set.." method.
	 * The only exception is if the units in the database are "unspecified" then
	 * the units in the database will be updated with the value of units supplied through 
	 * a "set.." method.
	 * 
	 * @param v true when the units in the database should be preserved
	 */
	public void setPreserveUnits(boolean v) {
		preserveUnits = v;
	}

	/** 
	 * Will set methods disallow updating a unit if the new unit is incompatible with the
	 * old unit.  Meters and feet are compatible, whereas meters and kilograms are not.
	 * Most of the time, one wants the enforcement that compatible units are required,
	 * but there may be some situations where this is undesired.
	 * @return true, if set methods must use compatible units with the units in the database
	 */
	public boolean isUnitCompatibility() {
		return unitCompatibility;
	}

	/**
	 * Will set methods disallow updating a unit if the new unit is incompatible with the
	 * old unit.  Meters and feet are compatible, whereas meters and kilograms are not.
	 * Most of the time, one wants the enforcement that compatible units are required,
	 * but there may be some situations where this is undesired.
	 * 
	 * @param v true when the units in a set method must be compatible with the database.
	 */
	public void setUnitCompatibility(boolean v) {
		unitCompatibility = v;
	}

	/** Number of parameters in this object
	 * 
	 * @return number of parameters
	 */
	public int size() {
		return parameters.size();
	}

	/**
	 * Returns a list of parameter key strings encountered.
	 * Note that this will reflect the original capitalization of the keys when they were first stored.
	 * 
	 * @return list of parameter key names
	 */
	public List<String> getList() {
		List<String> l = new ArrayList<String>(size());
		l.addAll(parameters.keySet());
		return l;
	}

	/**
	 * Returns a list of parameter assignment strings ("key = value") encountered.
	 * Note that this will reflect the original capitalization of the keys when they were first stored.
	 * 
	 * @return list of parameter key names
	 */
	public List<String> getListFull() {
		List<String> list = getList();
		ListIterator<String> li = list.listIterator();
		while (li.hasNext()) {
			String p = li.next();
			li.set(p + " = " + getString(p));
		}
		return list;
	}

	/**
	 * Removes all stored parameters.
	 */
	public void clear() {
		parameters.clear();
	}

	/**
	 * Returns true if the parameter key was defined.
	 * 
	 * @param key parameter key to check if it is in database
	 * @return true, if parameter is in database
	 */
	public boolean contains(String key) {
		return parameters.containsKey(key);
	}

	/** 
	 * Returns a list of parameters whose names contain the given parameter as a substring.
	 * Note that this will reflect the original capitalization of the keys when they were first stored.
	 * @param substr the substring to match against
	 * @return a list of parameter names
	 */
	public List<String> matchList(String substr) {
		String substrlc = substr.toLowerCase();
		List<String> ret = new ArrayList<String>();
		List<String> plist = getList();
		for (String i: plist) {
			String ilc = i.toLowerCase();
			if (ilc.contains(substrlc))
				ret.add(i);
		}
		return ret;
	}

	/**
	 * Returns the string value of the given parameter key. This may be a
	 * space-delimited list. If the key is not present, return the empty string.
	 * Parameter keys may be case-sensitive.
	 * 
	 * @param key parameter name
	 * @return string representation of parameter
	 */
	public String getString(String key) {
		if (!parameters.containsKey(key)) {
			return "";
		} else {
			return parameters.get(key).sval;
		}
	}

	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. If the key is not present or if the value is not a numeral, then
	 * return 0. Parameter keys may be case-sensitive.
	 * 
	 * @param key parameter name
	 * @return value of parameter (internal units)
	 */
	public double getValue(String key) {
		if (!parameters.containsKey(key)) {
			return 0.0;
		} else {
			return parameters.get(key).dval;
		}
	}

	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. Only in the case when units were not specified in the database, will
	 * the defaultUnit parameter be used. If the key is not present or if the
	 * value is not a numeral, then return 0. Parameter keys may be
	 * case-sensitive.
	 * 
	 * @param key name of parameter
	 * @param defaultUnit units to use if no units are in database
	 * @return value of parameter (internal units)
	 */
	public double getValue(String key, String defaultUnit) {
		return Units.fromInternal(defaultUnit, getUnit(key), getValue(key));
	}

	/**
	 * Returns the string representation of the specified unit of the given
	 * parameter key. If the key is not present or no unit was specified, return
	 * "unspecified". Parameter keys may be case-sensitive.
	 * 
	 * @param key name of parameter
	 * @return units of parameter
	 */
	public String getUnit(String key) {
		if (parameters.containsKey(key)) {
			return parameters.get(key).units;
		} else {
			return "unspecified";
		}
	}

	/**
	 * Returns the Boolean value of the given parameter key. If the key is not
	 * present, or not representation of "true", return the empty string.
	 * Parameter keys may be case-sensitive.
	 * 
	 * @param key name of parameter
	 * @return boolean representation of parameter
	 */
	public boolean getBool(String key) {
		if (parameters.containsKey(key)) {
			return parameters.get(key).bval;
		} else {
			return false;
		}
	}

	/**
	 * Returns the integer value of the given parameter key in internal units.
	 * If no units were specified in the file, then the defaultUnit parameter is used.
	 * If the key is not present or if the 
	 * value is not a numeral, then return 0.  This value is an integer version of the 
	 * double value (see the related getParameterValue() method).  If the double value is 
	 * larger than an integer, behavior is undefined.
	 * Parameter keys may be case-sensitive.
	 * 
	 * @param key name of parameter
	 * @return integer representation of parameter
	 */
	public int getInt(String key) {
		return (int) getValue(key);
	}

	/**
	 * Returns the long value of the given parameter key in internal units.
	 * If no units were specified in the file, then the defaultUnit parameter is used.
	 * If the key is not present or if the 
	 * value is not a numeral, then return 0.  This value is an integer version of the 
	 * double value (see the related getParameterValue() method).  If the double value is 
	 * larger than an long, behavior is undefined.  
	 * Parameter keys may be case-sensitive.
	 * 
	 * @param key name of parameter
	 * @return long representation of parameter
	 */
	public long getLong(String key) {
		return (long) getValue(key);
	}


	/**
	 * Parse a string, if it is a valid parameter, then add it to the database and return true, else false.
	 * @param str the string to parse
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
	private boolean parse_parameter_string(String str) {
		int loc = str.indexOf('=');
		if (loc > 0) {
			String id = str.substring(0,loc).trim();
			if (id.length() == 0) {
				return false;
			}
			String value = str.substring(loc+1).trim();
			return putParam(id,parse_parameter_value(value));
		} else {
			return false;
		}
	}

	/**
	 * Put entry in parameter map
	 * @param key
	 * @param entry
	 * @return true, if parameter was added successfully
	 */
	protected boolean putParam(String key, Pair<Boolean, ParameterEntry> entry) {
		boolean perform_conversion = entry.first;
		ParameterEntry newEntry = entry.second;

		boolean compatible = true;
		if (parameters.containsKey(key)) {
			ParameterEntry oldEntry = parameters.get(key);
			if (!Units.isCompatible(newEntry.units,oldEntry.units)) {
				compatible = false;
			} else {
				if (newEntry.units.equals("unspecified")) {
					if (perform_conversion) {
						String units = oldEntry.units;
						double convert = Units.from(units,newEntry.dval);
						newEntry.dval = convert;
						newEntry.units = units;
						newEntry.set_sval();
					} else {
						newEntry.units = oldEntry.units;
					}
				} else if (isPreserveUnits()) { // newEntry.third != "unspecified"
					if ( ! oldEntry.units.equals("unspecified")) {
						newEntry.units = oldEntry.units;
					} 
				}
			}
		}
		if (compatible || ! unitCompatibility) {
			parameters.put(key, newEntry);
			return true;
		} else {
			return false;
		}
	}

	/** Doesn't do error checking, string should be "trimmed." This should always return a PartameterEntry */
	private Pair<Boolean, ParameterEntry> parse_parameter_value(String value) {
		Boolean perform_conversion = Boolean.TRUE;
		double dbl = Units.parse(value,Double.MAX_VALUE);
		if (dbl == Double.MAX_VALUE) { // unrecognized units, error
			perform_conversion = Boolean.FALSE;
			dbl = 0.0;
		}
		String unit = Units.parseUnits(value);
		ParameterEntry quad = new ParameterEntry(value,dbl,unit,Util.parse_boolean(value),"",-1);
		return Pair.make(perform_conversion,quad);
	}

	/**
	 * Parses the given string as a parameter assignment. If successful, returns
	 * a true value and adds the parameter. Otherwise returns a false value and
	 * makes no change to the parameter database.  If the supplied units
	 * are unspecified, then the units in the database are used to interpret the value
	 * given.  <p>
	 * 
	 * Examples of valid strings include:
	 * <ul>
	 * <li> a = true
	 * <li> b = hello everyone!
	 * <li> c = 10 [NM]
	 * </ul>
	 * 
	 * @param s the given string to parse
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
	public boolean set(String s) {
		return parse_parameter_string(s);
	}

	/**
	 * Associates a parameter key with a value (both represented as strings).
	 * The value field may include a units descriptor in addition to the actual
	 * value, usually in a format similar to '10 [NM]', representing 10 nautical 
	 * miles.  If the supplied units
	 * are unspecified, then the units in the database are used to interpret the value
	 * given.  
	 * 
	 * @param key name of parameter
	 * @param value string representation of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
	public boolean set(String key, String value) {
		return putParam(key,parse_parameter_value(value));
	}

	/**
	 * @deprecated Replaced by {@link #setBool(String key, boolean value)}
	 */
	@Deprecated public boolean set(String key, boolean value) {
		return setBool(key,value);
	}

	/** Associates a boolean value with a parameter key. 
	 * 
	 * @param key name of parameter
	 * @param value boolean representation of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean setBool(String key, boolean value) {
		ParameterEntry newEntry = ParameterEntry.makeBoolEntry(value);
		return putParam(key,Pair.make(Boolean.FALSE,newEntry));		
	}

	/** Associates true value with a parameter key. 
	 * 
	 * @param key name of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean setTrue(String key) {
		return setBool(key,true);	
	}

	/** Associates false value with a parameter key. 
	 * 
	 * @param key name of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean setFalse(String key) {
		return setBool(key,false);	
	}

	/** Associates an integer value with a parameter key. 
	 * 
	 * @param key name of parameter
	 * @param value integer representation of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean setInt(String key, int value) {
		ParameterEntry newEntry = ParameterEntry.makeIntEntry(value);
		return putParam(key,Pair.make(Boolean.FALSE,newEntry));		
	}

	/** Associates a value (in the given units) with a parameter key. If the supplied units
	 * are "unspecified," then the units in the database are used to interpret the value
	 * given.  How the units in the database
	 * are updated depends on the value of the setPreserveUnits() parameter. 
	 * 
	 * 
	 * @param key    the name of the parameter
	 * @param value  the value of the parameter in EXTERNAL units
	 * @param units  the units of the given parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean set(String key, double value, String units) {
		units = Units.clean(units);
		ParameterEntry newEntry = ParameterEntry.makeDoubleEntry(Units.from(units,value),units,Constants.get_output_precision());
		return putParam(key,Pair.make(Boolean.TRUE,newEntry));
	}

	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place, if "unspecified" any old value is preserved)
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean setInternal(String key, double value, String units) {
		return setInternal(key, value, units, Constants.get_output_precision());
	}

	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place, if "unspecified" any old value is preserved)
	 * @param prec  precision 
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	public boolean setInternal(String key, double value, String units, int prec) {
		units = Units.clean(units);
		ParameterEntry newEntry = ParameterEntry.makeDoubleEntry(value,units,prec);
		return putParam(key,Pair.make(Boolean.FALSE,newEntry));
	}

	/**
	 * Updates the unit for an entry. This ignores the setPreservedUnits() flag.
	 * 
	 * @param key name of parameter
	 * @param unit unit for this parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updateUnit(String key, String unit) {
		ParameterEntry entry = parameters.get(key);
		if (Units.isUnit(unit) && entry != null && 
				Units.isCompatible(entry.units,unit) &&
				!unit.equals("unitless") && !unit.equals("unspecified")) {
			entry.units = unit;
			entry.set_sval();
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Updates entry's comment
	 * 
	 * @param key name of parameter
	 * @param msg the new comment of the parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updateComment(String key, String msg) {
		ParameterEntry entry = parameters.get(key);
		if (entry != null) {
			entry.comment = msg;
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Updates entry's string
	 * 
	 * @param key name of parameter
	 * @param str the new string of the parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updateStr(String key, String str) {
		ParameterEntry entry = parameters.get(key);
		if (entry != null) {
			entry.sval = str;
			entry.dval = 0;
			entry.bval = false;
			entry.units = "unitless";
			return true;
		} else {
			return false;
		}
	}

	/** Updates entry's double value
	 * 
	 * @param key the name of the parameter
	 * @param val the new double value of the parameter in INTERNAL units
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updateDouble(String key, double val) {
		ParameterEntry entry = parameters.get(key);
		if (entry != null) {
			entry.dval = val;
			if (entry.precision == 0) {
				entry.precision = -1;
			}
			entry.set_sval();
			return true;
		} else {
			return false;
		}
	}

	/** Updates entry's boolean value
	 * 
	 * @param key the name of the parameter
	 * @param val the new boolean value of the parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updateBool(String key, boolean val) {
		ParameterEntry entry = parameters.get(key);
		if (entry != null) {
			entry.sval = val ? "true" : "false";
			entry.bval = val;
			return true;
		} else {
			return false;
		}
	}

	/** Updates entry's integer value
	 * 
	 * @param key the name of the parameter
	 * @param val the new integer value of the parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updateInt(String key, int val) {
		ParameterEntry entry = parameters.get(key);
		if (entry != null) {
			entry.precision = 0;
			entry.dval = val;
			entry.set_sval();
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Updates entry's precision
	 * 
	 * @param key name of parameter
	 * @param p the new precision of the parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean updatePrecision(String key, int p) {
		ParameterEntry entry = parameters.get(key);
		if (entry != null) {
			entry.precision = p;
			entry.set_sval();
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Checks the parameters against the supplied list, and returns a list of
	 * unrecognized parameters from the collection, possible empty.
	 * This operation is case insensitive.
	 * 
	 * @param c a collection (for instance a list) of parameter names
	 * @return a list of parameters from the original list that are not 
	 */
	public List<String> unrecognizedParameters(Collection<String> c) {
		List<String> p = new ArrayList<String>(parameters.keySet());
		p.removeAll(c);
		return p;
	}

	/** 
	 * Copy parameter entries from list of keys 
	 */
	public void listCopy(ParameterData p, List<String> plist, boolean overwrite) {
		for (String key: plist) {
			if (overwrite || ! contains(key)) {
				ParameterEntry entry = p.parameters.get(key);
				if (entry != null) {
					parameters.put(key,new ParameterEntry(entry));
				}
			}
		}
	}

	/**
	 * Copy a ParameterData object into this object.  That is, A.copy(B,true) means A &lt;--- B.
	 * @param p source ParameterData
	 * @param overwrite if a parameter key exists in both this object and p, if overwrite is true then p's value will be used, otherwise this object's value will be used
	 */
	public void copy(ParameterData p, boolean overwrite) {
		listCopy(p,p.getList(),overwrite);
	}

	/**
	 * This interprets a string as a Constants.wsPatternBase-delineated list of strings.
	 */
	private List<String> stringList(String instring) {
		String[] l = instring.split(Constants.separatorPattern,Integer.MIN_VALUE);
		List<String> a = new ArrayList<String>();
		for (String s: l) {
			a.add(s.trim());
		}
		return a;
	}

	/**
	 * This interprets a string as a Constants.wsPatternBase-delineated list of integer values. 
	 */
	private List<Integer> integerList(String instring) {
		List<String> l = stringList(instring);
		List<Integer> a = new ArrayList<Integer>();
		for (String s:l) {
			a.add(Util.parse_int(s.trim()));
		}
		return a;
	}

	private List<Double> doubleList(String instring) {
		List<String> l = stringList(instring);
		List<Double> a = new ArrayList<Double>();
		for (String s:l) {
			a.add(Units.parse(s.trim()));
		}
		return a;
	}

	private List<Boolean> boolList(String instring) {
		List<String> l = stringList(instring);
		List<Boolean> a = new ArrayList<Boolean>();
		for (String s: l) {
			a.add(Boolean.valueOf(Util.parse_boolean(s.trim()))); 
		}
		return a;
	}

	public List<Integer> getListInteger(String key) {
		if (parameters.containsKey(key)) {
			return integerList(parameters.get(key).sval);
		} else {
			return new ArrayList<Integer>();
		}
	}

	public List<Double> getListDouble(String key) {
		if (parameters.containsKey(key)) {
			return doubleList(parameters.get(key).sval);
		} else {
			return new ArrayList<Double>();
		}
	}

	public List<String> getListString(String key) {
		if (parameters.containsKey(key)) {
			return stringList(parameters.get(key).sval);
		} else {
			return new ArrayList<String>();
		}
	}

	public List<Boolean> getListBool(String key) {
		if (parameters.containsKey(key)) {
			return boolList(parameters.get(key).sval);
		} else {
			return new ArrayList<Boolean>();
		}
	}


	public <T> boolean set(String key, List<T> list) {
		String s = "";
		if (list.size() > 0) {
			s = ""+list.get(0);
			for (int i = 1; i < list.size(); i++) {
				s = s + ","+list.get(i);
			}
		}
		return set(key,s);
	}



	/**
	 * Returns true if the stored value for key is likely a boolean
	 * @param key parameter name
	 * @return true if key exists and the value is true/false/t/f, false otherwise
	 */
	public boolean isBoolean(String key) {
		if (!parameters.containsKey(key)) {
			return false;
		} else {
			return Util.is_boolean(parameters.get(key).sval);
		}
	}

	/**
	 * Returns true if the stored value for key is likely a number
	 * @param key parameter name
	 * @return true if key exists and the value is a parsable number
	 */
	public boolean isNumber(String key) {
		if (!parameters.containsKey(key)) {
			return false;
		} else {
			String s = parameters.get(key).sval;
			String[] fields = s.split(patternStr);
			if (fields.length == 1) {
				return Util.is_double(fields[0]);
			} else if (fields.length == 2) {
				return Util.is_double(fields[0]) && !Util.is_double(fields[1]);
			} else {
				return false; // probably a list
			}
		}
	}

	/**
	 * Returns true if the stored value for key is likely a string (or list).  Note,
	 * the getString() method will always return a string (assuming a valid key
	 * is provided).  This method returns a more narrow definition of a string,
	 * that is, something that is not a number or a boolean.
	 * @param key parameter name
	 * @return true if key exists and the value is not a parse-able number
	 */
	public boolean isString(String key) {
		if (!parameters.containsKey(key)) {
			return false;
		} else {
			return !isNumber(key) && !isBoolean(key);
		}

	}

	/**
	 * Returns a string listing all parameters in keys and their (original string) values
	 * 
	 * @param keys list of parameter entries
	 * @return multi-line string
	 */
	public String listToString(List<String> keys) {
		String s = "";
		for (String key : keys) {
			ParameterEntry val = parameters.get(key);
			if (val != null) {
				if (!val.comment.equals("")) {
					s += "# "+val.comment+"\n";
				}
				s += key+" = "+val.sval+"\n";
			} 
		}
		return s;
	}

	/**
	 * Returns a multi-line string listing all parameters and their (original string) values
	 */
	public String toString() {
		List<String> keys = getList();
		Collections.sort(keys);
		return listToString(keys);
	}

	/**
	 * Returns a string listing all parameters in keys 
	 * 
	 * @param keys list of parameter entries
	 * @param separator the separator (e.g., a comma) to separate each key
	 * @return string
	 */
	public String listToString(List<String> keys, String separator) {
		String s = "";
		boolean first = true;
		for (String key : keys) {
			ParameterEntry val = parameters.get(key);
			if (val != null) {
				if (first) {
					first = false;
				} else {
					s += separator;
				}
				s += key+"="+val.sval;
			} 
		}
		return s;
	}

	/**
	 * Returns a string listing all parameters
	 * @param separator the separator (e.g., a comma) to separate each key
	 * @return A separated string of all parameters
	 */
	public String toString(String separator) {
		List<String> keys = getList();
		Collections.sort(keys);
		return listToString(keys,separator);
	}

	/** 
	 * Parses the array of strings as if they were command line arguments and puts them
	 * into this ParameterData object.<p>
	 * 
	 * The prefix is the command line parameter designator.  For instance, if the
	 * command line argument is "-Px=10mm" then the command line designator is "-P"
	 * and the parameter is "x=10mm", that is, the parameter name "x" will be added
	 * with a value of 10 millimeters. Any arguments that do not begin with the
	 * command line designator are not included in this ParameterData object and are
	 * instead returned, in order.
	 * 
	 * In other words, this strips the array of strings of recognized parameters and 
	 * stores them, and returns the rest in a list.
	 * 
	 * @param prefix command line parameter designator
	 * @param args the command line arguments
	 * @return the command line arguments that do not begin with the designator 
	 */
	public List<String> parseArguments(String prefix, String[] args) {
		List<String> l = new ArrayList<String>(args.length);
		for (String a: args) {
			if (a.startsWith(prefix)) {
				set(a.substring(prefix.length()));
			} else {
				l.add(a);
			}
		}
		return l;
	}

	/**
	 * Remove the given key from this database.  If the key does not exist, do nothing.
	 * @param key
	 */
	public void remove(String key) {
		if (parameters.containsKey(key)) {
			parameters.remove(key);
		}
	}

	/**
	 * Remove all keys in the list from this database.  If any given key does not exist, do nothing.
	 * @param keys
	 */
	public void removeAll(Collection<String> keys) {
		for (String key: keys) {
			remove(key);
		}
	}

	/**
	 * Return this ParameterData as a single line string that can subsequently be parsed by parseLine()
	 * @param separator Unique, non-empty string to separate each entry.  If left empty or null, the defaultEntrySeparator is used.
	 * @return Single-line string representation of this ParameterData object, or the null object if the given pattern appears in any parameter definition, including whitespace.
	 */
	public String toParameterList(String separator) {
		if (separator == null || separator.length() < 1) {
			separator = defaultEntrySeparator;
		}
		String ret = "";
		for (String def : getListFull()) {
			if (def.contains(separator)) {
				return null;
			}
			ret += def+separator;
		}
		return ret;
	}

	/**
	 * Read in a set of parameters as created by toParameterList() with the same separator.
	 * @param separator string used to separate definitions.  If blank or null, use defaultEntrySeparator instead.
	 * @param line String to be read.
	 * @return true if string parsed successfully, false if an error occurred.  If this returns false, one or more entries were not added to the database.
	 */
	public boolean parseParameterList(String separator, String line) {
		if (separator == null || separator.length() < 1) {
			separator = defaultEntrySeparator;
		}
		boolean status = true;
		String[] s = line.trim().split(Pattern.quote(separator));
		for (String def : s) {
			if (def.length() > 3) { // minimum possible length for a parameter definition
				status = status && set(def);
			}
		}
		return status;
	}


	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result
				+ ((parameters == null) ? 0 : parameters.hashCode());
		result = prime * result
				+ ((patternStr == null) ? 0 : patternStr.hashCode());
		result = prime * result + (preserveUnits ? 1231 : 1237);
		result = prime * result + (unitCompatibility ? 1231 : 1237);
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
		ParameterData other = (ParameterData) obj;
		if (parameters == null) {
			if (other.parameters != null)
				return false;
		} else if (!parameters.equals(other.parameters))
			return false;
		if (patternStr == null) {
			if (other.patternStr != null)
				return false;
		} else if (!patternStr.equals(other.patternStr))
			return false;
		if (preserveUnits != other.preserveUnits)
			return false;
		if (unitCompatibility != other.unitCompatibility)
			return false;
		return true;
	}
}