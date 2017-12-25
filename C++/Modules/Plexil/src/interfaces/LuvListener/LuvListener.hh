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

#ifndef LUV_LISTENER_HH
#define LUV_LISTENER_HH

#include "plexil-config.h"

#include "ConstantMacros.hh"
#include "ExecListener.hh"

#ifdef HAVE_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#elif defined(__VXWORKS__)
#include <vxWorks.h>
#endif

#include <string>

// forward references outside of namespace

class Socket;

namespace PLEXIL 
{

  // forward references in namespace PLEXIL

  /**
   * @brief An Exec listener which supports the Plexil Viewer (nee' LUV).
   */
  class LuvListener : public ExecListener
  {
  public:
    //
    // Public class constants
    //

    // Defaults
    DECLARE_STATIC_CLASS_CONST(char*, LUV_DEFAULT_HOSTNAME, "localhost");
    DECLARE_STATIC_CLASS_CONST(unsigned int, LUV_DEFAULT_PORT, 49100);

    // Configuration XML
    DECLARE_STATIC_CLASS_CONST(char*, LUV_HOSTNAME_ATTR, "HostName");
    DECLARE_STATIC_CLASS_CONST(char*, LUV_PORT_ATTR, "Port");
    DECLARE_STATIC_CLASS_CONST(char*, LUV_BLOCKING_ATTR, "Blocking");
    DECLARE_STATIC_CLASS_CONST(char*, IGNORE_CONNECT_FAILURE_ATTR, "IgnoreConnectFailure");

    // Literal strings (yes, this is redundant with LuvFormat.hh)
    DECLARE_STATIC_CLASS_CONST(char*, TRUE_STR, "true");
    DECLARE_STATIC_CLASS_CONST(char*, FALSE_STR, "false");

    // End-of-message marker
    DECLARE_STATIC_CLASS_CONST(char, LUV_END_OF_MESSAGE, (char)4);

    /**
     * @brief Constructor from configuration XML.
     */
    LuvListener(pugi::xml_node const xml);

	//* Constructor from TestExec.
	LuvListener(const std::string& host, 
				const uint16_t port, 
				const bool block = false,
				const bool ignoreConnectionFailure = true);

	//* Destructor.
	virtual ~LuvListener();

    /**
     * @brief Notify that a node has changed state.
     * @param prevState The old state.
     * @param node The node that has transitioned.
     * @note The current state is accessible via the node.
     */
    void implementNotifyNodeTransition(NodeState prevState, 
									   Node *node) const;

    /**
     * @brief Notify that a plan has been received by the Exec.
     * @param plan The intermediate representation of the plan.
     * @param parent The name of the parent node under which this plan will be inserted.
     */
    void implementNotifyAddPlan(pugi::xml_node const plan) const;

    /**
     * @brief Notify that a library node has been received by the Exec.
     * @param libNode The intermediate representation of the plan.
     * @note The default method is deprecated and will go away in a future release.
     */
    void implementNotifyAddLibrary(pugi::xml_node const libNode) const;

    /**
     * @brief Notify that a variable assignment has been performed.
     * @param dest The Expression being assigned to.
     * @param destName A string naming the destination.
     * @param value The value (in internal Exec representation) being assigned.
     */
    void implementNotifyAssignment(Expression const *dest,
								   const std::string& destName,
								   const Value& value) const;

    /**
     * @brief Perform listener-specific initialization.
     * @return true if successful, false otherwise.
     */
    bool initialize();

    /**
     * @brief Perform listener-specific startup.
     * @return true if successful, false otherwise.
     */
    bool start();

    /**
     * @brief Perform listener-specific actions to stop.
     * @return true if successful, false otherwise.
     */
    bool stop();

    /**
     * @brief Perform listener-specific actions to reset to initialized state.
     * @return true if successful, false otherwise.
     */
    bool reset();

    /**
     * @brief Perform listener-specific actions to shut down.
     * @return true if successful, false otherwise.
     */
    bool shutdown();

	//* Report whether the listener is connected to the viewer.
	bool isConnected();

    /**
     * @brief Construct the appropriate configuration XML for the desired settings.
     * @param block true if the Exec should block until the user steps forward, false otherwise.
     * @param hostname The host name where the Luv instance is running.
     * @param port The port number for the Luv instance.
     */
    static pugi::xml_document* constructConfigurationXml(const bool& block = false,
														 const char* hostname = LUV_DEFAULT_HOSTNAME(),
														 const unsigned int port = LUV_DEFAULT_PORT());

  private:

	// deliberately unimplemented
	LuvListener();
	LuvListener(const LuvListener&);
	LuvListener& operator=(const LuvListener&);

	/**
	 * @brief Open the socket connection to the viewer.
	 * @param port The IP port to which we are connecting.
	 * @param host The hostname to which we are connecting.
	 * @param ignoreFailure If true, failure is silently ignored.
	 * @return False if the connection fails and ignoreFailure is false, true otherwise.
	 */
	bool openSocket(uint16_t port, 
					const char* host, 
					bool ignoreFailure);

	//* Close the socket.
	void closeSocket();

	//* Send a plan info header to the viewer.
	void sendPlanInfo() const;

	//* Send the message to the viewer.
	void sendMessage(const std::string& msg) const;

	//* Wait for acknowledgement from the viewer.
	void waitForAck() const;

	//
	// Member variables
	//
    Socket* m_socket;
	const char* m_host;
	uint16_t m_port;
    bool m_block;
    bool m_ignoreConnectFailure;
  };

}

#endif // LUV_LISTENER_HH
