/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

#ifndef PLEXIL_INTERFACE_MANAGER_HH
#define PLEXIL_INTERFACE_MANAGER_HH

#include "ExternalInterface.hh"
#include "AdapterExecInterface.hh"

#include <map>

// Forward reference
namespace pugi
{
  class xml_node;
  class xml_document;
}

namespace PLEXIL 
{
  class AdapterConfiguration;

  // Forward references
  class ExecApplication;

  class InterfaceAdapter;

  class AdapterConfiguration;

  class InputQueue;

  /**
   * @brief A concrete derived class implementing the APIs of the
   *        ExternalInterface and AdapterExecInterface classes.
   * @details The InterfaceManager class is responsible for keeping track
   *          of all the external interfaces used by the PlexilExec.  It
   *          maintains a queue of messages for the Exec to process.  Interface
   *          instantiation, initialization, startup, stopping, shutdown, and
   *          deallocation are all handled by the InterfaceManager instance.
   * @note Supersedes the old ThreadedExternalInterface class.
   */
  class InterfaceManager :
    public ExternalInterface,
    public AdapterExecInterface
  {
  public:

    /**
     * @brief Constructor.
     * @param app The ExecApplication instance to which this object belongs.
     */
    InterfaceManager(ExecApplication & app);

    /**
     * @brief Destructor.
     */
    virtual ~InterfaceManager();

    /**
     * @brief Get parent application.
     */
    inline ExecApplication& getApplication() const
    {
      return m_application;
    }

    //
    // API for all related objects
    //

    
    /**
     * @brief Return the number of "macro steps" since this instance was constructed.
     * @return The macro step count.
     * @note Needed by the StateCacheEntry API.
     */
    unsigned int getCycleCount() const
    {
      return ExternalInterface::getCycleCount();
    }

    /**
     * @brief Associate an arbitrary object with a string.
     * @param name The string naming the property.
     * @param thing The property value as an untyped pointer.
     */
    virtual void setProperty(const std::string& name, void * thing);

    /**
     * @brief Fetch the named property.
     * @param name The string naming the property.
     * @return The property value as an untyped pointer.
     */
    virtual void* getProperty(const std::string& name);


    //
    // API for ExecApplication
    //

    /**
     * @brief Performs basic initialization of the interface and all adapters.
     * @return true if successful, false otherwise.
     */
    virtual bool initialize();

    /**
     * @brief Prepares the interface and adapters for execution.
     * @return true if successful, false otherwise.
     */
    virtual bool start();

    /**
     * @brief Halts all interfaces.
     * @return true if successful, false otherwise.
     */
    virtual bool stop();

    /**
     * @brief Resets the interface prior to restarting.
     * @return true if successful, false otherwise.
     */
    virtual bool reset();

    /**
     * @brief Shuts down the interface.
     * @return true if successful, false otherwise.
     */
    virtual bool shutdown();
    
    /**
     * @brief Updates the state cache from the items in the queue.
     * @return True if the exec needs to be stepped, false otherwise.
     * @note Should only be called with exec locked by the current thread.
     */
    bool processQueue();

    /**
     * @brief Insert a mark in the value queue.
     * @return The sequence number of the mark.
     */
    unsigned int markQueue();

    /**
     * @brief Get the sequence number of the most recently processed mark.
     * @return The sequence number, 0 if no marks have yet been processed.
     */
    unsigned int getLastMark() const
    {
      return m_lastMark;
    }

    //
    // API for exec
    //
    
    /**
     * @brief Delete any entries in the queue.
     */
    void resetQueue();

    /**
     * @brief Perform an immediate lookup on an existing state.
     * @param state The state.
     * @return The current value of the state or UNKNOWN().
     */
    void lookupNow(State const &state, StateCacheEntry &cacheEntry);

    /**
     * @brief Inform the interface that it should report changes in value of this state.
     * @param state The state.
     */
    void subscribe(const State& state);

    /**
     * @brief Inform the interface that a lookup should no longer receive updates.
     */
    void unsubscribe(const State& state);

    /**
     * @brief Advise the interface of the current thresholds to use when reporting this state.
     * @param state The state.
     * @param hi The upper threshold, at or above which to report changes.
     * @param lo The lower threshold, at or below which to report changes.
     */
    void setThresholds(const State& state, double hi, double lo);
    void setThresholds(const State& state, int32_t hi, int32_t lo);

    void executeCommand(Command *cmd);

    /**
     * @brief Report the failure in the appropriate way for the application.
     */
    void reportCommandArbitrationFailure(Command *cmd);

    /**
     * @brief Abort one command in execution.
     * @param cmd The command.
     */
    void invokeAbort(Command *cmd);

    void executeUpdate(Update *upd);

    // Use most recent cached value of time
    double currentTime();

    // Query interface and actually retrieve the current time
    double queryTime();

    //
    // API to interface adapters
    //

    /**
     * @brief Notify of the availability of a new value for a lookup.
     * @param state The state for the new value.
     * @param value The new value.
     */
    void handleValueChange(const State& state, const Value& value);

    void handleCommandReturn(Command * cmd, Value const& value);
    void handleCommandAck(Command * cmd, CommandHandleValue value);
    void handleCommandAbortAck(Command * cmd, bool ack);

    void handleUpdateAck(Update * upd, bool ack);

    /**
     * @brief Notify the executive of a new plan.
     * @param planXml The TinyXML representation of the new plan.
     */
    void handleAddPlan(pugi::xml_node const planXml)
      throw (ParserException);

    /**
     * @brief Notify the executive of a new library node.
     * @param planXml The XML document containing the new library node.
     */
    void handleAddLibrary(pugi::xml_document *planXml)
      throw (ParserException);

    /**
     * @brief Load the named library from the library path.
     * @param libname Name of the library node.
     * @return True if successful, false if not found.
     */
    bool handleLoadLibrary(std::string const &libName)
      throw (ParserException);

    /**
     * @brief Determine whether the named library is loaded.
     * @return True if loaded, false otherwise.
     */
    bool isLibraryLoaded(const std::string& libName) const;

    /**
     * @brief Notify the executive that it should run one cycle.
    */
    void notifyOfExternalEvent();

#ifdef PLEXIL_WITH_THREADS
    /**
     * @brief Run the exec and wait until all events in the queue have been processed.
     */
    void notifyAndWaitForCompletion();
#endif

  protected:

    //
    // Internal functionality
    //

    // rejects a command due to non-availability of resources
    void rejectCommand(Command *cmd);

    friend class AdapterConfiguration;

  private:

    // Deliberately unimplemented
    InterfaceManager();
    InterfaceManager(const InterfaceManager &);
    InterfaceManager & operator=(const InterfaceManager &);

    //
    // Internal types and classes
    //

    //
    // Private member variables
    //

    //* Parent object
    ExecApplication& m_application;

    // Properties
    typedef std::map<const std::string, void*> PropertyMap;
    PropertyMap m_propertyMap;

    //* The queue
    InputQueue *m_inputQueue;

    //* Holds the most recent idea of the current time
    double m_currentTime;

    //* Most recent mark processed.
    unsigned int m_lastMark;

    //* Last mark enqueued.
    unsigned int m_markCount;
  };

  extern InterfaceManager *g_manager;

}

#endif // PLEXIL_INTERFACE_MANAGER_HH
