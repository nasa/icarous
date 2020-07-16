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

#include "RobotPositionServer.hh"
#include <iostream>

RobotPositionServer::RobotPositionServer(int _height, int _width) : 
  m_OccupancyGrid(std::vector<std::vector<std::string> >(_height,
                                                         std::vector<std::string>(_width, "EMPTY"))),
  m_RobotPositionMutex()
{
}

RobotPositionServer::~RobotPositionServer()
{
}

bool RobotPositionServer::setRobotPosition(const std::string& name, int row, int col)
{
  std::vector<int> pos2d(2);
  bool registered = false;
  PLEXIL::ThreadMutexGuard mg(m_RobotPositionMutex);
  
  m_NameToPositionMapIter = m_NameToPositionMap.find(name);
  
  if (m_NameToPositionMapIter != m_NameToPositionMap.end())
    {
      // Previous position known. Read it
      pos2d = m_NameToPositionMapIter->second;
      registered = true;
    }
  
  // Check if the desired position is free Occupancy grid.
  // If yes, remove the old position if any and update new.
  //         Also update the map.
  //         return true
  // if no, return false.
  if (!gridOccupied(row, col))
    {
      if (registered)
        {
          m_OccupancyGrid[pos2d[0]][pos2d[1]] = "EMPTY";
        }
      
      m_OccupancyGrid[row][col] = name;
      
      pos2d[0] = row;
      pos2d[1] = col;
      m_NameToPositionMap[name] = pos2d;
      
      return true;
    }
  else
    {
      return false;
    }        
}

bool RobotPositionServer::gridOccupied(int row, int col) const
{
  return (m_OccupancyGrid[row][col] != "EMPTY");
}

void RobotPositionServer::getRobotPosition(const std::string& name, int& row, int& col)
{
  PLEXIL::ThreadMutexGuard mg(m_RobotPositionMutex);
  
  m_NameToPositionMapIter = m_NameToPositionMap.find(name);
  
  if (m_NameToPositionMapIter != m_NameToPositionMap.end())
    {
      // Previous position known. Read it
      std::vector<int> pos2d = m_NameToPositionMapIter->second;
      row = pos2d[0];
      col = pos2d[1];
    }
  else
    std::cerr << "RobotPositionServer:readRobotPosition: Position of robot: "
              << name << " is not known." << std::endl;
}
