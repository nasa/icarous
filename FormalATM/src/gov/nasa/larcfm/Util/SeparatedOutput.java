/* 
 * SeparatedOutput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.io.Writer;
import java.io.PrintWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

/**
 * A class to writes a separated value file (separated by commas, spaces, or tabs).<p>
 * only one file can be created from an object.
 */
public final class SeparatedOutput implements ErrorReporter {
	private PrintWriter writer;
    private ErrorLog error;
	
	private boolean header;         // has header line been written? 
	private ArrayList<String> header_str;    // header line 
	private boolean units;          // Should units line be written?
	private ArrayList<String> units_str;     // Units type
	private ArrayList<String> line_str;      // raw line
	private long   size;
	private int    column_count;
	private int    header_count;
	private char   delim;
	private String space;
	private String comment_char;
	private String empty;
	private ArrayList<String> comments;
	private ArrayList<String> params;

	private static final String nl = System.getProperty("line.separator");

	private void init() {
		header = false;
		units = false;
        size = 0;
        column_count = -1;
        header_count = -1;
        delim = ',';
        space = "";
        empty = "";
        comment_char = "# ";
        comments = new ArrayList<String>();
        header_str = new ArrayList<String>();
        units_str = new ArrayList<String>();
        line_str = new ArrayList<String>();
        params = new ArrayList<String>();
	}

	/** Create a new SeparatedInput from the given reader */
	public SeparatedOutput(Writer w) {
		writer = new PrintWriter(w);
        error = new ErrorLog("SeparatedOutput(Writer)");
        init();
	}

//    /** A new SeparatedOutput to the given file. */
//	public SeparatedOutput(String filename) {
//		error = new ErrorLog("StateReader("+filename+")");
//		if (filename == null || filename.equals("")) {
//			error.addError("No file specified");
//			return;
//		}
//		try {    
//			FileWriter fw = new FileWriter(filename);
//			writer = new PrintWriter(fw);
//		} catch (IOException e) {
//			error.addError("File "+filename+" could not be written.");
//			return;
//		}
//		init();
//	}
//
	public void close() {
//		if (writer != null) {
//			writer.close();
//		}
	}

	/** Return the heading for the given column */ 
	public String getHeading(int i) {
      if (i < 0 || i >= line_str.size()) {
        error.addWarning("getHeading index "+i+", out of bounds");
        return "";
      }
		return header_str.get(i);
	}
 
	/** Return the number of rows written */ 
	public long length() {
		return size;
	}

	/** Return the number of columns */ 
	public long size() {
		return header_str.size();
	}

	/** 
	 * Should the output units be placed in the output file?
	 * @param output if true, then the units should be displayed
	 */
	public void setOutputUnits(boolean output) {
		units = output;
	}

	/**
	 * Set the heading for the given column number, columns begin at 0. 
	 * @param i  the column number
	 * @param name the name of this column heading
	 * @param unit the unit for this column.  If you don't know, then use "unspecified"
	 */
	public void setHeading(int i, String name, String unit) {
		while (header_str.size() <= i) {
			header_str.add(empty);
		}
		header_str.set(i, name);
		if ( ! Units.isUnit(unit)) {
			unit = "unspecified";
		}
		while (units_str.size() <= i) {
			units_str.add("unspecified");
		}
		units_str.set(i, unit);
	}

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param name the name of this column heading
	 * @param unit the unit for this column.  If you don't know, then use "unspecified"
	 */
	public void addHeading(String name, String unit) {
		header_count++;
		setHeading(header_count, name, unit);
	}

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param names the name of this column heading
	 * @param units the unit for this column.  If you don't know, then use "unspecified"
	 */
	public void addHeading(List<String> names, List<String> units) {
		Iterator<String> n = names.iterator();
		Iterator<String> u = units.iterator();
		while (n.hasNext() && u.hasNext()) {
			addHeading(n.next(), u.next());
		}
	}

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param names_and_units an array containing an alternating list of heading names and heading units.  The length of this list must be even.
	 */
	public void addHeading(List<String> names_and_units) {
		Iterator<String> nu = names_and_units.iterator();
		while (nu.hasNext()) {
			String n = nu.next();
			if (nu.hasNext()) {
				addHeading(n, nu.next());
			}
		}
	}

	/** 
	 * Find the index of the column with given heading.  If 
	 * the heading is not found, then -1 is returned. 
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading, boolean caseSensitive) {
		int rtn = -1;
		if ( ! caseSensitive) {
			heading = heading.toLowerCase();
		}
		for (int i = 0; i < header_str.size(); i++) {
			if (heading.equals(header_str.get(i))) {
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
	public int findHeading(String heading1, String heading2, boolean caseSensitive) {
        return findHeading(heading1, heading2, "", "", caseSensitive);
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading1, String heading2, String heading3, boolean caseSensitive) {
        return findHeading(heading1, heading2, heading3, "", caseSensitive);
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	public int findHeading(String heading1, String heading2, String heading3, String heading4, boolean caseSensitive) {
		int r = findHeading(heading1, caseSensitive);
		if (r < 0 && ! heading2.equals("")) {
			r = findHeading(heading2, caseSensitive);
		}
		if (r < 0 && ! heading3.equals("")) {
			r = findHeading(heading3, caseSensitive);
		}
		if (r < 0 && ! heading4.equals("")) {
			r = findHeading(heading4, caseSensitive);
		}
		return r;
	}
	
	/** 
	 * Returns the units string for the i-th column. If an invalid 
	 * column is entered, then "unspecified" is returned. 
	 */
	public String getUnit(int i) {
      if ( i < 0 || i >= units_str.size()) {
        return "unspecified";
      }
      return units_str.get(i);
	}
    
    /**
     * Sets the next column value equal to the given value. The value is in internal units.
     */
	public void setColumn(int i, double val) {
		setColumn(i, f.FmPrecision(Units.to(getUnit(i), val),1));
	}
	
    /**
     * Sets the next column value equal to the given value.  The value is in internal units.
     */
	public void setColumn(double val) {
		setColumn(++column_count, val);
    }

    /**
     * Sets the next column value equal to the given value.
     */
	public void setColumn(int i, String val) {
		while (line_str.size() <= i) {
			line_str.add(empty);
		}
		line_str.set(i, val);
	}
	
    /**
     * Adds the given value to the next column.
     */
	public void addColumn(String val) {
		setColumn(++column_count, val);
    }

    /**
     * Adds each of the given values to the next columns.
     */
	public void addColumn(OutputList ol) {
		addColumn(ol.toStringList());
    }

    /**
     * Adds each of the given values to the next columns.
     */
	public void addColumn(List<String> vals) {
		for (String val: vals) {
			addColumn(val);
		}
    }

   /** 
    * Sets the column delimiter to a tab
    */
   public void setColumnDelimiterTab() {
	   delim = '\t';
   }
   
   /** 
    * Sets the column delimiter to a comma
    */
   public void setColumnDelimiterComma() {
	   delim = ',';
   }
   
   /** 
    * Sets the column delimiter to a space.  If a space is used as a 
    * separator then the empty value should be set (see setEmptyValue).
    */
   public void setColumnDelimiterSpace() {
	   delim = ' ';
   }
   
   /** 
    * Sets the number of extra spaces after the delimiter
    */
   public void setColumnSpace(int num) {
	   if (num < 1) space = "";
	   if (num == 1) space = " ";
	   if (num == 2) space = "  ";
	   if (num == 3) space = "   ";
	   if (num == 4) space = "    ";
	   if (num == 5) space = "     ";
	   if (num == 6) space = "      ";
	   if (num == 7) space = "       ";
	   if (num == 8) space = "        ";
	   if (num == 9) space = "         ";
	   if (num == 10) space = "          ";
	   if (num == 11) space = "           ";
	   if (num == 12) space = "            ";
	   if (num == 13) space = "             ";
	   if (num == 14) space = "              ";
	   if (num >= 15) space = "               ";
   }
   
   /** 
    * The value to be displayed if a column is "skipped".  Empty values are only added inside a line, not at the end.
    */
   public void setEmptyValue(String e) {
	   empty = e;
   }
   
   /** 
    * Set the code indicating the start of a comment.
    */
   public void setCommentCharacter(String c) {
	   comment_char = c;
   }
   
   /** 
    * Set parameters.  Use all the parameters in the reader.
    */
   public void setParameters(ParameterData pr) {
	   for (String p: pr.getList()) {
		   String s = pr.getString(p);
		   params.add(p+" = "+s);
	   }
   }

   /** 
    * Clear all parameters.
    */
   public void clearParameters() {
	   params.clear();
   }
   
   /** 
    * Add the following line to the comments.
    */
   public void addComment(String c) {
		String [] tmparray = c.split("\n");
		for (String tmp: tmparray) {
			comments.add(tmp);			
		}
   }
   
	/**
	 * Writes a line of the output.  The first call to writeLine will write the column headings, units, etc.
	 */
	public void writeLine() {
		try {
			if ( comments.size() != 0 ) {
				for (String line: comments) {
					writer.println(comment_char+line);
					size++;
				}
				comments.clear();
			}
			if ( ! header) {
				if (params.size() != 0) {
					for (String p:params) {
						writer.println(p);
						size++;
					}
				}
				print_line(header_str);
				if (units) {
					print_line(units_str);
				}
				header = true;
			}
			print_line(line_str);
			line_str.clear();
			column_count = -1;
		} catch (IOException e) { 
          error.addError("*** An IO exeception has occured: "+e.getMessage());
		}
	}
    
    private void print_line(ArrayList<String> vals) throws IOException {
    	if (vals.size() == 0) {
    		return;
    	}
    	writer.print(vals.get(0));
    	for (int i = 1; i < vals.size(); i++) {
    		writer.print(delim+space+vals.get(i));
    	}
    	writer.println();
    	size++;
    }

    public void write(String hdr) {
    	writer.print(hdr);
    }
    
    
    public String toString() {
    	String str = "SeparateOutput: ";
    	str += nl;
    	str += " header_str:";
    	for (String line: header_str) {
    		str += ", " + line;  
    	}

    	str += nl;
    	str += " units_str:";
    	for (String line: units_str) {
    		str += ", " + line;  
    	}

    	str += nl;
    	str += " line_str:";
    	for (String line: line_str) {
    		str += ", " + line;  
    	}

    	return str;
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
  
   
}
