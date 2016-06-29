/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

import gov.nasa.larcfm.Util.f;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ConnectException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import com.sun.org.apache.xerces.internal.impl.dv.DatatypeException;


// simple socket echo server
public class CommBoxStub {

	public static void main(String[] args) throws IOException {

		HeartbeatThread hb = new HeartbeatThread();
		EchoThread echo = new EchoThread();

		hb.start();
		echo.start();
	}

}

class HeartbeatThread extends Thread {
	private int port = 4444;
	private long count = 0;

	public HeartbeatThread() {
		super("HeartbeatThread");
	}

	public void run() {

		f.pln("Commbox Starting Heartbeat");
		try (	Socket socket = new Socket("127.0.0.1", port);
				PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
				) {
			while (true) {
				count++;
				out.println("HB="+count);
				Thread.sleep(1000);
			}
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			f.pln("Combox HB Connection failed");
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

class EchoThread extends Thread {
	private int port = 5555;
	private long count = 0;

	public EchoThread() {
		super("EchoThread");
	}

	public void run() {
		f.pln("Commbox Starting Echo");
		try (	Socket socket = new Socket("127.0.0.1", port);
	            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				) {
			String line;
			while ((line = in.readLine()) != null) {
				f.pln("Commbox reads: "+line);
			}
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			f.pln("Combox HB Connection failed");
		}
	}
}
