/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import gov.nasa.larcfm.Util.f;


public class FilePushTransmitter implements Transmitter {
	String filename;
	String keyword;
	File outfile;
	File lockfile;
	boolean running;

	public FilePushTransmitter() {
		filename = null;
		keyword = null;
		outfile = null;
		lockfile = null;
		running = false;
	}

	@Override
	/**
	 * The actual file name used is "address.word"
	 */
	public void publish(String address, String word) {
		if (!running) {
			running = true;
			filename = address;
			keyword = word;
			try {
				// test if we can create and remove the files.
				outfile = new File(filename+"."+keyword);
				lockfile = new File(filename+"."+keyword+".lock");
				if (outfile.exists()) outfile.delete();
				if (lockfile.exists()) lockfile.delete();
				outfile.createNewFile();
				lockfile.createNewFile();
			} catch (IOException e) {
				System.out.println("FileTransmitter: Could not create file "+address+"."+keyword);
				e.printStackTrace();
			}
			if (lockfile.exists()) lockfile.delete();
		} else {
			System.out.println("FileTransmitter: already running with file "+address+"."+keyword);
		}
	}

	@Override
	public void update(String keyword, String data) {
		if (running) {
			while(lockfile.exists()) {
				// wait
			}
			try {
				lockfile.createNewFile();
				if (outfile.exists()) outfile.delete();
				PrintWriter pw = new PrintWriter(new FileWriter(outfile),true);
				pw.println(data);
				pw.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				System.out.println("FileTransmitter: Could not write to file "+filename);
				e.printStackTrace();
			}
			if (lockfile.exists()) lockfile.delete();
		}
	}

	@Override
	public boolean hasSubscribers(String word) {
		return running && keyword == word;
	}

	public void unpublish(String word) {
		if (running && keyword == word) {
			shutdown();
		}
	}

	public boolean shutdown() {
		running = false;
		if (outfile.exists()) outfile.delete();
		if (lockfile.exists()) lockfile.delete();
		return true;
	}
	
}
