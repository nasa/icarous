/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DATASERVER_H_
#define DATASERVER_H_

#include "Transmitter.h"
#include "GeneralState.h"
#include "GeneralPlan.h"
#include "ParameterData.h"
#include <string>
#include <vector>
#include <map>

namespace larcfm {

/**
 * This class represents a wrapper for a collection of Transmitter object that represents a server's behavior.
 * The server takes updates to plan, state, and/or parameter information and parcels it out to its various Transmitter object.
 * Each Transmitter is intended to handle a certain type of request (e.g. for plan information that is updated every second 
 * and sent over a socket) and handles distributing the information to its clients.
 * The particulars of the transmission of data are handled by the provided Transmitters.
 * 
 * Known keywords:
 * GeneralState, GeneralPlan, ParameterData, String
 */
class DataServer {
private:
	std::map<std::string,Transmitter*> transmitters;
	std::string word;
public:

	DataServer();

	virtual ~DataServer();

	/**
	 * Add a transmitter at a given address that will listen for clients
	 * @param address This will replace any existing transmitter at this address.
	 * @param t Pointer to transmitter to be added.  The destructor of this class will destroy all Transmitters included in it.
	 */
	void addTransmitter(const std::string& address, Transmitter* t);

	/**
	 * Return the transmitter associated with a given address.  This will return null if no transmitter is associated with the given address.
	 * Note: The destructor of this class will destroy all Transmitters included in it.
	 * @param address
	 * @return Pointer to Transmitter, or NULL, if none exists.
	 */
	Transmitter* getTransmitter(const std::string& address) const;

	/**
	 * Delete the transmitter associated with an address, if there is one.  This will destroy the Transmitter object.
	 * @param address
	 */
	void removeTransmitter(const std::string& address);

	/**
	 * Tell any clients that connect and ask what sort of data you provide.
	 * This propagates the same keyword to all current and future transmitters
	 * @param keyword
	 */
	void advertiseAll(const std::string& keyword);

	/**
	 * Associate a keyword with one particular Transmitter.
	 * This does not affect other Transmitters.
	 * @param address
	 * @param keyword
	 */
	void advertiseOne(const std::string& address, const std::string& keyword);

	/**
	 * Transmit String with user-specified keyword
	 * @param keyword
	 * @param data
	 */
	void update(const std::string& keyword, const std::string& data);

	/**
	 * Return true if this server has any clients looking for the given keyword
	 * @param keyword
	 * @return
	 */
	bool hasClients(const std::string& keyword) const;

	//*******************************


	/**
	 * Transmit a set of states
	 */
	void updateGeneralStates(const std::vector<GeneralState>& p);

	void updateGeneralStates(const std::vector<GeneralState>& p, const ParameterData& pd);

	/**
	 * Transmit a set of plans
	 */
	void updateGeneralPlans(const std::vector<GeneralPlan>& p);

	void updateGeneralPlans(const std::vector<GeneralPlan>& p, const ParameterData& pd);

	/**
	 * Transmit a ParameterData object
	 */
	void updateParameters(const ParameterData& p);

	/**
	 * Transmit a string
	 */
	void updateString(const std::string& s);

	/**
	 * Transmit a string with some associated keyword
	 * @param keyword
	 * @param s
	 */
	void updateString(const std::string& keyword, const std::string& s);

};

} /* namespace larcfm */

#endif /* DATASERVER_H_ */
