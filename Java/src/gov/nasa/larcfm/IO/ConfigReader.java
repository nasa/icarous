/* 
 * StateReader
 *
 * Contact: George Hagen
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterProvider;
import gov.nasa.larcfm.Util.ParameterReader;
import gov.nasa.larcfm.Util.Units;

import java.io.Reader;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;

/**
 * This object will read in and store a set of configuration parameters.  The parameters can be listed 
 * in the conventional way, such as: <code>param = 10 [NM]</code>.  In addition, the parameters can
 * be listed in a table format: <br>
 * <code>
 * param1 param2 param3
 * 1      5      100
 * 2      10     99 
 * 3      20     98
 * 4      10     97
 * 5      5      96
 * </code>
 * If the parameters are stored in this way, then every call to the "next()" method will update the parameters
 * (for this example, the parameters: param1, param2, and param3) with the next values in the file. <p>
 *   
 * Instead of next(), the nextIterate() method can be called which will return all of the param1 values with
 * each of the param2 values, and so on.  In this case the parameters may require different numbers of
 * values; so, the columns of values should be augmented with an invalid value that will
 * be ignored, for example
 * <code>
 * param1 param2 param3
 * 1      5      100
 * 2      10     99 
 * 3      20     x
 * 4      x      x
 * 5      x      x
 * </code>
 * 
 * includeConfigFile:
 * A special set parameters will be stripped and processed immediately.  Parameters starting with the string "includeConfigFile"
 * will launch a temporary ConfigReader that will recursively attempt to read any parameters in the indicated file and add them
 * it this reader.  Parameters read from secondary files will not overwrite parameters set in their parent files.
 */
public final class ConfigReader implements ParameterReader, ParameterProvider, ErrorReporter {
	private ErrorLog error;
	private String param_var[];
	private boolean param_isValue[];
	private ArrayList<String[]> param_val;
	private int count;
	private int count_itr[];
	private boolean hasRead;
	private ParameterData pd;
  
    
	//private boolean interpretUnits;

    /** A new, empty ConfigReader.  This may be used to store parameters, but nothing else. */
	public ConfigReader() {
		error = new ErrorLog("ConfigReader()");
		param_var = new String[0];
		param_isValue = new boolean[0];
		param_val = new ArrayList<String[]>(10);
		count_itr = new int[0];
		count = 0;
		pd = new ParameterData();
	}

	/** 
	 * Read a new file into an existing ConfigReader.  
	 * Parameters are preserved if they are not specified in the file. 
	 * */
	public void open(String filename) {
		if (filename == null || filename.equals("")) {
			error.addError("No filename given");
			return;
		}
		Reader fr;
		try {
			fr = new BufferedReader(new FileReader(filename));
			open(fr);
			fr.close();
		} catch (FileNotFoundException e) {
			error.addError("File "+filename+" read protected or not found");
			if (param_var != null) {
				param_var = new String[0];
				param_val.clear();
			}
			return;
		} catch (IOException e) {
			error.addError("On close: "+e.getMessage());
			if (param_var != null) {
				param_var = new String[0];
				param_val.clear();
			}
			return;
		}
	}
	
	
	public void open(Reader r) {
		if (r == null) {
			error.addError("null given for open(Reader)");
			return;
		}
		
		SeparatedInput si;
		si = new SeparatedInput(r);
		si.setCaseSensitive(false);
		
		loadfile(si);
		if (si.hasError()) {
			error.addError(si.getMessage());
		}
		if (si.hasMessage()) {
			error.addWarning(si.getMessage());
		}
		
		count = 0;
		for (int i=0; i < count_itr.length; i++) {
			count_itr[i] = 0;
		}
	}	
	
	private void loadfile(SeparatedInput input) {
		hasRead = false;
      
		while ( ! input.readLine()) {
			// look for each possible heading
			if ( ! hasRead) {
				pd.copy(input.getParametersRef(), true);
				param_var = new String[input.size()];
				param_isValue = new boolean[input.size()];
				param_val = new ArrayList<String[]>(10);
				count_itr = new int[input.size()];
				for(int i = 0; i < input.size(); i++) {
					if ( ! pd.contains(input.getHeading(i))) {
						pd.set(input.getHeading(i),"0.0 ["+input.getUnit(i)+"]");						
					}
					param_var[i] = input.getHeading(i);
					param_isValue[i] = true;
					if (Double.MAX_VALUE == Units.parse(input.getColumnString(i), Double.MAX_VALUE)) {
						param_isValue[i] = false;
					}
				}
				hasRead = true;
			} 

			String[] values = new String[input.size()];
			for(int i = 0; i < input.size(); i++) {
				String s = input.getColumnString(i);
				String unit = input.getUnit(i);
				if (unit.equals("unspecified")) {
					unit = pd.getUnit(param_var[i]);
				}
				try {
					values[i] = s+" ["+unit+"]";
					Double.parseDouble(s);
				} catch (NumberFormatException e) {
					values[i] = s;  // if not a double, then just add the string
				}
			}
			param_val.add(values);			
		}
		if ( ! hasRead) {
			pd.copy(input.getParametersRef(), true);
		}
		
	}

    /** Return the number of parameters configurations in the file */
	public int size() {
		return param_var.length;
	}

	/** Gets the next set of parameters from a line in the file.  Returns true, if the end of file has been reached. */
	public boolean next() {
		//f.pln("Next "+count+" "+param_val.size());
		if (count < param_val.size()) {
			String[] values = param_val.get(count);
			for (int i = 0; i < values.length; i++) {
				pd.set(param_var[i],values[i]);
			}
			count++;
			return false; // means not EOF
		}
		return true; // means EOF
	}

	/** Gets the next set of parameters.  If there is more than one column of parameters in the 
	 * file, then each member of one column are iterated through all members of all the other columns.
	 * Returns true, if the end of file has been reached. 
	 */
	public boolean nextIterate() {
		if (count_itr.length > 0 && count_itr[0] >= param_val.size()) {
			return true; // means EOF
		}
		
		// Get the parameters
		for (int j=0; j<count_itr.length; j++) {
			pd.set(param_var[j],param_val.get(count_itr[j])[j]);
		}
		
		// Update the counts, start at the rightmost column and move in
		//   Note, if it gets to this point, then a set of valid
		//   parameters have been loaded into the ParameterData structure;
		//   therefore, from this point on, only a "false" should be returned
		//   from this call to nextIterate().  This means
		//   the user should use the parameters that have been set.
		//   The most this code should do is set it up so the next call to
		//   nextIterate() should return true (meaning there are no more
		//   iterations on parameters available).
		for (int j=count_itr.length-1; j >= 0; j--) {
			if (count_itr[j] < param_val.size()-1) { // there are more values in this column
				count_itr[j]++;
				
				// check if the value in the column is invalid, if so, skip this column and move to the next
				if (param_isValue[j] && Double.MAX_VALUE == Units.parse(param_val.get(count_itr[j])[j], Double.MAX_VALUE)) {
					count_itr[j] = 0;
					if (j == 0) { // if there is an invalid value in the first column, then we are done
						count_itr[j] = param_val.size();
						return false;
					}
				} else {
					return false;
				}
			} else { // there are no more values in this column so reset the counter
				count_itr[j] = 0;				
				if (j == 0) { // if we reset the first column, then we are done
					count_itr[j] = param_val.size();
				}
			}
		}
		return false;
	}

	/** Return the number of columns. Must call "open" first */ 
	public int getNumberColumns() {
		return param_var.length;
	}

	/** Return the heading for the given column.  Must call "open" first. */ 
	public String getHeading(int i) {
      if (i < 0 || i >= param_var.length) {
        return "";
      }
		return param_var[i];
	}

	/**
	 * Return the parameter database
	 */
	public ParameterData getParametersRef() {
		return pd;
	}

	public ParameterData getParameters() {
		return new ParameterData(pd);
	}

	public void updateParameterData(ParameterData p) {
		p.copy(pd,true);
	}
	
	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError();
	}
	public boolean hasMessage() {
		return error.hasMessage();
	}
	public String getMessage() {
		return error.getMessage();
	}
	public String getMessageNoClear() {
		return error.getMessageNoClear();
	}

//	public static void includeConfigFile(String file) {
//		ParameterData includes = pd.extractPrefix("includeConfigFile");
//		for (String key : includes.getList()) {
//			ParameterData includedParams = ConfigReader.includeConfigFile(includes.getString(key));
//			pd.remove(key);
//			pd.copy(includedParams, false);
//		}
//
//		ConfigReader reader = new ConfigReader();
//		reader.open(file);
//		ps.reader.getParameters();
//
//		
//	}
	
}
