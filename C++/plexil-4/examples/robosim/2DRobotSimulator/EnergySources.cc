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

#include <climits>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


#include "MyOpenGL.hh"
#include "EnergySources.hh"
#include "Macros.hh"

EnergySources::EnergySources(const std::string& fName, int _size, double _radius) : 
  m_EnergySourceListMutex(),
  m_Size(_size),
  m_Radius(_radius)
{
  readEnergySourceLocations(fName);
}

EnergySources::~EnergySources()
{
}

double EnergySources::acquireEnergySource(int row, int col)
{
  // Lock
  // if available return eliminate from m_EnergySourceLocations and return
  // true
  // else false
  
  bool found = false;
  double resLevel = 0.0;
  PLEXIL::ThreadMutexGuard mg(m_EnergySourceListMutex);
  
  for (std::vector<std::vector<int> >::iterator iter = m_EnergySourceLocations.begin();
       (iter != m_EnergySourceLocations.end()) && !found; ++iter)
    {
      if (((*iter)[0] == row) && (*iter)[1] == col)
        {
          resLevel = 1.0;
          m_EnergySourceLocations.erase(iter);
          found = true;
        }
    }
  
  return resLevel;
}

double EnergySources::determineEnergySourceLevel(int rowCurr, int colCurr)
{
  // Loop through each resource and return the max.
  
  PLEXIL::ThreadMutexGuard mg(m_EnergySourceListMutex);
  
  double maxValue = -1.0 * static_cast<double>(INT_MAX);
  
  for (unsigned int i = 0; i < m_EnergySourceLocations.size(); ++i)
    {
      int row = m_EnergySourceLocations[i][0];
      int col = m_EnergySourceLocations[i][1];
      
      // Get Euclidean distance
      double d = EUCLIDEAN_DISTANCE(rowCurr, colCurr, row, col);
      
      // linearly interpolate value of distance is < m_Radius. Else = 0;
      double rValue = (d < m_Radius) ? (1.0 - d / m_Radius) : 0.0;
      
      if (rValue > maxValue) maxValue = rValue;
    }
  
  return maxValue;
}

void EnergySources::displayEnergySources()
{
  double rWidth = 2.0 / static_cast<double>(m_Size);
  double radius = m_Radius * rWidth;
  
  PLEXIL::ThreadMutexGuard mg(m_EnergySourceListMutex);
  for (unsigned int i = 0; i < m_EnergySourceLocations.size(); ++i)
    {
      int row = m_EnergySourceLocations[i][0];
      int col = m_EnergySourceLocations[i][1];
      
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(.75, 0.0, .75);
      
      glVertex2f(-1.0+col*rWidth+rWidth/2.0, 1.0-row*rWidth-rWidth/2.0);
      
      glColor3f(0.0, 0.0, 0.0);
      for (double theta = 0; theta <= 360; theta += 10.0)
        {
          glVertex2f(-1.0+col*rWidth+rWidth/2.0 + radius*cos(theta*PI/180.0), 
                     1.0-row*rWidth-rWidth/2.0-radius*sin(theta*PI/180.0));
        }
      glEnd();   
    }
}

bool EnergySources::parseOneElement(const std::string& delimiter, const std::string& dataStr,
                                std::string::size_type& pos, std::string& result)
{
  std::string::size_type lastPos = dataStr.find_first_not_of(delimiter, pos);
  pos = dataStr.find_first_of(delimiter, lastPos);
  if ((std::string::npos == pos) && (std::string::npos == lastPos))
    {
      std::cerr << "Error reading element [" << lastPos << ", " << pos 
                << "] in string: " << dataStr << std::endl;
      return false;
    }
  result = dataStr.substr(lastPos, pos - lastPos);
  return true;
}

void EnergySources::readEnergySourceLocations(const std::string& fName)
{
  std::string dataStr;
  std::string delimiter = " ";
  std::ifstream myFile;
  
  myFile.open(fName.c_str());
  if (!myFile.is_open())
    {
      std::cerr << "The file: " << fName << " does not exist" << std::endl;
      exit(0);
    }
  while (!myFile.eof())
    {
      std::getline(myFile, dataStr);
      
      if (dataStr.substr(0,1) != "%")
        {
          std::string::size_type pos = 0; 
          std::string result;
          
          std::vector<int> eachRow;
          for (int c = 0; c < 2; ++c)
            {
              if (parseOneElement(delimiter, dataStr, pos, result))
                {
                  eachRow.push_back(atoi(result.c_str()));
                }
              else
                {
                  myFile.close();
                  exit(0);
                }
            }
          m_EnergySourceLocations.push_back(eachRow);
        }
    }
  myFile.close();
}

