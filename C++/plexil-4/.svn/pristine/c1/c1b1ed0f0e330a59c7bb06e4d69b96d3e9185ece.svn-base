/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

// This is a barebones publish-subscribe facility for the sample PLEXIL
// application.  It provides a set of subscription functions specific to various
// combinations of return type and parameters.

#ifndef _H__system
#define _H__system

#include <string>

// Subscriber types
typedef void (* SubscribeInt) (const std::string& state_name, int val);
typedef void (* SubscribeReal) (const std::string& state_name, float val);
typedef void (* SubscribeString) (const std::string& state_name,
                                  const std::string& val);
typedef void (* SubscribeBoolString) (const std::string& state_name,
                                      bool val, const std::string& arg);
typedef void (* SubscribeBoolIntInt) (const std::string& state_name,
                                      bool val, int arg1, int arg2);

// Setters for subscriber.
void setSubscriberInt (SubscribeInt);
void setSubscriberReal (SubscribeReal);
void setSubscriberString (SubscribeString);
void setSubscriberBoolString (SubscribeBoolString);
void setSubscriberBoolIntInt (SubscribeBoolIntInt);

// Publish a state name, which notifies the subscriber.
void publish (const std::string& state_name, int val);
void publish (const std::string& state_name, float val);
void publish (const std::string& state_name, const std::string& val);
void publish (const std::string& state_name, bool val, const std::string& arg);
void publish (const std::string& state_name, bool val, int arg1, int arg2);

#endif
