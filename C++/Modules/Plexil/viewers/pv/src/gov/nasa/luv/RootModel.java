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

// *** TODO:
// - Resolve library load/update issues

package gov.nasa.luv;

import java.io.File;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.Set;
import java.util.Vector;

public class RootModel {
    private static List<Plan> plans =
        Collections.synchronizedList(new Vector<Plan>());
    private static Map<String, Plan> libraries =
        Collections.synchronizedMap(new HashMap<String, Plan>());
    private static Map<String, Vector<Plan> > libraryCallers =
        Collections.synchronizedMap(new HashMap<String, Vector<Plan> >());

    public static Plan getPlan(String name) {
        synchronized(plans) {
            for (Plan p : plans)
                if (p.getName().equals(name)) 
                    return p;
        }
        return null;
    }

    public static void removePlanNamed(String name) {
        Plan oldPlan = null;
        synchronized(plans) {
            for (int i = 0; i < plans.size(); ++i) {
                Plan p = plans.get(i);
                if (p.getName().equals(name)) {
                    oldPlan = p;
                    plans.remove(i);
                    break;
                }
            }
        }
        if (oldPlan != null && oldPlan.hasLibraryCalls())
            removeFromLibraryCallers(oldPlan);
    }

    public static void removePlan(Plan oldPlan) {
        boolean found = false;
        synchronized(plans) {
            for (int i = 0; i < plans.size(); ++i) {
                if (plans.get(i) == oldPlan) {
                    found = true;
                    plans.remove(i);
                    break;
                }
            }
        }
        if (found && oldPlan.hasLibraryCalls())
            removeFromLibraryCallers(oldPlan);
    }

    private static void removeFromLibraryCallers(Plan p) {
        Collection<String> libs = p.getLibraryNames();
        synchronized(libraryCallers) {
            for (String s : libs) {
                Vector<Plan> callers = libraryCallers.get(s);
                if (s != null)
                    callers.remove(p);
            }
        }
    }

    // TODO: Link libs if possible
    public static void addPlan(Plan plan) {
        final String name = plan.getName();
        removePlanNamed(name);

        synchronized(plans) {
            plans.add(plan);
        }

        // Notify library callers, if any
        // TODO: Determine whether new library or update
        synchronized(libraryCallers) {
            Vector<Plan> callers = libraryCallers.get(name);
            if (callers != null)
                for (Plan caller : callers) 
                    plan.libraryLoaded(name, plan);
        }
    }

    public static void addLibraryCaller(String libName, Plan p) {
        synchronized(libraryCallers) {
            Vector<Plan> callers = libraryCallers.get(libName);
            if (callers == null) {
                callers = new Vector<Plan>();
                libraryCallers.put(libName, callers);
            }
            callers.add(p);
        }
    }

    // Should only be used when a plan is deleted.
    public static void removeLibraryCaller(String libName, Plan p) {
        synchronized(libraryCallers) {
            Vector<Plan> callers = libraryCallers.get(libName);
            if (callers != null)
                callers.remove(p);
        }
    }

    public static Plan getLibrary(String name) {
        synchronized(libraries) {
            return libraries.get(name);
        }
    }

    public static void loadPlanLibraryFiles(Plan p) {
        Collection<File> libfiles = p.getLibraryFiles();
        if (libfiles == null || libfiles.isEmpty())
            return;

        for (File f : libfiles)
            tryLoadLibrary(f);
    }

    public static Plan findLibrary(String name, Plan referrer) {
        Plan result = getLibrary(name);
        if (result != null)
            return result; // trivial case

        String libFileName = name + ".plx";

        // Try working directory
        String dirname = System.getProperty("user.dir");
        if (null != dirname
            && null != (result = tryLoadLibrary(new File(dirname, libFileName))))
            return result;

        // Try plan's directory
        File planfile = referrer.getPlanFile();
        if (null != planfile
            && null != (result = tryLoadLibrary(new File(planfile.getParent(), libFileName))))
            return result;

        // Search specified path
        Collection<File> path = referrer.getLibraryPath();
        if (path == null)
            return null; // nothing to search

        for (File dir : path) {
            try {
                if (!dir.isDirectory() || !dir.canRead())
                    continue; // not a readable library
            }
            catch (SecurityException e) {
                continue; // don't have permission to find out
            }

            if (null != (result = tryLoadLibrary(new File(dir, libFileName))))
                return result;
        }

        return null;
    }

    // Helper fn for above
    private static Plan tryLoadLibrary(File f) {
        try {
            if (!f.isFile() || !f.canRead())
                return null; // not a readable file
        }
        catch (SecurityException e) {
            return null; // don't have permission to find out
        }

        Plan result = FileHandler.readPlan(f);
        if (result == null)
            return null;
        libraryLoaded(result);
        return result;
    }

    /**
     * Save the newly loaded library, overwriting any older versions.
     * @param nodeName Name of the library.
     * @param library The library node.
     */

    public static void libraryLoaded(Plan library) {
        String name = library.getName();
        synchronized(libraries) {
            libraries.put(name, library);
        }
        synchronized(plans) {
            for (Plan p : plans)
                p.libraryLoaded(name, library);
        }
    }

    // Used by Filehandler
    public static void removeLibrary(String name) {
        synchronized(libraries) {
            libraries.remove(name);
        }
    }

    public static Set<String> getLoadedLibraryNames() {
        synchronized(libraries) {
            return libraries.keySet();
        }
    }

}
