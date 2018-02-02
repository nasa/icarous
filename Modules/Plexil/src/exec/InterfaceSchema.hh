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

#include "ConstantMacros.hh"
#include <string>
#include <vector>

namespace PLEXIL
{

  /**
   * @brief This class provides utilities related to the generic interface XML schema.
   */

  class InterfaceSchema
  {
  public:

    //
    // XML tags
    //

    DECLARE_STATIC_CLASS_CONST(char*, ADAPTER_TAG, "Adapter");
    DECLARE_STATIC_CLASS_CONST(char*, COMMAND_NAMES_TAG, "CommandNames");
    DECLARE_STATIC_CLASS_CONST(char*, DEFAULT_ADAPTER_TAG, "DefaultAdapter");
    DECLARE_STATIC_CLASS_CONST(char*, DEFAULT_COMMAND_ADAPTER_TAG, "DefaultCommandAdapter");
    DECLARE_STATIC_CLASS_CONST(char*, DEFAULT_LOOKUP_ADAPTER_TAG, "DefaultLookupAdapter");
    DECLARE_STATIC_CLASS_CONST(char*, FILTER_TAG, "Filter");
    DECLARE_STATIC_CLASS_CONST(char*, INTERFACES_TAG, "Interfaces");
    DECLARE_STATIC_CLASS_CONST(char*, LIBRARY_NODE_PATH_TAG, "LibraryNodePath");
    DECLARE_STATIC_CLASS_CONST(char*, LISTENER_TAG, "Listener");
    DECLARE_STATIC_CLASS_CONST(char*, LOOKUP_NAMES_TAG, "LookupNames");
    DECLARE_STATIC_CLASS_CONST(char*, PLAN_PATH_TAG, "PlanPath");
    DECLARE_STATIC_CLASS_CONST(char*, PLANNER_UPDATE_TAG, "PlannerUpdate");
    DECLARE_STATIC_CLASS_CONST(char*, IP_ADDRESS_TAG, "IpAddress");
    DECLARE_STATIC_CLASS_CONST(char*, PORT_NUMBER_TAG, "PortNumber");

    //
    // Attributes
    //

    DECLARE_STATIC_CLASS_CONST(char*, ADAPTER_TYPE_ATTR, "AdapterType");
    DECLARE_STATIC_CLASS_CONST(char*, LIB_PATH_ATTR, "LibPath");
    DECLARE_STATIC_CLASS_CONST(char*, FILTER_TYPE_ATTR, "FilterType");
    DECLARE_STATIC_CLASS_CONST(char*, LISTENER_TYPE_ATTR, "ListenerType");
    DECLARE_STATIC_CLASS_CONST(char*, TELEMETRY_ONLY_ATTR, "TelemetryOnly");

    /**
     * Extract comma separated arguments from a character string.
     * @return pointer to vector of strings
     * @note Caller is responsible for disposing of the vector.
     */

    static std::vector<std::string> * parseCommaSeparatedArgs(const char * argString);

  };
}
