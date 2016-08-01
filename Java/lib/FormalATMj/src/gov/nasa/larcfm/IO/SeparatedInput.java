/* 
 * SeparatedInput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Cesar Munoz, George Hagen
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterReader;
import gov.nasa.larcfm.Util.Units;

import java.io.Reader;
import java.io.LineNumberReader;
import java.io.IOException;
import java.util.regex.PatternSyntaxException;

/**
 * A class to parse files with input that are in columns 
 * separated by commas, spaces, or tabs.  <p>
 * 
 * General notes on the file:
 * <ul>
 * <li>All blank lines are skipped
 * <li>All lines that begin with a '#' are skipped
 * <li>The first section of the file contains parameters.  This
 * parameter section must be before the data
 * <li>The next section is the main data of the file.  It contains 
 * columns of data.
 * <li>The file can be read with case sensitive or insensitive strings.
 * Case sensitivity applies to header names and parameter keys (if case 
 * insensitive, these will be converted to lower case internally)
 * <li>The first line after the column name heading may be an optional line 
 * specifying nonstandard units for each column.  These are represented as 
 * [nmi], [fpm], [deg], etc.  If so defined, then values 
 * stored will be converted from these units into internal units.  
 * Otherwise it is assumed that values are already in internal units.  
 * If the default units as understood by the end user are not internal 
 * units (for example using degrees for latitude instead of radians), 
 * then further processing of the stored data may be necessary upon 
 * retrieval. Use [unspecified] for any column that has no applicable units 
 * (e.g. strings or booleans).
 * </ul>
 * 
 * Notes on the columns of data
 * <ul>
 * <li>The columns of data may be separated by any number of commas, spaces, 
 * or tabs (but see {@link setColumnDelimiters})
 * <li>The first line of the columns of data is considered the "headings."
 * These are strings which can be accessed with the getHeading() method.
 * <li>The next line is optional.  It contains the a string representing the 
 * units for a column.  A line is considered a line of units if at least half 
 * of the fields read in are interpreted as valid units.
 * <li>The columns of data can be strings or double-precision floating point 
 * values
 * <li>If the values are doubles, then they are assumed to be in internal units.
 * If a unit type is specified, their values will be converted into internal units
 * and stored.  The end user is responsible for converting values if the unspecified
 * default units are something other than the internal ones.
 * </ul>
 * 
 * Parameters are essentially a "key/value" pair.  Once an parameter is entered
 * (either from the setParameter method or from the file), then that parameter
 * name is reserved.  If any future updates to the parameter (either from 
 * the setParameter method or the file), then the previous value will be 
 * overwritten.  Parameters have the following rules:
 * <ul>
 * <li>Parameters have the basic form "key = value"
 * <li>Parameter keys can be any combination of letters and numbers.
 * <li>There must be spaces around the equals sign
 * <li>The value may be a string, a double-precision value, or a boolean value.
 * <li>If the value is a double precision value, then units may be added.  
 * For example,  "param = 10 [nmi]"
 * <li>The units are optional.
 * <li>Boolean values are represented by T, F, true, false, TRUE, or FALSE.
 * <li>If a parameter has not been set and it is read, something will be 
 * returned.  It is not defined what that something is.  To avoid this,
 * check is a parameter exists with the containsParameter() method.
 * </ul>
 */
public final class SeparatedInput implements ParameterReader, ErrorReporter {

    private static class SeparatedInputException extends Exception {
      static final long serialVersionUID = 0;
    
      SeparatedInputException(String name) {
        super(name);
      }
    };
  

	private LineNumberReader reader;

    private ErrorLog error;
	
	private boolean header;         // header line read in
	private String[] header_str;    // header line raw string
	private boolean units;          // units line read in
	private String[] units_str;     // Units type
	private double[] units_factor;  // Units conversion value
	private String[] line_str;      // raw line

	private boolean fixed_width;    // Instead of using a delimiter, use fixed width columns
	private int[] width_int;        // The width of columns

    private boolean caseSensitive;

	private String patternStr;
	
    //private HashMap<String, Quad<String, Double, String, Boolean>> parameters;
	private ParameterData parameters;

    /** Create an "empty" separated input that can only store parameters */
    public SeparatedInput() {
		reader = null;
		header = false;
		units = false;
        error = new ErrorLog("SeparatedInput");
        parameters = new ParameterData();
        caseSensitive = true;
        //parameters.setCaseSensitive(caseSensitive);
        //parameters = new HashMap<String,Quad<String, Double, String, Boolean>>();
        patternStr = Constants.wsPatternBase;
     }
    
	/** Create a new SeparatedInput from the given reader */
	public SeparatedInput(Reader r) {
		reader = new LineNumberReader(r);
		header = false;
		units = false;
        error = new ErrorLog("SeparatedInput(Reader)");
        parameters = new ParameterData();
        caseSensitive = true;
        //parameters.setCaseSensitive(caseSensitive);
        //parameters = new HashMap<String,Quad<String, Double, String, Boolean>>();
        patternStr = Constants.wsPatternBase;
	}
	
	/** Return the heading for the given column */ 
	public String getHeading(int i) {
      if (i < 0 || i >= line_str.length) {
        error.addWarning("getHeading index "+i+", line "+reader.getLineNumber()+" out of bounds");
        return "";
      }
		return header_str[i];
	}
 
	/** Return the number of columns */ 
	public int size() {
		return header_str.length;
	}
 
	/** 
	 * Find the index of the column with given heading.  If 
	 * the heading is not found, then -1 is returned. 
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading) {
		int rtn = -1;
		if ( ! caseSensitive) {
			heading = heading.toLowerCase();
		}
		for (int i = 0; i < header_str.length; i++) {
			String heading_col = (caseSensitive ? header_str[i] : header_str[i].toLowerCase());
			if (heading.equals(heading_col)) {
				rtn = i;
				break;
			}
		}
		return rtn;
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading1, String heading2) {
        return findHeading(heading1, heading2, "", "");
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading1, String heading2, String heading3) {
        return findHeading(heading1, heading2, heading3, "");
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading1, String heading2, String heading3, String heading4) {
		int r = findHeading(heading1);
		if (r < 0 && ! heading2.equals("")) {
			r = findHeading(heading2);
		}
		if (r < 0 && ! heading3.equals("")) {
			r = findHeading(heading3);
		}
		if (r < 0 && ! heading4.equals("")) {
			r = findHeading(heading4);
		}
		return r;
	}
	
	public int findHeading(String[] headings) {
		int r = -1;
		for (int i = 0; r < 0 && i < headings.length; i++) {
			r = findHeading(headings[i]);
		}
		return r;
	}
	
	/** 
	 * Returns the units string for the i-th column. If an invalid 
	 * column is entered, then "unspecified" is returned. 
	 */
	public String getUnit(int i) {
      if ( ! units || i < 0 || i >= units_str.length) {
        return "unspecified";
      }
      return units_str[i];
	}

	private double getUnitFactor(int i) {
		if ( ! units || i < 0 || i >= units_str.length) {
        	return Units.unspecified;
		}
		return units_factor[i];
	}

    /**
     * Returns true if a line defining column units was detected.
     */
    public boolean unitFieldsDefined() {
      return units;
    }

    /** If set to false, all read-in headers and parameters will be converted to lower case. */
    public void setCaseSensitive(boolean b) {
      caseSensitive = b;
      //parameters.setCaseSensitive(b);
    }

    /** If true, headers & parameters will be case sensitive.  If false, all headers & parameters will be converted to lower case. */
    public boolean getCaseSensitive() {
      return caseSensitive;
    }  
    
    public ParameterData getParametersRef() {
		return parameters;
	}

    /**
     * Return true if the column entry has some (nonempty) value, otherwise return false.
     * A value of "-" is considered a lack of a value.
     */
    public boolean columnHasValue(int i) {
   		return (i >= 0 && i < line_str.length && !line_str[i].equals("") && !line_str[i].equals("-"));    	
    }
    
    /**
     * Returns the raw string of the given column read.
     */
	public String getColumnString(int i) {
       if (i < 0 || i >= line_str.length) {
         error.addWarning("getColumnString index "+i+", line "+reader.getLineNumber()+" out of bounds");
         return "";
       }
	   return line_str[i];
	}
	
    /**
     * Returns the value of the given column (as a double) in internal units.
     */
	public double getColumn(int i) {
      if (i < 0 || i >= line_str.length) {
        error.addWarning("getColumn index "+i+", line "+reader.getLineNumber()+" out of bounds");
        return 0.0;
      }
		double rtn = 0.0;
    	try {
    		rtn = Units.from(getUnitFactor(i), Double.parseDouble(line_str[i]));
    	} catch (NumberFormatException e) {
            error.addWarning("could not parse as a double in getColumn("+i+"), line "+reader.getLineNumber()+": "+line_str[i]);
    		rtn = 0.0;  // arbitrary value
    	}
    	return rtn;
    }

	/** 
	 * Returns the value of the given column (as a double) in internal units.  If 
	 * no units were specified in the file, then this value is assumed to be
	 * in the given default units and an appropriate conversion takes place.  
	 */
	public double getColumn(int i, String default_unit) {
		if (getUnit(i).equals("unspecified")) {
			return Units.from(default_unit, getColumn(i));
		}
		return getColumn(i);
	}

   /** 
    * Sets the regular expression used to divide each line into columns.  If the supplied parameter 
    * is not a valid regular expression, then the current delimiter is retained.  This should be set 
    * before the first "readLine" is called. 
    * 
    */
   public void setColumnDelimiters(String delim) {
	   try {
		   @SuppressWarnings("unused")  // because we are just error checking the supplied parameter
		   String[] tokens = (new String("This is a test")).split(delim);
		   patternStr = delim;
	   } catch (PatternSyntaxException e) {
		   error.addWarning("invalid delimiter string: "+delim+" retained original: "+patternStr);
		   // ignore the attempt to set an invalid delimiter string
	   }
   }

	public void setFixedColumn(String widths, String nameList, String unitList) {
		try {
			String[] fields;
			fields = widths.split(",");
			width_int = new int[fields.length];
			for (int i = 0; i < fields.length; i++) {
				width_int[i] = Integer.parseInt(fields[i]);
			}
			
			fields = nameList.split(",");
			if (width_int.length != fields.length) {
				throw new Exception("In parsing fixed width file, number of names does not match number of widths");
			}
			header_str = new String[fields.length];
			for (int i = 0; i < fields.length; i++) {
				header_str[i] = fields[i];
			}
			
			fields = unitList.split(",");
			if (width_int.length != fields.length) {
				throw new Exception("In parsing fixed width file, number of units does not match number of widths");
			}
			units_str = new String[fields.length];
			units_factor = new double[fields.length];
			for (int i = 0; i < fields.length; i++) {
				if (Units.isUnit(fields[i])) {
					units_str[i] = fields[i];
					units_factor[i] = Units.getFactor(fields[i]);					
				} else {
					units_str[i] = "unspecified";
					units_factor[i] = Units.unspecified;
				}
			}
			
			fixed_width = true;
			header = true;
			units = true;
		} catch (Exception e) {
			error.addError(e.getMessage());
		}
	}

	public void skipLines(int numLines) {
		if (reader == null) return;
		try {
			for (int i = 0; i < numLines; i++) {
				if (reader.readLine() == null) {
					break;
				}
			}
		} catch (IOException e) { 
			error.addError("*** An IO error occured at line "+reader.getLineNumber()
					+ "The error was:"+e.getMessage());
		}
	}


	/**
	 * Reads a line of the input.  The first call to readLine will read the column headings, units, etc.
	 * 
	 * @return true if end of file
	 */
	public boolean readLine() {
	    String str = null;
		try {
		    while ((str = reader.readLine()) != null) {
		    	str = str.trim();
		    	// Skip empty lines or lines starts with #
		    	if (str.length() == 0 || str.charAt(0) == '#') {
		    		continue;
		    	}
                if ( ! header) {
		    		header = process_preamble(str);
		    	} else if ( ! units) {
                  try {
		    		units = process_units(str);
                  } catch (SeparatedInputException e) {
                    // use default units
                    units = true;
                    process_line(str);
                    break;
                  }
		    	} else {
		    		process_line(str);
		    		break;
		    	}
		    }//while    
		} catch (IOException e) { 
          error.addError("*** An IO error occured at line "+reader.getLineNumber()
                         + "The error was:"+e.getMessage());
          str = null;
		}
		if (str == null) {
			return true; // end of file
		} else {
			return false;
		}
	}
    
    /** Returns the number of the most recently read in line */
    public int lineNumber() {
      return reader.getLineNumber();
    }
	
    private boolean process_preamble(String str) {

    	String[] fields = str.split("=",2);
    	//String[] fields = str.split(patternStr);
    	//f.pln("fields: "+Arrays.toString(fields));
    	
        // parameter keys are lower case only
    	if (fields.length == 2 && fields[0].length() > 0) {
    	    String id = fields[0].trim();
            if ( ! caseSensitive) id = id.toLowerCase();
            parameters.set(id,fields[1].trim());
    	    return false;
    	} else if (fields.length == 1 && str.contains("=")) {
    	    String id = fields[0].trim();
    		parameters.set(id,"");
    		return false;
    	} else {
        	fields = str.split(patternStr);
    		if ( ! caseSensitive) {
    			for (int i = 0; i < fields.length; i++) {
    				fields[i] = fields[i].toLowerCase().trim();
        		} 
      		}
      		header_str = fields;
    		return true;
    	} 
    }

	private boolean process_units(String str) throws SeparatedInputException {
    	String[] fields = str.split(patternStr);
    	
        // if units are optional, we need to determine if any were read in...
        // a unit line is considered true if AT LEASE HALF of the fields read in are interpreted as valid units
        int notFound = 0;
        
    	units_str = new String[fields.length];
    	units_factor = new double[fields.length];
    	for (int i=0; i < fields.length; i++) {
    		
    		try {
    			units_str[i] = Units.clean(fields[i]);
    			units_factor[i] = Units.getFactor(units_str[i]);
    			if (units_str[i].equals("unspecified")) {
    				notFound++;
    			}
    		} catch (Units.UnitException e) {
                notFound++;
    			units_str[i] = "unspecified";
    			units_factor[i] = Units.unspecified;
    		}
    	}
        if (notFound > fields.length/2) {
          throw new SeparatedInputException("default units");
        }
		return true;
	}

	private void process_line(String str) {
    	String[] fields;
		if (fixed_width) {
			int idx = 0;
			fields = new String[width_int.length];
			for (int i = 0; i < width_int.length; i++) {
				int end = idx+width_int[i];
				if (idx < str.length() && end <= str.length()) {
					fields[i] = str.substring(idx, idx+width_int[i]);					
				} else if (idx < str.length() && end > str.length()) {
					fields[i] = str.substring(idx, str.length());										
				} else {
					fields[i] = "";
				}
				idx = idx + width_int[i];
			}
		} else {
	    	fields = str.split(patternStr);
	    	//f.pln(" $$ SeparatedInput.process_line: patternStr = "+patternStr);
	    	for (int i = 0; i < fields.length; i++) {
	    		fields[i] = fields[i].trim();
	    		//f.pln(" $$ SeparatedInput.process_line: fields["+i+"] = "+fields[i]);
	    	}
		}		
    	line_str = fields;
	}
	
	/** Return the last line read as a comma-delineated string */
	public String getLine() {
		String s = "";
		if (line_str.length > 0) {
			s = line_str[0];
		}
		for (int i = 1; i < line_str.length; i++) {
			s += ", "+line_str[i].trim();
		}
		return s;
	}
	
	public void close() {
    	if (reader != null) {
    		try {
    			reader.close();
    		} catch (IOException e) {
    			error.addError("IO Exception in close(): "+e.getMessage());
    		}
    	}
	}
	
    // ErrorReporter Interface Methods
  
    public boolean hasError() {
      return error.hasError();
    }
    public boolean hasMessage() {
      return error.hasMessage();
    }
    public String getMessage() {
      return error.getMessage();
    }
    public String getMessageNoClear() {
      return error.getMessageNoClear();
    }
  
  
    public String toString() {
        String str = "SeparateInput: \n header_str:";
    	for (int i=0; i < header_str.length; i++) {
	    str = str + ", " + header_str[i];  
	}

//        str = str + "\n parameters: "+parameters.toString();

        str = str + "\n units_str:";
    	for (int i=0; i < units_str.length; i++) {
	    str = str + ", " + units_str[i];  
	}

        str = str+ "\n line_str:";
    	for (int i=0; i < line_str.length; i++) {
	    str = str + ", " + line_str[i];  
	}

        return str;
    }

}
