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

#ifndef PLEXIL_ADAPTER_EXEC_INTERFACE_HH
#define PLEXIL_ADAPTER_EXEC_INTERFACE_HH

#include "CommandHandle.hh"
#include "ParserException.hh"

#include <vector>

// forward reference
namespace pugi
{
  class xml_node;
  class xml_document;
}

namespace PLEXIL
{
  // forward references
  class Command;
  class State;
  class Update;
  class Value;

  /**
   * @brief An abstract base class representing the InterfaceManager API
   *        from the interface adapter's point of view.
   * @note This class exists so that InterfaceAdapter and its derived classes
   *       need not be aware of the implementation details of InterfaceManager.
   */

  class AdapterExecInterface
  {
  public:

    /**
     * @brief Return the number of "macro steps" since this instance was constructed.
     * @return The macro step count.
     */
    virtual unsigned int getCycleCount() const = 0;

    /**
     * @brief Notify of the availability of a new value for a lookup.
     * @param state The state for the new value.
     * @param value The new value.
     */
    virtual void handleValueChange(State const &state, const Value& value) = 0;

    /**
     * @brief Notify of the availability of a command handle value for a command.
     * @param cmd Pointer to the Command instance.
     * @param value The new value.
     */
    virtual void handleCommandAck(Command * cmd, CommandHandleValue value) = 0;

    /**
     * @brief Notify of the availability of a return value for a command.
     * @param cmd Pointer to the Command instance.
     * @param value The new value.
     */
    virtual void handleCommandReturn(Command * cmd, Value const& value) = 0;

    /**
     * @brief Notify of the availability of a command abort acknowledgment.
     * @param cmd Pointer to the Command instance.
     * @param ack The acknowledgment value.
     */
    virtual void handleCommandAbortAck(Command * cmd, bool ack) = 0;

    /**
     * @brief Notify of the availability of a planner update acknowledgment.
     * @param upd Pointer to the Update instance.
     * @param ack The acknowledgment value.
     */
    virtual void handleUpdateAck(Update * upd, bool ack) = 0;

    /**
     * @brief Notify the executive of a new plan.
     * @param planXml The TinyXML representation of the new plan.
     */
    virtual void handleAddPlan(pugi::xml_node const planXml)
      throw (ParserException)
      = 0;

    /**
     * @brief Notify the executive of a new library node.
     * @param planXml The XML document containing the new library node
     */
    virtual void handleAddLibrary(pugi::xml_document *planXml)
      throw (ParserException)
      = 0;

    /**
     * @brief Notify the executive that it should run one cycle.  This should be sent after
     each batch of lookup and command return data.
    */
    virtual void notifyOfExternalEvent() = 0;

#ifdef PLEXIL_WITH_THREADS
    /**
     * @brief Run the exec and wait until all events in the queue have been processed.
     */
    virtual void notifyAndWaitForCompletion() = 0;
#endif

    /**
     * @brief Get the Exec's idea of the current time.
     * @return Seconds since the epoch as a double float.
     */
    virtual double currentTime() = 0;

    //
    // Property list API (formerly on InterfaceManagerBase)
    //

    /**
     * @brief Associate an arbitrary object with a string.
     * @param name The string naming the property.
     * @param thing The property value as an untyped pointer.
     */
    virtual void setProperty(const std::string& name, void * thing) = 0;

    /**
     * @brief Fetch the named property.
     * @param name The string naming the property.
     * @return The property value as an untyped pointer.
     */
    virtual void* getProperty(const std::string& name) = 0;

    //
    // Static utility functions
    //
    
    static std::string getText(const State& c);

  protected:

    /**
     * @brief Default constructor method.
     */
    AdapterExecInterface();

    /**
     * @brief Destructor method.
     */
    virtual ~AdapterExecInterface();

  private:

    // Deliberately unimplemented
    AdapterExecInterface(const AdapterExecInterface&);
    AdapterExecInterface& operator=(const AdapterExecInterface&);

  };

}

#endif // PLEXIL_ADAPTER_EXEC_INTERFACE_HH
