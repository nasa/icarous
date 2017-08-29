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

#ifndef PLEXIL_EXTERNAL_INTERFACE_HH
#define PLEXIL_EXTERNAL_INTERFACE_HH

#include "CommandHandle.hh"
#include "Expression.hh"
#include "LinkedQueue.hh"
#include "State.hh"

namespace PLEXIL {
  // Forward declarations
  class Command;
  class StateCacheEntry;
  class ResourceArbiterInterface;
  class Update;

  /**
   * @class ExternalInterface
   * @brief Abstract base class for anything that interfaces the Exec to the outside world.
   */

  class ExternalInterface {
  public:

    virtual ~ExternalInterface();

    //
    // API to Lookup and StateCacheEntry
    //

    //
    // The cycle counter is used by the Lookup interface to check whether a value is stale.
    // It is incremented by the PlexilExec.
    //

    /**
     * @brief Return the number of "macro steps" since this instance was constructed.
     * @return The macro step count.
     */
    unsigned int getCycleCount() const;

    /**
     * @brief Perform an immediate lookup on an existing state.
     * @param state The state.
     * @param cacheEntry The entry in the state cache.
     * @note Value is returned via callback on StateCacheEntry.
     */
    virtual void lookupNow(State const &state, StateCacheEntry &cacheEntry) = 0;

    /**
     * @brief Inform the interface that it should report changes in value of this state.
     * @param state The state.
     */
    virtual void subscribe(const State& state) = 0;

    /**
     * @brief Inform the interface that a lookup should no longer receive updates.
     */
    virtual void unsubscribe(const State& state) = 0;

    /**
     * @brief Advise the interface of the current thresholds to use when reporting this state.
     * @param state The state.
     * @param hi The upper threshold, at or above which to report changes.
     * @param lo The lower threshold, at or below which to report changes.
     */
    virtual void setThresholds(const State& state, double hi, double lo) = 0;
    virtual void setThresholds(const State& state, int32_t hi, int32_t lo) = 0;

    //
    // API to Node classes
    //

    // Made virtual for convenience of module tests

    /**
     * @brief Schedule this command for execution.
     */
    virtual void enqueueCommand(Command *cmd);

    /**
     * @brief Abort the pending command.
     */
    virtual void abortCommand(Command *cmd);

    /**
     * @brief Schedule this update for execution.
     */
    virtual void enqueueUpdate(Update *update);

    //
    // API to Command
    //
    
    /**
     * @brief Release resources in use by the command.
     */
    void releaseResourcesForCommand(Command *cmd);

    //
    // API to Exec
    //

    /**
     * @brief Send all pending commands and updates to the external system(s).
     */
    virtual void executeOutboundQueue();

    /**
     * @brief See if the command and update queues are empty.
     * @return True if both empty, false otherwise.
     */
    bool outboundQueueEmpty() const;

    // Returns the current time.
    // FIXME - use real time type
    virtual double currentTime() = 0;

    /**
     * @brief Increment the macro step count and return the new value.
     * @return The updated macro step count.
     */
    unsigned int incrementCycleCount();

    //
    // Interface from outside world to plan state
    //

    /**
     * @brief Return a value from a lookup.
     * @param state Const reference to the state.
     * @param value Const reference to the value.
     */
    void lookupReturn(State const &state, Value const &value);

    /**
     * @brief Return a value from a command
     * @param cmd Pointer to the Command.
     * @param value Const reference to the value.
     */
    void commandReturn(Command *cmd, Value const &value);

    /**
     * @brief Return a command handle value for a command.
     * @param cmd Pointer to the Command.
     * @param value The command handle value.
     */
    void commandHandleReturn(Command *cmd, CommandHandleValue val);

    /**
     * @brief Return an abort-acknowledge value for a command.
     * @param cmd Pointer to the Command.
     * @param ack The acknowledgement value.
     */
    void commandAbortAcknowledge(Command *cmd, bool ack);

    /**
     * @brief Return an update acknowledgment value
     * @param upd Update ID reference.
     * @param value The ack value.
     */
    void acknowledgeUpdate(Update *upd, bool val);

    //
    // API to application
    //
    
    /**
     * @brief Read command resource hierarchy from the named file.
     * @param fname File name.
     * @return True if successful, false otherwise.
     */

    bool readResourceFile(std::string const &fname);

  protected:

    // Default constructor.
    ExternalInterface();

    //
    // Member functions that are expected to be implemented by derived classes
    //

    /**
     * @brief Report the failure in the appropriate way for the application.
     */
    virtual void reportCommandArbitrationFailure(Command *cmd) = 0;

    /**
     * @brief Schedule this command for execution.
     */
    virtual void executeCommand(Command *cmd) = 0;

    /**
     * @brief Abort the pending command.
     * @param cmd The command.
     */
    virtual void invokeAbort(Command *cmd) = 0;

    /**
     * @brief Schedule this update for execution.
     */
    virtual void executeUpdate(Update *update) = 0;

  private:

    // Copy, assign disallowed
    ExternalInterface(ExternalInterface const &);
    ExternalInterface &operator=(ExternalInterface const &);

    LinkedQueue<Update> m_updatesToExecute;
    LinkedQueue<Command> m_commandsToExecute;
    ResourceArbiterInterface *m_raInterface;
    unsigned int m_cycleCount;
  };

  extern ExternalInterface *g_interface;
}

#endif
