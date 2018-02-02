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

#include "ExecListenerFactory.hh"
#include "Debug.hh"
#ifdef HAVE_DLFCN_H
#include "DynamicLoader.h"
#endif
#include "Error.hh"
#include "ExecListener.hh"
#include "InterfaceSchema.hh"
#include "lifecycle-utils.h"

namespace PLEXIL
{
  //
  // ExecListenerFactory
  //

  /**
   * @brief Creates a new ExecListener instance with the type associated with the name and
   *        the given configuration XML.
   * @param xml The configuration XML specifying the ExecListener.
   * @return The new ExecListener.
   */

  ExecListener *
  ExecListenerFactory::createInstance(pugi::xml_node const xml)
  {
    // Can't do anything without the spec
    assertTrueMsg(!xml.empty(),
                  "ExecListenerFactory::createInstance: null configuration XML");

    // Get the kind of listener to make
    const char* listenerType = 
      xml.attribute(InterfaceSchema::LISTENER_TYPE_ATTR()).value();
    if (!*listenerType) {
      warn("ExecListenerFactory: missing "
           << InterfaceSchema::LISTENER_TYPE_ATTR()
           << " attribute in listener XML:\n"
           << *xml);
      return NULL;
    }

    // Make it
    return createInstance(std::string(listenerType), xml);
  }

  /**
   * @brief Creates a new ExecListener instance with the type associated with the name and
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the ExecListener constructor.
   * @return The new ExecListener.
   */

  ExecListener * 
  ExecListenerFactory::createInstance(std::string const &name,
                                      pugi::xml_node const xml)
  {
    std::map<std::string, ExecListenerFactory*>::const_iterator it = factoryMap().find(name);
#ifdef HAVE_DLFCN_H
    if (it == factoryMap().end()) {
      debugMsg("ExecListenerFactory:createInstance", 
               "Attempting to dynamically load listener type \""
               << name.c_str() << "\"");
      // Attempt to dynamically load library
      const char* libCPath =
        xml.attribute(InterfaceSchema::LIB_PATH_ATTR()).value();
      if (!dynamicLoadModule(name.c_str(), libCPath)) {
        warn("ExecListenerFactory: Unable to load module for listener type \""
             << name.c_str() << "\"");
        return NULL;
      }
      // See if it's registered now
      it = factoryMap().find(name);
    }
#endif

    if (it == factoryMap().end()) {
      warn("ExecListenerFactory: No factory registered for listener type \""
           << name.c_str() << "\"");
      return NULL;
    }
    ExecListener *retval = it->second->create(xml);
    debugMsg("ExecListenerFactory:createInstance", " Created Exec listener " << name.c_str());
    return retval;
  }

  std::map<std::string, ExecListenerFactory*>& ExecListenerFactory::factoryMap() 
  {
    static std::map<std::string, ExecListenerFactory*> sl_map;
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
  void ExecListenerFactory::purge()
  {
    for (std::map<std::string, ExecListenerFactory*>::iterator it = factoryMap().begin();
         it != factoryMap().end();
         ++it)
      delete it->second;
    factoryMap().clear();
  }

  /**
   * @brief Registers an ExecListenerFactory with the specific name.
   * @param name The name by which the Exec Listener shall be known.
   * @param factory The ExecListenerFactory instance.
   */
  void ExecListenerFactory::registerFactory(std::string const &name, ExecListenerFactory* factory)
  {
    assertTrue_1(factory != NULL);
    // FIXME: Assert, or replace old factory?
    if (factoryMap().find(name) != factoryMap().end()) {
      warn("Attempted to register an exec listener factory for name \""
           << name.c_str()
           << "\" twice, ignoring.");
      delete factory;
      return;
    }
    factoryMap()[name] = factory;
    debugMsg("ExecListenerFactory:registerFactory",
             " Registered exec listener factory for name \"" << name.c_str() << "\"");
  }

  bool ExecListenerFactory::isRegistered(std::string const &name) {
    return factoryMap().find(name) != factoryMap().end();
  }
}
