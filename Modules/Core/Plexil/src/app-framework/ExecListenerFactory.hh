/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#ifndef EXEC_LISTENER_FACTORY_H
#define EXEC_LISTENER_FACTORY_H

#include "pugixml.hpp"

#include <map>
#include <string>

namespace PLEXIL
{
  //
  // Forward declarations
  //

  class ExecListener;

  /**
   * @brief Factory class for ExecListener instances.
   *        Implements the AbstractFactory design pattern.
   */
  class ExecListenerFactory 
  {
  public:

    /**
     * @brief Creates a new ExecListener instance with the type associated with the name and
     *        the given configuration XML.
     * @param xml The configuration XML specifying the ExecListener.
     * @return The new ExecListener.
     */

    static ExecListener *createInstance(pugi::xml_node const xml);

    /**
     * @brief Creates a new ExecListener instance with the type associated with the name and
     *        the given configuration XML.
     * @param name The registered name for the factory.
     * @param xml The configuration XML to be passed to the ExecListener constructor.
     * @return The new ExecListener.
     */

    static ExecListener *createInstance(std::string const &name, 
                                        pugi::xml_node const xml);

    /**
     * @brief Checks whether or not the given ExecListenerFactory is registered.
     * @param name The registered name for the factory
     * @return True if the factory is registered, false otherwise
     */

    static bool isRegistered(std::string const &name);

    /**
     * @brief Deallocate all factories
     */
    static void purge();

    std::string const &getName() const {return m_name;}

  protected:

    virtual ~ExecListenerFactory()
    {}

    /**
     * @brief Registers an ExecListenerFactory with the specific name.
     * @param name The name by which the listener shall be known.
     * @param factory The ExecListenerFactory instance.
     */
    static void registerFactory(std::string const &name, ExecListenerFactory* factory);

    /**
     * @brief Instantiates a new ExecListener of the appropriate type.
     * @param xml The configuration XML for the instantiated listener.
     * @return The new ExecListener.
     */
    virtual ExecListener *create(pugi::xml_node const xml) const = 0;

    ExecListenerFactory(std::string const &name)
      : m_name(name)
    {
      registerFactory(m_name, this);
    }

  private:
    // Deliberately unimplemented
    ExecListenerFactory();
    ExecListenerFactory(const ExecListenerFactory&);
    ExecListenerFactory& operator=(const ExecListenerFactory&);

    /**
     * @brief The map from names (std::string) to concrete ExecListenerFactory instances.
     * This pattern of wrapping static data in a static method is to ensure proper loading
     * when used as a shared library.
     */
    static std::map<std::string, ExecListenerFactory*>& factoryMap();

    const std::string m_name; /*!< Name used for lookup */
  };

  /**
   * @brief Concrete factory class, templated for each listener type.
   */
  template<class ListenerType>
  class ConcreteExecListenerFactory : public ExecListenerFactory 
  {
  public:
    ConcreteExecListenerFactory(std::string const &name)
      : ExecListenerFactory(name) 
    {}

  private:
    // Deliberately unimplemented
    ConcreteExecListenerFactory();
    ConcreteExecListenerFactory(const ConcreteExecListenerFactory&);
    ConcreteExecListenerFactory& operator=(const ConcreteExecListenerFactory&);

    /**
     * @brief Instantiates a new ExecListener of the appropriate type.
     * @param xml The configuration XML for the instantiated listener.
     * @return The new ExecListener.
     */

    ExecListener *create(pugi::xml_node const xml) const
    {
      return new ListenerType(xml);
    }
  };

#define REGISTER_EXEC_LISTENER(CLASS,NAME) {new PLEXIL::ConcreteExecListenerFactory<CLASS>(NAME);}

} // namespace PLEXIL

#endif // EXEC_LISTENER_FACTORY_H
