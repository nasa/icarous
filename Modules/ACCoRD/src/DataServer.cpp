/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DataServer.h"
#include "GeneralStateWriter.h"
#include "GeneralPlanWriter.h"
#include "GeneralPlan.h"
#include "GeneralState.h"
#include "ParameterData.h"
#include "Transmitter.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>


namespace larcfm {

DataServer::DataServer() {
	word = "";
}

DataServer::~DataServer() {
	std::map<std::string,Transmitter*>::iterator pos;
	for (pos = transmitters.begin(); pos != transmitters.end(); ++pos) {
		delete pos->second;
	}
}

void DataServer::addTransmitter(const std::string& address, Transmitter* t) {
	removeTransmitter(address);
	transmitters[address] = t;
	if (word.length() > 0) {
		getTransmitter(address)->publish(address,word);
	}
}

Transmitter* DataServer::getTransmitter(const std::string& address) const {
	if (transmitters.find(address) != transmitters.end()) {
		return transmitters.find(address)->second;
	}
	return NULL;
}

void DataServer::removeTransmitter(const std::string& address) {
	if (transmitters.find(address) != transmitters.end()) {
		transmitters[address]->shutdown();
		delete transmitters[address];
		transmitters.erase(address);
	}
}

void DataServer::advertiseAll(const std::string& keyword) {
	word = keyword;
	std::map<std::string,Transmitter*>::iterator pos;
	for (pos = transmitters.begin(); pos != transmitters.end(); ++pos) {
		pos->second->publish(pos->first, keyword);
	}
}

void DataServer::advertiseOne(const std::string& address, const std::string& keyword) {
	if (transmitters.find(address) != transmitters.end()) {
		transmitters[address]->publish(address, keyword);
	}
}

void DataServer::update(const std::string& keyword, const std::string& data) {
	std::map<std::string,Transmitter*>::iterator pos;
	for (pos = transmitters.begin(); pos != transmitters.end(); ++pos) {
		pos->second->update(keyword, data);
	}
}

bool DataServer::hasClients(const std::string& keyword) const {
	std::map<std::string,Transmitter*>::const_iterator pos;
	for (pos = transmitters.begin(); pos != transmitters.end(); ++pos) {
		if (pos->second->hasSubscribers(keyword)) return true;
	}
	return false;

}

void DataServer::updateGeneralStates(const std::vector<GeneralState>& p) {
	ParameterData pd;
	updateGeneralStates(p,pd);
}

void DataServer::updateGeneralStates(const std::vector<GeneralState>& p, const ParameterData& pd) {
	GeneralStateWriter gsw = GeneralStateWriter();
	std::ostringstream oss;
	gsw.open(&oss);
	if (pd.size() > 0) gsw.setParameters(pd);
	for (int i = 0; i < (int) p.size(); i++) {
		gsw.writeState(p[i]);
	}
	update("GeneralState", oss.str());
	gsw.close();
}

void DataServer::updateGeneralPlans(const std::vector<GeneralPlan>& p) {
	ParameterData pd;
	updateGeneralPlans(p,pd);
}

void DataServer::updateGeneralPlans(const std::vector<GeneralPlan>& p, const ParameterData& pd) {
	GeneralPlanWriter gpw = GeneralPlanWriter();
	std::ostringstream oss;
	gpw.open(&oss);
	if (pd.size() > 0) gpw.setParameters(pd);
	for (int i = 0; i < (int) p.size(); i++) {
		gpw.writePlan(p[i]);
	}
	update("GeneralPlan", oss.str());
	gpw.close();

}

void DataServer::updateParameters(const ParameterData& p) {
	update("ParameterData", p.toString());
}

void DataServer::updateString(const std::string& s) {
	update("String", s);
}

void DataServer::updateString(const std::string& keyword, const std::string& s) {
	update(keyword, s);
}

} /* namespace larcfm */
