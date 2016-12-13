/**
 * Main entry point for ICAROUS
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. 
 *  All rights reserved.
 *     
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, 
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, 
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT 
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS 
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND 
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE 
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

package gov.nasa.larcfm.ICAROUS;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.IO.SeparatedInput;
import com.MAVLink.common.*;
import java.io.*;

public class Icarous {

	private boolean verbose;
	private String sitlhost;
	private String px4port;
	private String bcastgroup;
	private String comport;
	private String radioport;
	private String mode;
	private int px4baud;
	private int radiobaud;
	private int sitlport;
	private int bcastport;
	private int comportin;
	private int comportout;
	private ParameterData pData;
	private AircraftData FlightData;
	private Interface APInt;
	private Interface COMInt;
	private Interface BCASTInt;
	private Aircraft uasQuad;
	private FMS fms_module;
	private DAQ daq_module;
	private COM com_module;
	private BCAST bcast_module;
	private Mission Task;
	private String version;

	public Icarous(String args[], Mission task) {

		version = "1.0";
		verbose = false;
		sitlhost = null;
		px4port = null;
		bcastgroup = null;
		comport = null;
		radioport = null;
		mode = null;
		px4baud = 0;
		radiobaud = 0;
		sitlport = 0;
		bcastport = 0;
		comportin = 0;
		comportout = 0;
		APInt = null;
		COMInt = null;
		BCASTInt = null;
		Task = task;

		// Read in initial value for all parameters (Note this can also be set
		// from
		// the ground station later.
		pData = null;
		try {
			FileReader in = new FileReader("params/icarous.txt");
			SeparatedInput reader = new SeparatedInput(in);

			reader.readLine();
			pData = reader.getParametersRef();
		} catch (FileNotFoundException e) {
			System.out.println("parameter file not found");
		}

		// Process input arguments
		for (int i = 0; i < args.length && args[i].startsWith("-"); ++i) {
			if (args[i].startsWith("-v")) {
				verbose = true;
			}

			else if (args[i].startsWith("--px4")) {
				px4port = args[++i];
				px4baud = Integer.parseInt(args[++i]);
			}

			else if (args[i].startsWith("--sitl")) {
				sitlhost = args[++i];
				sitlport = Integer.parseInt(args[++i]);
			}

			else if (args[i].startsWith("--com")) {
				comport = args[++i];
				comportin = Integer.parseInt(args[++i]);
				comportout = Integer.parseInt(args[++i]);
			}

			else if (args[i].startsWith("--radio")) {
				radioport = args[++i];
				radiobaud = Integer.parseInt(args[++i]);
			}

			else if (args[i].startsWith("--bc")) {
				bcastgroup = args[++i];
				bcastport = Integer.parseInt(args[++i]);
			}

			else if (args[i].startsWith("--mode")) {
				mode = args[++i];
			}

			else if (args[i].startsWith("-")) {
				System.out.println("Invalid option " + args[i]);
				System.exit(0);
			}

		}

		System.out.println("ICAROUS version: " + version);

		FlightData = new AircraftData(pData);

		if (sitlport > 0) {
			APInt = new Interface(Interface.SOCKET, sitlhost, sitlport, 0, FlightData);

		} else {
			APInt = new Interface(Interface.SERIAL, px4port, px4baud, FlightData);
		}

		if (radioport == null) {
			COMInt = new Interface(Interface.SOCKET, comport, comportin, comportout, FlightData);
		} else {
			COMInt = new Interface(Interface.SERIAL, radioport, radiobaud, FlightData);

		}

		COMInt.SendStatusText("ICAROUS version: " + String.valueOf(version));

		// Create broad cast thread if necessary
		if (bcastport > 0) {

			BCASTInt = new Interface(Interface.SOCKET, bcastgroup, 0, bcastport, FlightData);

			bcast_module = new BCAST("Broadcast", uasQuad, BCASTInt, pData);

		}

		uasQuad = new Aircraft(APInt, COMInt, FlightData, Task, pData);

		fms_module = new FMS("Flight management", uasQuad, pData);
		daq_module = new DAQ("Data acquisition", uasQuad, pData);
		com_module = new COM("Communications", uasQuad, pData);

		uasQuad.error.setConsoleOutput(verbose);
		com_module.error.setConsoleOutput(verbose);

		COMInt.SendStatusText("DAQ, FMS, COM Initialized");
	}

	public void SetDaidaludusConfig(String filename) {
		uasQuad.fsam.SetDaaConfig(filename);
	}

	public String GetVersion() {
		return version;
	}

	public void run() {
		if (mode.equals("passthrough")) {
			System.out.println("ICAROUS pass through mode");
			while (true) {
				Interface.PassThrough(APInt, COMInt);
			}
		} else {

			daq_module.start();

			msg_heartbeat msgHeartbeatAP = FlightData.Inbox.GetHeartbeat_AP();
			while (msgHeartbeatAP == null) {
				msgHeartbeatAP = FlightData.Inbox.GetHeartbeat_AP();
			}

			System.out.println("Received heartbeat from AP");
			COMInt.SendStatusText("Connected to Pixhawk");

			uasQuad.EnableDataStream(1);

			com_module.start();

			if (bcastport > 0) {
				bcast_module.start();
			}

			if (mode.equals("passive")) {

				System.out.println("ICAROUS passive mode");
				COMInt.SendStatusText("ICAROUS is passive");
				while (true) {
					uasQuad.FlightData.GetGPSdata();
					uasQuad.FlightData.GetAttitude();
					uasQuad.fsam.Monitor();
				}
			} else {
				System.out.println("ICAROUS active mode");
				COMInt.SendStatusText("ICAROUS is active");
				fms_module.start();

				while (fms_module.isFMSrunning()) {
					// DO nothing
				}
			} // end of mode else (passive)
		} // end of mode else (passthrough)
	}// end of run
}// end of class
