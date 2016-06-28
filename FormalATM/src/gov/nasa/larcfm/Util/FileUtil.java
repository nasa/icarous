/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

/** A collection of utility functions useful for files */
public final class FileUtil {

	  /**
	   * Given a file name return the path of this name.
	   * This also converts backward slashes to forward slashes.
	   * @filename the name of the file
	   * @return the path of the file, if no path, then an empty string is returned.
	   */
	  public static String get_path(String filename) {
		  return filename.substring(0,filename.replaceAll("\\\\", "/").lastIndexOf("/") + 1);
	  }
	  
	  /**
	   * Given a file name return the extension of this name.
	   * @filename the name of the file
	   * @return the extension--part of filename after last period (.)
	   */
	  public static String get_extension(String filename) {
		  return filename.substring(filename.lastIndexOf(".") + 1);
	  }
	  
	  /**
	   * Given a file name return the Java version of this name.
	   * This converts backward slashes to forward slashes.
	   * @filename the name of the file
	   * @return the Java-native version of the filename
	   */
	  public static String unifyFileName(String filename) {
		  return filename.replaceAll("\\\\", "/");
	  }
	  
	/**
	 * Returns the base (no path) file name. 
	 * <ul> 
	 * <li>a/b/c.txt = c.txt
	 * <li>a.txt = a.txt
	 * <li>a/b/ = ""
	 * </ul>
	 * This also converts backward slashes to forward slashes.
	 * @param filename  filename string
	 * @return filename without path information
	 */
	public static String no_path(String filename) {
		if (filename == null) {
			return null;
		}
		// replace all \ (dos) with / (unix), then find last
		return filename.substring(filename.replaceAll("\\\\", "/").lastIndexOf("/") + 1);
	}

	/**
	 * Returns the file name without an extension (such as, ".txt").  Also removes path information. 
	 * @param filename  filename string
	 * @return filename without extension information
	 */
	public static String no_extension(String filename) {
		if (filename == null) {
			return null;
		}
		// replace all \ (dos) with / (unix), then find last
		String newname = no_path(filename);
		return newname.substring(0,newname.lastIndexOf("."));
	}

	
	/**
	 * Determine if the file exists, as a file.  
	 * Directories (i.e., folders) are not considered files.
	 * 
	 * @param name filename, possibly including a path.
	 * @return true, it the file exists
	 */
	public static boolean file_exists(String name) {
		boolean ret = false;
		File file = new File(name);
		if (file.canRead() && ! file.isDirectory()) {
			ret = true;
		}
		file = null;

		return ret;
	}

	/**
	 * Given a filename (perhaps with a path), return the next filename, that does not already exist.
	 * If the files test_1.txt test_2.txt and test_3.txt exist, then get_next_filename("test_1.txt") will
	 * return "test_4.txt".
	 * 
	 * @param name the candidate file name
	 * @return the next file name
	 */
	public static String get_next_filename(String name) {
		String path = get_path(name);
		String ext = "."+get_extension(name);
		
		String short_name = no_extension(no_path(name));
		int idx = short_name.lastIndexOf("_");
		
		String base = short_name;
		int num = 0;
		
		if ( idx >= 0) { // is there an _ ?
			if (short_name.length() - 1 == idx) { // is _ the last character?
				base = short_name.substring(0,idx); // strip off final _
				num = 0;
			} else {
				String number = short_name.substring(idx+1);
				if (Util.is_int(number)) { // is the stuff after the _ a number?
					base = short_name.substring(0,idx);
					num = Util.parse_int(number);
				} 
			}
		}
		
		String new_name;
		do { 
			new_name = path + base + "_" + num + ext;
			num++;
		} while ( file_exists(new_name) );
		
		return new_name;		
	}

	  
	  /** Given a list of names that may include files or directories,
	   * return a list of files that contains (1) all of the files in
	   * the original list and (2) all files ending with given extension in
	   * directories from the original list. 
	   * 
	   * @param names a list of names of files or directories
	   * @param extension the extension to search the directory for (include the period, use ".txt" not "txt")
	   * @return a list of file names
	   */
	  public static List<String> getFileNames(String[] names, String extension) {
		  ArrayList<String> txtFiles = new ArrayList<String>(names.length);
		  for (int i=0; i < names.length; i++) {
			  txtFiles.addAll(getFileName(names[i], extension));
		  }
		  return txtFiles;	  
	  }
	  
	  /** Given a list of names that may include files or directories,
	   * return a list of files that contains (1) all of the files in
	   * the original list and (2) all files ending with given extension in
	   * directories from the original list. 
	   * 
	   * @param names a list of names of files or directories
	   * @param extension the extension to search the directory for (include the period, use ".txt" not "txt")
	   * @return a list of file names
	   */
	  public static List<String> getFileNames(List<String> names, String extension) {
		  ArrayList<String> txtFiles = new ArrayList<String>(names.size());
		  for (String name: names) {
			  txtFiles.addAll(getFileName(name, extension));
		  }
		  return txtFiles;	  
	  }
	  
	  /** 
	   * Given a name that may be a file or directory,
	   * return a list of files that contains (1) the file provided as a
	   * parameter or (2) all files ending with the given extension in the
	   * directory provided as a parameter. 
	   * 
	   * @param name the name of a file or directory
	   * @param extension the extension to search the directory for (include the period, use ".txt" not "txt")
	   * @return a list of file names
	   */
	  public static List<String> getFileName(String name, String extension) {
		  ArrayList<String> txtFiles = new ArrayList<String>();
		  File file = new File(name);
		  if (file.canRead()) {
			  if (file.isDirectory()) {
				  final String ext = extension; 
				  File[] fs=file.listFiles(new FilenameFilter() {
					  public boolean accept(File f, String name) {
						  return name.endsWith(ext);
					  }                       
				  }); 
				  for (File txtfile:fs) {
					  txtFiles.add(txtfile.getPath());
				  }
			  } else {
				  txtFiles.add(file.getPath());
			  }
		  }
		  return txtFiles;
	  }
	  
	  /**
	   * Writes a key and value to a file. This can be used for persistence 
	   * (i.e., when an application starts up, the last file is known).
	   * 
	   * @param configFile the name of the configuration file
	   * @param key the key
	   * @param value the value
	   */
	  public static void write_persistent(String configFile, String key, String value) {
		  //TODO: need to rework this to handle when to add to a config file and when to overwrite/make-a-new file
		try {
			PrintWriter pw = open_PrintWriter(configFile);
			pw.println(key+" = "+value);
			pw.close();
		} catch (Exception e2) {
			// do nothing
		}
	  }

	  /**
	   * Given a file with a key/value parameter, return the value of the given key.
	   * This 
	   * can be used for persistence (i.e., when an application starts up, the last file is known)
	   * 
	   * @param configFile the name of the file to read
	   * @param key the key in the configuration file
	   * @return the value of the key, empty string if the key is not found
	   */
	  public static String read_persistent(String configFile, String key) {
		  String rtn = "";
		  SeparatedInput cr;
		  try {
			  cr = new SeparatedInput(new BufferedReader(new FileReader(configFile)));
			  cr.readLine();
			  if ( ! cr.hasMessage()) {
				  rtn = cr.getParametersRef().getString(key);
			  }
			  return rtn;
		  } catch (FileNotFoundException e1) {
			  return rtn;
		  } 
	  }

	  public static PrintWriter open_PrintWriter(String outFileName) {
		  PrintWriter outFile;
		  try {
			  // Create the output stream.
			  outFile = new PrintWriter(new BufferedWriter(new FileWriter(outFileName)));
		  }
		  catch (IOException e) {
			  //System.err.println("Can't open file " + outFileName + "!");
			  //System.err.println("Error: " + e);
			  return null; 
		  }
		  return outFile;
	  }

	  private static Object FileLocker = new Object() {};
	  /**
	   *  Add to the given file a line represented by the list of strings (using UTF-8 character encoding).  This method
	   *  will work even if multiple programs (or multiple threads within the same program) are trying to write to the 
	   *  same file at the same time.  If the file is already opened by another thread, then this method will wait until
	   *  the lock is released, then it will append its line.  (Warning: there is a possibility for deadlock).<p>
	   *  
	   *  For threads within the same program, the guarantee that two threads will not write at the same time is
	   *  only valid if this method is used.  If the file is opened with regular Java IO calls, then there is no
	   *  guarantee of single access.<p>
	   *  
	   * @param pathname the name of the file to append a line (including, possibly, a path)
	   * @param list a list of strings to write to the file
	   * @param delimiter the string to place between each element of the list of string (a comma or a tab character are typical choices)  
	   * @throws FileNotFoundException if the file cannot be opened (because it is a directory, etc.)
	   */
	  public static void append_file(String pathname, List<String> list, String delimiter) throws FileNotFoundException {

		  synchronized (FileLocker) {
			  FileOutputStream in = new FileOutputStream(pathname, true);
			  try {
				  java.nio.channels.FileLock lock = in.getChannel().lock();
				  try {
					  PrintWriter writer = new PrintWriter(new OutputStreamWriter(in, "UTF-8"));
					  writer.println(f.list2str(list,delimiter));
					  writer.close();
				  } finally {
					  lock.release();
				  }
			  } catch (IOException exp) {
				  // do nothing?
			  } finally {
				  try { 
					  in.close();
				  } catch (IOException exp) {
					  // do nothing
				  }
			  }
		  }
	  }


}
