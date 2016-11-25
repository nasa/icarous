/* 
 * StateReader
 *
 * Contact: George Hagen
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef SEQUENCEREADER_H
#define SEQUENCEREADER_H

#include "StateReader.h"
#include "AircraftState.h"
#include <string>
#include <vector>
#include <set>
#include <map>

namespace larcfm {

/**
 * This reads in and stores a set of aircraft states, possibly over time, (and parameters) from a file
 * The Aircraft states are stored in an ArrayList&lt;AircraftState&gt;.<p>
 * 
 * This can read state/history files as well as sequence files (slight variations on history files).<p>
 *
 * State files consist of comma or space-separated values, with one point per line.
 * Required columns include aircraft name, 3 position columns (either x[nmi]/y[nmi]/z[ft] or latitude[deg]/longitude[deg]/altitude[ft]) and
 * 3 velocity columns (either vx[kn]/vy[kn]/vz[fpm] or track[deg]/gs[kn]/vs[fpm]).<p>
 *
 * An optional column is time [s].  If it is included, a "history" will be build if an aircraft has more than one entry.
 * If it is not included, only the last entry for an aircraft will be stored.<p>
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).<p>
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (&quot;).
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.<p>
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.<p>
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form &lt;key&gt; = &lt;value&gt;,
 * one per line, where &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is either a numeral or string.  The &lt;value&gt;
 * may include a unit, such as <tt>dist = 50 [m]</tt>.  Note that parameters require a space on either side of the equals sign.
 * Note that it is possible to also update the stored parameter values (or store additional ones) through API calls.<p>
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.<p>
 * 
 * If the optional parameter <tt>filetype</tt> is specified, its value must be <tt>state</tt>, <tt>history</tt>, or <tt>sequence</tt> for this reader
 * to accept the file without error.<p>
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
	
	void loadfile();
	void buildActive(double tm);
	
public:
    /** A new, empty StateReader.  This may be used to store parameters, but nothing else. */
	SequenceReader();
	
	SequenceReader(const std::string& filename);

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	void readFile(const std::string& filename);

	/** Return the number of sequence entries in the file */
	int sequenceSize() const;
	
	/**
	 * Sets the window size for the active sequence set
	 * @param s > 0
	 */
	void setWindowSize(int s);

	/**
	 * Returns the current window size
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

	/** Returns true if an entry exists for the given name and time */
	bool hasEntry(const std::string& name, double time);
	
	/** Returns the Position entry for a given name and time.  If no entry for this name and time, returns a zero position and sets a warning. */
	Position getSequencePosition(const std::string& name, double time);

	/** Returns the Velocity entry for a given name and time.  If no entry for this name and time, returns a zero velocity and sets a warning. */
	Velocity getSequenceVelocity(const std::string& name, double time);


	/** sets a particular entry without reading in from a file */
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

//    std::string toString() const;
};

}

#endif
