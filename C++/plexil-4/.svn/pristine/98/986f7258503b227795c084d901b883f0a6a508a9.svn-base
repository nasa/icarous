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

#include "AdapterFactory.hh"
#include "InterfaceAdapter.hh"
#include "AdapterExecInterface.hh"
#include "Debug.hh"
#ifdef HAVE_DLFCN_H
#include "DynamicLoader.h"
#endif
#include "Error.hh"
#include "InterfaceSchema.hh"
#include "lifecycle-utils.h"

namespace PLEXIL
{
  /**
   * @brief Creates a new InterfaceAdapter instance as specified by
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the InterfaceAdapter constructor.
   * @param execInterface Reference to the parent InterfaceManager instance.
   * @return The Id for the new InterfaceAdapter.  May not be unique.
   */

  InterfaceAdapter *
  AdapterFactory::createInstance(pugi::xml_node const xml,
                                 AdapterExecInterface& execInterface)
  {
    // Can't do anything without the spec
    assertTrueMsg(xml != NULL,
                  "AdapterFactory::createInstance: null configuration XML");

    // Get the kind of adapter to make
    const char* adapterType = 
      xml.attribute(InterfaceSchema::ADAPTER_TYPE_ATTR()).value();
    if (!*adapterType) {
      warn("AdapterFactory: missing "
           << InterfaceSchema::ADAPTER_TYPE_ATTR()
           << " attribute in adapter XML:\n" << *xml);
      return NULL;
    }

    // Make it
    bool dummy;
    return createInstance(adapterType, xml, execInterface, dummy);
  }

  /**
   * @brief Creates a new InterfaceAdapter instance with the type associated with the name and
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the InterfaceAdapter constructor.
   * @param execInterface Reference to the parent InterfaceManager instance.
   * @return The Id for the new InterfaceAdapter.  May not be unique.
   */

  InterfaceAdapter *
  AdapterFactory::createInstance(std::string const& name,
                                 pugi::xml_node const xml,
                                 AdapterExecInterface& execInterface)
  {
    bool dummy;
    return createInstance(name, xml, execInterface, dummy);
  }


  /**
   * @brief Creates a new InterfaceAdapter instance with the type associated with the name and
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the InterfaceAdapter constructor.
   * @param wasCreated Reference to a boolean variable;
   *                   variable will be set to true if new object created, false otherwise.
   * @return The Id for the new InterfaceAdapter.  If wasCreated is set to false, is not unique.
   */

  InterfaceAdapter *
  AdapterFactory::createInstance(std::string const& name,
                                 pugi::xml_node const xml,
                                 AdapterExecInterface& execInterface,
                                 bool& wasCreated)
  {
    std::map<std::string, AdapterFactory*>::const_iterator it = factoryMap().find(name);
#ifdef HAVE_DLFCN_H
    if (it == factoryMap().end()) {
      debugMsg("AdapterFactory:createInstance", 
               "Attempting to dynamically load adapter type \""
               << name.c_str() << "\"");
      // Attempt to dynamically load library
      const char* libCPath =
        xml.attribute(InterfaceSchema::LIB_PATH_ATTR()).value();
      if (!dynamicLoadModule(name.c_str(), libCPath)) {
        warn("AdapterFactory: unable to load module for adapter type \""
             << name.c_str() << "\"");
        wasCreated = false;
        return NULL;
      }

      // See if it's registered now
      it = factoryMap().find(name);
    }
#endif

    if (it == factoryMap().end()) {
      warn("AdapterFactory: No factory registered for adapter type \""
           << name.c_str()
           << "\".");
      wasCreated = false;
      return NULL;
    }
    InterfaceAdapter *retval = it->second->create(xml, execInterface, wasCreated);
    debugMsg("AdapterFactory:createInstance", " Created adapter " << name.c_str());
    return retval;
  }

  bool AdapterFactory::isRegistered(std::string const& name) {
    return factoryMap().find(name) != factoryMap().end();
  }

  std::map<std::string, AdapterFactory*>& AdapterFactory::factoryMap() 
  {
    static std::map<std::string, AdapterFactory*> sl_map;
    static bool sl_inited = false;
    if (!sl_inited) {
      plexilAddFinalizer(&purge);
      sl_inited = true;
    }
    return sl_map;
  }

  /**
   * @brief Deallocate all factories
   */
  void AdapterFactory::purge()
  {
    for (std::map<std::string, AdapterFactory*>::iterator it = factoryMap().begin();
         it != factoryMap().end();
         ++it)
      delete it->second;
    factoryMap().clear();
  }

  /**
   * @brief Registers an AdapterFactory with the specific name.
   * @param name The name by which the Adapter shall be known.
   * @param factory The AdapterFactory instance.
   */
  void AdapterFactory::registerFactory(std::string const& name, AdapterFactory* factory)
  {
    assertTrue_1(factory != NULL);
    if (factoryMap().find(name) != factoryMap().end()) {
      warn("Attempted to register an adapter factory for name \""
           << name.c_str()
           << "\" twice, ignoring.");
      delete factory;
      return;
    }
    factoryMap()[name] = factory;
    debugMsg("AdapterFactory:registerFactory",
             " Registered adapter factory for name \"" << name.c_str() << "\"");
  }

}
