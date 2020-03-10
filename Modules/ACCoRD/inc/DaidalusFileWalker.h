/*
 * Copyright (c) 2015-2019 United States Government as represented by
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
#include "ParameterData.h"
#include "Daidalus.h"
#include <vector>
#include <string>

namespace larcfm {

class DaidalusFileWalker : public ErrorReporter {
  private:
  SequenceReader sr_;
  ParameterData p_;
  std::vector<double> times_;
  int index_;

  public:
  DaidalusFileWalker(const std::string& filename);

  void resetInputFile(const std::string& filename);

  private:
  void init();

  static ParameterData extraColumnsToParameters(const SequenceReader& sr, double time, const std::string& ac_name);

  public:
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

  static void readExtraColumns(Daidalus& daa, const SequenceReader& sr, int ac_idx);

  void readState(Daidalus& daa);
  bool hasError() const;

  bool hasMessage() const;

  std::string getMessage();

  std::string getMessageNoClear() const;

};

}

#endif /* DAIDALUSFILEWALKER_H_ */
