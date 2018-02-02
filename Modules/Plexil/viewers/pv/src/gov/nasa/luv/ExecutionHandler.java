/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package gov.nasa.luv;

import gov.nasa.luv.Constants.AppType;
import static gov.nasa.luv.Constants.AppType.*;
import static gov.nasa.luv.Constants.PLEXIL_SCRIPTS_DIR;
import static gov.nasa.luv.Constants.UNKNOWN;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.Vector;

import static java.util.concurrent.TimeUnit.SECONDS;

/** The ExecutionHandler class runs an instance of the Universal Executive. */

/**
 * Adapted by Hector Fabio Cadavid Rengifo. hector.cadavid@escuelaing.edu.co
 */
public class ExecutionHandler
{
    private Plan currentPlan;
    private PlanView currentView;
    private Process execProcess;
    private Thread execMonitorThread;

    private boolean block;

    private static File CHECKER_PROG = new File(Constants.PLEXIL_SCRIPTS_DIR, "checkPlexil");

    /** Represents the "universal executive" for PLEXIL. */ 
    public static final String    UE_EXEC = "universalExec";

    /** Represents the "test executive" for PLEXIL. */ 
    public static final String    UE_TEST_EXEC = "TestExec";
      
    /** Represents the script for running the "Universal executive" for PLEXIL. */ 
    public static final String    UE_SCRIPT = "plexilexec";

    /** Represents the script path for running the "Universal executive" for PLEXIL. */ 
    public static final String    RUN_UE_EXEC = (new File(PLEXIL_SCRIPTS_DIR, UE_SCRIPT)).getAbsolutePath();
      
    /** Represents the script for running the "Test executive" for PLEXIL. */ 
    public static final String    TE_SCRIPT = "plexiltest";
      
    /** Represents the script path for running the "Test executive" for PLEXIL. */ 
    public static final String    RUN_TEST_EXEC = (new File(PLEXIL_SCRIPTS_DIR, TE_SCRIPT)).getAbsolutePath();
      
    /** Represents the script for running the "Plexil Simulator (includes Universal Exec)" for PLEXIL. */ 
    public static final String    SIM_SCRIPT = "plexilsim";
      
    /** Represents the script path for running the "Plexil Simulator" for PLEXIL. */ 
    public static final String    RUN_SIMULATOR = (new File(PLEXIL_SCRIPTS_DIR, SIM_SCRIPT)).getAbsolutePath();

    private static ExecutionHandler _the_instance_ = null;

    private ExecutionHandler() {
        currentPlan = null;
        execProcess = null;
        execMonitorThread = null;
        block = false;
    }

    public static ExecutionHandler instance() {
        if (_the_instance_ == null)
            _the_instance_ = new ExecutionHandler();
        return _the_instance_;
    }

    public boolean shouldBlock() {
        return block;
    }

    public void blocked() {
        if (currentView != null)
            currentView.blocked();
        else
            StatusMessageHandler.instance().displayErrorMessage(null,
                                                                "Execution paused but no view to resume it");
    }

	private void cleanup(Process p) {
        try {
            p.getInputStream().close();
        }
        catch (IOException e) {
        }
        try {
            p.getOutputStream().close();
        }
        catch (IOException e) {
        }
        try {
            p.getErrorStream().close();
        }
        catch (IOException e) {
        }
	}

    public boolean isAvailable() {
        return currentPlan == null;
    }

  	/**
  	 * Determine status of execution.
  	 * 
  	 * @return whether an internal exec instance is runnning
  	 */

    public boolean isExecuting() {
        return execProcess != null
            && execProcess.isAlive();
    }

    public PlanView getPlanView() {
        return currentView;
    }
    
    /**
     * @brief Starts the selected PLEXIL Exec and a thread to monitor its output.
     * @return True if the process was launched, false if an error occurred.
     */

    public boolean runExec(Plan p, PlanView v) {
        if (!isAvailable()) {
            v.showErrorMessage(null, "Already executing another plan");
            return false;
        }

        currentPlan = p;
        currentView = v;

        boolean result;
        if (!(result = runExecInternal())) {
            currentPlan = null;
            currentView = null;
        }

        return true;
    }

    private boolean runExecInternal() {
        CommandGenerator g = null;
        try {
            g = getCommandGenerator();
            if (g == null) {
                currentView.showErrorMessage(null,
                                             "Error in Exec command generation: "
                                             + "Couldn't find command generator");
                return false;
            }
        }
        catch (CommandGenerationException c) {
            currentView.showErrorMessage(null,
                                         "Error in Exec command generation");
            return false;
        }
        catch (Exception e) {
            currentView.showErrorMessage(e, "Error constructing Exec command line");
            return false;
        }

        try {
            g.checkPlanFile(currentPlan);
        }
        catch (CommandGenerationException c) {
            currentView.showErrorMessage(null,
                                         "Error locating PLEXIL plan");
            return false;
        }
        catch (Exception e) {
            currentView.showErrorMessage(e, "Error locating PLEXIL plan");
            return false;
        }

        Settings s = Settings.instance();
        if (s.checkPlan())
            try {
                String[] cmd = {CHECKER_PROG.toString(), currentPlan.toString()};
                currentView.showMessage("Checking plan file " + currentPlan);
                if (0 != Runtime.getRuntime().exec(cmd).waitFor()) {
                    currentView.showErrorMessage(null,
                                                 "Plan " + currentPlan.getPlanFile() + " failed static checks.");
                    return false;
                }
            }
            catch (Exception e) {
                currentView.showErrorMessage(e, "Error checking PLEXIL plan" + currentPlan.getPlanFile());
                return false;
            }

        List<String> cmd;
        try {
            cmd = createCommand(g, currentPlan, s);
        }
        catch (CommandGenerationException c) {
            currentView.showErrorMessage(null,
                                         "Error constructing PLEXIL Exec command line");
            return false;
        }
        catch (Exception e) {
            currentView.showErrorMessage(e, "Error constructing PLEXIL Exec command line");
            return false;
        }

        if (cmd == null) {
            currentView.showInfoDialog("Sorry",
                                       "Application mode doesn't support execution from Viewer");
            return false;
        }

        // Block immediately after start if requested
        if (s.blocksExec())
            block = true;
        try {
            execMonitorThread = new Thread(new ExecRunner(cmd));
            execMonitorThread.start();
        }
        catch (Exception e) {
            currentView.showErrorMessage(e, "Error launching PLEXIL Exec process");
            return false;
        }

        return true;
    }
    
    /** Stop running the UE. */

    // TODO: Ensure all subshells have exited
    
    public void stop() {
        if (execProcess == null) {
            currentView.executionComplete();
            return;
        }

        if (execProcess.isAlive()) {
            cleanup(execProcess);
            execProcess.destroy();
            boolean terminated = false;
            try {
                terminated = execProcess.waitFor(5, SECONDS);
            }
            catch (InterruptedException e) {
            }
            while (!terminated)
                try {
                    execProcess.destroyForcibly().waitFor();
                    terminated = true;
                }
                catch (InterruptedException i) {
                }
        }
        currentView.executionTerminated();
    }

    public void breakpointReached(LuvBreakPoint bp) {
        block = true;
    }

    public void step() {
        block = true;
        LuvSocketServer.resume();
    }

    public void resume() {
        block = false;
        LuvSocketServer.resume();
    }

    public void pause() {
        if (Settings.instance().blocksExec())
            block = true;
    }

    /**
     * This methods returns a concrete CommandGenerator for the selected mode.
     * If the environment variable ALT_EXECUTIVE is set, returns an instance of the
     * class named to generate the command.
     * @return a plexil command generator.
     */
    // TODO: don't construct a new instance for each call
    // *** FIXME: Only look for ALT_EXECUTIVE when in USER_SPECIFIED mode. ***
    @SuppressWarnings("unchecked")
    private CommandGenerator getCommandGenerator()
        throws CommandGenerationException
    {
        String alternativeExecutive = System.getenv("ALT_EXECUTIVE");
        if (alternativeExecutive != null) {
            Class ecgClass;
            try {
				ecgClass = Class.forName(alternativeExecutive);
            } catch (ClassNotFoundException e) {
				throw new CommandGenerationException("The class named by the ALT_EXECUTIVE system variable, "
                                                     + alternativeExecutive + ", doesn't exist.");
			}
            Constructor ecgClCons;
            try {
				ecgClCons = ecgClass.getConstructor(new Class[]{});
			} catch (SecurityException e) {
				throw new CommandGenerationException("Error trying to get constructor for user-defined command generator "
                                                     + alternativeExecutive + ":",
                                                     e);
			} catch (NoSuchMethodException e) {
				throw new CommandGenerationException("Error trying to get constructor for user-defined command generator "
                                                     + alternativeExecutive + ":",
                                                     e);
            }
            Object o;
            try {
                // N.B. IllegalArgumentException shouldn't happen here.
                o = ecgClCons.newInstance(new Object[]{});
			} catch (IllegalArgumentException e) {
				throw new CommandGenerationException("Error trying to create instance of user-defined command generator "
                                                     + alternativeExecutive + ":",
                                                     e);
			} catch (InstantiationException e) {
				throw new CommandGenerationException("Error trying to create instance of user-defined command generator "
                                                     + alternativeExecutive + ":",
                                                     e);
			} catch (IllegalAccessException e) {
				throw new CommandGenerationException("Error trying to create instance of user-defined command generator "
                                                     + alternativeExecutive + ":",
                                                     e);
			} catch (InvocationTargetException e) {
				throw new CommandGenerationException("Error trying to create instance of user-defined command generator "
                                                     + alternativeExecutive + ":",
                                                     e);
			}
            if (o instanceof CommandGenerator)
                return (CommandGenerator) o;
            else
                throw new CommandGenerationException("The class named by the ALT_EXECUTIVE system variable, "
                                                     + alternativeExecutive
                                                     + ", is not a subclass of gov.nasa.luv.CommandGenerator.");
        } else {
            switch (currentPlan.getAppType()) {
            case PLEXIL_EXEC:
                return new PlexilUniversalExecutiveCommandGenerator();

            case PLEXIL_TEST:
                return new PlexilTestExecutiveCommandGenerator();

            case PLEXIL_SIM:
                return new PlexilSimulatorCommandGenerator();

            case EXTERNAL_APP:
                return null;
            }
        }
        return null; // make f'n compiler happy
    }
      
    /** Creates the command to execute the Universal Executive.
     * 
     *  @return the command to execute or an error message if the command could not be created.
     */      
    private List<String> createCommand(CommandGenerator g, Plan p, Settings s)
        throws IOException, CommandGenerationException {
        AppType t = p.getAppType();
        if (t == EXTERNAL_APP || t == NO_APP)
            return null;

        if (!g.checkFiles(p, s)) // can throw, exception has reason
            throw new CommandGenerationException("Can't run exec. Check plan and Viewer settings.");

        return g.generateCommand(p, s); // can throw
    }
      
    /** Kills the currently running instance of the Universal Executive. */
    // Only caller is shutdown handler for Luv class.
    public void killUEProcess()
        throws IOException {   
        stop();
    }

    //
    // Helper classes
    //

    private class ExecRunner
        implements Runnable {

        private List<String> commandList;

        public ExecRunner(List<String> cmd) {
            commandList = cmd;
        }

        public void run() {
            ProcessBuilder builder = new ProcessBuilder(commandList);
            builder.redirectErrorStream(true); // funnel stderr to stdout
            // other environment setup goes here

            // Launch the process
            try {
                // Echo the command to console
                for (String token : commandList) {
                    System.out.print(token);
                    System.out.print(' ');
                }
                System.out.println();
                execProcess = builder.start();
            }
            catch (Exception e) {
                currentView.showErrorMessage(e, 
                                             "ERROR: unable to start PLEXIL Exec process");
                return;
            }

            // Monitor process output (may contain error messages from exec)
            BufferedReader is =
                new BufferedReader(new InputStreamReader(execProcess.getInputStream()));
            try {
                String line;
                while ((line = is.readLine()) != null) {
                    System.out.println(line);
                    if (line.startsWith("ERROR:")) {
                        currentView.showErrorMessage(null, "ERROR: error reported by the Executive: " + line);
                    }
                    else if (line.contains("null interface adapter for command")) { // see src/app-framework/InterfaceManager.cc
                        currentView.showErrorMessage(null,
                                                     "interface configuration error for "
                                                     + line.substring(line.indexOf("command"), line.length()));
                    }
                }
            }
            catch (Exception e) {
                currentView.showErrorMessage(e,
                                             "ERROR: error in monitoring PLEXIL Exec process.");
            }

            // If we get here, process is ending (has ended), or an error has interrupted monitoring
            boolean waitSucceeded = false;
            try {
                waitSucceeded = execProcess.waitFor(1, SECONDS);
            }
            catch (InterruptedException e) {
            }
            if (!waitSucceeded)
                stop();
            else if (execProcess.exitValue() != 0)
                currentView.executionFailed();
            else
                currentView.executionComplete();

            execProcess = null;
            currentPlan = null;
            currentView = null;
        }
    }

    private class PlexilUniversalExecutiveCommandGenerator
        extends CommandGeneratorBase
        implements CommandGenerator {

        public boolean checkFiles(Plan p, Settings s)
            throws CommandGenerationException {
            return checkPlanFile(p)
                && checkConfigFile(p);
        }

        public List<String> generateCommand(Plan p, Settings s) {
            Vector<String> command = new Vector<String>();
  
            System.out.println("Using Universal Executive...");

            command.add(RUN_UE_EXEC);
            addCommonOptions(command, p, s);
	  	  
            // add interface configuration
            command.add("-c");
            command.add(p.getConfigFile().toString());

            return command;
        }

        public void forceKillSubprocesses() {
            // TODO
        }
    }

    private class PlexilTestExecutiveCommandGenerator
        extends CommandGeneratorBase
        implements CommandGenerator {

        public boolean checkFiles(Plan p, Settings s)
            throws CommandGenerationException {
            return checkPlanFile(p)
                && checkScriptFile(p);
        }

        public List<String> generateCommand(Plan p, Settings s) {
            Vector<String> command = new Vector<String>();
		  
            System.out.println("Using Test Executive...");

            command.add(RUN_TEST_EXEC);
            addCommonOptions(command, p, s);

            command.add("-s");
            command.add(s.getScriptLocation().toString());

            return command;
        }	

        public void forceKillSubprocesses() {
            // TODO
        }
    }

    private class PlexilSimulatorCommandGenerator
        extends CommandGeneratorBase
        implements CommandGenerator {

        public boolean checkFiles(Plan p, Settings s)
            throws CommandGenerationException {
            return checkPlanFile(p)
                && checkConfigFile(p)
                && checkScriptFile(p);
        }

        public List<String> generateCommand(Plan p, Settings s) {
            Vector<String> command = new Vector<String>();	 
  
            System.out.println("Using PlexilSim...");

            //viewer
            command.add(RUN_SIMULATOR);
            addCommonOptions(command, p, s);

            // TODO: sim can have debug file too!

            // Interface configuration file
            command.add("-c");
            command.add(p.getConfigFile().toString());
	  	  
            // Sim script
            command.add("-s");
            command.add(p.getScriptFile().toString());

            return command;
        }

        public void forceKillSubprocesses() {
            // TODO
        }
    }
}
