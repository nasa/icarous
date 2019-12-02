/* 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ColoredValue.h"
#include "DaidalusParameters.h"
#include "format.h"

namespace larcfm {


ColoredValue::ColoredValue(BandsRegion::Region l, double v, BandsRegion::Region r) {
    val = v;
    color_left = l;
    color_right = r;
}

std::string ColoredValue::toString() const {
    std::string s = "<"+BandsRegion::to_string(color_left)+", "+FmPrecision(val)+", "+BandsRegion::to_string(color_right)+">";
    return s;
}

/*
 * Initialize a list of colored values, with min < max values, and interior color.
 * Initial list is has two colored values: (unknown,min,int_color) and
 * (int_color,max,unknown). Assumes l is empty.
 */
void ColoredValue::init(std::vector<ColoredValue>& l, double min, double max, BandsRegion::Region int_color) {
    init(l,min,max,min,max,int_color);
}

/*
 * Initialize a list of colored values, with min <= min_val < max_val <= max values, and interior color.
 * Initial list is has two colored values: (unknown,min,int_color) and
 * (int_color,max,unknown). Assumes l is empty.
 */
void ColoredValue::init(std::vector<ColoredValue>& l, double min, double max, double min_val, double max_val, BandsRegion::Region int_color) {
    l.clear();
    if (Util::almost_less(min,min_val,DaidalusParameters::ALMOST_)) {
        l.push_back(ColoredValue(BandsRegion::UNKNOWN,min,BandsRegion::UNKNOWN));
    }
    l.push_back(ColoredValue(BandsRegion::UNKNOWN,min_val,int_color));
    l.push_back(ColoredValue(int_color,max_val,BandsRegion::UNKNOWN));
    if (Util::almost_less(max_val,max,DaidalusParameters::ALMOST_)) {
        l.push_back(ColoredValue(BandsRegion::UNKNOWN,max,BandsRegion::UNKNOWN));
    }
}

/*
 * Initialize a list of colored values, with min != max, and 0 <= min < max <= mod values,
 * and interior color. Assumes l is empty.
 * If min = max: Initilis list has two colored values (unknown,0,int_color) and  (int_color,max,unknown)
 * If min < max: Initial list has four colored values: (unknown,0,unknown),
 * (unknown,min,int_color), (int_color,max,unknown), and (unknown,mod,unknown).
 * If min > max: Initial list is has four colored values: (unknown,0,int_color),
 * (int_color,max,unknown), (unknown,min,int_color), and (int_color,mod,unknown).
 */
void ColoredValue::init(std::vector<ColoredValue>& l, double min, double max, double mod, BandsRegion::Region int_color) {
    l.clear();
    if (Util::almost_equals(min,max,DaidalusParameters::ALMOST_)) {
        l.push_back( ColoredValue(BandsRegion::UNKNOWN,0,int_color));
        l.push_back( ColoredValue(int_color,mod,BandsRegion::UNKNOWN));
    } else if (min < max) {
        if (!Util::almost_equals(0,min,DaidalusParameters::ALMOST_)) {
            l.push_back( ColoredValue(BandsRegion::UNKNOWN,0,BandsRegion::UNKNOWN));
        }
        l.push_back( ColoredValue(BandsRegion::UNKNOWN,min,int_color));
        l.push_back( ColoredValue(int_color,max,BandsRegion::UNKNOWN));
        if (!Util::almost_equals(max,mod,DaidalusParameters::ALMOST_)) {
            l.push_back( ColoredValue(BandsRegion::UNKNOWN,mod,BandsRegion::UNKNOWN));
        }
    } else {
        l.push_back( ColoredValue(BandsRegion::UNKNOWN,0,int_color));
        l.push_back( ColoredValue(int_color,max,BandsRegion::UNKNOWN));
        l.push_back( ColoredValue(BandsRegion::UNKNOWN,min,int_color));
        l.push_back( ColoredValue(int_color,mod,BandsRegion::UNKNOWN));
    }
}

/*
 * List l has been initialized and it's not empty. The bound l(0).val <= lb < ub <= l(n-1), where
 * n is the length of l. This function inserts (lb,ub) with the interior color int_color.
 */
void ColoredValue::insert(std::vector<ColoredValue>& l, double lb, double ub, BandsRegion::Region int_color) {
    if (l.empty() || Util::almost_geq(lb,ub,DaidalusParameters::ALMOST_) ||
            Util::almost_leq(ub,l.at(0).val,DaidalusParameters::ALMOST_) ||
            Util::almost_geq(lb,l.at(l.size()-1).val,DaidalusParameters::ALMOST_)) {
        return;
    }
    lb = Util::max(lb,l.at(0).val);
    ub = Util::min(ub,l.at(l.size()-1).val);
    int pivotl,pivotr;
    // Find a place to insert the lower bound of the interval
    pivotl = 0;
    pivotr = l.size()-1;
    while (pivotl+1 < pivotr) {
        int mid = (pivotl+pivotr)/2;
        if (Util::almost_less(lb,l.at(mid).val,DaidalusParameters::ALMOST_)) {
            pivotr = mid;
        } else if (Util::almost_greater(lb,l.at(mid).val,DaidalusParameters::ALMOST_)) {
            pivotl = mid;
        } else {
            pivotl = mid;
            pivotr = mid+1;
        }
    }
    int i = pivotl;
    // Insert lower bound as the colored value (color,lb,int_color)
    if (i < (int) l.size()-1 && !Util::almost_equals(l.at(i).val,lb,DaidalusParameters::ALMOST_) &&
            l.at(i).color_right != BandsRegion::UNKNOWN &&
            BandsRegion::orderOfConflictRegion(l.at(i).color_right) < BandsRegion::orderOfConflictRegion(int_color)) {
        // Insert the colored value (ext_color,ii.low,color) to the right of the i-th point
        BandsRegion::Region ext_color = l.at(i).color_right;
        l.insert(l.begin()+i+1,ColoredValue(ext_color,lb,ext_color));
        // The right color of the lb is set to ext_color to avoid breaking the color invariant.
        // This color will be repainted in the next loop.
        ++i;
    }
    // Find a place j where to insert the upper bound of the interval
    // Everything from the right of i to the left of j that can be overridden
    // by ext_color is re-painted
    for (; i < (int) l.size()-1 && Util::almost_leq(l.at(i+1).val,ub,DaidalusParameters::ALMOST_); ++i) {
        if (l.at(i).color_right != BandsRegion::UNKNOWN &&
            BandsRegion::orderOfConflictRegion(l.at(i).color_right) < BandsRegion::orderOfConflictRegion(int_color)) {
            l[i].color_right = int_color;
            l[i+1].color_left = int_color;
        }
    }
    // Insert upper bound as the colored value (int_color,ub,color)
    if (i < (int) l.size()-1 && !Util::almost_equals(l.at(i).val, ub, DaidalusParameters::ALMOST_) &&
            l.at(i).color_right != BandsRegion::UNKNOWN &&
            BandsRegion::orderOfConflictRegion(l.at(i).color_right) < BandsRegion::orderOfConflictRegion(int_color)) {
        // Insert the colored value (color,ii.up,ext_color) to the right of the i-th point
      BandsRegion::Region ext_color = l.at(i).color_right;
        l[i].color_right = int_color;
        l.insert(l.begin()+i+1,ColoredValue(int_color,ub,ext_color));
        ++i;
    }
}

/*
 * Transforms a list of colored values into a list of BandRanges. Assume ranges is empty.
 * This function avoids adding colored points where the left and right colors are the same.
 */
void ColoredValue::fromColoredValuestoBandsRanges(std::vector<BandsRange>& ranges, const std::vector<ColoredValue>& l) {
    ranges.clear();
    int next=0;
    for (int i = 1; i < (int) l.size(); ++i) {
        if (l.at(i).color_left != l.at(i).color_right ||
                i == (int) l.size()-1) {
            ranges.push_back(BandsRange(Interval(l.at(next).val,l.at(i).val), l.at(i).color_left));
            next = i;
        }
    }
}

/*
 *  Insert a "none set" of intervals into consistent list of colored values. A "none set" is a
 *  sorted list of intervals, where values OUTSIDE the intervals have a given background color.
 *  The color inside the intervals is NONE, which means "transparent".
 *  The DAIDALUS core banding algorithm computes a none set for each bands region.
 */
void ColoredValue::insertNoneSetToColoredValues(std::vector<ColoredValue>& l, IntervalSet none_set, BandsRegion::Region bg_color) {
    double min = l.at(0).val;
    double max = l.at(l.size()-1).val;
    for (int i=0; i < none_set.size();i++) {
        insert(l, min, none_set.getInterval(i).low, bg_color);
        min = none_set.getInterval(i).up;
    }
    insert(l,min,max,bg_color);
}

}
