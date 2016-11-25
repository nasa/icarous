/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_VERTICAL_H_
#define WCV_VERTICAL_H_

namespace larcfm {
class WCV_Vertical {


public:
  double time_in;
  double time_out;


  WCV_Vertical();
  static bool vertical_WCV(double ZTHR, double TCOA, double sz, double vz);
  void vertical_WCV_interval(double ZTHR, double TCOA, double B, double T, double sz, double vz);

};
}
#endif
