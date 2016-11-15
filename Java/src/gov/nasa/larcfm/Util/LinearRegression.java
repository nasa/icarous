/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.List;

public class LinearRegression {
 
  /**
   *  return pair (m,b) where y = mx+b
   */
  public static Pair<Double,Double> calculate(List<Double> x, List<Double> y) {
	if (x.size() != y.size()) System.out.println("!!!Error!!!");;
    int n = x.size();
    double CC = (n*sumProdBoth(x,x) - sumArray(x)*sumArray(x));
    double m = (n*sumProdBoth(x,y)-sumArray(x)*sumArray(y))/CC;
    double b = (sumProdBoth(x,x)*sumArray(y) - sumProdBoth(x,y)*sumArray(x))/CC;
    return new Pair<Double,Double>(m,b);
  }

  private static double sumArray (List<Double> u) {
    double sum = 0;
    for (int j = 0; j < u.size(); j++)
      sum += u.get(j);
    return sum;
  }

  private static double sumProdBoth (List<Double> v, List<Double> w) {
    double sum = 0;
    for (int j = 0; j < v.size(); j++)
      sum += v.get(j)*w.get(j);
    return sum;
  }

  
}
