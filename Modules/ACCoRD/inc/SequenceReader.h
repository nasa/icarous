/* 
 * StateReader
 *
 * Contact: George Hagen
 * 
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef SEQUENCEREADER_H
#define SEQUENCEREADER_H

#include "StateReader.h"
#include "AircraftState.h"
#include "Triple.h"
#include <string>
#include <vector>
#include <set>
#include <map>

namespace larcfm {

/**
 * <p>This reads in and stores a set of aircraft states, possibly over time, (and parameters) from a file
 * The Aircraft states are stored in an ArrayList&lt;AircraftState&gt;.</p>
 * 
 * <p>This can read state/history files as well as sequence files (slight variations on history files).</p>
 *
 * <p>State files consist of comma or space-separated values, with one point per line.
 * Required columns include aircraft name, 3 position columns (either x[nmi]/y[nmi]/z[ft] or latitude[deg]/longitude[deg]/altitude[ft]) and
 * 3 velocity columns (either vx[kn]/vy[kn]/vz[fpm] or track[deg]/gs[kn]/vs[fpm]).</p>
 *
 * <p>An optional column is time [s].  If it is included, a "history" will be build if an aircraft has more than one entry.
 * If it is not included, only the last entry for an aircraft will be stored.</p>
 *
 * <p>It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).</p>
 *
 * <p>If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (&quot;).
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.</p>
 *
 * <p>Any empty line or any line starting with a hash sign (#) is ignored.</p>
 *
 * <p>Files may also include parameter definitions prior to other data.  Parameter definitions are of the form &lt;key&gt; = &lt;value&gt;,
 * one per line, where &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is either a numeral or string.  The &lt;value&gt;
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on either side of the equals sign.
 * Note that it is possible to also update the stored parameter values (or store additional ones) through API calls.</p>
 *
 * <p>Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.</p>
 * 
 * <p>If the optional parameter "filetype" is specified, its value must be "state", "history", or "sequence" for this reader
 * to accept the file without error.</p>
 * 
 * <p>This allows for arbitrary additional user-defined columns. New columns' information may 
 * be accessed by the get getNewColumnValue(), getNewColumnBoolean(), or getNewColumnString() 
 * methods. The 2-parameter versions (index, column) inherited from StateReader will only 
 * return the last active values for a given aircraft (which may be blank).  To retrieve 
 * values at arbitrary times, use the 3-parameter versions (time, name, column). New columns 
 * are assumed unitless unless units are specified.</p>
 *
 */
class SequenceReader : public StateReader {
private:
	typedef std::pair<Position, Velocity> DataPoint;
	typedef std::map<std::string, DataPoint> SequenceEntry;
	typedef std::map<double, SequenceEntry> SequenceEntryMap;

	int windowSize;
	SequenceEntryMap sequenceTable;
	std::vector<std::string> nameIndex;
	std::set<std::string> names;
	
	typedef std::map<Triple<double,std::string,int>,Triple<double,bool,std::string> > allExtraTblType;
	allExtraTblType allExtracolumnValues;



	void loadfile();
	void buildActive(double tm);
	
public:
    /** A new, empty StateReader.  This may be used to store parameters, but nothing else. */
	SequenceReader();
	
	explicit SequenceReader(const std::string& filename);

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	virtual void open(const std::string& filename);

    virtual void open(std::istream* in);


	/** Return the number of sequence entries in the file 
	 * @return size*/
	int sequenceSize() const;
	
	/**
	 * Sets the window size for the active sequence set
	 * @param s new window size
	 */
	void setWindowSize(int s);

	/**
	 * Returns the current window size
	 * @return size
	 */
	int getWindowSize() const;
	
	
	/**
	 * Given a sequence key, set the active set of states.  If no such key exists, the active set is left empty.
	 * @param tm Sequence key (time)
	 */
	void setActive(double tm);
	
	/**
	 * Set the first entry to be the active one.
	 */
	void setFirstActive();

	/**
	 * Set the last entry to be the active one.
	 */
	void setLastActive();

	/** Returns true if an entry exists for the given name and time 
	 * @param name 
	 * @param time 
	 * @return true, if aircraft exists
	 * */
	bool hasEntry(const std::string& name, double time);
	
	/** Returns the Position entry for a given name and time.  If no entry for this name and time, returns a zero position and sets a warning. 
	 * @param name 
	 * @param time 
	 * @return position
	 * */
	Position getSequencePosition(const std::string& name, double time);

	/** Returns the Velocity entry for a given name and time.  If no entry for this name and time, returns a zero velocity and sets a warning. 
	 * @param name 
	 * @param time 
	 * @return velocity
	 * */
	Velocity getSequenceVelocity(const std::string& name, double time);


	/** sets a particular entry without reading in from a file 
	 * @param time 
	 * @param name 
	 * @param p 
	 * @param v */
	void setEntry(double time, const std::string& name, const Position& p, const Velocity& v);

	/** remove any time-point entries for which there is only one aircraft (and so no chance of conflict) */
	void clearSingletons();

	/**
	 * This purges all references of a given set of aircraft from this reader.
	 * This then resets the active time to the last time in the list
	 * @param alist List of aircraft identifiers
	 */
	void removeAircraft(const std::vector<std::string>& alist);

	/**
	 *  Returns a sorted list of all sequence keys
	 */
	std::vector<double> sequenceKeys();

	/** a list of n > 0 sequence keys, stopping at the given time (inclusive) */ 
	std::vector<double> sequenceKeysUpTo(int n, double tm);

	// otherwise the StateReader versions are hidden, despite having distinct signatures.
	using StateReader::hasExtraColumnData;
	using StateReader::getExtraColumnValue;
	using StateReader::getExtraColumnBool;
	using StateReader::getExtraColumnString;

	/**
	 * Return true if the given aircraft has data for the indicated column at the inicated time.
	 * @param time 
	 * @param acName
	 * @param colname
	 * @return true, if extra data is available
	 */
	bool hasExtraColumnData(double time, const std::string& acName, const std::string& colname) const;

	/**
	 * Returns the column value associated with a given aircraft at a given time, interpreted as a double, or NaN if there is no info  
	 * @param time 
	 * @param acName 
	 * @param colname 
	 * @return information
	 */
	double getExtraColumnValue(double time, const std::string& acName, const std::string& colname) const;

	/**
	 * Returns the column value associated with a given aircraft at a given time, interpreted as a boolean, or false if there is no info  
	 * @param time 
	 * @param acName 
	 * @param colname 
	 * @return information
	 */
	bool getExtraColumnBool(double time, const std::string& acName, const std::string& colname) const;

	/**
	 * Returns the column value associated with a given aircraft at a given time, interpreted as a string, or the empty string if there is no info  
	 * @param time 
	 * @param acName 
	 * @param colname 
	 * @return information
	 */
	std::string getExtraColumnString(double time, const std::string& acName, const std::string& colname) const;


//    std::string toString() const;
};

}

#endif
