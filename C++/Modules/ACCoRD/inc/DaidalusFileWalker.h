/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * DaidalusFileWalker.h
 *
 */

#ifndef DAIDALUSFILEWALKER_H_
#define DAIDALUSFILEWALKER_H_

#include "SequenceReader.h"
#include "Daidalus.h"

namespace larcfm {

class DaidalusFileWalker {

private:
  SequenceReader sr_;
  std::vector<double> times_;
  int index_;
  void init();

public:
  DaidalusFileWalker(const std::string& filename);
  void resetInputFile(const std::string& filename);
  double firstTime() const;
  double lastTime() const;
  int getIndex() const;
  double getTime() const;
  bool atBeginning() const;
  bool atEnd() const;
  bool goToTime(double t);
  bool goToTimeStep(int i);
  void goToBeginning();
  void goToEnd();
  void goNext();
  void goPrev();
  int indexOfTime(double t) const;
  void readState(Daidalus& daa);

};
}

#endif /* DAIDALUSFILEWALKER_H_ */
