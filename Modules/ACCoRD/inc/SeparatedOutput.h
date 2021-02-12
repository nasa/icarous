/* 
 * SeparatedOutput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2014-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef SEPARATEDOUTPUT_H
#define SEPARATEDOUTPUT_H

#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "ParameterReader.h"
#include "Quad.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>

namespace larcfm {


/**
 * <p>A class to writes a separated value file (separated by commas, spaces, or tabs).<p>
 * only one file can be created from an object.</p>
 * 
 * <p>
 * Future: handle a memory buffer, standard output/error, file, socket?
 * </p>
 */
class SeparatedOutput : public ErrorReporter {
private:
	std::ostream* writer;
    ErrorLog error;
	
	bool header;         // has header line been written?
	std::vector<std::string> header_str;    // header line
	bool bunits;          // Should units line be written?
	std::vector<std::string> units_str;     // Units type
	std::vector<std::string> line_str;      // raw line
	long   size_l;
	int    column_count;
	int    header_count;
	char   delim;
	std::string space;
	std::string comment_char;
	std::string empty;
	std::vector<std::string> comments;
	std::vector<std::string> params;

	void init();

public:
    /** Create an "empty" separated output */
    SeparatedOutput();

	/** Create a new SeparatedOutput from the given writer 
	 * @param w writer object 
	 * */
	SeparatedOutput(std::ostream* w);


//    /** Copy Constructor.  This should not be used. */
//    SeparatedOutput(const SeparatedOutput& x);

    /** Assignment Operator. */
    SeparatedOutput& operator=(const SeparatedOutput& x);


	void close();

	/** Return the heading for the given column 
	 * @param i column index 
	 * @return heading 
	 * */ 
	std::string getHeading(int i);
 
	/** Return the number of rows written 
	 * @return number of rows
	 * */ 
	long length();

	/** Return the number of columns 
	 * @return number of columns
	 * */ 
	long size();

	/** 
	 * Should the output units be placed in the output file?
	 * @param output if true, then the units should be displayed
	 */
	void setOutputUnits(bool output);

	/**
	 * Set the heading for the given column number, columns begin at 0. 
	 * @param i  the column number
	 * @param name the name of this column heading
	 * @param unit the unit for this column.  If you don't know, then use "unspecified"
	 */
	void setHeading(int i, const std::string& name, const std::string& unit);

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param name the name of this column heading
	 * @param unit the unit for this column.  If you don't know, then use "unspecified"
	 */
	void addHeading(const std::string& name, const std::string& unit);

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param names the name of this column heading
	 * @param units the unit for this column.  If you don't know, then use "unspecified"
	 */
	void addHeading(const std::vector<std::string>& names, const std::vector<std::string>& units);

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param names_and_units an array containing an alternating list of heading names and heading units.  The length of this list must be even.
	 */
	void addHeading(const std::vector<std::string>& names_and_units);

	/** 
	 * Find the index of the column with given heading.  If 
	 * the heading is not found, then -1 is returned. 
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 * @param heading name of heading
	 * @param caseSensitive false if ignoring case
	 * @return  index of heading
	 */
	int findHeading(const std::string& heading, bool caseSensitive);

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 * @param heading1 name of heading
	 * @param heading2 alternate name of heading
	 * @param caseSensitive false if ignore case
	 * @return index of heading
	 */
	int findHeading(const std::string& heading1, const std::string& heading2, bool caseSensitive);

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 * @param heading1 name of heading
	 * @param heading2 alternate name of heading
	 * @param heading3 alternate name of heading
	 * @param caseSensitive false if ignore heading
	 * @return index of heading
	 */
	int findHeading(const std::string& heading1, const std::string& heading2, const std::string& heading3, bool caseSensitive);

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 * @param heading1 name of heading
	 * @param heading2 alternate name of heading
	 * @param heading3 alternate name of heading
	 * @param heading4 alternate name of heading
	 * @param caseSensitive false if ignore heading
	 * @return index of heading
	 */
	int findHeading(const std::string& heading1, const std::string& heading2, const std::string& heading3, const std::string& heading4, bool caseSensitive);
	
	/** 
	 * Returns the units string for the i-th column. If an invalid 
	 * column is entered, then "unspecified" is returned. 
	 * @param i column index
	 * @return unit
	 */
	std::string getUnit(int i);
    
    /**
     * Sets the next column value equal to the given value. The value is in internal units.
     */
	void setColumn(int i, double val);
	
    /**
     * Sets the next column value equal to the given value.  The value is in internal units.
     */
	void setColumn(double val);

    /**
     * Sets the next column value equal to the given value.
     * @param i index of column
     * @param val value
     */
	void setColumn(int i, const std::string& val);
	
    /**
     * Adds the given value to the next column.
     * @param val value
     */
	void addColumn(const std::string& val);

//    /**
//     * Adds each of the given values to the next columns.
//     */
//	void addColumn(OutputList ol);

    /**
     * Adds each of the given values to the next columns.
     * @param vals  list of values
     */
	void addColumn(const std::vector<std::string>& vals);

   /** 
    * Sets the column delimiter to a tab
    */
   void setColumnDelimiterTab();
   
   /** 
    * Sets the column delimiter to a comma
    */
   void setColumnDelimiterComma();
   
   /** 
    * Sets the column delimiter to a space.  If a space is used as a 
    * separator then the empty value should be set (see setEmptyValue).
    */
   void setColumnDelimiterSpace();
   
   /** 
    * Sets the number of extra spaces after the delimiter
    * @param num number of spaces
    */
   void setColumnSpace(int num);
   
   /** 
    * The value to be displayed if a column is &quot;skipped&quot;.  Empty values are only added inside a line, not at the end.
    * @param e value to indicate empty
    */
   void setEmptyValue(std::string e);
   
   /** 
    * Set the code indicating the start of a comment.
    * @param c comment character
    */
   void setCommentCharacter(const std::string& c);
   
   /** 
    * Additively set parameters.  (This does not delete existing parameters, but will overwrite them.) 
    * @param pr  parameter object
    */
   void setParameters(const ParameterData& pr);

   void setParameter(const std::string& key, const std::string& value);


   /**
    * Clear all parameters.
    */
   void clearParameters();

   /** 
    * Add the following line to the comments.
    * @param c comment string
    */
   void addComment(const std::string& c);
   
	/**
	 * Writes a line of the output.  The first call to writeLine will write the column headings, units, etc.
	 */
	void writeLine();
    
	void flush();

private:
    void print_line(std::vector<std::string> vals); // throws IOException;

public:
    std::string toString();

	
    // ErrorReporter Interface Methods
  
    bool hasError() const;
    bool hasMessage() const;
    std::string getMessage();
    std::string getMessageNoClear() const;
   
};

}

#endif //SEPARATEDOUTPUT_H
