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

#ifndef TIME_ADAPTER_H
#define TIME_ADAPTER_H

#include "InterfaceAdapter.hh"
#include "InterfaceError.hh"

#include <unistd.h>
#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS - 200112L) >= 0L || defined(PLEXIL_ANDROID))
// POSIX timers are supported
#define TIME_ADAPTER_CLASS PosixTimeAdapter
#elif defined(HAVE_SETITIMER)
// BSD timers are supported
// Currently only Mac OS X
#define TIME_ADAPTER_CLASS DarwinTimeAdapter
#else
// Custom implementation needed
#error No time adapter class implemented for this platform.
#endif

namespace PLEXIL
{

  /**
   * @brief A virtual base class for an interface adapter for Unix-like systems,
   *        using native time facilities to implement LookupNow and LookupOnChange.
   */
  class TimeAdapter : public InterfaceAdapter
  {
  public:
    /**
     * @brief Constructor.
     * @param execInterface Reference to the parent AdapterExecInterface object.
     */
    TimeAdapter(AdapterExecInterface& execInterface);

    /**
     * @brief Constructor from configuration XML.
     * @param execInterface Reference to the parent AdapterExecInterface object.
     * @param xml An XML element describing this adapter
     * @note The instance maintains a shared pointer to the XML element.
     */
    TimeAdapter(AdapterExecInterface& execInterface, 
                pugi::xml_node const xml);

    /**
     * @brief Destructor.
     */
    virtual ~TimeAdapter();

    //
    // InterfaceAdapter API implemented by derived classes, not needed here.
    //

    /**
     * @brief Get the current time from the operating system.
     * @return A double representing the current time.
     * @note Implemented by derived classes.
     */
    virtual double getCurrentTime() throw (InterfaceError) = 0;

  private:

    // Deliberately unimplemented
    TimeAdapter();
    TimeAdapter(const TimeAdapter &);
    TimeAdapter & operator=(const TimeAdapter &);
  };

} // namespace PLEXIL

#endif // TIME_ADAPTER_H
