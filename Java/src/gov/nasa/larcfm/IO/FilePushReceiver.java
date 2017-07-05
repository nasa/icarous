/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.attribute.BasicFileAttributes;

import gov.nasa.larcfm.Util.f;


public class FilePushReceiver implements Receiver {
	String filename;
	String keyword;
	File infile;
	File lockfile;
	long filetime;
	String data;
	boolean running;

	public FilePushReceiver() {
		running = false;
	}

	@Override
	/**
	 * The actual file name used is "address.word"
	 */
	public boolean subscribe(String address, String word) {
		if (!running) {
			filename = address;
			keyword = word;
			data = "";
			infile = new File(filename+"."+keyword);
			lockfile = new File(filename+"."+keyword+".lock");
			if (infile.exists() && infile.canRead()) {
				filetime = 0;
				running = true;
				return true;
			} else {
				System.out.println("FileReceiver cannot read file "+filename+"."+keyword);
				return false;
			}
		} else {
			System.out.println("FileReceiver already running with file "+filename+"."+keyword);
			return false;
		}
	}

	@Override
	public void unsubscribe(String address, String keyword) {
		running = false;
	}

	@Override
	public String request(String keyword) {
		if (running) {
			return data;
		} else {
			return "";
		}
	}

	@Override
	public boolean hasUpdate(String keyword) {
		if (running) {
			while (lockfile.exists()) {
				// wait
			}
			try {
				lockfile.createNewFile();
				BasicFileAttributes attr = Files.readAttributes(Paths.get(infile.getAbsolutePath()), BasicFileAttributes.class);
				long ftime = attr.lastModifiedTime().toMillis();
				boolean up = ftime > filetime;
				if (up) {
					BufferedReader br = new BufferedReader(new FileReader(infile));
					data = "";
					String tmp = br.readLine();
					while (tmp != null) {
						data += tmp+"\n";
						tmp = br.readLine();
					}
					br.close();
				}
				if (lockfile.exists()) lockfile.delete();
				return up;
			} catch (IOException e) {
				if (lockfile.exists()) lockfile.delete();
				System.out.println("FileReceiver cannot read file "+filename+"."+keyword);
				e.printStackTrace();
				return false;
			}
		} else {
			return false;
		}
	}

}
