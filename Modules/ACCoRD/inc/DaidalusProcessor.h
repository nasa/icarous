/*
 * DaidalusProcessor.h
 *
 *  Created on: Aug 14, 2015
 *      Author: cmunoz
 */

#ifndef DAIDALUSPROCESSOR_H_
#define DAIDALUSPROCESSOR_H_

#include "Daidalus.h"
#include <vector>

class DaidalusProcessor {
private:
	double from_;
	double to_;
	double relative_;
	std::string options_;
	std::string ownship_;

public:
	DaidalusProcessor(const std::string& own);
	DaidalusProcessor();
	void setOwnship(const std::string& own);
	double getFrom() const;
	double getTo() const;
	std::string getOwnship() const;
	virtual ~DaidalusProcessor() { }
	static void getFileNames(std::vector<std::string>& txtFiles, const std::vector<std::string>& names, const std::string& ext, int i);
	static std::string getHelpString();
	bool processOptions(const char* args[], int i);
	std::string getOptionsString();
	void processFile(const std::string& filename, larcfm::Daidalus& daa);
	virtual void processTime(larcfm::Daidalus& daa, const std::string& filename) = 0;
};

#endif /* DAIDALUSPROCESSOR_H_ */
