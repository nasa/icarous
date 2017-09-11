/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
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
#ifndef ROBOSIM_RESPONSE_HH
#define ROBOSIM_RESPONSE_HH

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "ResponseBase.hh"
#include "ResponseMessage.hh"

class MoveResponse : public ResponseBase
{
public:
  MoveResponse(const std::string& name, timeval delay, const int returnValue)
    : ResponseBase(name, delay), m_ReturnValue(returnValue) {}

  ~MoveResponse(){}

  virtual ResponseMessage* createResponseMessage()
  {
    std::cout << "Creating a Move response: " << m_ReturnValue << std::endl;
    std::ostringstream str;
    str << m_ReturnValue;
    return new ResponseMessage(-1, str.str());
  }

private:
  const int m_ReturnValue;
};


class QueryEnergyLevelResponse : public ResponseBase
{
public:
  QueryEnergyLevelResponse(const std::string& name, timeval delay, const std::vector<double> energyLevel) 
    : ResponseBase(name, delay), m_EnergyLevel(energyLevel) {}

  ~QueryEnergyLevelResponse(){}

  virtual ResponseMessage* createResponseMessage()
  {
    std::ostringstream str;
    for(unsigned int i = 0; i < m_EnergyLevel.size(); ++i)
      {
        str << m_EnergyLevel[i];
        if (i < (m_EnergyLevel.size() - 1))
          str << ",";
      }
      
    return new ResponseMessage(-1, str.str());
  }

private:
  const std::vector<double> m_EnergyLevel;
};

class RobotStateResponse : public ResponseBase
{
public:
  RobotStateResponse(const std::string& name, timeval delay, const std::vector<double> state) 
    : ResponseBase(name, delay), m_State(state) {}

  ~RobotStateResponse(){}

  virtual ResponseMessage* createResponseMessage()
  {
    std::ostringstream str;
    for(unsigned int i = 0; i < m_State.size(); ++i)
      {
        str << m_State[i];
        if (i < (m_State.size() - 1))
          str << ",";
      }
      
    return new ResponseMessage(-1, str.str());
  }

private:
  const std::vector<double> m_State;
};

#endif //ROBOSIM_RESPONSE_HH
