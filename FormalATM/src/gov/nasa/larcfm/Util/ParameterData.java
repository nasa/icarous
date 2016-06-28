/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.ListIterator;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.event.AncestorEvent;

/**
 * This class stores a database of parameters. In addition, it performs various
 * operations parameter strings, including parsing some more complicated
 * parameter strings into various data structures.<p>
 * 
 * All key accesses are case insensitive
 * 
 */
public class ParameterData {

	public static final String parenPattern = "[()]+";
	public static final String defaultEntrySeparator = "?";
	//private boolean caseSensitive;
	private boolean preserveUnits;
	private boolean unitCompatibility;
	private Map<String, Quad<String, Double, String, Boolean>> parameters; // string representation, double representation, unit, boolean representation
	private String patternStr;

	/** A database of parameters.  A parameter can be a string, double value, or a boolean value.
	 * Units are also stored with each parameter.
	 */
	public ParameterData() {
		//caseSensitive = true;
		preserveUnits = false;
		unitCompatibility = true;
		patternStr = Constants.wsPatternBase;
		//parameters = new HashMap<String, Quad<String, Double, String, Boolean>>(10);
		parameters = new TreeMap<String, Quad<String, Double, String, Boolean>>(String.CASE_INSENSITIVE_ORDER);
	}

	/**
	 * Copy constructor.
	 * @param p
	 */
	public ParameterData(ParameterData p) {
		preserveUnits = p.preserveUnits;
		unitCompatibility = p.unitCompatibility;
		patternStr = p.patternStr;
		parameters = new TreeMap<String, Quad<String, Double, String, Boolean>>(String.CASE_INSENSITIVE_ORDER);
		copy(p,true);
	}
	
	/**
	 * Return a copy of this ParameterData object, with all key values being prepended with the string prefix.
	 * This is intended to create a unique ParameterData object representing the parameters of a particular 
	 * instance (of many) of an object, that can then be collected along with others into a larger ParameterData object representing the containing object.
	 * @param prefix
	 * @return
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
	 * 
	 * @param prefix
	 * @return
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
	 * Return a new ParameterData object that is a subset of this object
	 * @param keylist list of keys to be included
	 * @return new ParameterData object that is a subset of this object, only containing elements of this object that match those keys in keylist.
	 * If keylist contains keys not in this object, they will not be included in the returned subset.
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
	 * Return a list of parameters that have the same values in both this object and p
	 * @param p ParameterData object to compare this object to.
	 * @return list of Parameter keys that have the same values in both objects.
	 */
	public List<String> intersection(ParameterData p) {
		ArrayList<String> s = new ArrayList<String>();
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
	 * @deprecated
	 * Is this database case sensitive for the parameter names
	 * @return true if the parameters names are case sensitive.
	 */
	public boolean isCaseSensitive() {
//		return caseSensitive;
		return true;
	}

	/**
	 * @deprecated
	 * Sets the case sensitive flag.  Odd results will happen if the case
	 * sensitive flag is set after parameters have already been added to the 
	 * database.
	 * 
	 * Note: this flag is always true
	 * @param v true if the database is case sensitive.
	 */
	public void setCaseSensitive(boolean v) {
//		caseSensitive = v;
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
		
	public int size() {
		return parameters.size();
	}

	/**
	 * Returns a list of parameter key strings encountered.
	 */
	public List<String> getList() {
		//return parameters.keySet().toArray(new String[0]);
		ArrayList<String> l = new ArrayList<String>(size());
		l.addAll(parameters.keySet());
		return l;
	}

	/**
	 * Returns a list of parameter assignment strings ("key = value") encountered.
	 */
	public List<String> getListFull() {
		List<String> list = getList();
		ListIterator<String> li = list.listIterator();
		while (li.hasNext()) {
			String p = li.next();
			li.set(p + " = " + getString(p));
		}
//		for (int i = 0; i < list.length; i++) {
//			list[i] = list[i] + " = " + getParameterString(list[i]);
//		}
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
	 */
	public boolean contains(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		return parameters.containsKey(key);
	}

	/** 
	 * Returns a list of parameters whose names contain the given parameter as a substring.
	 * @param substr the substring to match against
	 * @return a list of parameter names
	 */
	public List<String> matchList(String substr) {
//		if (!caseSensitive)
//			substr = substr.toLowerCase();
		String substrlc = substr.toLowerCase();
		ArrayList<String> ret = new ArrayList<String>();
//		String[] plist = getParameterList();
//		for (int i = 0; i < plist.length; i++) {
//			if (plist[i].contains(key))
//				ret.add(plist[i]);
//		}
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
	 */
	public String getString(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		if (!parameters.containsKey(key)) {
			return "";
		} else {
			return parameters.get(key).getFirst();
		}
	}

	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. If the key is not present or if the value is not a numeral, then
	 * return 0. Parameter keys may be case-sensitive.
	 */
	public double getValue(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		if (!parameters.containsKey(key)) {
			return 0.0;
		} else {
			return parameters.get(key).getSecond();
		}
	}

	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. Only in the case when units were not specified in the file, will
	 * the defaultUnit parameter be used. If the key is not present or if the
	 * value is not a numeral, then return 0. Parameter keys may be
	 * case-sensitive.
	 */
	public double getValue(String key, String defaultUnit) {
		return Units.fromInternal(defaultUnit, getUnit(key), getValue(key));
	}

	/**
	 * Returns the string representation of the specified unit of the given
	 * parameter key. If the key is not present or no unit was specified, return
	 * "unspecified". Parameter keys may be case-sensitive.
	 */
	public String getUnit(String key) {
//		if (!caseSensitive) {
//			key = key.toLowerCase();
//		}
		if (parameters.containsKey(key)) {
			return parameters.get(key).getThird();
		} else {
			return "unspecified";
		}
	}

	/**
	 * Returns the Boolean value of the given parameter key. If the key is not
	 * present, or not representation of "true", return the empty string.
	 * Parameter keys may be case-sensitive.
	 */
	public boolean getBool(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		if (parameters.containsKey(key)) {
			return parameters.get(key).getFourth();
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
		//f.pln("PD "+str+" XX "+loc);

		if (loc > 0) {
			String id = str.substring(0,loc).trim();
			if (id.length() == 0) {
				return false;
			}
			String value = str.substring(loc+1).trim();
			return putParam(id, parse_parameter_value(value));
		} else {
			return false;
		}
	}

	/**
	 * Put the Quad in for the parameter
	 * @param key
	 * @param newEntry
	 * @param preserve
	 */
	private boolean putParam(String key, Pair<Boolean, Quad<String, Double, String, Boolean>> entry) {
		//f.pln("ParameterData.putParam "+key+" "+newEntry);
		boolean preserve_string = entry.first;
		Quad<String, Double, String, Boolean> newEntry = entry.second;
//		if ( ! caseSensitive) {
//			key = key.toLowerCase();
//		}		
		
		boolean compatible = true;
		if (parameters.containsKey(key)) {
			Quad<String, Double, String, Boolean> oldEntry = parameters.get(key);
			if ( ! Units.isCompatible(newEntry.third,oldEntry.third)) {
				compatible = false;
			} else {
				if (newEntry.third.equals("unspecified") && ! preserve_string) {
					String units = oldEntry.third;
					double convert = Units.from(units,newEntry.second);
					newEntry = Quad.make(Units.strX(units,newEntry.second),convert,units,newEntry.fourth);
				} else if (isPreserveUnits()) { // newEntry.third != "unspecified"
					if ( ! oldEntry.third.equals("unspecified")) {
						newEntry = Quad.make(newEntry.first,newEntry.second,oldEntry.third,newEntry.fourth);
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
    
	/** Doesn't do error checking, string should be "trimmed." this should always return a Quad */
	private Pair<Boolean, Quad<String, Double, String, Boolean>> parse_parameter_value(String value) {
		Boolean preserve_string = Boolean.FALSE;
		double dbl = Units.parse(value,Double.MAX_VALUE);
		if (dbl == Double.MAX_VALUE) {
			preserve_string = Boolean.TRUE;
			dbl = 0.0;
		}
		String unit = Units.parseUnits(value);
		Boolean boolx = Boolean.FALSE;
		if (value.equalsIgnoreCase("true") || value.equalsIgnoreCase("T")) {
			boolx = Boolean.TRUE;
		}
		Quad<String, Double, String, Boolean> quad = Quad.make(
				value,dbl,unit,boolx);
		return Pair.make(preserve_string,quad);
	}
	
	
//	private boolean parse_parameter_string(String str) {
//
//		String[] fields = str.split(patternStr);
//
//		// parameter keys are lower case only
//		if (fields.length >= 3
//				&& (fields[1].equals("=") || fields[1].equals("=="))
//				&& fields[0].length() > 0) {
//			String id = fields[0];
//			if (!caseSensitive)
//				id = id.toLowerCase();
//
//			StringBuilder value = new StringBuilder(fields[2]);
//			for (int i = 3; i < fields.length; i++) {
//				value.append(" " + fields[i]);
//			} // values become a space-delineated list string
//
//			// String value
//			String strv = value.toString();
//
//			// Double value
//			double dbl = 0.0;
//			String unit = "unspecified";
//			try {
//				dbl = Double.parseDouble(fields[2]);
//				if (fields.length >= 4) {
//					unit = Units.clean(fields[3]);
//					dbl = Units.from(unit, dbl);
//				}
//			} catch (NumberFormatException e) {
//				dbl = 0.0; // string is not a number so use a default value
//			}
//
//			// Boolean value
//			Boolean bool = Boolean.FALSE;
//			if (strv.equalsIgnoreCase("true") || strv.equalsIgnoreCase("T")) {
//				bool = Boolean.TRUE;
//			}
//			Quad<String, Double, String, Boolean> quad = Quad.make(
//					str.substring(str.indexOf(fields[2], str.indexOf("=")))
//							.trim(), dbl, unit, bool);
////			 Quad<String,Double,String,Boolean> quad = Quad.make(strv, dbl,
////			 unit, bool);
//			parameters.put(id, quad);
//			return true;
//		} else {
//			return false;
//		}
//	}

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
     * @return true if the database was updated, false otherwise.  The database may not
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
     * @return true if the database was updated, false otherwise.  The database may not
     * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
	public boolean set(String key, String value) {
		//parse_parameter_string(key + " = " + value, false);
		return putParam(key,parse_parameter_value(value));
	}

    /** Associates a boolean value with a parameter key. 
     * 
     * @return true if the database was updated, false otherwise.  The database may not
     * be updated because of incompatible units, etc.
     * */
    public boolean set(String key, boolean value) {
    	//set(key, Boolean.toString(value));
    	if (value) {
    		Quad<String, Double, String, Boolean> newEntry = 
    				Quad.make("true",0.0,"unitless",Boolean.TRUE);
    		return putParam(key,Pair.make(Boolean.TRUE,newEntry));
    	} else {
    		Quad<String, Double, String, Boolean> newEntry = 
    				Quad.make("false",0.0,"unitless",Boolean.FALSE);
    		return putParam(key,Pair.make(Boolean.TRUE,newEntry));
    	}
    }
	
	/** Associates a value (in the given units) with a parameter key. If the supplied units
	 * are "unspecified," then the units in the database are used to interpret the value
	 * given.  How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter. 
	 * 
	 * 
     * @param key    the name of the parameter
     * @param value  the value of the parameter in EXTERNAL units
     * @param units  the units of the given parameter
     * @return true if the database was updated, false otherwise.  The database may not
     * be updated because of incompatible units, etc.
     */
	public boolean set(String key, double value, String units) {
		units = Units.clean(units);
		Quad<String, Double, String, Boolean> newEntry = 
				Quad.make(f.Fm8(value) + " [" + units +"]",Units.from(units,value),units,Boolean.FALSE);
		return putParam(key,Pair.make(Boolean.FALSE,newEntry));
	}

	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place)
     * @return true if the database was updated, false otherwise.  The database may not
     * be updated because of incompatible units, etc.
	 */
	public boolean setInternal(String key, double value, String units) {
		units = Units.clean(units);
		Quad<String, Double, String, Boolean> newEntry = 
				Quad.make(Units.str(units,value,8),value,units,Boolean.FALSE);
		return putParam(key,Pair.make(Boolean.FALSE,newEntry));
	}

	/**
	 * Updates the default unit for an entry.
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	public boolean setDefaultUnit(String key, String unit) {
		if (Units.isUnit(unit) && parameters.containsKey(key)) {
			Quad<String, Double, String, Boolean> newEntry = 
					Quad.make(parameters.get(key).first,parameters.get(key).second, unit,parameters.get(key).fourth);
			parameters.put(key, newEntry);
			return true;
		} else {
			return false;
		}
	}

	
	/**
	 * Checks the parameters against the supplied list, and returns a list of
	 * unrecognized parameters that have been read, possible empty.
	 */
	public List<String> unrecognizedParameters(Collection<String> c) {
//		if (!caseSensitive) {
//			ArrayList<String> c2 = new ArrayList<String>(c.size());
//			Iterator<String> it = c.iterator();
//			while (it.hasNext()) {
//				c2.add(it.next().toLowerCase());
//			}
//			c = c2;
//		}
		ArrayList<String> p = new ArrayList<String>(parameters.keySet());
		p.removeAll(c);
		//return p.toArray(new String[p.size()]);
		return p;
	}

//	/**
//	 * Checks the parameters against the supplied list, and returns a list
//	 * of unrecognized parameters.
//	 */
//	public List<String> validateParameters(Collection<String> c) {
//		c.add("filetype");
////		String u[] = unrecognizedParameters(c);
////		return u;
//		return unrecognizedParameters(c);
//	}

	/**
	 * Copy a ParameterData object into this object.
	 * @param p source ParameterData
	 * @param overwrite if a parameter key exists in both this object and p, if overwrite is true then p's value will be used, otherwise this object's value will be used
	 */
	public void copy(ParameterData p, boolean overwrite) {
		List<String> plist = p.getList();
		for (String key: plist) {
			if (overwrite || ! this.contains(key)) {
				this.set(key, p.getString(key));
			}
		}
	}
	

//	/**
//	 * This interprets a string as a Constants.wsPatternBase-delineated array of integer values. 
//	 */
//	public static int[] intArray(String instring) {
//		String[] s = instring.trim().split(Constants.wsPatternBase);
//		ArrayList<Integer> a = new ArrayList<Integer>();
//		for (int j = 0; j < s.length; j++) {
//			try {
//				a.add(Integer.parseInt(s[j]));
//			} catch (Exception e) {
//			}
//		}
//		int[] aa = new int[a.size()];
//		for (int j = 0; j < a.size(); j++) {
//			aa[j] = a.get(j);
//		}
//		return aa;
//	}
	
	/**
	 * This interprets a string as a Constants.wsPatternBase-delineated list of integer values. 
	 */
	private ArrayList<Integer> integerList(String instring) {
		String[] s = instring.trim().split(Constants.wsPatternBase);
		ArrayList<Integer> a = new ArrayList<Integer>();
		for (int j = 0; j < s.length; j++) {
			try {
				a.add(Integer.parseInt(s[j]));
			} catch (Exception e) {
			}
		}
		return a;
	}

	private ArrayList<Double> doubleList(String instring) {
		String[] s = instring.trim().split(Constants.wsPatternBase);
		ArrayList<Double> a = new ArrayList<Double>();
		for (int j = 0; j < s.length; j++) {
			try {
				a.add(Double.parseDouble(s[j]));
			} catch (Exception e) {
			}
		}
		return a;
	}

	private ArrayList<Boolean> boolList(String instring) {
		String[] s = instring.trim().split(Constants.wsPatternBase);
		ArrayList<Boolean> a = new ArrayList<Boolean>();
		for (int j = 0; j < s.length; j++) {
			try {
				a.add(Boolean.parseBoolean(s[j]));
			} catch (Exception e) {
			}
		}
		return a;
	}

	public ArrayList<Integer> getListInteger(String key) {
		if (parameters.containsKey(key)) {
			return integerList(parameters.get(key).first);
		} else {
			return new ArrayList<Integer>();
		}
	}

	public ArrayList<Double> getListDouble(String key) {
		if (parameters.containsKey(key)) {
			return doubleList(parameters.get(key).first);
		} else {
			return new ArrayList<Double>();
		}
	}

	public ArrayList<String> getListString(String key) {
		if (parameters.containsKey(key)) {
			return stringList(parameters.get(key).first);
		} else {
			return new ArrayList<String>();
		}
	}

	public ArrayList<Boolean> getListBool(String key) {
		if (parameters.containsKey(key)) {
			return boolList(parameters.get(key).first);
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

	
//	/**
//	 * This interprets a string as a Constants.wsPatternBase-delineated array of strings. 
//	 */
//	private String[] stringArray(String instring) {
//		return instring.trim().split(Constants.wsPatternBase);
//	}

	/**
	 * This interprets a string as a Constants.wsPatternBase-delineated list of strings.
	 */
	private ArrayList<String> stringList(String instring) {
		return new ArrayList<String>(Arrays.asList(instring.trim().split(Constants.wsPatternBase)));
	}
	
	
	
	private static final Pattern pp1 = Pattern
			.compile("\"([^\"]+)\"\\s*,\\s*\"([^\"]+)\""); // "([^"]+)\s*,
	private static final Pattern pp2 = Pattern
			.compile("([^,]+)\\s*,\\s*\"([^\"]+)\"");
	private static final Pattern pp3 = Pattern
			.compile("\"([^\"]+)\"\\s*,\\s*(\\S.*)");
	private static final Pattern pp4 = Pattern
			.compile("([^,]+)\\s*,\\s*(\\S.*)");


//	/**
//	 * This interprets a string as a pair of strings separated by commas.
//	 * The elements of the pair may themselves contain commas, but only if the element is surrounded by double quotation marks.
//	 * Quotation mark nesting is not allowed.
//	 */
//	public static Pair<String, String> readPair(String instring) {
//		Matcher m = pp1.matcher(instring);
//		if (m.matches()) {
//			return new Pair<String, String>(m.group(1).trim(), m.group(2)
//					.trim());
//		}
//		m = pp2.matcher(instring);
//		if (m.matches()) {
//			return new Pair<String, String>(m.group(1).trim(), m.group(2)
//					.trim());
//		}
//		m = pp3.matcher(instring);
//		if (m.matches()) {
//			return new Pair<String, String>(m.group(1).trim(), m.group(2)
//					.trim());
//		}
//		m = pp4.matcher(instring);
//		if (m.matches()) {
//			return new Pair<String, String>(m.group(1).trim(), m.group(2)
//					.trim());
//		}
//		return null;
//	}
//
//	/**
//	 * This interprets a string as a triple of strings.
//	 */
//	public static Triple<String, String, String> readTriple(String instring) {
//		String[] s = stringArray(instring);
//		if (s.length < 3)
//			return null;
//		return new Triple<String, String, String>(s[0], s[1], s[2]);
//	}
//
//	/**
//	 * This interprets a string as a list of pairs of strings.
//	 * Each pair must be parenthesis or square bracket delineated.
//	 */
//	public static ArrayList<Pair<String, String>> readPairList(String instring) {
//		String[] s = instring.trim().split(parenPattern);
//		ArrayList<Pair<String, String>> a = new ArrayList<Pair<String, String>>();
//		for (int j = 0; j < s.length; j++) {
//			Pair<String, String> p = readPair(s[j]);
//			if (p != null && !p.first.equals("") && !p.second.equals(""))
//				a.add(p);
//		}
//		return a;
//	}
//
//	/**
//	 * This interprets a string as a list of triples of strings.
//	 * Each triple must be parenthesis or square bracket delineated.
//	 */
//	public static ArrayList<Triple<String, String, String>> readTripleList(
//			String instring) {
//		String[] s = instring.trim().split(parenPattern);
//		ArrayList<Triple<String, String, String>> a = new ArrayList<Triple<String, String, String>>();
//		for (int j = 0; j < s.length; j++) {
//			Triple<String, String, String> p = readTriple(s[j]);
//			if (p != null && !p.first.equals("") && !p.second.equals("")
//					&& !p.third.equals(""))
//				a.add(p);
//		}
//		return a;
//	}
//
//	/**
//	 * This interprets a string as an associative list of string pairs (a Map of key:value pairs)
//	 * The input string should be in the form of a list of string pairs.
//	 * This returns a Map<String,String>
//	 */
//	public static Map<String, String> readMap11(String instring) {
//		ArrayList<Pair<String, String>> a = readPairList(instring);
//		HashMap<String, String> m = new HashMap<String, String>();
//		for (int i = 0; i < a.size(); i++) {
//			m.put(a.get(i).first, a.get(i).second);
//		}
//		return m;
//	}
//
////	public static String writeMap11(Map<String,String> a) {
////		String s = "";
////		boolean first = true;
////		for (String key : a.keySet()) {
////			if (!first) s+=",";
////			s += "("+key+","+a.get(key)+")";
////			first = false;
////		}
////		return s;
////	}
//	
//	/**
//	 * This interprets a string as an associative list of string pairs mapped to single strings (a Map of key:value pairs)
//	 * The input string should be in the form of a list of string triples.
//	 * This returns a Map<Pair<String,String>, String>
//	 */
//	public static Map<Pair<String, String>, String> readMap21(
//			String instring) {
//		ArrayList<Triple<String, String, String>> a = readTripleList(instring);
//		HashMap<Pair<String, String>, String> m = new HashMap<Pair<String, String>, String>();
//		for (int i = 0; i < a.size(); i++) {
//			m.put(new Pair<String, String>(a.get(i).first, a.get(i).second),
//					a.get(i).third);
//		}
//		return m;
//	}
//
////	public static String writeMap21(Map<Pair<String,String>,String> a) {
////		String s = "";
////		boolean first = true;
////		for (Pair<String,String> key : a.keySet()) {
////			if (!first) s+=",";
////			s += "("+key.first+","+key.second+","+a.get(key)+")";
////			first = false;
////		}
////		return s;
////	}
//	
//	/**
//	 * This interprets a string as an associative list of strings mapped to pairs of strings (a Map of key:value pairs)
//	 * The input string should be in the form of a list of string triples.
//	 * This returns a Map<String, Pair<String,String>>
//	 */
//	public static HashMap<String, Pair<String, String>> readMap12(
//			String instring) {
//		ArrayList<Triple<String, String, String>> a = readTripleList(instring);
//		HashMap<String, Pair<String, String>> m = new HashMap<String, Pair<String, String>>();
//		for (int i = 0; i < a.size(); i++) {
//			m.put(a.get(i).first,
//					new Pair<String, String>(a.get(i).second, a.get(i).third));
//		}
//		return m;
//	}
//
//	
//	
////	public static String writeMap12(Map<String,Pair<String,String>> a) {
////		String s = "";
////		boolean first = true;
////		for (String key : a.keySet()) {
////			if (!first) s+=",";
////			s += "("+key+","+a.get(key).first+","+a.get(key).second+")";
////			first = false;
////		}
////		return s;
////	}

	
	/**
	 * Returns true if the stored value for key is likely a boolean
	 * @param key parameter name
	 * @return true if key exists and the value is true/false/t/f, false otherwise
	 */
	public boolean isBoolean(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		if (!parameters.containsKey(key)) {
			return false;
		} else {
			String s = parameters.get(key).getFirst();
			return (s.equalsIgnoreCase("true") || s.equalsIgnoreCase("T") || s.equalsIgnoreCase("false") || s.equalsIgnoreCase("F"));
		}
	}
	
	/**
	 * Returns true if the stored value for key is likely a number
	 * @param key parameter name
	 * @return true if key exists and the value is a parsable number
	 */
	public boolean isNumber(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		if (!parameters.containsKey(key)) {
			return false;
		} else {
			String s = parameters.get(key).getFirst();
			String[] fields = s.split(patternStr);
			if (fields.length > 2) return false; // probably a list
			try {
				Double.parseDouble(fields[0]);
				return true;
			} catch (NumberFormatException e) {
				return false;
			}
		}
	}
	
	/**
	 * Returns true if the stored value for key is likely a string (or list or tuple or map)
	 * @param key parameter name
	 * @return true if key exists and the value is not a parsable number
	 */
	public boolean isString(String key) {
//		if (!caseSensitive)
//			key = key.toLowerCase();
		if (!parameters.containsKey(key)) {
			return false;
		} else {
			return !isNumber(key) && !isBoolean(key);
		}
		
	}
	
	/**
	 * Returns a string listing all parameters and their (original string) values
	 */
	public String toString() {
		String s = "";
		List<String> keys = getList();
		Collections.sort(keys);
		for (String key : keys) {
			String val = parameters.get(key).first;
			s = s+key+" = "+val+"\n";
		}
		return s;
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

	
	/**
	 * @deprecated
	 * Coding assistant utility function.
	 * Given a populated ParameterData, print a guess at the standard method definitions for ParameterDataAcceptor interface.
	 * These will probably need to be corrected by hand.
	 */
	public static void dumpAcceptorCode(ParameterData p) {
		  String s = "public ParameterData getParameterData() {\n  ParameterData p = new ParameterData();\n  updateParameterData(p);\n  return p;\n}\n";
		  System.out.println(s);
		  s = "public void updateParameterData(ParameterData p) {\n";
		  for (String key : p.parameters.keySet()) {
			  if (p.getUnit(key).equals("unspecified")) {
				  s += "  p.set(\""+key+"\","+key+")\n";
			  } else {
				  s += "  p.setInternal(\""+key+"\","+key+",\""+p.getUnit(key)+"\");\n";
			  }
		  }
		  s += "}\n";
		  System.out.println(s);
		  s = "public void setParameters(ParameterData p) {\n";
		  for (String key : p.parameters.keySet()) {
			  s += "  if (p.contains(\""+key+"\")) {\n    ";
			  if (p.isNumber(key)) {
				  s += key+" = p.getValue(\""+key+"\")\n";
			  } else if (p.isBoolean(key)) {
				  s += key+" = p.getBoolean(\""+key+"\")\n";
			  } else {
				  s += key+" = p.getString(\""+key+"\")\n";
			  }
			  s += "  }\n";
		  }		  
		  s += "}\n";
		  System.out.println(s);
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