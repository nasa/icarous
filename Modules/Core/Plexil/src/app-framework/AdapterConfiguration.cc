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

#include "AdapterConfiguration.hh"

#include "AdapterFactory.hh"
#include "Debug.hh"
#include "DummyAdapter.hh"
#include "Error.hh"
#include "ExecListenerFactory.hh"
#include "ExecListenerFilterFactory.hh"
#include "ExecListenerHub.hh"
#include "InterfaceManager.hh"
#include "InterfaceSchema.hh"
#include "ListenerFilters.hh"
#include "planLibrary.hh"
#ifdef PLEXIL_WITH_THREADS
#include "SerializedInputQueue.hh"
#else
#include "SimpleInputQueue.hh"
#endif
#include "UtilityAdapter.hh"

#if HAVE_LUV_LISTENER
#include "LuvListener.hh"
#endif

#if HAVE_DEBUG_LISTENER
#include "PlanDebugListener.hh"
#endif

#ifdef PLEXIL_WITH_UNIX_TIME 
#include "TimeAdapter.hh"
#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS - 200112L) >= 0L || defined(PLEXIL_ANDROID))
#include "PosixTimeAdapter.hh"
#elif defined(HAVE_SETITIMER)
#include "DarwinTimeAdapter.hh"
//#else
//#error "No time adapter implementation class for this environment"
#endif
#endif

#include <cstring>

namespace PLEXIL {

  AdapterConfiguration::AdapterConfiguration() :
    m_defaultInterface(),
    m_defaultCommandInterface(),
    m_defaultLookupInterface(),
    m_plannerUpdateInterface(),
    m_listenerHub(new ExecListenerHub())
  {
    // Every application has access to the dummy and utility adapters
    REGISTER_ADAPTER(DummyAdapter, "Dummy");
    REGISTER_ADAPTER(UtilityAdapter, "Utility");

#ifdef PLEXIL_WITH_UNIX_TIME
    // Every application has access to the OS-native time adapter
    REGISTER_ADAPTER(TIME_ADAPTER_CLASS, "OSNativeTime");
#endif

    registerExecListenerFilters();

#if HAVE_DEBUG_LISTENER
      // Every application should have access to the Plan Debug Listener
      REGISTER_EXEC_LISTENER(PlanDebugListener, "PlanDebugListener");
#endif

#if HAVE_LUV_LISTENER
    // Every application should have access to the Plexil Viewer (formerly LUV) Listener
    REGISTER_EXEC_LISTENER(LuvListener, "LuvListener");
#endif
  }

  AdapterConfiguration::~AdapterConfiguration()
  {
    clearAdapterRegistry();

    // unregister and delete adapters
    // *** kludge for buggy std::set template ***
    std::set<InterfaceAdapter *>::iterator it = m_adapters.begin();
    while (it != m_adapters.end()) {
      InterfaceAdapter *ia = *it;
      m_adapters.erase(it); // these two lines should be:
      it = m_adapters.begin(); // it = m_adapters.erase(it)
      delete ia;
    }
  }

  /**
   * @brief Constructs interface adapters from the provided XML.
   * @param configXml The XML element used for interface configuration.
   * @return true if successful, false otherwise.
   */
  bool AdapterConfiguration::constructInterfaces(pugi::xml_node const configXml)
  {
    if (configXml.empty()) {
      debugMsg("AdapterConfiguration:constructInterfaces",
               " empty configuration, nothing to construct");
      return true;
    }

    debugMsg("AdapterConfiguration:verboseConstructInterfaces", " parsing configuration XML");
    const char* elementType = configXml.name();
    if (strcmp(elementType, InterfaceSchema::INTERFACES_TAG()) != 0) {
      debugMsg("AdapterConfiguration:constructInterfaces",
               " invalid configuration XML: no " << InterfaceSchema::INTERFACES_TAG() << " element");
      return false;
    }

    // Walk the children of the configuration XML element
    // and register the adapter according to the data found there
    pugi::xml_node element = configXml.first_child();
    while (!element.empty()) {
      debugMsg("AdapterConfiguration:verboseConstructInterfaces", " found element " << element.name());
      const char* elementType = element.name();
      if (strcmp(elementType, InterfaceSchema::ADAPTER_TAG()) == 0) {
        // Construct the adapter
        debugMsg("AdapterConfiguration:constructInterfaces",
                 " constructing adapter type \""
                 << element.attribute(InterfaceSchema::ADAPTER_TYPE_ATTR()).value()
                 << "\"");
        InterfaceAdapter *adapter = 
          AdapterFactory::createInstance(element,
                                         *static_cast<AdapterExecInterface *>(g_manager));
        if (!adapter) {
          warn("constructInterfaces: failed to construct adapter type \""
               << element.attribute(InterfaceSchema::ADAPTER_TYPE_ATTR()).value()
               << "\"");
          return false;
        }
        m_adapters.insert(adapter);
      }
      else if (strcmp(elementType, InterfaceSchema::LISTENER_TAG()) == 0) {
        // Construct an ExecListener instance and attach it to the Exec
        debugMsg("AdapterConfiguration:constructInterfaces",
                 " constructing listener type \""
                 << element.attribute(InterfaceSchema::LISTENER_TYPE_ATTR()).value()
                 << '"');
        ExecListener *listener = 
          ExecListenerFactory::createInstance(element);
        if (!listener) {
          warn("constructInterfaces: failed to construct listener type \""
               << element.attribute(InterfaceSchema::LISTENER_TYPE_ATTR()).value()
               << '"');
          return false;
        }
        m_listenerHub->addListener(listener);
      }
      else if (strcmp(elementType, InterfaceSchema::LIBRARY_NODE_PATH_TAG()) == 0) {
        // Add to library path
        const char* pathstring = element.child_value();
        if (*pathstring != '\0') {
          std::vector<std::string> * path = InterfaceSchema::parseCommaSeparatedArgs(pathstring);
          for (std::vector<std::string>::const_iterator it = path->begin();
               it != path->end();
               ++it)
            appendLibraryPath(*it);
          delete path;
        }
      }
      else if (strcmp(elementType, InterfaceSchema::PLAN_PATH_TAG()) == 0) {
        // Add to plan path
        const char* pathstring = element.child_value();
        if (*pathstring != '\0') {
          std::vector<std::string> * path = InterfaceSchema::parseCommaSeparatedArgs(pathstring);
          for (std::vector<std::string>::const_iterator it = path->begin();
               it != path->end();
               ++it)
            m_planPath.push_back(*it);
          delete path;
        }
      }
      else {
        debugMsg("AdapterConfiguration:constructInterfaces",
                 " ignoring unrecognized XML element \""
                 << elementType << "\"");
      }

      element = element.next_sibling();
    }

    debugMsg("AdapterConfiguration:verboseConstructInterfaces", " done.");
    return true;
  }

  /**
   * @brief Performs basic initialization of the interface and all adapters.
   * @return true if successful, false otherwise.
   */
  bool AdapterConfiguration::initialize()
  {
    debugMsg("AdapterConfiguration:initialize", " initializing interface adapters");
    bool success = true;
    for (std::set<InterfaceAdapter *>::iterator it = m_adapters.begin();
         success && it != m_adapters.end();
         ++it) {
      InterfaceAdapter *a = *it;
      success = a->initialize();
      if (!success) {
        warn("initialize: failed for adapter type \""
		 << a->getXml().attribute(InterfaceSchema::ADAPTER_TYPE_ATTR()).value()
		 << '"');
        m_adapters.erase(it);
        delete a;
        return false;
      }
    }
    success = m_listenerHub->initialize();
    if (!success) {
      warn("initialize: failed to initialize Exec listener(s)");
      return false;
    }

    return success;
  }

  /**
   * @brief Prepares the interface and adapters for execution.
   * @return true if successful, false otherwise.
   */
  bool AdapterConfiguration::start()
  {
    debugMsg("AdapterConfiguration:start", " starting interface adapters");
    bool success = true;
    for (std::set<InterfaceAdapter *>::iterator it = m_adapters.begin();
         success && it != m_adapters.end();
         ++it) {
      success = (*it)->start();
      if (!success) {
        warn("start: start failed for adapter type \""
             << (*it)->getXml().attribute(InterfaceSchema::ADAPTER_TYPE_ATTR()).value()
             << '"');
        return false;
      }
    }

    success = m_listenerHub->start();
    if (!success) {
      warn("start: failed to start Exec listener(s)");
    }
    return success;
  }

  /**
   * @brief Halts all interfaces.
   * @return true if successful, false otherwise.
   */
  bool AdapterConfiguration::stop()
  {
    debugMsg("AdapterConfiguration:stop", " entered");

    // halt adapters
    bool success = true;
    for (std::set<InterfaceAdapter *>::iterator it = m_adapters.begin();
         it != m_adapters.end();
         ++it)
      success = (*it)->stop() && success;

    success = m_listenerHub->stop() && success;

    debugMsg("AdapterConfiguration:stop", " completed");
    return success;
  }

  /**
   * @brief Resets the interface prior to restarting.
   * @return true if successful, false otherwise.
   */
  bool AdapterConfiguration::reset()
  {
    debugMsg("AdapterConfiguration:reset", " entered");

    clearAdapterRegistry();
    bool success = true;
    for (std::set<InterfaceAdapter *>::iterator it = m_adapters.begin();
         it != m_adapters.end();
         ++it)
      success = (*it)->reset() && success;

    success = m_listenerHub->reset() && success;
    debugMsg("AdapterConfiguration:reset", " completed");
    return success;
  }

  /**
   * @brief Shuts down the interface.
   * @return true if successful, false otherwise.
   */
  bool AdapterConfiguration::shutdown()
  {
    debugMsg("AdapterConfiguration:shutdown", " entered");
    clearAdapterRegistry();

    bool success = true;
    for (std::set<InterfaceAdapter *>::iterator it = m_adapters.begin();
         it != m_adapters.end();
         ++it)
      success = (*it)->shutdown() && success;
    success = m_listenerHub->shutdown() && success;

    // Clean up
    // *** NYI ***

    debugMsg("AdapterConfiguration:shutdown", " completed");
    return success;
  }

  /**
   * @brief Add an externally constructed interface adapter.
   * @param The adapter ID.
   */
  void AdapterConfiguration::addInterfaceAdapter(InterfaceAdapter *adapter)
  {
    if (m_adapters.find(adapter) == m_adapters.end())
      m_adapters.insert(adapter);
  }

  /**
   * @brief Add an externally constructed ExecListener.
   * @param listener The ExecListener ID.
   */
  void AdapterConfiguration::addExecListener(PlexilListener *listener)
  {
    m_listenerHub->addListener(listener);
  }

  /**
   * @brief Construct the input queue specified by the configuration data.
   * @return Pointer to instance of a class derived from InputQueue.
   */
  // TODO: actually get type from input data
  InputQueue *AdapterConfiguration::getInputQueue() const
  {
    return 
#ifdef PLEXIL_WITH_THREADS
      new SerializedInputQueue();
#else
      new SimpleInputQueue();
#endif
  }

  /**
   * @brief Get the search path for library nodes.
   * @return A reference to the library search path.
   */
  const std::vector<std::string>& AdapterConfiguration::getLibraryPath() const
  {
    return getLibraryPaths();
  }

  /**
   * @brief Get the search path for plans.
   * @return A reference to the plan search path.
   */
  const std::vector<std::string>& AdapterConfiguration::getPlanPath() const
  {
    return m_planPath;
  }

  /**
   * @brief Add the specified directory name to the end of the library node loading path.
   * @param libdir The directory name.
   */
  void AdapterConfiguration::addLibraryPath(const std::string &libdir)
  {
    appendLibraryPath(libdir);
  }

  /**
   * @brief Add the specified directory names to the end of the library node loading path.
   * @param libdirs The vector of directory names.
   */
  void AdapterConfiguration::addLibraryPath(const std::vector<std::string>& libdirs)
  {
    for (std::vector<std::string>::const_iterator it = libdirs.begin();
         it != libdirs.end();
         ++it)
      appendLibraryPath(*it);
  }

  /**
   * @brief Add the specified directory name to the end of the plan loading path.
   * @param libdir The directory name.
   */
  void AdapterConfiguration::addPlanPath(const std::string &libdir)
  {
    m_planPath.push_back(libdir);
  }

  /**
   * @brief Add the specified directory names to the end of the plan loading path.
   * @param libdirs The vector of directory names.
   */
  void AdapterConfiguration::addPlanPath(const std::vector<std::string>& libdirs)
  {
    for (std::vector<std::string>::const_iterator it = libdirs.begin();
         it != libdirs.end();
         ++it) {
      m_planPath.push_back(*it);
    }
  }

  /**
   * @brief Register the given interface adapter as the default.
   * @param adapter The interface adapter.
   */

  void AdapterConfiguration::defaultRegisterAdapter(InterfaceAdapter *adapter) 
  {
    debugMsg("AdapterConfiguration:defaultRegisterAdapter", " for adapter " << adapter);
    // Walk the children of the configuration XML element
    // and register the adapter according to the data found there
    pugi::xml_node element = adapter->getXml().first_child();
    while (!element.empty()) {
      const char* elementType = element.name();
      if (strcmp(elementType, InterfaceSchema::DEFAULT_ADAPTER_TAG()) == 0) {
        setDefaultInterface(adapter);
      } 
      else if (strcmp(elementType, InterfaceSchema::DEFAULT_COMMAND_ADAPTER_TAG()) == 0) {
        setDefaultCommandInterface(adapter);
      }
      else if (strcmp(elementType, InterfaceSchema::DEFAULT_LOOKUP_ADAPTER_TAG()) == 0) {
        setDefaultLookupInterface(adapter);
      }
      else if (strcmp(elementType, InterfaceSchema::PLANNER_UPDATE_TAG()) == 0) {
        registerPlannerUpdateInterface(adapter);
      }
      else if (strcmp(elementType, InterfaceSchema::COMMAND_NAMES_TAG()) == 0) {
        const pugi::xml_node firstChild = element.first_child();
        const char* text = NULL;
        if (!firstChild.empty() && firstChild.type() == pugi::node_pcdata)
          text = firstChild.value();
        checkError(text && *text != '\0',
                   "registerAdapter: Invalid configuration XML: "
                   << InterfaceSchema::COMMAND_NAMES_TAG()
                   << " requires one or more comma-separated command names");
        std::vector<std::string> * cmdNames = InterfaceSchema::parseCommaSeparatedArgs(text);
        for (std::vector<std::string>::const_iterator it = cmdNames->begin(); it != cmdNames->end(); ++it)
          registerCommandInterface(*it, adapter);
        delete cmdNames;
      } 
      else if (strcmp(elementType, InterfaceSchema::LOOKUP_NAMES_TAG()) == 0) {
        const pugi::xml_node firstChild = element.first_child();
        const char* text = NULL;
        if (!firstChild.empty() && firstChild.type() == pugi::node_pcdata)
          text = firstChild.value();
        checkError(text && *text != '\0',
                   "registerAdapter: Invalid configuration XML: "
                   << InterfaceSchema::LOOKUP_NAMES_TAG()
                   << " requires one or more comma-separated lookup names");
        std::vector<std::string> * lookupNames = InterfaceSchema::parseCommaSeparatedArgs(text);
        bool telemOnly = element.attribute(InterfaceSchema::TELEMETRY_ONLY_ATTR()).as_bool();
        for (std::vector<std::string>::const_iterator it = lookupNames->begin(); it != lookupNames->end(); ++it)
          registerLookupInterface(*it, adapter, telemOnly);
        delete lookupNames;
      }
      // ignore other tags, they're for adapter's use

      element = element.next_sibling();
    }
  }

  /**
   * @brief Register the given interface adapter for this command.
   Returns true if successful.  Fails and returns false
   iff the command name already has an adapter registered
            or setting a command interface is not implemented.
   * @param commandName The command to map to this adapter.
   * @param intf The interface adapter to handle this command.
   */
  bool AdapterConfiguration::registerCommandInterface(std::string const &commandName,
                                                      InterfaceAdapter *intf) {
    InterfaceMap::iterator it = m_commandMap.find(commandName);
    if (it == m_commandMap.end()) {
      // Not found, OK to add
      debugMsg("AdapterConfiguration:registerCommandInterface",
               " registering interface " << intf << " for command '" << commandName << "'");
      m_commandMap.insert(std::pair<std::string, InterfaceAdapter *>(commandName, intf));
      m_adapters.insert(intf);
      return true;
    } else {
      debugMsg("AdapterConfiguration:registerCommandInterface",
               " interface already registered for command '" << commandName << "'");
      return false;
    }
  }

  /**
   * @brief Register the given interface adapter for lookups to this state.
   Returns true if successful.  Fails and returns false
   if the state name already has an adapter registered
            or registering a lookup interface is not implemented.
   * @param stateName The name of the state to map to this adapter.
   * @param intf The interface adapter to handle this lookup.
   * @param telemetryOnly False if this interface implements LookupNow, true otherwise.
   */
  bool AdapterConfiguration::registerLookupInterface(std::string const &stateName,
                                                     InterfaceAdapter *intf,
                                                     bool telemetryOnly) {
    InterfaceMap::iterator it = m_lookupMap.find(stateName);
    if (it == m_lookupMap.end()) {
      // Not found, OK to add
      debugMsg("AdapterConfiguration:registerLookupInterface",
               " registering interface " << intf << " for lookup '" << stateName << "'");
      m_lookupMap.insert(std::pair<std::string, InterfaceAdapter *>(stateName, intf));
      m_adapters.insert(intf);
      if (telemetryOnly)
        m_telemetryLookups.insert(stateName);
      return true;
    } else {
      debugMsg("AdapterConfiguration:registerLookupInterface",
               " interface already registered for lookup '" << stateName << "'");
      return false;
    }
  }

  /**
   * @brief Register the given interface adapter for planner updates.
            Returns true if successful.  Fails and returns false
            iff an adapter is already registered
            or setting the default planner update interface is not implemented.
   * @param intf The interface adapter to handle planner updates.
   */
  bool AdapterConfiguration::registerPlannerUpdateInterface(InterfaceAdapter *intf) {
    if (m_plannerUpdateInterface) {
      debugMsg("AdapterConfiguration:registerPlannerUpdateInterface",
               " planner update interface already registered");
      return false;
    }
    debugMsg("AdapterConfiguration:registerPlannerUpdateInterface",
             " registering planner update interface " << intf);
    m_plannerUpdateInterface = intf;
    m_adapters.insert(intf);
    return true;
  }

  /**
   * @brief Register the given interface adapter as the default for all lookups and commands
   which do not have a specific adapter.  Returns true if successful.
   Fails and returns false if there is already a default adapter registered
            or setting the default interface is not implemented.
   * @param intf The interface adapter to use as the default.
   */
  bool AdapterConfiguration::setDefaultInterface(InterfaceAdapter *intf) {
    if (m_defaultInterface) {
      debugMsg("AdapterConfiguration:setDefaultInterface",
               " attempt to overwrite default interface adapter " << m_defaultInterface);
      return false;
    }
    m_defaultInterface = intf;
    m_adapters.insert(intf);
    debugMsg("AdapterConfiguration:setDefaultInterface",
             " setting default interface " << intf);
    return true;
  }

  /**
   * @brief Register the given interface adapter as the default for lookups.
            This interface will be used for all lookups which do not have
        a specific adapter.
            Returns true if successful.
        Fails and returns false if there is already a default lookup adapter registered
            or setting the default lookup interface is not implemented.
   * @param intf The interface adapter to use as the default.
   * @return True if successful, false if there is already a default adapter registered.
   */
  bool AdapterConfiguration::setDefaultLookupInterface(InterfaceAdapter *intf) {
    if (m_defaultLookupInterface) {
      debugMsg("AdapterConfiguration:setDefaultLookupInterface",
               " attempt to overwrite default lookup interface adapter " << m_defaultLookupInterface);
      return false;
    }
    m_defaultLookupInterface = intf;
    m_adapters.insert(intf);
    debugMsg("AdapterConfiguration:setDefaultLookupInterface",
             " setting default lookup interface " << intf);
    return true;
  }

  /**
   * @brief Register the given interface adapter as the default for commands.
            This interface will be used for all commands which do not have
        a specific adapter.
            Returns true if successful.
        Fails and returns false if there is already a default command adapter registered.
   * @param intf The interface adapter to use as the default.
   * @return True if successful, false if there is already a default adapter registered.
   */
  bool AdapterConfiguration::setDefaultCommandInterface(InterfaceAdapter *intf) {
    if (m_defaultCommandInterface) {
      debugMsg("AdapterConfiguration:setDefaultCommandInterface",
               " attempt to overwrite default command interface adapter " << m_defaultCommandInterface);
      return false;
    }
    m_defaultCommandInterface = intf;
    m_adapters.insert(intf);
    debugMsg("AdapterConfiguration:setDefaultCommandInterface",
             " setting default command interface " << intf);
    return true;
  }

  /**
   * @brief Return the interface adapter in effect for this command, whether
   specifically registered or default. May return NULL.
   * @param commandName The command.
   */
  InterfaceAdapter *AdapterConfiguration:: getCommandInterface(std::string const &commandName) {
    InterfaceMap::iterator it = m_commandMap.find(commandName);
    if (it != m_commandMap.end()) {
      debugMsg("AdapterConfiguration:getCommandInterface",
               " found specific interface " << (*it).second
               << " for command '" << commandName << "'");
      return (*it).second;
    }
    // check default command i/f
    if (m_defaultCommandInterface) {
      debugMsg("AdapterConfiguration:getCommandInterface",
               " returning default command interface " << m_defaultCommandInterface
               << " for command '" << commandName << "'");
      return m_defaultCommandInterface;
    }
    // fall back on default default
    debugMsg("AdapterConfiguration:getCommandInterface",
             " returning default interface " << m_defaultInterface
             << " for command '" << commandName << "'");
    return m_defaultInterface;
  }

  /**
   * @brief Return the current default interface adapter for commands.
            May return NULL. Returns NULL if default interfaces are not implemented.
   */
  InterfaceAdapter *AdapterConfiguration:: getDefaultCommandInterface() {
    return m_defaultCommandInterface;
  }

  /**
   * @brief Return the interface adapter in effect for lookups with this state name,
   whether specifically registered or default. May return NULL. Returns NULL if default interfaces are not implemented.
   * @param stateName The state.
   */
  InterfaceAdapter *AdapterConfiguration:: getLookupInterface(std::string const &stateName) {
    InterfaceMap::iterator it = m_lookupMap.find(stateName);
    if (it != m_lookupMap.end()) {
      debugMsg("AdapterConfiguration:getLookupInterface",
               " found specific interface " << (*it).second
               << " for lookup '" << stateName << "'");
      return (*it).second;
    }
    // try defaults
    if (m_defaultLookupInterface) {
      debugMsg("AdapterConfiguration:getLookupInterface",
               " returning default lookup interface " << m_defaultLookupInterface
               << " for lookup '" << stateName << "'");
      return m_defaultLookupInterface;
    }
    // try default defaults
    debugMsg("AdapterConfiguration:getLookupInterface",
             " returning default interface " << m_defaultInterface
             << " for lookup '" << stateName << "'");
    return m_defaultInterface;
  }

  /**
   * @brief Query configuration data to determine if a state is only available as telemetry.
   * @param stateName The state.
   * @return True if state is declared telemetry-only, false otherwise.
   * @note In the absence of a declaration, a state is presumed not to be telemetry.
   */
  bool AdapterConfiguration::lookupIsTelemetry(std::string const &stateName) const
  {
    return m_telemetryLookups.find(stateName) != m_telemetryLookups.end();
  }

  /**
   * @brief Return the current default interface adapter for lookups.
            May return NULL.
   */
  InterfaceAdapter *AdapterConfiguration:: getDefaultLookupInterface() {
    return m_defaultLookupInterface;
  }

  /**
   * @brief Return the interface adapter in effect for planner updates,
            whether specifically registered or default. May return NULL.
            Returns NULL if default interfaces are not defined.
   */
  InterfaceAdapter *AdapterConfiguration:: getPlannerUpdateInterface() {
    if (!m_plannerUpdateInterface) {
      debugMsg("AdapterConfiguration:getPlannerUpdateInterface",
               " returning default interface " << m_defaultInterface);
      return m_defaultInterface;
    }
    debugMsg("AdapterConfiguration:getPlannerUpdateInterface",
             " found specific interface " << m_plannerUpdateInterface);
    return m_plannerUpdateInterface;
  }

  /**
   * @brief Return the current default interface adapter. May return NULL.
   */
  InterfaceAdapter *AdapterConfiguration:: getDefaultInterface() {
    return m_defaultInterface;
  }

  /**
   * @brief Returns true if the given adapter is a known interface in the system. False otherwise
   */
  bool AdapterConfiguration::isKnown(InterfaceAdapter *intf) {
    // Check the easy places first
    if (intf == m_defaultInterface
        || intf == m_defaultCommandInterface
        || intf == m_defaultLookupInterface
        || intf == m_plannerUpdateInterface)
      return true;

    // See if the adapter is in any of the tables
    for (InterfaceMap::iterator it = m_lookupMap.begin(); it != m_lookupMap.end(); ++it)
      if (it->second == intf)
        return true;
    for (InterfaceMap::iterator it = m_commandMap.begin(); it != m_commandMap.end(); ++it)
      if (it->second == intf)
        return true;
    return false;
  }

  /**
   * @brief Clears the interface adapter registry.
   */
  void AdapterConfiguration:: clearAdapterRegistry() 
  {
    m_lookupMap.clear();
    m_commandMap.clear();
    m_telemetryLookups.clear();
    m_plannerUpdateInterface = NULL;
    m_defaultInterface = NULL;
    m_defaultCommandInterface = NULL;
    m_defaultLookupInterface = NULL;
  }

  /**
   * @brief Deletes the given adapter from the interface manager
   * @return true if the given adapter existed and was deleted. False if not found
   */
  bool AdapterConfiguration::deleteAdapter(InterfaceAdapter *intf) {
    int res = m_adapters.erase(intf);
    return res != 0;
  }

  // Initialize global variable
  AdapterConfiguration *g_configuration = NULL;

}
