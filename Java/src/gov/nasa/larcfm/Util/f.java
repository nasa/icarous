/* 
 * String Formatting functions
 *
 * Contact: Rick Butler (r.w.butler@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 */

package gov.nasa.larcfm.Util;

import java.io.File;
import java.io.FilenameFilter;
import java.text.DecimalFormat;
import java.util.Collection;
import java.util.Hashtable;
import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Arrays;
import java.util.Map;

/** Various printout operations, used in testing and debugging. */   
final public class f {


	/** Given a list of names that may include files or directories,
	 * return a list of files that contains (1) all of the files in
	 * the original list and (2) all files ending with ".txt" in
	 * directories from the original list. */
	public static List<String> getFileNames(String[] names) {
		ArrayList<String> txtFiles = new ArrayList<String>();
		for (int i=0; i < names.length; i++) {
			File file = new File(names[i]);
			if (file.canRead()) {
				if (file.isDirectory()) {
					File[] fs=file.listFiles(new FilenameFilter() {
						public boolean accept(File f, String name) {
							return name.endsWith(".txt");
							//return true;	
						}                       
					}); 
					for (File txtfile:fs) {
						txtFiles.add(txtfile.getPath());
					}
				} else {
					txtFiles.add(file.getPath());
				}
			}
		}
		return txtFiles;
	}

	private static StringBuffer sb;
	private static final String nl = System.getProperty("line.separator");

	/** 
	 * Redirect "f.pln" and "f.p" methods to the given StringBuffer.  To restore 
	 * these methods to use standard output, call this method with a null;
	 * @param buffer the output sent by f.pln and f.p calls
	 */
	public static void redirect_print(StringBuffer buffer) {
		sb = buffer;
	}

	/** Send string to the console with a "carriage return." Also flushes both stdout and stderr */
	public static void pln(String string) {
		if (sb == null) {
			System.err.flush();
			System.out.println(string);
			//System.out.flush();
		} else {
			sb.append(string);
			sb.append(nl);
		}
	}

	/** Send string to the error console with a "carriage return." Also flushes both stdout and stderr */
	public static void dln(String string) {
		System.err.println("$$ERROR$$ "+string);
		System.out.flush();
	}

	/** Send a "carriage return" to the console. Also flushes both stdout and stderr */
	public static void pln() {
		pln("");
	}

	/** send a string to the console without a "carriage return" */
	public static void p(String string) {
		if (sb == null) {
			System.out.print(string);
		} else {
			sb.append(string);
		}
	}

	//  /** Format a position vector */
	//  public static String sStr(Vect2 s, boolean ll) {
	//    if (ll)
	//      return "("+Units.str("deg",s.x)+", "+Units.str("deg",s.y)+")";
	//    else
	//      return "("+Units.str("nmi",s.x)+" ,"+Units.str("nmi",s.y)+")";
	//  }
	//
	//  /* Format a position vector */
	//  public static String sStr(Vect3 s, boolean ll) {
	//    if (ll)
	//      return "("+Units.str("deg",s.x)+", "+Units.str("deg",s.y)+", "+Units.str("ft",s.z)+")";
	//    else
	//      return "("+Units.str("nmi",s.x)+" ,"+Units.str("nmi",s.y)+" ,"+Units.str("ft",s.z)+")";
	//  }
	//
	/** Format a position vector as a Euclidean position */
	public static String sStr(Vect2 s) {
		return "(" + Units.str("nmi", s.x) + " ," + Units.str("nmi", s.y) + ")";
	}

	/** Format a position vector as a Euclidean position */
	public static String sStr(Vect3 s) {
		return "(" + Units.str("nmi", s.x) + " ," + Units.str("nmi", s.y) + " ," 	+ Units.str("ft", s.z) + ")";
	}

	//  /** Format a position vector as a Euclidean position */
	//  public static String sStr16(Vect3 s) {
	//    return "(" + Units.str("nmi",s.x,16) + " ," + Units.str("nmi",s.y,16) + " ," 	+ Units.str("ft",s.z,16) + ")";
	//  }

	/** Format a position vector as a Euclidean position */
	public static String sStr8(Vect3 s) {
		return "(" + Units.str("nmi",s.x,8) + " ," + Units.str("nmi",s.y,8) + " ," 	+ Units.str("ft",s.z,8) + ")";
	}

	/** Format a position vector as a Euclidean position */
	public static String sStrNP(Vect3 s) {
		return Fm4(Units.to("NM", s.x)) + " " + Fm4(Units.to("NM", s.y)) + " " 	+ Fm4(Units.to("ft", s.z));
	}

	/** Format a position vector as a Euclidean position */
	public static String sStr8NP(Vect3 s) {
		return Fm8(Units.to("NM", s.x)) + " " + Fm8(Units.to("NM", s.y)) + " " 	+ Fm8(Units.to("ft", s.z));
	}


	//  /** Format a position vector as a Euclidean position */
	//  public static String sStr12NP(Vect3 s) {
	//    return Fm12(Units.to("NM", s.x)) + " " + Fm12(Units.to("NM", s.y)) + " " 	+ Fm12(Units.to("ft", s.z));
	//  }

	/** Format a position vector as a Euclidean position */
	public static String sStr15NP(Vect3 s) {
		return Fm16(Units.to("NM", s.x)) + " " + Fm16(Units.to("NM", s.y)) + " " 	+ Fm16(Units.to("ft", s.z));
	}


	//	/** Format a velocity vector as a Euclidean velocity */
	//	public static String vStr(Vect2 s) {
	//		return "(" + Units.str("knot", s.x) + " ," + Units.str("knot", s.y) + ")";
	//	}
	//
	//  /** Format a velocity vector as a polar velocity */
	//  public static String vStr2(Vect2 v) {
	//    return "("+Units.str("deg",v.compassAngle())+", "+Units.str("knot",v.norm())+")";
	//  }
	//
	//	/** Format a velocity vector as a Euclidean velocity */	
	//	public static String vStr(Vect3 s) {
	//		return "(" + Units.str("knot", s.x) + " ," + Units.str("knot", s.y) + " ," 	+ Units.str("fpm", s.z) + ")";
	//	}
	//
	//	/** Format a velocity vector as a polar velocity */
	//	public static String vStr2(Vect3 v) {
	//		return "("+Units.str("deg",v.vect2().compassAngle())+", "+Units.str("knot",v.vect2().norm())+", "+Units.str("fpm",v.z())+")";
	//	}
	//
	//	/** Format a velocity vector as a polar velocity */
	//	public static String vStr8(Vect3 v) {
	//		return "("+Units.str8("deg",v.vect2().compassAngle())+", "+Units.str8("knot",v.vect2().norm())+", "+Units.str8("fpm",v.z())+")";
	//	}
	//
	//	/** Format a velocity vector as a polar velocity */
	//	public static String vStrNP(Vect3 v) {
	//		return Fm4(Units.to("deg",v.vect2().compassAngle()))+", "+Fm4(Units.to("knot",v.vect2().norm()))+", "+Fm4(Units.to("fpm",v.z()));
	//	}
	//
	public static double fm_nz(double v, int precision) {
		if (v < 0.0 && Math.ceil(v*Math.pow(10,precision)-0.5) == 0.0) 
			return 0.0;
		return v+0.0; // This removes actual negative zeros
	}

	private static DecimalFormat Frm0 = new DecimalFormat("0");
	private static DecimalFormat Frm1 = new DecimalFormat("0.0");
	private static DecimalFormat Frm2 = new DecimalFormat("0.00");
	private static DecimalFormat Frm3 = new DecimalFormat("0.000");
	private static DecimalFormat Frm4 = new DecimalFormat("0.0000");
	private static DecimalFormat Frm5 = new DecimalFormat("0.00000");
	private static DecimalFormat Frm6 = new DecimalFormat("0.000000");
	private static DecimalFormat Frm7 = new DecimalFormat("0.0000000");
	private static DecimalFormat Frm8 = new DecimalFormat("0.00000000");
	private static DecimalFormat Frm9 = new DecimalFormat("0.000000000");
	private static DecimalFormat Frm10 = new DecimalFormat("0.0000000000");
	private static DecimalFormat Frm11 = new DecimalFormat("0.00000000000");
	private static DecimalFormat Frm12 = new DecimalFormat("0.000000000000");
	private static DecimalFormat Frm13 = new DecimalFormat("0.0000000000000");
	private static DecimalFormat Frm14 = new DecimalFormat("0.00000000000000");
	private static DecimalFormat Frm15 = new DecimalFormat("0.000000000000000");
	private static DecimalFormat Frm16 = new DecimalFormat("0.0000000000000000");

	private static DecimalFormat Frm_2 = new DecimalFormat("00");
	private static DecimalFormat Frm_3 = new DecimalFormat("000");
	private static DecimalFormat Frm_4 = new DecimalFormat("0000");
	private static DecimalFormat Frm_5 = new DecimalFormat("00000");
	private static DecimalFormat Frm_6 = new DecimalFormat("000000");
	private static DecimalFormat Frm_7 = new DecimalFormat("0000000");
	private static DecimalFormat Frm_8 = new DecimalFormat("00000000");
	private static DecimalFormat Frm_9 = new DecimalFormat("000000000");

	/**
	 * Add leading zeros to a string representation of an integer.  If the integer
	 * is greater than the number of lead length, then just return the integer 
	 * as a string.<p>
	 * 
	 * <ul>
	 * <li>FmLead(3,2) == "02"
	 * <li>FmLead(100,2) == "100"
	 * </ul>
	 * 
	 * @param v the integer
	 * @param minLength the minimum length.
	 * @return
	 */
	public static String FmLead(int v, int minLength) {
		switch(minLength) {
		case 2 : return Frm_2.format(v);
		case 3 : return Frm_3.format(v);
		case 4 : return Frm_4.format(v);
		case 5 : return Frm_5.format(v);
		case 6 : return Frm_6.format(v);
		case 7 : return Frm_7.format(v);
		case 8 : return Frm_8.format(v);
		case 9 : return Frm_9.format(v);
		default : return Frm0.format(v);
		}
	}

	public static String FmPrecision(double v) {
		return FmPrecision(v,Constants.get_output_precision());
	}

	public static String FmPrecision(double v, int precision) {
		if (Double.isNaN(v)) {
			return "NaN";
		}
		if (Double.isInfinite(v)) {
			String s="infty";
			if (v < 0) return "-"+s;
			return s;
		} 
		v = fm_nz(v,precision+1);
		switch(precision) {
		case 1 : return Frm1.format(v);
		case 2 : return Frm2.format(v);
		case 3 : return Frm3.format(v);
		case 4 : return Frm4.format(v);
		case 5 : return Frm5.format(v);
		case 6 : return Frm6.format(v);
		case 7 : return Frm7.format(v);
		case 8 : return Frm8.format(v);
		case 9 : return Frm9.format(v);
		case 10: return Frm10.format(v);
		case 11: return Frm11.format(v);
		case 12: return Frm12.format(v);
		case 13: return Frm13.format(v);
		case 14: return Frm14.format(v);
		case 15: return Frm15.format(v);
		case 16: return Frm16.format(v);
		default : return Frm0.format(v);
		}
	}

	public static String Fmi(int v) {
		return ""+v;
	}

	/** Format a double with 2 decimal places */
	public static String Fm(double v) {
		return FmPrecision(v,2);
	}

	/** Format a double with 0 decimal places */
	public static String Fm0(double v) {
		return FmPrecision(v,0);
	}

	/** Format a double with 1 decimal places */
	public static String Fm1(double v) {
		return FmPrecision(v,1);
	}

	/** Format a double with 2 decimal places */
	public static String Fm2(double v) {
		return FmPrecision(v,2);
	}

	/** Format a double with 3 decimal places */
	public static String Fm3(double v) {
		return FmPrecision(v,3);
	}

	/** Format a double with 4 decimal places */
	public static String Fm4(double v) {
		return FmPrecision(v,4);
	}

	/** Format a double with 6 decimal places */
	public static String Fm6(double v) {
		return FmPrecision(v,6);
	}

	/** Format a double with 8 decimal places */
	public static String Fm8(double v) {
		return FmPrecision(v,8);
	}

	/** Format a double with 12 decimal places */
	public static String Fm12(double v) {
		return FmPrecision(v,12);
	}

	/** Format a double with 16 decimal places */
	public static String Fm16(double v) {
		return FmPrecision(v,16);
	}

	public static String FmPair(Pair<Vect3,Velocity> pp) {
		return " "+sStr8NP(pp.first)+" "+pp.second.toStringNP();
	}

	/**
	 * Format the double value after converting into the given units.  The returned string has
	 * a given width and the value has the given number of digits of precision.
	 * @param v
	 * @param units
	 * @param width
	 * @param precision
	 * @return a string represents the value
	 */
	public static String fmt(double v, String units, int width, int precision) {
		return padLeft(FmPrecision(Units.to(units,v),precision), width);
	}

	/**
	 * Format the double value.  The returned string has
	 * a given width and the value has the given number of digits of precision.
	 * 
	 * @param v
	 * @param width
	 * @param precision
	 * @return a string represents the value
	 */
	public static String fmt(double v, int width, int precision) {
		return padLeft(FmPrecision(v,precision), width);
	}





	private static DecimalFormat Frm_e4 = new DecimalFormat("0.0000E00");// new DecimalFormat("0.0000");

	/** Format a double in exponential notation with 4 decimal places */
	public static String Fme4(double v) {
		return Frm_e4.format(v);
	}


	public static String padLeft(String s, int n) {
		if (n == 0) {
			return s;
		}
		String str = "";
		str = String.format("%1$" + n + "s", s);  
		return str;
	}

	public static String padRight(String s, int n) {
		if (n == 0) {
			return s;
		}
		String str = "";
		str = String.format("%1$-" + n + "s", s);  
		return str;
	}


	public static String bool2char(boolean b) {
		if (b) return "T"; 
		else return "F";
	}


	public static String bool2str(boolean b) {
		if (b) return "True"; 
		else return "False";
	}

	/** Indent all lines of source string the given number of spaces */
	public static String indent(String s, int i) {
		String tab = "";
		for (int j=0; j < i; j++) tab += " ";
		String s2 = tab+s;
		int j = 0;
		int k = s2.indexOf("\n");
		while (k > j && k < s2.length()-2) {
			s2 = s2.substring(0,k+1)+tab+s2.substring(k+1);
			j = k+i+1;
			k = s2.indexOf("\n",j);
		}
		return s2;
	}

	@SuppressWarnings("unchecked")
	public static String Fobj(Object o) {

		if (o == null) {
			return "null";
		}
		if (o instanceof Pair) {
			Pair<Object,Object> p = (Pair<Object,Object>)o;
			return "("+Fobj(p.first)+","+Fobj(p.second)+")"; 
		}
		if (o instanceof Triple) {
			Triple<Object,Object,Object> p = (Triple<Object,Object,Object>)o;
			return "("+Fobj(p.first)+","+Fobj(p.second)+","+Fobj(p.third)+")"; 
		}
		if (o instanceof Quad) {
			Quad<Object,Object,Object,Object> p = (Quad<Object,Object,Object,Object>)o;
			return "("+Fobj(p.first)+","+Fobj(p.second)+","+Fobj(p.third)+","+Fobj(p.fourth)+")"; 
		}
		if (o instanceof Collection) {
			Collection<Object> a = (Collection<Object>)o;
			String s = "[";
			Iterator<Object> it = a.iterator();
			if (a.size() > 0) {
				s += Fobj(it.next());
			}
			while (it.hasNext()) {
				s += ", "+Fobj(it.next());
			}
			s += "]";
			return s;
		}
		if (o instanceof Map) {
			Map<Object,Object> h = (Map<Object,Object>)o;
			String s = "[Map:"+nl;
			for (Object k : h.keySet()) {
				s+=Fobj(k)+"->"+Fobj(h.get(k))+nl;
			}
			s+= "]";
			return s;
		}
		if (o instanceof Object[]) {
			Object[] a = (Object[])o;
			return Arrays.toString(a);
		}
		if (o instanceof int[]) {
			int[] a = (int[])o;
			return Arrays.toString(a);
		}
		if (o instanceof double[]) {
			double[] a = (double[])o;
			return Arrays.toString(a);
		}
		if (o instanceof java.awt.Polygon) {
			java.awt.Polygon p = (java.awt.Polygon)o;
			String s = "[java.awt.Polygon:";
			for (int i = 0; i < p.npoints; i++) {
				s += "("+p.xpoints[i]+","+p.ypoints[i]+") ";
			}
			s += "]";
			return s;
		}
		return o.toString();
	}

	/** 
	 * Create a string with the contents of the given list of strings.  Between each string insert the delimiter.
	 * @param l a list of strings
	 * @param delimiter the separator to use between the strings
	 * @return a string
	 */
	public static String list2str(List<String> l, String delimiter) {
		StringBuffer sb = new StringBuffer(100);
		Iterator<String> i = l.iterator();
		if (i.hasNext()) {
			sb.append(i.next());
			while (i.hasNext()) {
				sb.append(delimiter);
				sb.append(i.next());
			}
		}
		return sb.toString();
	}

	/** 
	 * Create a string with the contents of the OutputList object.  Between each string insert the delimiter.
	 * @param ol the object to get strings from
	 * @param delimiter the separator to use between the strings
	 * @return a string
	 */
	public static String list2str(OutputList ol, String delimiter) {
		return list2str(ol.toStringList(), delimiter);
	}

	/** 
	 * Create a string with the contents of the OutputList object.  Between each string insert the delimiter.
	 * @param ol the object to get strings from
	 * @param precision the number of digits to return for each value
	 * @param delimiter the separator to use between the strings
	 * @return a string
	 */
	public static String list2str(OutputList ol, int precision, String delimiter) {
		return list2str(ol.toStringList(precision), delimiter);
	}

	/**
	 * Return a string representation of the data in this 1-dimensional array.
	 * A newline is NOT added to the end.
	 * 
	 * @param data
	 * @param unit the unit to convert the values to
	 * @param width
	 * @param precision
	 * @return the resulting string
	 */
	public static String array2str(double[] data, String unit, int width, int precision) {
		StringBuffer sb = new StringBuffer(1000);
		sb.append("[");
		for (int i = 0; i < data.length; i++) {
			sb.append(f.fmt(data[i],unit,width,precision));
			if (i != data.length-1) {
				sb.append(" ");
			}
		}
		sb.append("]");
		return sb.toString();
	}

	/**
	 * Return a string representation of the data in this 2-dimensional table.
	 * A newline is NOT added to the end.
	 * 
	 * @param data
	 * @param unit the unit to convert the values to
	 * @param width
	 * @param precision
	 * @return the resulting string
	 */
	public static String array2str(double[][] data, String unit, int width, int precision) {
		StringBuffer sb = new StringBuffer(1000);
		for (int i = 0; i < data.length; i++) {
			if (i == 0) {
				sb.append("[");
			} else {
				sb.append(" ");
			}
			sb.append(array2str(data[i],unit,width,precision));
			if (i == data.length-1) {
				sb.append("]");
			} else {
				sb.append("\n");
			}
		}
		return sb.toString();
	}

	/**
	 * Return a string representation of the data in this 3-dimensional table.
	 * A newline is NOT added to the end.
	 * 
	 * @param data
	 * @param unit the unit to convert the values to
	 * @param width
	 * @param precision
	 * @return the resulting string
	 */
	public static String array2str(double[][][] data, String unit, int width, int precision) {
		StringBuffer sb = new StringBuffer(1000);
		for (int i = 0; i < data.length; i++) {
			for (int j = 0; j < data[i].length; j++) {
				if (i == 0 && j == 0) {
					sb.append("[[");
				}  else if (j == 0) {
					sb.append(" [");
				} else {
					sb.append("  ");
				}
				sb.append(array2str(data[i][j],unit,width,precision));
				if (j == data[i].length-1) {
					sb.append("]");
				} else {
					sb.append("\n");
				}
			}
			if (i == data.length-1) {
				sb.append("]");
			} else {
				sb.append("\n");
			}
		}
		return sb.toString();
	}

	public static String double2PVS(double val, int prec) {
		return f.FmPrecision(val,prec)+"::ereal";
	}


} 
