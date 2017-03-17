/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.ACCoRD.Daidalus;
import gov.nasa.larcfm.IO.SequenceReader;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;

import java.util.List;


public class DaidalusFileWalker {

  private SequenceReader sr_;
  private List<Double> times_;
  private int index_;

  public DaidalusFileWalker(String filename) {
    sr_ = new SequenceReader(filename);
    init();
  }

  public void resetInputFile(String filename) {
    sr_ = new SequenceReader(filename);
    init();
  }

  private void init() {
    sr_.setWindowSize(1);
    index_ = 0;
    times_ = sr_.sequenceKeys();
    if (times_.size() > 0) 
      sr_.setActive(times_.get(0));
  }

  public double firstTime() {
    if (!times_.isEmpty()) {
      return times_.get(0);
    } 
    return Double.POSITIVE_INFINITY;
  }

  public double lastTime() {
    if (!times_.isEmpty()) {
      return times_.get(times_.size()-1);
    }
    return Double.NEGATIVE_INFINITY;
  }

  public int getIndex() {
    return index_;
  }

  public double getTime() {
    if (0 <= index_ && index_ < times_.size()) {
      return times_.get(index_);
    } else {
      return Double.NaN;
    }
  }

  public boolean atBeginning() {
    return index_ == 0;
  }

  public boolean atEnd() {
    return index_ == times_.size();
  }

  public boolean goToTime(double t) {
    return goToTimeStep(indexOfTime(t));
  }

  public boolean goToTimeStep(int i) {
    if (0 <= i && i < times_.size()) {
      index_ = i;
      sr_.setActive(times_.get(index_));
      return true;
    }
    return false;
  }

  public void goToBeginning() {
    goToTimeStep(0);
  }

  public void goToEnd() {
    goToTimeStep(times_.size());
  }

  public void goNext() {
    boolean ok = goToTimeStep(index_+1);
    if (!ok) {
      index_ = times_.size();
    }
  }

  public void goPrev() {
    if (!atBeginning()) {
      goToTimeStep(index_-1);
    }
  }

  public int indexOfTime(double t) {
    int i = -1;
    if (t >= firstTime() && t <= lastTime()) {
      i = 0;
      for (; i < times_.size()-1; ++i) {
        if (t >= times_.get(i) && t < times_.get(i+1)) {
          break;
        }
      }
    }
    return i;
  }

  public void readState(Daidalus daa) {
    daa.reset();
    for (int ac = 0; ac < sr_.size();++ac) {
      String ida = sr_.getName(ac);
      Position sa = sr_.getPosition(ac);
      Velocity va = sr_. getVelocity(ac);
      if (ac==0) {
        daa.setOwnshipState(ida,sa,va,getTime());
      } else {
        daa.addTrafficState(ida,sa,va);
      }
    }
    goNext();
  }

}
