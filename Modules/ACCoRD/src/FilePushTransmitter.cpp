/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "FilePushTransmitter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <stdio.h>

namespace larcfm {

FilePushTransmitter::FilePushTransmitter() {
	filename = "";
	keyword = "";
	outfile = "";
	lockfile = "";
	running = false;
}
FilePushTransmitter::~FilePushTransmitter() {}

bool FilePushTransmitter::exists(const std::string& name) {
	struct stat statbuf;
	return (stat(name.c_str(), &statbuf) == 0);
}

bool FilePushTransmitter::createNewFile(const std::string& name) {
	std::ofstream os(name.c_str());
	os << " " << std::endl;
	os.close();
	return true;
}

bool FilePushTransmitter::deleteFile(const std::string& name) {
	return (remove(name.c_str()) == 0);
}


void FilePushTransmitter::publish(const std::string& address, const std::string& word) {
	if (!running) {
		running = true;
		filename = address;
		keyword = word;
		bool ok = true;
		// test if we can create and remove the files.
		outfile = filename+"."+keyword;
		lockfile = filename+"."+keyword+".lock";
		if (exists(outfile)) {
			ok &= deleteFile(outfile);
		}
		if (exists(lockfile)) {
			ok &= deleteFile(lockfile);
		}
		ok &= createNewFile(outfile);
		ok &= createNewFile(lockfile);
		if (!ok) {
			std::cout << "FileTransmitter: Could not create file " << address << "." << keyword << std::endl;
		}
		if (exists(lockfile)) deleteFile(lockfile);
	} else {
		std::cout << "FileTransmitter: already running with file " << address << "." << keyword << std::endl;
	}
}

void FilePushTransmitter::update(const std::string& keyword, const std::string& data) {
	if (running) {
		while(exists(lockfile)) {
			// wait
		}
		createNewFile(lockfile);
		if (exists(outfile)) deleteFile(outfile);
		std::ofstream os(outfile.c_str());
		os << data << std::endl;
		os.close();
		if (exists(lockfile)) deleteFile(lockfile);
	}
}

bool FilePushTransmitter::hasSubscribers(const std::string& word) const {
	return running && keyword == word;
}

void FilePushTransmitter::unpublish(const std::string& word) {
	if (running && keyword == word) {
		shutdown();
	}
}

bool FilePushTransmitter::shutdown() {
	running = false;
	if (exists(outfile)) deleteFile(outfile);
	if (exists(lockfile)) deleteFile(lockfile);
	return true;
}
} /* namespace larcfm */
