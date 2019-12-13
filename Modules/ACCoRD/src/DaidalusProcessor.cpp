#include "DaidalusProcessor.h"
#include "Daidalus.h"
#include "DaidalusFileWalker.h"
#include "Position.h"
#include "SequenceReader.h"
#include "Velocity.h"
#include "Util.h"
#include "string_util.h"

using namespace larcfm;

DaidalusProcessor::DaidalusProcessor() {
	from_ = -1;
	to_ = -1;
	relative_ = 0;
	options_ = "";
	ownship_ = "";
}

DaidalusProcessor::DaidalusProcessor(const std::string& own) {
	from_ = -1;
	to_ = -1;
	relative_ = 0;
	options_ = "";
	ownship_ = own;
}

double DaidalusProcessor::getFrom() const {
	return from_;
}

double DaidalusProcessor::getTo() const {
	return to_;
}

void DaidalusProcessor::setOwnship(const std::string& own) {
	ownship_ = own;
}

std::string DaidalusProcessor::getOwnship() const {
	return ownship_;
}

std::string DaidalusProcessor::getHelpString() {
	std::string s = "";
	s += "  --ownship <id>\n\tSet ownship to aircraft with identifier <id>\n";
	s += "  --from\n\tCheck from time t\n";
	s += "  --to t\n\tCheck up to time t\n";
	s += "  --at [t | t+k | t-k]\n\tCheck times t, [t,t+k], or [t-k,t]. ";
	s += "First time is denoted by +0. Last time is denoted by -0\n";
	return s;
}

bool DaidalusProcessor::processOptions(const char* args[], int i) {
	std::string argi = args[i];
	if (startsWith(argi,"--own") || startsWith(argi,"-own")) {
		++i;
		argi = args[i];
		ownship_ = argi;
		options_ += argi+" ";
	} else if (argi == "--from" || argi == "-from") {
		++i;
		argi = args[i];
		from_ = Util::parse_double(argi);
		options_ += argi+" ";
	} else if (argi == "--to" || argi == "-to") {
		++i;
		argi = args[i];
		to_ = Util::parse_double(argi);
		options_ += argi+" ";
	} else if (argi == "--at" || argi == "-at") {
		++i;
		argi = args[i];
		options_ += argi+" ";
		int k = argi.find("+");
		if (k == 0) {
			relative_ = Util::parse_double(argi)+0.001;
		} else if (k > 0) {
			from_ = Util::parse_double(argi.substr(0,k));
			relative_ = Util::parse_double(argi.substr(k));
		} else {
			k = argi.find("-");
			if (k == 0) {
				relative_ = Util::parse_double(argi)-0.001;
			} else if (k > 0) {
				to_ = Util::parse_double(argi.substr(0,k));
				relative_ = Util::parse_double(argi.substr(k));
			} else {
				k = argi.find("*");
				if (k > 0) {
					from_ = Util::parse_double(argi.substr(0,k));
					relative_ = Util::parse_double(argi.substr(k+1));
					from_ -= relative_;
					relative_ *= 2;
				} else {
					from_ = Util::parse_double(argi);
					to_ = from_;
				}
			}
		}
	} else {
		return false;
	}
	return true;
}

std::string DaidalusProcessor::getOptionsString() {
	return options_;
}

void DaidalusProcessor::processFile(const std::string& filename, Daidalus &daa) {
	DaidalusFileWalker dw = DaidalusFileWalker(filename);
	double from = from_;
	double to = to_;
	if (from < 0) {
		from = dw.firstTime();
	}
	if (to < 0) {
		to = dw.lastTime();
	}
	if (relative_ > 0) {
		to = from + relative_;
	}
	if (relative_ < 0) {
		from = to + relative_;
	}
	if (dw.goToTime(from) && from <= to) {
		while (!dw.atEnd() && dw.getTime() <= to) {
			double t = dw.getTime();
			dw.readState(daa);
			if (ownship_ != "") {
				daa.resetOwnship(ownship_);
				if (daa.hasError()) {
					std::cerr << "** Warning: State for ownship aircraft ("<< ownship_ <<
							") not found at time. Skipping time " << t << " [s]" << std::endl;
					continue;
				}
			}
			processTime(daa,filename);
		}
	}
}




