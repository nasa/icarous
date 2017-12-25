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

//  Copyright Notices

//  This software was developed for use by the U.S. Government as
//  represented by the Administrator of the National Aeronautics and
//  Space Administration. No copyright is claimed in the United States
//  under 17 U.S.C. 105.

//  This software may be used, copied, and provided to others only as
//  permitted under the terms of the contract or other agreement under
//  which it was acquired from the U.S. Government.  Neither title to nor
//  ownership of the software is hereby transferred.  This notice shall
//  remain on all copies of the software.

/**
  @file Debug.hh
  @author Europa project

  @brief Numerous declarations related to debugging.
*/

#ifndef PLEXIL_DEBUG_HH
#define PLEXIL_DEBUG_HH

#include "plexil-config.h"

#include <iostream>

#ifdef NO_DEBUG_MESSAGE_SUPPORT

#define debugMsg(marker, data)
#define condDebugMsg(cond, marker, data)
#define debugStmt(marker, stmt)
#define condDebugStmt(cond, marker, stmt)
#define SHOW(thing)
#define MARK

namespace PLEXIL {

// Dummies
inline bool setDebugOutputStream(std::ostream & /* os */)
{
  return true;
}

inline bool readDebugConfigStream(std::istream & /* is */)
{
  return true;
}

}

#else

#include "DebugMessage.hh"

/**
   @brief The SHOW() macro is intended as a convenience debugging tool
   inserted briefly and removed when certainty is restored.  It prints
   the name of the passed parameter and it's value, along with the file
   and line number where it occurs in the code.
 */

#define SHOW(thing) (std::cout << __FILE__ << "(" << __LINE__ << ") " << #thing << ": " << (thing) << std::endl)

/**
   @brief The MARK macro is intended as a convenience debugging tool
   inserted briefly and removed when certainty is restored.  It prints
   the file and line number where it occurs in the code.
 */

#define MARK (std::cout << __FILE__ << "(" << __LINE__ << ") MARK" << std::endl)

/**
  @brief Use the debugMsg() macro to create a debug message that
  will be printed when the code is executed if and only if this
  particular debug message has been 'enabled'.
  @param marker A string that "marks" the message to enable it by.
  @param data The data to be printed when the message is enabled.
  @brief The data argument can be any "chain" of C++ expressions to
  output to the debug stream returned by DebugMessage::getStream()
  when this debug message is enabled (via, e.g. DebugMessage::enable()
  or DebugMessage::enableAll()).
  @see condDebugMsg
  @see debugStmt
  @see condDebugStmt
  @see DebugMessage
*/
#define debugMsg(marker, data) condDebugMsg(true, marker, data)

/**
  @brief Create a conditional debug message, which will
  only be created or used when the given condition is true at run time.
  @param cond An additional condition to be checked before printing the message,
         which can be any C/C++ expression that could be used in an if statement.
  @param marker A string that "marks" the message to enable it by.
  @param data The data to be printed when the message is enabled.
  @see debugMsg
  @see condDebugMsg
  @see debugStmt
  @see condDebugStmt
  @see DebugMessage
*/
#define condDebugMsg(cond, marker, data) { \
  static PLEXIL::DebugMessage dm(marker);     \
  if (dm.enabled && (cond)) { \
    PLEXIL::getDebugOutputStream() << "[" << marker << "]" << data << std::endl; \
  } \
}

/**
  @brief Add code to be executed only if the DebugMessage is enabled.
  @param marker A string that "marks" the message to enable it by.
  @param stmt The code to be executed when the message is enabled.
  @see debugMsg
  @see condDebugMsg
  @see condDebugStmt
  @see DebugMessage
*/
#define debugStmt(marker, stmt) condDebugStmt(true, marker, stmt)

/**
  @brief Add code to be executed only if the DebugMessage is enabled and
         the condition is true.
  @param cond An additional condition to be checked before printing the message,
         which can be any C/C++ expression that could be used in an if statement.
  @param marker A string that "marks" the message to enable it by.
  @param stmt The code to be executed when the message is enabled and the condition
         is true.
  @see debugMsg
  @see condDebugMsg
  @see debugStmt
  @see DebugMessage
*/
#define condDebugStmt(cond, marker, stmt) { \
  static PLEXIL::DebugMessage dm(marker); \
  if (dm.enabled && (cond)) { \
    stmt ; \
  } \
}

#endif /* NO_DEBUG_MESSAGE_SUPPORT */

#endif /* PLEXIL_DEBUG_HH */
