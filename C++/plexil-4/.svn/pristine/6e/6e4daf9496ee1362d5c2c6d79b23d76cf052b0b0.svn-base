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

#include "LuvListener.hh"
#include "LuvFormat.hh"

#include "Debug.hh"
#include "Expression.hh"
#include "Node.hh"

#include "ClientSocket.h"
#include "SocketException.h"

#include <sstream>

#include <cstring>
#include <cstdlib>

namespace PLEXIL
{

  /**
   * @brief Constructor from configuration XML.
   */
  LuvListener::LuvListener(pugi::xml_node const xml)
	: ExecListener(xml),
	  m_socket(NULL),
	  m_host(NULL),
	  m_port(0),
	  m_block(false),
	  m_ignoreConnectFailure(true)
  {
    // Parse XML
    char const *hostname = xml.attribute(LUV_HOSTNAME_ATTR()).value();
    if (hostname && *hostname)
      m_host = strdup(hostname);
    else
      m_host = strdup(LUV_DEFAULT_HOSTNAME());

    pugi::xml_attribute portattr = xml.attribute(LUV_PORT_ATTR());
    if (portattr)
      m_port = portattr.as_uint(0);
    if (!m_port)
      m_port = LUV_DEFAULT_PORT();
    
    pugi::xml_attribute blockattr = xml.attribute(LUV_BLOCKING_ATTR());
    if (blockattr)
      m_block = blockattr.as_bool(false);
  }

  //* Constructor from TestExec.
  LuvListener::LuvListener(const std::string& host, 
						   const uint16_t port, 
						   const bool block,
						   const bool ignoreConnectionFailure)
	: ExecListener(),
	  m_socket(NULL),
	  m_host(strdup(host.c_str())),
	  m_port(port),
	  m_block(block),
	  m_ignoreConnectFailure(ignoreConnectionFailure)
  {
	// open the socket
	openSocket(m_port, m_host, m_ignoreConnectFailure);
  }

  //* Destructor.
  LuvListener::~LuvListener()
  {
	closeSocket();
    free((void *) m_host);
  }

  /**
   * @brief Perform listener-specific initialization.
   * @return true if successful, false otherwise.
   */
  bool LuvListener::initialize()
  {
    // parse XML to find host, port, blocking flag
    pugi::xml_node const xml = this->getXml();
	if (xml.empty())
	  // Have to presume that things were constructed correctly
	  return true;

	pugi::xml_attribute hostAttr = xml.attribute(LUV_HOSTNAME_ATTR());
    if (hostAttr.empty()) {
	  debugMsg("LuvListener:initialize",
			   " no " << LUV_HOSTNAME_ATTR()
			   << " attribute found, using default host " << LUV_DEFAULT_HOSTNAME());
	  m_host = LUV_DEFAULT_HOSTNAME();
	}
	else {
	  // FIXME: add sanity check?
	  m_host = hostAttr.value();
	}


	pugi::xml_attribute portAttr = xml.attribute(LUV_PORT_ATTR());
    if (portAttr.empty()) {
	  debugMsg("LuvListener:initialize",
			   " no " << LUV_PORT_ATTR()
			   << " attribute found, using default port " << LUV_DEFAULT_PORT());
	  m_port = LUV_DEFAULT_PORT();
	}
    else {
	  // Should range check here
	  // *** NYI ***
	  m_port = (uint16_t) portAttr.as_uint();
	}

	pugi::xml_attribute blockAttr = xml.attribute(LUV_BLOCKING_ATTR());
    if (blockAttr.empty()) {
	  debugMsg("LuvListener:initialize",
			   " no " << LUV_BLOCKING_ATTR()
			   << " attribute found, using default \"false\"");
	  m_block = false;
	}
    else {
	  m_block = blockAttr.as_bool();
	}

	pugi::xml_attribute ignoreFailAttr = xml.attribute(IGNORE_CONNECT_FAILURE_ATTR());
    if (ignoreFailAttr.empty()) {
	  debugMsg("LuvListener:initialize",
			   " no " << IGNORE_CONNECT_FAILURE_ATTR()
			   << " attribute found, using default \"true\"");
	  m_ignoreConnectFailure = true;
	}
    else {
	  m_ignoreConnectFailure = ignoreFailAttr.as_bool();
	}

    return true; 
  }

  /**
   * @brief Perform listener-specific startup.
   * @return true if successful, false otherwise.
   */
  bool LuvListener::start() 
  { 
    return openSocket(m_port, m_host, m_ignoreConnectFailure); 
  }

  /**
   * @brief Perform listener-specific actions to stop.
   * @return true if successful, false otherwise.
   */
  bool LuvListener::stop() 
  {
    return true; 
  }

  /**
   * @brief Perform listener-specific actions to reset to initialized state.
   * @return true if successful, false otherwise.
   */
  bool LuvListener::reset() 
  {
	this->closeSocket();
    return true; 
  }

  /**
   * @brief Perform listener-specific actions to shut down.
   * @return true if successful, false otherwise.
   */
  bool LuvListener::shutdown() 
  { 
	this->closeSocket();
    return true; 
  }

  //
  // Public class member functions
  //

  /**
   * @brief Construct the appropriate configuration XML for the desired settings.
   * @param block true if the Exec should block until the user steps forward, false otherwise.
   * @param hostname The host name where the Luv instance is running.
   * @param port The port number for the Luv instance.
   */
  pugi::xml_document* LuvListener::constructConfigurationXml(const bool& block,
															 const char* hostname,
															 const unsigned int port)
  {
	pugi::xml_document* result = new pugi::xml_document();
	pugi::xml_node toplevel = result->append_child("Listener");
    toplevel.append_attribute("ListenerType").set_value("LuvListener");
    toplevel.append_attribute(LUV_BLOCKING_ATTR()).set_value(block);
    toplevel.append_attribute(LUV_HOSTNAME_ATTR()).set_value(hostname);
    toplevel.append_attribute(LUV_PORT_ATTR()).set_value(port);
    return result;
  }


  /**
   * @brief Notify that a node has changed state.
   * @param prevState The old state.
   * @param node The node that has transitioned.
   * @note The current state is accessible via the node.
   */
  void 
  LuvListener::implementNotifyNodeTransition(NodeState prevState, 
											 Node *node) const 
  {
	if (m_socket != NULL) {
	  std::ostringstream s;
	  LuvFormat::formatTransition(s, prevState, node);
	  sendMessage(s.str());
	}
  }


  /**
   * @brief Notify that a plan has been received by the Exec.
   * @param plan The intermediate representation of the plan.
   * @param parent The name of the parent node under which this plan will be inserted.
   */
  void
  LuvListener::implementNotifyAddPlan(pugi::xml_node const plan) const 
  {
	if (m_socket != NULL) {
      sendPlanInfo();
      std::ostringstream s;
      LuvFormat::formatPlan(s, plan);
      sendMessage(s.str());
	}
  }

  /**
   * @brief Notify that a library node has been received by the Exec.
   * @param libNode The intermediate representation of the plan.
   * @note The default method is deprecated and will go away in a future release.
   */
  void
  LuvListener::implementNotifyAddLibrary(pugi::xml_node const libNode) const 
  {
	if (m_socket != NULL) {
	  sendPlanInfo();
      std::ostringstream s;
      LuvFormat::formatLibrary(s, libNode);
      sendMessage(s.str());
	}
  }

  /**
   * @brief Notify that a variable assignment has been performed.
   * @param dest The Expression being assigned to.
   * @param destName A string naming the destination.
   * @param value The value (in internal Exec representation) being assigned.
   */
  void 
  LuvListener::implementNotifyAssignment(Expression const *dest,
										 std::string const &destName,
										 Value const &value) const
  {
	if (m_socket != NULL) {
	  std::ostringstream s;
	  LuvFormat::formatAssignment(s, dest, destName, value);
	  sendMessage(s.str());
	}
  }



  /**
   * @brief Open the socket connection to the viewer.
   * @param port The IP port to which we are connecting.
   * @param host The hostname to which we are connecting.
   * @param ignoreFailure If true, failure is silently ignored.
   * @return False if the connection fails and ignoreFailure is false, true otherwise.
   */
  bool
  LuvListener::openSocket(uint16_t port, 
						  const char* host, 
						  bool ignoreFailure) 
  {
	try {
	  debugMsg("LuvListener:start",
			   " opening client socket to host " << host << ", port " << port);
	  m_socket = new ClientSocket(std::string(host), port);
	}    
	catch (const SocketException &e) {
	  debugMsg("LuvListener:start",
			   " socket error: " << e.description());
	  delete m_socket;
	  m_socket = NULL;
	  return ignoreFailure;
	}

	// Success!
    return true; 
  }


  //* Close the socket.
  void LuvListener::closeSocket()
  {
	delete m_socket;
	m_socket = NULL;
  }

  //* Report whether the listener is connected to the viewer.
  bool LuvListener::isConnected()
  {
	return m_socket != NULL;
  }

  //* Send a plan info header to the viewer.
  void LuvListener::sendPlanInfo() const
  {
	std::ostringstream s;
	LuvFormat::formatPlanInfo(s, m_block);
	sendMessage(s.str());
  }

  //* Send the message to the viewer.
  void LuvListener::sendMessage(const std::string& msg) const
  {
	debugMsg("LuvListener:sendMessage", " sending:\n" << msg);
    *m_socket << msg << LUV_END_OF_MESSAGE();
    waitForAck();
  }

  //* Wait for acknowledgement from the viewer.
  void LuvListener::waitForAck() const
  {
    if (m_block)
      {
        std::string buffer;
		do
          {
            *m_socket >> buffer;
          }
        while (buffer[0] != LUV_END_OF_MESSAGE());
      }
  }

}
