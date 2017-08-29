/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

#include "ExecApplication.hh"

#include "AdapterConfiguration.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExecListenerHub.hh"
#include "Expressions.hh"
#include "InterfaceAdapter.hh"
#include "InterfaceManager.hh"
#include "PlexilExec.hh"
#include "PlexilSchema.hh"
#include "pugixml.hpp"

#include <cstring>

namespace PLEXIL
{
  ExecApplication::ExecApplication()
    :
#ifdef PLEXIL_WITH_THREADS
      m_execThread(),
      m_execMutex(),
      m_stateMutex(),
      m_sem(),
      m_markSem(),
      m_shutdownSem(),
#endif
      m_nBlockedSignals(0),
      m_state(APP_UNINITED),
      m_threadLaunched(false),
      m_runExecInBkgndOnly(true),
      m_stop(false),
      m_suspended(false)
  {
    for (size_t i = 0; i <= EXEC_APPLICATION_MAX_N_SIGNALS; i++)
      m_blockedSignals[i] = 0;

    // connect exec and interface manager
    g_configuration = new AdapterConfiguration();
    g_exec = new PlexilExec();
    g_exec->setExecListener(g_configuration->getListenerHub());
    g_manager = new InterfaceManager(*this);
    g_interface = static_cast<ExternalInterface *>(g_manager);
  }

  ExecApplication::~ExecApplication()
  {
    delete g_configuration;
    g_interface = NULL;
    delete g_manager;
    delete g_exec;
  }

  /**
   * @brief Add the specified directory name to the end of the library node loading path.
   * @param libdir The directory name.
   */
  void ExecApplication::addLibraryPath(const std::string& libdir)
  {
    g_configuration->addLibraryPath(libdir);
  }

  /**
   * @brief Add the specified directory names to the end of the library node loading path.
   * @param libdirs The vector of directory names.
   */
  void ExecApplication::addLibraryPath(const std::vector<std::string>& libdirs)
  {
    g_configuration->addLibraryPath(libdirs);
  }

  /**
   * @brief Initialize all internal data structures and interfaces.
   * @param configXml Configuration data to use.
   * @return true if successful, false otherwise.
   * @note The caller must ensure that all adapter and listener factories
   *       have been created and registered before this call.
   */
  bool ExecApplication::initialize(pugi::xml_node const configXml)
  {
    condDebugMsg(configXml.empty(), "ExecApplication:initialize", " configuration is NULL");
    condDebugMsg(!configXml.empty(), "ExecApplication:initialize",
                 " configuration = " << configXml); // *** FIXME - PRINTS "1" ***

    if (m_state != APP_UNINITED) {
      debugMsg("ExecApplication:initialize",
               " application already initialized");
      return false;
    }

    // Perform one-time initializations

    // Load debug configuration from XML
    // *** NYI ***

    // Initialize Exec static data structures
    initializeExpressions();

    // Construct interfaces
    if (!g_configuration->constructInterfaces(configXml)) {
      debugMsg("ExecApplication:initialize",
	       " construction of interfaces failed");
      return false;
    }

    // Initialize them
    if (!g_manager->initialize()) {
      debugMsg("ExecApplication:initialize",
	       " initialization of interfaces failed");
      return false;
    }

    // Set the application state and return
    return setApplicationState(APP_INITED);
  }

  /**
   * @brief Start all the interfaces prior to execution.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::startInterfaces()
  {
    if (m_state != APP_INITED)
      return false;

    // Start 'em up!

    if (!g_manager->start()) {
      debugMsg("ExecApplication:startInterfaces",
               " failed to start interfaces");
      return false;
    }
      
    return setApplicationState(APP_READY);
  }


  /**
   * @brief Step the Exec once.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::step()
  {
    if (m_state != APP_READY)
      return false;

    {
#ifdef PLEXIL_WITH_THREADS
      RTMutexGuard guard(m_execMutex);
#endif
      g_manager->processQueue();           // for effect
      double now = g_manager->queryTime(); // update time before attempting to step
      if (g_exec->needsStep()) {
	g_exec->step(now);
        debugMsg("ExecApplication:step", " complete");
      }
      else {
        debugMsg("ExecApplication:step", " no step required");
      }
    }

    return true;
  }

  bool ExecApplication::isQuiescent()
  {
    if (m_state != APP_READY)
      return true; // can't execute if not ready

    {
#ifdef PLEXIL_WITH_THREADS
      RTMutexGuard guard(m_execMutex);
#endif
      return !g_exec->needsStep();
    }
  }

  /**
   * @brief Step the Exec until the queue is empty.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::stepUntilQuiescent()
  {
    if (m_state != APP_READY)
      return false;

    {
#ifdef PLEXIL_WITH_THREADS
      RTMutexGuard guard(m_execMutex);
#endif
      debugMsg("ExecApplication:stepUntilQuiescent", " Checking interface queue");
      g_manager->processQueue(); // for effect
      double now = g_manager->queryTime(); // update time before attempting to step
      while (g_exec->needsStep()) {
        debugMsg("ExecApplication:stepUntilQuiescent", " Stepping exec");
        g_exec->step(now);
	now = g_manager->queryTime(); // update time before attempting to step again
      }
      g_exec->deleteFinishedPlans();
    }
    debugMsg("ExecApplication:stepUntilQuiescent", " completed, queue empty and Exec quiescent.");

    return true;
  }


  /**
   * @brief Runs the initialized Exec.
   * @return true if successful, false otherwise.
   */

  bool ExecApplication::run()
  {
#ifdef PLEXIL_WITH_THREADS
    if (m_state != APP_READY)
      return false;

    // Clear suspended flag just in case
    m_suspended = false;

#ifndef BROKEN_ANDROID_PTHREAD_SIGMASK
    // Set up signal handling in main thread
    if (!initializeMainSignalHandling()) {
      warn("ExecApplication: failed to initialize main thread signal handling");
      return false;
    }
#endif // !BROKEN_ANDROID_PTHREAD_SIGMASK

    // Start the event listener thread
    return spawnExecThread();
#else // !defined(PLEXIL_WITH_THREADS)
    warn("ExecApplication: Can't run background thread; threads not enabled in the build");
    return false;
#endif // PLEXIL_WITH_THREADS
  }

  /**
   * @brief Suspends the running Exec.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::suspend()
  {
    if (m_state == APP_READY)
      return true; // already paused

    if (m_state != APP_RUNNING)
      return false;

    // Suspend the Exec 
    m_suspended = true;
    
    // *** NYI: wait here til current step completes ***
    return setApplicationState(APP_READY);
  }

  /**
   * @brief Resumes a suspended Exec.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::resume()
  {
    // Can only resume if ready and suspended
    if (m_state != APP_READY || !m_suspended)
      return false;

    // Resume the Exec
    m_suspended = false;
    notifyExec();
    
    return setApplicationState(APP_RUNNING);
  }

  /**
   * @brief Stops the Exec.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::stop()
  {
    if (m_state != APP_RUNNING
        && m_state != APP_READY)
      return false;

    // Stop interfaces
    g_manager->stop();

#ifdef PLEXIL_WITH_THREADS
    // Stop the Exec
    if (m_threadLaunched) {
      debugMsg("ExecApplication:stop", " Halting top level thread");
      m_stop = true;
      int status = m_sem.post();
      if (status) {
        warn("ExecApplication: semaphore post failed, status = " << status);
        return false;
      }
      sleep(1);

      if (m_stop) {
        // Exec thread failed to acknowledge stop - resort to stronger measures
        status = pthread_kill(m_execThread, SIGUSR2);
        if (status) {
          warn("ExecApplication: pthread_kill failed, status = " << status);
          return false;
        }
        sleep(1);
      }

      status = pthread_join(m_execThread, NULL);
      if (status) {
        debugMsg("ExecApplication:stop", 
                 " pthread_join() failed, error = " << status);
        return false;
      }
      debugMsg("ExecApplication:stop", " Top level thread halted");

#ifndef BROKEN_ANDROID_PTHREAD_SIGMASK
      if (!restoreMainSignalHandling()) {
        warn("ExecApplication: failed to restore signal handling for main thread");
        return false;
      }
#endif // !BROKEN_ANDROID_PTHREAD_SIGMASK
    }
#endif // PLEXIL_WITH_THREADS
    
    return setApplicationState(APP_STOPPED);
  }

   
  /**
   * @brief Resets a stopped Exec so that it can be run again.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::reset()
  {
    debugMsg("ExecApplication:reset", " entered");
    if (m_state != APP_STOPPED)
      return false;

    // Reset interfaces
    g_manager->reset();

    // Clear suspended flag
    m_suspended = false;

    // Reset the Exec
    // *** NYI ***
    
    debugMsg("ExecApplication:reset", " completed");
    return setApplicationState(APP_INITED);
  }


  /**
   * @brief Shuts down a stopped Exec.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::shutdown()
  {
    debugMsg("ExecApplication:shutdown", " entered");
    if (m_state != APP_STOPPED)
      return false;

    // Shut down the Exec
    // *** NYI ***

    // Shut down interfaces
    g_manager->shutdown();
    
    debugMsg("ExecApplication:shutdown", " completed");
    return setApplicationState(APP_SHUTDOWN);
  }

  /**
   * @brief Add a library as an XML document.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::addLibrary(pugi::xml_document* libraryXml)
  {
    if (m_state != APP_RUNNING && m_state != APP_READY)
      return false;

    // Delegate to InterfaceManager
    try {
      g_manager->handleAddLibrary(libraryXml);
    }
    catch (const ParserException& e) {
      std::cerr << "ExecApplication::addLibrary: Plan parser error:\n" << e.what() << std::endl;
      return false;
    }

    debugMsg("ExecApplication:addLibrary", " Library added");
    return true;
  }

  /**
   * @brief Load the named library from the library path.
   * @param name The name of the library.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::loadLibrary(std::string const &name)
  {
    if (m_state != APP_RUNNING && m_state != APP_READY)
      return false;

    bool result = false;

    // Delegate to InterfaceManager
    try {
      result = g_manager->handleLoadLibrary(name);
    }
    catch (const ParserException& e) {
      std::cerr << "ExecApplication::loadLibrary: Error:\n" << e.what() << std::endl;
      return false;
    }

    if (result) {
      debugMsg("ExecApplication:loadLibrary", " Library " << name << " loaded");
    }
    else {
      debugMsg("ExecApplication:loadLibrary", " Library " << name << " not found");
    }
    return result;
  }

  /**
   * @brief Add a plan as an XML document.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::addPlan(pugi::xml_document* planXml)
  {
    if (m_state != APP_RUNNING && m_state != APP_READY)
      return false;

    // Delegate to InterfaceManager
    try {
      g_manager->handleAddPlan(planXml->document_element());
      debugMsg("ExecApplication:addPlan", " successful");
      return true;
    }
    catch (const ParserException& e) {
      std::cerr << "ExecApplication::addPlan: Plan parser error: \n" << e.what() << std::endl;
      return false;
    }
  }

#ifdef PLEXIL_WITH_THREADS
  /**
   * @brief Spawns a thread which runs the exec's top level loop.
   * @return true if successful, false otherwise.
   */
  bool ExecApplication::spawnExecThread()
  {
    debugMsg("ExecApplication:run", " Spawning top level thread");
    int success = pthread_create(&m_execThread,
                                 NULL,
                                 execTopLevel,
                                 this);
    if (success != 0) {
      std::cerr << "Error: unable to spawn exec thread" << std::endl;
      return false;
    }
    m_threadLaunched = true;
    debugMsg("ExecApplication:run", " Top level thread running");
    return setApplicationState(APP_RUNNING);
  }

  void * ExecApplication::execTopLevel(void * this_as_void_ptr)
  {
    assertTrue_1(this_as_void_ptr != NULL); 
    (reinterpret_cast<ExecApplication*>(this_as_void_ptr))->runInternal();
    return 0;
  }

  void ExecApplication::runInternal()
  {
    debugMsg("ExecApplication:runInternal", " Thread started");

    // set up signal handling environment for this thread
    if (!initializeWorkerSignalHandling()) {
      warn("ExecApplication: Worker signal handling initialization failed.");
      return;
    }

    // must step exec once to initialize time
    runExec(true);
    debugMsg("ExecApplication:runInternal", " Initial step complete");

    while (waitForExternalEvent()) {
      if (m_stop) {
        debugMsg("ExecApplication:runInternal", " Received stop request");
        m_stop = false; // acknowledge stop request
        break;
      }
      runExec(false);
    }

    // restore old signal handlers for this thread
    // don't bother to check for errors
    restoreWorkerSignalHandling();

    debugMsg("ExecApplication:runInternal", " Ending the thread loop.");
  }
#endif // PLEXIL_WITH_THREADS

  /**
   * @brief Run the exec until the queue is empty.
   * @param stepFirst True if the exec should be stepped before checking the queue.
   * @note Acquires m_execMutex and holds until done.  
   */

  void
  ExecApplication::runExec(bool stepFirst)
  {
#ifdef PLEXIL_WITH_THREADS
    RTMutexGuard guard(m_execMutex);
#endif
    if (stepFirst) {
      debugMsg("ExecApplication:runExec", " Stepping exec because stepFirst is set");
      g_exec->step(g_manager->queryTime());
    }
    if (m_suspended) {
      debugMsg("ExecApplication:runExec", " Suspended");
    }
    else {
      g_manager->processQueue(); // for effect
      do {
        double now = g_manager->queryTime(); // update time before attempting to step
        while (g_exec->needsStep()) {
          debugMsg("ExecApplication:runExec", " Stepping exec");
          g_exec->step(now);
          now = g_manager->queryTime(); // update time before stepping again
        }
      } while (g_manager->processQueue());
      debugMsg("ExecApplication:runExec", " Queue empty and exec quiescent");
    }

    // Clean up
    g_exec->deleteFinishedPlans();

  }

#ifdef PLEXIL_WITH_THREADS
  /**
   * @brief Suspends the calling thread until another thread has
   *         placed a call to notifyExec().  Can return
   *        immediately if the call to wait() returns an error.
   * @return true if resumed normally, false if wait resulted in an error.
   * @note Can wait here indefinitely while the application is suspended.
   */
  bool ExecApplication::waitForExternalEvent()
  {
#ifndef BROKEN_ANDROID_PTHREAD_SIGMASK
    if (m_nBlockedSignals == 0) {
      warn("ExecApplication: signal handling not initialized.");
      return false;
    }
#endif // !BROKEN_ANDROID_PTHREAD_SIGMASK

    debugMsg("ExecApplication:wait", " waiting for external event");
    int status;
    do {
      status = m_sem.wait();
      if (status == 0) {
        condDebugMsg(!m_suspended, 
                     "ExecApplication:wait",
                     " acquired semaphore, processing external event");
        condDebugMsg(m_suspended, 
                     "ExecApplication:wait",
                     " Application is suspended, ignoring external event");
      }
    } 
    while (m_suspended);
    return (status == 0);
  }
#endif // PLEXIL_WITH_THREADS

  /**
   * @brief Suspend the current thread until the plan finishes executing.
   */
  void
  ExecApplication::waitForPlanFinished()
  {
#ifdef PLEXIL_WITH_THREADS
    // Should never happen, but just in case...
    assertTrueMsg(!m_execMutex.isLockedByCurrentThread(),
                  "Internal error: waitForPlanFinished: called with Exec mutex locked!");
    bool finished = false;
    while (!finished)
      {
        // sleep for a bit so as not to hog the CPU
        sleep(1);
    
        // grab the exec and find out if it's finished yet
        RTMutexGuard guard(m_execMutex);
        finished = g_exec->allPlansFinished();
      }
#else // !defined(PLEXIL_WITH_THREADS)
    warn("waitForPlanFinished: threads not enabled in build");
#endif // PLEXIL_WITH_THREADS
  }

  /**
   * @brief Suspend the current thread until the application reaches APP_SHUTDOWN state.
   * @note May be called by multiple threads
   * @note Wait can be interrupted by signal handling; calling threads should block (e.g.) SIGALRM.
   */
  void
  ExecApplication::waitForShutdown() {
#ifdef PLEXIL_WITH_THREADS
    int waitStatus = PLEXIL_SEMAPHORE_STATUS_INTERRUPTED;
    while ((waitStatus = m_shutdownSem.wait()) == PLEXIL_SEMAPHORE_STATUS_INTERRUPTED)
      continue;
    if (waitStatus == 0)
      m_shutdownSem.post(); // pass it on to the next, if any
#else // !defined(PLEXIL_WITH_THREADS)
    warn("waitForShutdown: threads not enabled in build");
#endif // PLEXIL_WITH_THREADS
  }

  /**
   * @brief Whatever state the application may be in, bring it down in a controlled fashion.
   */
  void ExecApplication::terminate() {
    std::cout << "Terminating PLEXIL Exec application" << std::endl;

    ApplicationState initState = getApplicationState();
    debugMsg("ExecApplication:terminate", " from state " << getApplicationStateName(initState));

    switch (initState) {
    case APP_UNINITED:
    case APP_SHUTDOWN:
      // nothing to do
      break;

    case APP_INITED:
    case APP_READY:
      // Shut down interfaces
      g_manager->shutdown();
      break;

    case APP_RUNNING:
      stop();
      // fall through to shutdown

    case APP_STOPPED:
      shutdown();
      break;
    }
    std::cout << "PLEXIL Exec terminated" << std::endl;
  }

  /**
   * @brief Get the application's current state.
   */
  ExecApplication::ApplicationState 
  ExecApplication::getApplicationState() {
#ifdef PLEXIL_WITH_THREADS
    ThreadMutexGuard guard(m_stateMutex);
#endif
    return m_state;
  }

  /**
   * @brief Transitions the application to the new state.
   * @return true if the new state is a legal transition from the current state, false if not.
   */ 
  bool 
  ExecApplication::setApplicationState(const ExecApplication::ApplicationState& newState) {
    debugMsg("ExecApplication:setApplicationState",
             "(" << getApplicationStateName(newState)
             << ") from " << getApplicationStateName(m_state));

    assertTrueMsg(newState != APP_UNINITED,
                  "APP_UNINITED is an invalid state for setApplicationState");

    // variable binding context for guard -- DO NOT DELETE THESE BRACES!
    {
#ifdef PLEXIL_WITH_THREADS
      ThreadMutexGuard guard(m_stateMutex);
#endif
      switch (newState) {
      case APP_INITED:
        if (m_state != APP_UNINITED && m_state != APP_STOPPED) {
          debugMsg("ExecApplication:setApplicationState",
                   " Illegal application state transition to APP_INITED");
          return false;
        }
        m_state = newState;
        break;

      case APP_READY:
        if (m_state != APP_INITED && m_state != APP_RUNNING) {
          debugMsg("ExecApplication:setApplicationState",
                   " Illegal application state transition to APP_READY");
          return false;
        }
        m_state = newState;
        break;

      case APP_RUNNING:
        if (m_state != APP_READY) {
          debugMsg("ExecApplication:setApplicationState", 
                   " Illegal application state transition to APP_RUNNING");
          return false;
        }
        m_state = newState;
        break;

      case APP_STOPPED:
        if (m_state != APP_RUNNING && m_state != APP_READY) {
          debugMsg("ExecApplication:setApplicationState", 
                   " Illegal application state transition to APP_STOPPED");
          return false;
        }
        m_state = newState;
        break;

      case APP_SHUTDOWN:
        if (m_state != APP_STOPPED) {
          debugMsg("ExecApplication:setApplicationState", 
                   " Illegal application state transition to APP_SHUTDOWN");
          return false;
        }
        m_state = newState;
        break;

      default:
        debugMsg("ExecApplication:setApplicationState",
                 " Attempt to set state to illegal value " << newState);
        break;

      }
      // end variable binding context for guard -- DO NOT DELETE THESE BRACES!
    }

    if (newState == APP_SHUTDOWN) {
#ifdef PLEXIL_WITH_THREADS
      // Notify any threads waiting for this state
      m_shutdownSem.post();
#endif
    }

    debugMsg("ExecApplication:setApplicationState",
             " to " << getApplicationStateName(newState) << " successful");
    return true;
  }

  //
  // Signal handling
  //

  /**
   * @brief Dummy signal handler function for signals we process.
   * @note This should never be called!
   */
  void dummySignalHandler(int /* signo */) {}

#ifdef PLEXIL_WITH_THREADS
  /**
   * @brief Handler for asynchronous kill of Exec thread
   * @param signo The signal.
   */
  void emergencyStop(int signo) 
  {
    debugMsg("ExecApplication:stop", " Received signal " << signo);
    pthread_exit((void *) 0);
  }

  // Prevent the Exec run thread from seeing the signals listed below.
  // Applications are free to deal with them in other ways.

  bool ExecApplication::initializeWorkerSignalHandling()
  {
    static int signumsToIgnore[EXEC_APPLICATION_MAX_N_SIGNALS] =
      {
        SIGINT,   // user interrupt (i.e. control-C)
        SIGHUP,   // hangup
        SIGQUIT,  // quit
        SIGTERM,  // kill
        SIGALRM,  // timer interrupt, used by (e.g.) Darwin time adapter
        SIGUSR1,  // user defined
        0,
        0
      };
    int errnum = 0;

#ifndef BROKEN_ANDROID_PTHREAD_SIGMASK
    //
    // Generate the mask
    //
    errnum = sigemptyset(&m_workerSigset);
    if (errnum != 0) {
      debugMsg("ExecApplication:initializeWorkerSignalHandling", " sigemptyset returned " << errnum);
      return false;
    }
    for (m_nBlockedSignals = 0;
         m_nBlockedSignals < EXEC_APPLICATION_MAX_N_SIGNALS && signumsToIgnore[m_nBlockedSignals] != 0;
         m_nBlockedSignals++) {
      int sig = signumsToIgnore[m_nBlockedSignals];
      m_blockedSignals[m_nBlockedSignals] = sig; // save to restore it later
      errnum = sigaddset(&m_workerSigset, sig);
      if (errnum != 0) {
        debugMsg("ExecApplication:initializeWorkerSignalHandling", " sigaddset returned " << errnum);
        return false;
      }
    }
    // Set the mask for this thread
    errnum = pthread_sigmask(SIG_BLOCK, &m_workerSigset, &m_restoreWorkerSigset);
    if (errnum != 0) {
      debugMsg("ExecApplication:initializeWorkerSignalHandling", " pthread_sigmask returned " << errnum);
      return false;
    }
#endif // !BROKEN_ANDROID_PTHREAD_SIGMASK

    // Add a handler for SIGUSR2 for killing the thread
    struct sigaction sa;
    sigemptyset(&sa.sa_mask); // *** is this enough?? ***
    sa.sa_flags = 0;
    sa.sa_handler = emergencyStop;

    errnum = sigaction(SIGUSR2, &sa, &m_restoreUSR2Handler);
    if (errnum != 0) {
      debugMsg("ExecApplication:initializeWorkerSignalHandling", " sigaction returned " << errnum);
      return errnum;
    }

    debugMsg("ExecApplication:initializeWorkerSignalHandling", " complete");
    return true;
  }

  bool ExecApplication::restoreWorkerSignalHandling()
  {
    //
    // Restore old SIGUSR2 handler
    // 
    int errnum = sigaction(SIGUSR2, &m_restoreUSR2Handler, NULL);
    if (errnum != 0) {
      debugMsg("ExecApplication:restoreWorkerSignalHandling", " sigaction returned " << errnum);
      return errnum;
    }

#ifndef BROKEN_ANDROID_PTHREAD_SIGMASK
    //
    // Restore old mask
    //
    errnum = pthread_sigmask(SIG_SETMASK, &m_restoreWorkerSigset, NULL);
    if (errnum != 0) { 
      debugMsg("ExecApplication:restoreWorkerSignalHandling", " failed; sigprocmask returned " << errnum);
      return false;
    }
#endif // !BROKEN_ANDROID_PTHREAD_SIGMASK

    // flag as complete
    m_nBlockedSignals = 0;

    debugMsg("ExecApplication:restoreWorkerSignalHandling", " complete");
    return true;
  }

  // Prevent the main thread from seeing the worker loop kill signal.

  bool ExecApplication::initializeMainSignalHandling()
  {
    //
    // Generate the mask
    //
    int errnum = sigemptyset(&m_mainSigset);
    if (errnum != 0) {
      debugMsg("ExecApplication:initializeMainSignalHandling", " sigemptyset returned " << errnum);
      return false;
    }
    errnum = sigaddset(&m_mainSigset, SIGUSR2);
    if (errnum != 0) {
      debugMsg("ExecApplication:initializeMainSignalHandling", " sigaddset returned " << errnum);
      return false;
    }

    // Set the mask for this thread
    errnum = pthread_sigmask(SIG_BLOCK, &m_mainSigset, &m_restoreMainSigset);
    if (errnum != 0) {
      debugMsg("ExecApplication:initializeMainSignalHandling", " pthread_sigmask returned " << errnum);
      return false;
    }

    debugMsg("ExecApplication:initializeMainSignalHandling", " complete");
    return true;
  }

  bool ExecApplication::restoreMainSignalHandling()
  {
    //
    // Restore old mask
    //
    int errnum = pthread_sigmask(SIG_SETMASK, &m_restoreMainSigset, NULL);
    if (errnum != 0) { 
      debugMsg("ExecApplication:restoreMainSignalHandling", " failed; pthread_sigmask returned " << errnum);
      return false;
    }

    debugMsg("ExecApplication:restoreMainSignalHandling", " complete");
    return true;
  }
#endif // PLEXIL_WITH_THREADS

  //
  // Static helper methods
  //

  /**
   * @brief Return a human-readable name for the ApplicationState.
   * @param state An ApplicationState.
   * @return The name of the state as a const char*.
   */

  const char* 
  ExecApplication::getApplicationStateName(ApplicationState state) {
    switch (state) {
    case APP_UNINITED:
      return "APP_UNINITED";
      break;

    case APP_INITED:
      return "APP_INITED";
      break;

    case APP_READY:
      return "APP_READY";
      break;

    case APP_RUNNING:
      return "APP_RUNNING";
      break;

    case APP_STOPPED:
      return "APP_STOPPED";
      break;

    case APP_SHUTDOWN:
      return "APP_SHUTDOWN";
      break;

    default:
      return "*** ILLEGAL APPLICATION STATE ***";
      break;
    }
  }

  /**
   * @brief Notify the executive that it should run one cycle.  
   *        This should be sent after each batch of lookup, command
   *        return, and function return data.
   */
  void
  ExecApplication::notifyExec()
  {
#ifdef PLEXIL_WITH_THREADS
    if (m_runExecInBkgndOnly || m_execMutex.isLocked()) {
      // Some thread currently owns the exec. Could be this thread.
      // runExec() could notice, or not.
      // Post to semaphore to ensure event is not lost.
      int status = m_sem.post();
      if (status) {
        warn("notifyExec: semaphore post failed, status = " << status);
      }
      else {
        debugMsg("ExecApplication:notify", " released semaphore");
      }
      return;
    }
    // Exec is idle, so run it
    // If another thread grabs it first, no worries.
    debugMsg("ExecApplication:notify", " exec was idle, stepping it");
    this->runExec();
#else
    // Don't do a thing - caller will tell us when to run
#endif
  }

  /**
   * @brief Run the exec and wait until all events in the queue have been processed. 
  */
  void
  ExecApplication::notifyAndWaitForCompletion()
  {
#ifdef PLEXIL_WITH_THREADS
    debugMsg("ExecApplication:notifyAndWait", " received external event");
    unsigned int sequence = g_manager->markQueue();
    notifyExec();
    while (g_manager->getLastMark() < sequence) {
      m_markSem.wait();
      m_markSem.post(); // in case it's not our mark and we got there first
    }
#else // !defined(PLEXIL_WITH_THREADS)
    warn("notifyAndWaitForCompletion: threads not enabled in build");
#endif // PLEXIL_WITH_THREADS
  }

  /**
   * @brief Notify the application that a queue mark was processed.
   */
  void
  ExecApplication::markProcessed()
  {
#ifdef PLEXIL_WITH_THREADS
    m_markSem.post();
#endif
  }

}
