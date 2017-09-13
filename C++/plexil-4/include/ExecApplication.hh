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

#ifndef EXEC_APPLICATION_H
#define EXEC_APPLICATION_H

#include <plexil-config.h>

#ifdef PLEXIL_WITH_THREADS
#include "ThreadMutex.hh"
#include "RecursiveThreadMutex.hh"
#include "ThreadSemaphore.hh"
#include <pthread.h>
#endif

// STL
#include <set>
#include <string>
#include <vector>

#include <csignal>

#define EXEC_APPLICATION_MAX_N_SIGNALS 8

// Forward references
namespace pugi
{
  class xml_document;
  class xml_node;
}

namespace PLEXIL
{

  // forward references
  class InterfaceAdapter;

  /**
   * @brief Provides a "pre-packaged" application skeleton for Universal Exec users.
   */
  class ExecApplication
  {
  public:
    enum ApplicationState
      {
        APP_UNINITED,
        APP_INITED,
        APP_READY,
        APP_RUNNING,
        APP_STOPPED,
        APP_SHUTDOWN
      };

    /**
     * @brief Default constructor.
     */
    ExecApplication();

    /**
     * @brief Destructor.
     */
    virtual ~ExecApplication();

    /**
     * @brief Add the specified directory name to the end of the library node loading path.
     * @param libdir The directory name.
     */
    void addLibraryPath(const std::string& libdir);

    /**
     * @brief Add the specified directory names to the end of the library node loading path.
     * @param libdirs The vector of directory names.
     */
    void addLibraryPath(const std::vector<std::string>& libdirs);

    /**
     * @brief Initialize all internal data structures and interfaces.
     * @param configXml Configuration data to use.
     * @return true if successful, false otherwise.
     * @note The caller must ensure that all adapter and listener factories
     *       have been created and registered before this call.
     */
    virtual bool initialize(pugi::xml_node const configXml);

    /**
     * @brief Start all the interfaces prior to execution.
     * @return true if successful, false otherwise.
     */
    virtual bool startInterfaces();

    /**
     * @brief Step the Exec once.
     * @return true if successful, false otherwise.
     * @note Can only be called in APP_READY state.
     * @note Can be called when application is suspended.
     * @note Acquires m_execMutex and holds until done.  
     */
    virtual bool step();

    /**
     * @brief Ask the Exec whether it is done transitioning.
     * @return true if all has settled, false if more transitions are possible.
     * @note Meant to be called after step(), to see if more work needs doing.
     * @note Acquires m_execMutex and holds until done.  
     */
    virtual bool isQuiescent();

    /**
     * @brief Step the Exec until the queue is empty.
     * @return true if successful, false otherwise.
     * @note Can only be called in APP_READY state.
     * @note Can be called when application is suspended.
     * @note Acquires m_execMutex and holds until done.  
     */
    virtual bool stepUntilQuiescent();

    /**
     * @brief Runs the initialized Exec.
     * @return true if successful, false otherwise.
     */
    virtual bool run();

    /**
     * @brief Suspends the running Exec.
     * @return true if successful, false otherwise.
     * @note Can only be suspended from APP_RUNNING state.
     */
    virtual bool suspend();

    /**
     * @brief Resumes a suspended Exec.
     * @return true if successful, false otherwise.
     * @note Can only resume from suspended state, i.e. 
     *   application state is APP_READY and isSuspended() is true.
     */
    virtual bool resume();

    /**
     * @brief Stops the Exec.
     * @return true if successful, false otherwise.
     */
    virtual bool stop();
   
    /**
     * @brief Resets a stopped Exec so that it can be run again.
     * @return true if successful, false otherwise.
     */
    virtual bool reset();

    /**
     * @brief Shuts down a stopped Exec.
     * @return true if successful, false otherwise.
     */
    virtual bool shutdown();

    /**
     * @brief Notify the executive that it should run one cycle.  This should be sent after
     each batch of lookup and command return data.
    */
    virtual void notifyExec();

    /**
     * @brief Notify the application that a queue mark was processed.
     */
    virtual void markProcessed();

    /**
     * @brief Add a library as an XML document.
     * @return true if successful, false otherwise.
     */
    virtual bool addLibrary(pugi::xml_document* libXml);

    /**
     * @brief Load the named library from the library path.
     * @param name The name of the library.
     * @return true if successful, false otherwise.
     */
    virtual bool loadLibrary(std::string const &name);

    /**
     * @brief Add a plan as an XML document.
     * @return true if successful, false otherwise.
     */
    virtual bool addPlan(pugi::xml_document* planXml);

    /**
     * @brief Notify the executive and wait for all queue entries to be processed.
     */
    virtual void notifyAndWaitForCompletion();

    /**
     * @brief Suspend the current thread until the plan finishes executing.
     * @note Acquires m_execMutex while checking exec status.
     */
    virtual void waitForPlanFinished();

    /**
     * @brief Suspend the current thread until the application reaches APP_SHUTDOWN state.
     * @note May be called by multiple threads
     * @note Wait can be interrupted by signal handling; calling threads should block (e.g.) SIGALRM.
     */
    virtual void waitForShutdown();

    /**
     * @brief Whatever state the application may be in, bring it down in a controlled fashion.
     */
    virtual void terminate();

    /**
     * @brief Select whether the exec runs opportunistically or only in background thread.
     * @param bkgndOnly True if background only, false if opportunistic.
     * @note Default is background only.
     */
    void setRunExecInBkgndOnly(bool bkgndOnly)
    { 
      m_runExecInBkgndOnly = bkgndOnly; 
    }

    /**
     * @brief Get the application's current state.
     */
    ApplicationState getApplicationState();

    /**
     * @brief Return a human-readable name for the ApplicationState.
     * @param state An ApplicationState.
     * @return The name of the state as a const char*.
     */
    static const char* getApplicationStateName(ApplicationState state);

    /**
     * @brief Query whether the Exec has been suspended. 
     * @return True if suspended, false otherwise.
     * @note Can only be suspended from APP_RUNNING.
     */
    bool isSuspended() const
    {
      return m_suspended;
    }

  protected:

    //
    // Exec top level
    //

    /**
     * @brief Run the exec until the queue is empty.
     * @param stepFirst True if the exec should be stepped before checking the queue.
     * @note Acquires m_execMutex and holds until done.  
     */
    void runExec(bool stepFirst = false);

#ifdef PLEXIL_WITH_THREADS
    /**
     * @brief Start the exec thread
     */
    bool spawnExecThread();

    /**
     * @brief Exec top level loop for use with pthread_create
     * @param this_as_void_ptr A pointer to the ExecApplication instance as a void *
     */
    static void * execTopLevel(void * this_as_void_ptr);

    /**
     * @brief Exec top level loop
     */
    void runInternal();

    /**
     * @brief Suspends the calling thread until another thread has
     *         placed a call to notifyOfExternalEvent().  Can return
     *         immediately if the calling thread is canceled.
     * @return true if resumed normally, false if thread was canceled.
     * @note Can wait here indefinitely while the application is suspended.
     */
    bool waitForExternalEvent();
#endif


    //
    // Common methods provided to subclasses
    //

    /**
     * @brief Transitions the application to the new state.
     * @return true if the new state is a legal transition from the current state, false if not.
     */ 
    bool setApplicationState(const ApplicationState& newState);

#ifdef PLEXIL_WITH_THREADS
    /**
     * @brief Establish signal handling environment for exec worker thread.
     * @return True if successful, false otherwise.
     */
    bool initializeWorkerSignalHandling();

    /**
     * @brief Restore previous signal handling environment for exec worker thread.
     * @return True if successful, false otherwise.
     */
    bool restoreWorkerSignalHandling();

    /**
     * @brief Establish signal handling environment for main thread.
     * @return True if successful, false otherwise.
     */
    bool initializeMainSignalHandling();

    /**
     * @brief Restore previous signal handling environment for main thread.
     * @return True if successful, false otherwise.
     */
    bool restoreMainSignalHandling();
#endif

  private:

    //
    // Deliberately unimplemented
    //
    ExecApplication(const ExecApplication&);
    ExecApplication& operator=(const ExecApplication&);
    
    //
    // Member variables
    //

#ifdef PLEXIL_WITH_THREADS
    //
    // Synchronization and mutual exclusion
    //

    // Thread in which the Exec runs
    pthread_t m_execThread;

    // Serialize execution in exec to guarantee in-order processing of events
    RecursiveThreadMutex m_execMutex;

    // Mutex for application state
    ThreadMutex m_stateMutex;

    // Semaphore for notifying the Exec of external events
    ThreadSemaphore m_sem;

    // Semaphore for notifyAndWaitForCompletion()
    ThreadSemaphore m_markSem;

    // Semaphore for notifying external threads that the application is shut down
    ThreadSemaphore m_shutdownSem;
#endif 

    //
    // Signal handling
    //
    sigset_t m_workerSigset;
    sigset_t m_restoreWorkerSigset;
    sigset_t m_mainSigset;
    sigset_t m_restoreMainSigset;
    struct sigaction m_restoreUSR2Handler;
    size_t m_nBlockedSignals;
    int m_blockedSignals[EXEC_APPLICATION_MAX_N_SIGNALS + 1];

    // Current state of the application
    ApplicationState m_state;

    // True if exec is running in a separate thread
    bool m_threadLaunched;

    // Flag to determine whether exec should run conservatively
    bool m_runExecInBkgndOnly;

    // Flag for halting the Exec thread
    bool m_stop;

    // Flag for suspend/resume
    bool m_suspended;

  };

}

#endif // EXEC_APPLICATION_H
