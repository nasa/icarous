/* 
 * SeparatedOutput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2014-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "SeparatedOutput.h"
#include "string_util.h"
#include "format.h"
#include <vector>
#include <iostream>
#include <cstdio>
#include <string>

namespace larcfm {
	using std::vector;
	using std::string;

	void SeparatedOutput::init() {
		header = false;
		units = false;
        size_l = 0;
        column_count = -1;
        header_count = -1;
        delim = ',';
        space = "";
        empty = "";
        comment_char = "# ";
//        comments = new ArrayList<String>();
//        header_str = new ArrayList<String>();
//        units_str = new ArrayList<String>();
//        line_str = new ArrayList<String>();
//        params = new ArrayList<String>();
	}

	SeparatedOutput::SeparatedOutput(): error("SeparatedOutput()") {
		//writer = new PrintWriter(w);
		writer = NULL;
        //error = new ErrorLog("SeparatedOutput(Writer)");
        init();
	}

	/** Create a new SeparatedInput from the given reader */
	SeparatedOutput::SeparatedOutput(std::ostream* w): error("SeparatedOutput(Writer)") {
		//writer = new PrintWriter(w);
		writer = w;
        //error = new ErrorLog("SeparatedOutput(Writer)");
        init();
	}

	// This should never be used, it should exit
	SeparatedOutput& SeparatedOutput::operator=(const SeparatedOutput& x) {
		error = x.error;
		writer = x.writer;
		header = x.header;
		units = x.units;
		header_str = x.header_str;
		units_str = x.units_str;
		line_str = x.line_str;

		size_l = x.size_l;
		column_count = x.column_count;
		header_count = x.header_count;
		delim = x.delim;
		space = x.space;
		comment_char = x.comment_char;
		empty = x.empty;
		comments = x.comments;
		params = x.params;

		//std::cout << "SeparatedOutput assignment operator failure" << std::endl;
		//exit(1);
		return *this;  // should never get here
	}

	void SeparatedOutput::close() {
		if (writer != NULL) {
//			try {
//				std::ofstream* fwriter = dynamic_cast<std::ofstream*>(writer);
//				fwriter->close();
//			} catch (std::bad_cast e) {
//				// not an ofstream, so do nothing
//			}
			writer = NULL;
		}
	}

	/** Return the heading for the given column */ 
	std::string SeparatedOutput::getHeading(int i) {
      if (i < 0 || i >= (int) line_str.size()) {
        error.addWarning("getHeading index "+Fm0(i)+", out of bounds");
        return "";
      }
		return header_str[i];
	}
 
	/** Return the number of rows written */ 
	long SeparatedOutput::length() {
		return size_l;
	}

	/** Return the number of columns */ 
	long SeparatedOutput::size() {
		return header_str.size();
	}

	/** 
	 * Should the output units be placed in the output file?
	 * @param output if true, then the units should be displayed
	 */
	void SeparatedOutput::setOutputUnits(bool output) {
		units = output;
	}

	/**
	 * Set the heading for the given column number, columns begin at 0. 
	 * @param i  the column number
	 * @param name the name of this column heading
	 * @param unit the unit for this column.  If you don't know, then use "unspecified"
	 */
	void SeparatedOutput::setHeading(int i, const std::string& name, const std::string& unit) {
		string unit_new = unit;
		while ((int) header_str.size() <= i) {
			header_str.push_back(empty);
		}
		header_str[i] = name;
		if ( ! Units::isUnit(unit)) {
			unit_new = "unspecified";
		}
		while ((int) units_str.size() <= i) {
			units_str.push_back("unspecified");
		}
		units_str[i] = unit_new;
	}

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param name the name of this column heading
	 * @param unit the unit for this column.  If you don't know, then use "unspecified"
	 */
	void SeparatedOutput::addHeading(const std::string& name, const std::string& unit) {
		++header_count;
		setHeading(header_count, name, unit);
	}

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param names the name of this column heading
	 * @param units the unit for this column.  If you don't know, then use "unspecified"
	 */
	void SeparatedOutput::addHeading(const std::vector<std::string>& names, const std::vector<std::string>& units) {
		vector<string>::const_iterator n = names.begin();
		vector<string>::const_iterator u = units.begin();
		while(n != names.end() && u != units.end()) {
			string nn = *n;
			string uu = *u;
			addHeading(nn, uu);
			++n;
			++u;
		}
	}

	/** 
	 * Add the given heading (and unit) to the next column
	 * @param names_and_units an array containing an alternating list of heading names and heading units.  The length of this list must be even.
	 */
	void SeparatedOutput::addHeading(const std::vector<std::string>& names_and_units) {
		vector<string>::const_iterator nu = names_and_units.begin();
		while (nu != names_and_units.end()) {
			std::string n = *nu;
			++nu;
			if (nu != names_and_units.end()) {
				string nuu = *nu;
				addHeading(n, nuu);
			}
			++nu;
		}
	}

	/** 
	 * Find the index of the column with given heading.  If 
	 * the heading is not found, then -1 is returned. 
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	int SeparatedOutput::findHeading(const std::string& heading, bool caseSensitive) {
		string heading_new = heading;
		int rtn = -1;
		if ( ! caseSensitive) {
			heading_new = toLowerCase(heading);
		}
		for (int i = 0; i < (int) header_str.size(); ++i) {
			if (heading_new == header_str[i]) {
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
	int SeparatedOutput::findHeading(const std::string& heading1, const std::string& heading2, bool caseSensitive) {
        return findHeading(heading1, heading2, "", "", caseSensitive);
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	int SeparatedOutput::findHeading(const std::string& heading1, const std::string& heading2, const std::string& heading3, bool caseSensitive) {
        return findHeading(heading1, heading2, heading3, "", caseSensitive);
	}

	/** 
	 * Find the index of the column with any of the given headings.  If none of 
	 * the given headings is found, then -1 is returned. This tries to find the 
	 * first heading, and if it finds it then returns that index.  If it doesn't 
	 * find it, it moves to the next heading, etc.
     * Note: If you are getting some oddly large indexes, there are probably some nonstandard characters in the input.
	 */
	int SeparatedOutput::findHeading(const std::string& heading1, const std::string& heading2, const std::string& heading3, const std::string& heading4, bool caseSensitive) {
		int r = findHeading(heading1, caseSensitive);
		if (r < 0 && heading2 != "") {
			r = findHeading(heading2, caseSensitive);
		}
		if (r < 0 && heading3 != "") {
			r = findHeading(heading3, caseSensitive);
		}
		if (r < 0 && heading4 != "") {
			r = findHeading(heading4, caseSensitive);
		}
		return r;
	}
	
	/** 
	 * Returns the units string for the i-th column. If an invalid 
	 * column is entered, then "unspecified" is returned. 
	 */
	std::string SeparatedOutput::getUnit(int i) {
      if ( i < 0 || i >= (int) units_str.size()) {
        return "unspecified";
      }
      return units_str[i];
	}
    
    /**
     * Sets the next column value equal to the given value. The value is in internal units.
     */
	void SeparatedOutput::setColumn(int i, double val) {
		setColumn(i, FmPrecision(Units::to(getUnit(i), val),1));
	}
	
    /**
     * Sets the next column value equal to the given value.  The value is in internal units.
     */
	void SeparatedOutput::setColumn(double val) {
		setColumn(++column_count, val);
    }

    /**
     * Sets the next column value equal to the given value.
     */
	void SeparatedOutput::setColumn(int i, const std::string& val) {
		while ((int) line_str.size() <= i) {
			line_str.push_back(empty);
		}
		line_str[i] = val;
	}
	
    /**
     * Adds the given value to the next column.
     */
	void SeparatedOutput::addColumn(const std::string& val) {
		setColumn(++column_count, val);
    }

//    /**
//     * Adds each of the given values to the next columns.
//     */
//	void SeparatedOutput::addColumn(OutputList ol) {
//		addColumn(ol.toStringList());
//    }

    /**
     * Adds each of the given values to the next columns.
     */
	void SeparatedOutput::addColumn(const std::vector<std::string>& vals) {
		std::vector<std::string>::const_iterator val;
		for (val = vals.begin(); val != vals.end(); ++val) {
			addColumn(*val);
		}
    }

   /** 
    * Sets the column delimiter to a tab
    */
   void SeparatedOutput::setColumnDelimiterTab() {
	   delim = '\t';
   }
   
   /** 
    * Sets the column delimiter to a comma
    */
   void SeparatedOutput::setColumnDelimiterComma() {
	   delim = ',';
   }
   
   /** 
    * Sets the column delimiter to a space.  If a space is used as a 
    * separator then the empty value should be set (see setEmptyValue).
    */
   void SeparatedOutput::setColumnDelimiterSpace() {
	   delim = ' ';
   }
   
   /** 
    * Sets the number of extra spaces after the delimiter
    */
   void SeparatedOutput::setColumnSpace(int num) {
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
   void SeparatedOutput::setEmptyValue(std::string e) {
	   empty = e;
   }
   
   /** 
    * Set the code indicating the start of a comment.
    */
   void SeparatedOutput::setCommentCharacter(const std::string& c) {
	   comment_char = c;
   }
   
   /**
    * Set parameters.  Use all the parameters in the reader.
    */
   void SeparatedOutput::setParameters(const ParameterData& pr) {
	   std::vector<std::string> l = pr.getKeyList();
	   std::vector<std::string>::const_iterator p;
	   for (p = l.begin(); p != l.end(); ++p) {
		   params.push_back(*p+" = "+pr.getString(*p));
	   }
   }

   void SeparatedOutput::setParameter(const std::string& key, const std::string& value) {
	   params.push_back(key+" = "+value);
   }


   void SeparatedOutput::clearParameters() {
	   params.clear();
   }

   /** 
    * Add the following line to the comments.
    */
   void SeparatedOutput::addComment(const std::string& c) {
		std::vector<std::string> tmparray = split(c, "\n");
		std::vector<std::string>::const_iterator tmp;
		for (tmp = tmparray.begin(); tmp != tmparray.end(); ++tmp) {
			comments.push_back(*tmp);
		}
   }
   
	/**
	 * Writes a line of the output.  The first call to readLine will read the column headings, units, etc.
	 */
	void SeparatedOutput::writeLine() {
//		try {
			if ( comments.size() != 0 ) {
				for (std::vector<std::string>::const_iterator line = comments.begin(); line != comments.end(); ++line) {
					fp(writer, comment_char);
					fpln(writer, *line);
					++size_l;
				}
				comments.clear();
			}
			if ( ! header) {
				if (params.size() != 0) {
					for (std::vector<std::string>::const_iterator p = params.begin(); p != params.end(); ++p) {
						fpln(writer,*p);
						++size_l;
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
//		}
//		catch (IOException e) {
//          error.addError("*** An IO exception has occurred: "+e.getMessage());
//		}
	}
    
    void SeparatedOutput::print_line(std::vector<std::string> vals) { // throws IOException {
    	if (vals.size() == 0) {
    		return;
    	}
    	fp(writer, vals[0]);
    	for (int i = 1; i < (int) vals.size(); ++i) {
        	fp(writer, delim+space+vals[i]);
    	}
    	fpln(writer,"");
    	++size_l;
    }

    
    std::string SeparatedOutput::toString() {
    	std::string str = "SeparateOutput: ";
    	str += "\n";
    	str += " header_str:";
		for (std::vector<std::string>::const_iterator line = header_str.begin(); line != header_str.end(); ++line) {
    		str += ", " + *line;
    	}

    	str += "\n";
    	str += " units_str:";
		for (std::vector<std::string>::const_iterator line = units_str.begin(); line != units_str.end(); ++line) {
    		str += ", " + *line;
    	}

    	str += "\n";
    	str += " line_str:";
		for (std::vector<std::string>::const_iterator line = line_str.begin(); line != line_str.end(); ++line) {
    		str += ", " + *line;
    	}

    	return str;
    }

	void SeparatedOutput::flush() {
		writer->flush();
	}


    // ErrorReporter Interface Methods
  
    bool SeparatedOutput::hasError() const {
      return error.hasError();
    }
    bool SeparatedOutput::hasMessage() const {
      return error.hasMessage();
    }
    std::string SeparatedOutput::getMessage() {
      return error.getMessage();
    }
    std::string SeparatedOutput::getMessageNoClear() const {
      return error.getMessageNoClear();
    }
  
   
}
