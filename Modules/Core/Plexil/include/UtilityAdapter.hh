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

// This interface adapter provides the following useful utilities for PLEXIL plans:
//   Command: print(exp1 exp2 ...)  - prints arguments to standard output
//   Command: pprint(exp1 exp2 ...) - "pretty print", as above but separates 
//                                    items with whitespace and adds newline
// This adapter is accessed by including the following entry in your interface
// configuration file:    <Adapter AdapterType="Utility"/>

#ifndef PLEXIL_UTILITY_ADAPTER_HH
#define PLEXIL_UTILITY_ADAPTER_HH

#include "InterfaceAdapter.hh"

namespace PLEXIL {

class UtilityAdapter : public InterfaceAdapter
{
public:
  UtilityAdapter (AdapterExecInterface&, pugi::xml_node const);

  bool initialize();
  bool start();
  bool stop();
  bool reset();
  bool shutdown();

  void executeCommand(Command *cmd);

  void invokeAbort(Command *cmd);

};

extern "C" {
  void initUtilityAdapter();
}

} // namespace PLEXIL

#endif
