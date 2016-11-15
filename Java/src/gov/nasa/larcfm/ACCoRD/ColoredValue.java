/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.f;

import java.util.List;

public class ColoredValue {
  public double val;
  public BandsRegion color;

  public ColoredValue(double v, BandsRegion c) {
    val = v;
    color = c;
  }

  public String toString() {
    String s = "("+f.Fm2(val)+", "+color+")";
    return s;
  }

  public static void insert(List<ColoredValue> l, Interval ii, 
      BandsRegion lb_color, BandsRegion ub_color) {
    int i;
    for (i=0; i < l.size() && Util.almost_less(l.get(i).val,ii.low); ++i);
    if (i == l.size()) {
      l.add(new ColoredValue(ii.low,lb_color));
    } else if (!Util.almost_equals(l.get(i).val,ii.low)) {
      l.add(i,new ColoredValue(ii.low,lb_color));
    }
    for (;i < l.size() && Util.almost_less(l.get(i).val,ii.up); ++i);
    if (i == l.size()) {
      l.add(new ColoredValue(ii.up,ub_color));
    } else if (Util.almost_equals(l.get(i).val,ii.up)) {
      l.get(i).color = ub_color;
    } else {
      l.add(i,new ColoredValue(ii.up,ub_color));
    }
  }

  public static void toBands(List<BandsRange> ranges, List<ColoredValue> l) { 
    for (int i = 1; i < l.size(); ++i) {
      ranges.add(new BandsRange(new Interval(l.get(i-1).val,l.get(i).val),l.get(i).color));
    }
  }

}
