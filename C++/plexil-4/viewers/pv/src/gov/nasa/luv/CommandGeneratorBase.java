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

import java.io.File;

import java.util.Collection;
import java.util.List;
import java.util.Set;

// Shared logic for classes implementing interface CommandGenerator

public class CommandGeneratorBase {
    protected File getPlanFile(Plan plan)
        throws CommandGenerationException {
        if (plan == null)
            throw new CommandGenerationException("Plan not specified");

        File p = plan.getPlanFile();
        if (p != null)
            return p;
        else
            throw new CommandGenerationException("Plan not specified");
    }

    public boolean checkPlanFile(Plan plan)
        throws CommandGenerationException {
        File p = getPlanFile(plan);
        if (!p.exists())
            throw new CommandGenerationException("Plan file " + p.toString() + " not found");
        if (!p.isFile())
            throw new CommandGenerationException("Plan file " + p.toString() + " is not a plain file");
        if (!p.canRead())
            throw new CommandGenerationException("Plan file " + p.toString() + " is not readable");
        return true;
    }

    protected boolean checkScriptFile(Plan p)
        throws CommandGenerationException {
        File scr = p.getScriptFile();
        if (scr == null)
            throw new CommandGenerationException("No script file specified");
        if (!scr.exists())
            throw new CommandGenerationException("Script file " + scr.toString() + " not found");
        if (!scr.isFile())
            throw new CommandGenerationException("Script file " + scr.toString() + " is not a plain file");
        if (!scr.canRead())
            throw new CommandGenerationException("Script file " + scr.toString() + " is not readable");
        return true;
    }

    protected boolean checkConfigFile(Plan p)
        throws CommandGenerationException {
        File c = p.getConfigFile();
        if (c == null)
            throw new CommandGenerationException("No config file specified");
        if (!c.exists())
            throw new CommandGenerationException("Config file " + c.toString() + " not found");
        if (!c.isFile())
            throw new CommandGenerationException("Config file " + c.toString() + " is not a plain file");
        if (!c.canRead())
            throw new CommandGenerationException("Config file " + c.toString() + " is not readable");
        return true;
    }

    protected boolean checkDebugFile(Plan p)
        throws CommandGenerationException {
        File c = p.getDebugFile();
        if (c == null)
            return true; // completely optional
        if (!c.exists())
            throw new CommandGenerationException("Debug file " + c.toString() + " not found");
        if (!c.isFile())
            throw new CommandGenerationException("Debug file " + c.toString() + " is not a plain file");
        if (!c.canRead())
            throw new CommandGenerationException("Debug file " + c.toString() + " is not readable");
        return true;
    }

    //* Adds exec options common to all PLEXIL executive types to the command.
    protected void addCommonOptions(List<String> command, Plan p, Settings s) {
        //automation to allow PID capture
        command.add("--for-viewer");

        // LuvListener options
        command.add("-v");
        //port
        command.add("-n");
        command.add(Integer.toString(s.getPort()));
        //breaks
        if (s.blocksExec())
            command.add("-b");

        //debug file
        command.add("-d");
        File debug = p.getDebugFile();
        if (debug != null) 
            command.add(debug.toString());
        else
            command.add("/dev/null"); // override default

        // get plan
        command.add("-p");
        command.add(s.getPlanLocation().toString());

        addLibraryPath(command, p);
        addLibraryFiles(command, p);
    }

    protected void addLibraryPath(List<String> command, Plan p) {
        Collection<File> path = p.getLibraryPath();
        if (path != null && !path.isEmpty())
            for (File ld : path) {
                command.add("-L");
                command.add(ld.toString());
            }
    }

    protected void addLibraryFiles(List<String> command, Plan p) {
        Collection<File> libs = p.getLibraryFiles();
        if (libs != null && !libs.isEmpty())
            for (File lf: p.getLibraryFiles()) {
                command.add("-l");
                command.add(lf.toString());
            }
    }

}
