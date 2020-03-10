/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef FILEPUSHTRANSMITTER_H_
#define FILEPUSHTRANSMITTER_H_

#include "Transmitter.h"
#include <string>

namespace larcfm {

class FilePushTransmitter : public Transmitter {
private:
	std::string filename;
	std::string keyword;
	std::string outfile;
	std::string lockfile;
	bool running;

	static bool exists(const std::string& name);
	static bool createNewFile(const std::string& name);
	static bool deleteFile(const std::string& name);

public:
	FilePushTransmitter();
	virtual ~FilePushTransmitter();

	virtual void publish(const std::string& address, const std::string& keyword);
	virtual void unpublish(const std::string& keyword);
	virtual void update(const std::string& keyword, const std::string& data);
	virtual bool hasSubscribers(const std::string& keyword) const;
	virtual bool shutdown();

};

} /* namespace larcfm */

#endif /* SRC_FILEPUSHTRANSMITTER_H_ */
