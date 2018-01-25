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

#ifndef PLEXIL_DEBUG_MESSAGE_HH
#define PLEXIL_DEBUG_MESSAGE_HH

#include <iosfwd> // redundant with #include <iostream> in Debug.hh
#include <string>

namespace PLEXIL
{
  // C++ sucks at information hiding.
  void enableMatchingDebugMessages(std::string const &pattern);

  struct DebugMessage
  {
    friend void enableMatchingDebugMessages(std::string const &);

  public:
    /**
     * @brief Construct a DebugMessage.
     * @param marker Name for the particular instance (not required to be unique within the process).
     */
    DebugMessage(char const *mrkr);

    ~DebugMessage();

    /**
       @brief Marker given when this instance was created.
    */
    char const * const marker;

  private:

    /**
     * @brief Pointer to next (previous) message in list.
     */
    DebugMessage *next;

  public:
  
    /**
       @brief Whether this instance is 'enabled' or not.
    */
    bool enabled;

  private:

    // Not implemented
    DebugMessage();
    DebugMessage(DebugMessage const &);
    DebugMessage& operator=(DebugMessage const &);
  };

  std::ostream &operator<<(std::ostream &os, DebugMessage const &dm);

  extern std::ostream &getDebugOutputStream();
  extern bool setDebugOutputStream(std::ostream &os);

  extern bool readDebugConfigStream(std::istream &is);

} // namespace PLEXIL

#endif // PLEXIL_DEBUG_MESSAGE_HH
