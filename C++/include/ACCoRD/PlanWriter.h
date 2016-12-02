/* 
 * StateWriter
 *
 * Contact: Jeff Maddalon
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef PLANWRITER_H
#define PLANWRITER_H

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedOutput.h"
#include "ParameterData.h"
#include "Plan.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>

namespace larcfm {

class PlanWriter : public ErrorReporter {
private:
	ErrorLog error;
	SeparatedOutput output;
	bool latlon;
	bool first_line;
	bool display_units;
	bool tcpColumns;
	int precision;
	int lines;
	std::string fname;
	//std::ofstream fw;

public:
    /** A new PlanWriter. */
	PlanWriter();
    /** A new StateReader based on the given file. */
	void open(const std::string& filename);

	void open(std::ostream* writer);

    void close();
	
	int getPrecision() const;

	void setPrecision(int precision);


	/** 
	 * Sets the column delimiter to a tab.  This method can only be used before the first "writeState" method.
	 */
	void setColumnDelimiterTab();

	/** 
	 * Sets the column delimiter to a comma.  This method can only be used before the first "writeState" method.
	 */
	void setColumnDelimiterComma();

	/** 
	 * Sets the column delimiter to a space.  This method can only be used before the first "writeState" method.
	 */
	void setColumnDelimiterSpace();

	/** 
	 * Adds a comment line to the file.
	 */
	void addComment(const std::string& comment);

	/** 
	 * Set parameters.  Use all the parameters in the reader.
	 */
	void setParameters(const ParameterData& pr);
	
	void clearParameters();

	void writePlan(const Plan& p, bool tcpColumnsLocal);

	
	/** Return the number of states added to the file */
	int size() const;

    bool isLatLon() const;
    
	
	// ErrorReporter Interface Methods

	bool hasError() const;
	bool hasMessage() const;
	std::string getMessage();
	std::string getMessageNoClear() const;

};
}

#endif //PLANWRITER_H
