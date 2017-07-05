/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include <string>

namespace larcfm {


class Transmitter {

public:

	virtual ~Transmitter() {};

	/**
	 * Publisher announces it has data of type keyword.
	 * @param address string representing the address to be listened to for subscribers (e.g. port)
	 * @param keyword Keywords should be cumulative for all data types presented
	 */
	virtual void publish(const std::string& address, const std::string& keyword) = 0;

	/**
	 * Publisher removes a certain keyword from its publishing list.
	 * @param keyword Keywords to remove
	 */
	virtual void unpublish(const std::string& keyword) = 0;

	/**
	 * Publisher updates data to be transmitted.  Depending on the connection type, this may immediately
	 * push the data to all subscribers or cache it until requested.
	 * @param keyword which keyword this data is associated with
	 * @param data
	 */
	virtual void update(const std::string& keyword, const std::string& data) = 0;

	/**
	 * Returns true if there is at least one active subscriber associated with this keyword
	 * @param keyword
	 * @return
	 */
	virtual bool hasSubscribers(const std::string& keyword) const = 0;

	/**
	 * Attempts to cleanly shuts down this transmitter, releasing any associated resources.  This does not "unpublish" the transmitter.
	 * @return true if shut down successfully.
	 */
	virtual bool shutdown() = 0;

};

}

#endif
