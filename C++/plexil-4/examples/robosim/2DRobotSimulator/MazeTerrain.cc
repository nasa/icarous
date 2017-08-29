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

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "MyOpenGL.hh"
#include "MazeTerrain.hh"

MazeTerrain::MazeTerrain(const std::string& fName)
{
  readTerrain(fName);
}

MazeTerrain::~MazeTerrain()
{
}
  
void MazeTerrain::displayFixedTerrain()
{
  double cellW = 2.0 / getWidth();
  double cellH = 2.0 / getHeight();
  double delta = 0.005;
  
  // The walls
  for (unsigned int i = 0; i < m_Terrain.size(); ++i)
    {
      std::vector<int> eachRow = m_Terrain[i];
      
      if (eachRow[0] == eachRow[2])
        {
          // vertical wall
          glBegin(GL_POLYGON);
          glColor3f(1.0, 0.0, 0.0);
          glVertex2f(-1.0+eachRow[3]*cellW-delta, 1.0-eachRow[0]*cellH);
          glVertex2f(-1.0+eachRow[3]*cellW+delta, 1.0-eachRow[0]*cellH);
          glVertex2f(-1.0+eachRow[3]*cellW+delta, 1.0-(eachRow[0]+1)*cellH);
          glVertex2f(-1.0+eachRow[3]*cellW-delta, 1.0-(eachRow[0]+1)*cellH);
          glEnd();
        }
      else
        {
          // horz wall
          glBegin(GL_POLYGON);
          glColor3f(1.0, 0.0, 0.0);
          glVertex2f(-1.0+eachRow[1]*cellW, 1.0-eachRow[2]*cellH+delta);
          glVertex2f(-1.0+(eachRow[1]+1)*cellW, 1.0-eachRow[2]*cellH+delta);
          glVertex2f(-1.0+(eachRow[1]+1)*cellW, 1.0-eachRow[2]*cellH-delta);
          glVertex2f(-1.0+eachRow[1]*cellW, 1.0-eachRow[2]*cellH-delta);
          glEnd();
        }
    }
}
  
  
bool MazeTerrain::isTraversable(int row1, int col1, int row2, int col2) const
{
  if ((row1 < 0) || (col1 < 0) || (row2 < 0) || (col2 < 0) ||
      (row1 >= getHeight()) || (col1 >= getWidth()) || 
      (row2 >= getHeight()) || (col2 >= getWidth()))
    return false;
  
  bool traversable = true;
  // Loop through the list of walls to find a matching cell pair.
  for (std::vector<std::vector<int> >::const_iterator wIter = m_Terrain.begin();
       (wIter != m_Terrain.end()) && traversable; ++wIter)
    {
      std::vector<int> aWall = *wIter;
      
      // Check if the cells on either side of the wall match.
      // left and right cells for vertical wall and
      // top and bottom cells for horizontal wall.
      if (((aWall[0] == row1) && (aWall[1] == col1) && 
           (aWall[2] == row2) && (aWall[3] == col2)) ||
          ((aWall[0] == row2) && (aWall[1] == col2) && 
           (aWall[2] == row1) && (aWall[3] == col1)))
        traversable = false;
    }
  
  return traversable;
}

bool MazeTerrain::parseOneElement(const std::string& delimiter, const std::string& dataStr,
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

void MazeTerrain::readTerrain(const std::string& fName)
{
  std::string dataStr;
  std::string delimiter = " ";
  std::ifstream myFile;
  int i = 0;
  
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
          
          if (i == 0)
            {
              if (parseOneElement(delimiter, dataStr, pos, result))
                {
                  setHeight() = atoi(result.c_str());
                }
              else
                {
                  myFile.close();
                  exit(0);
                }
              
              if (parseOneElement(delimiter, dataStr, pos, result))
                {
                  setWidth() = atoi(result.c_str());
                }
              else
                {
                  myFile.close();
                  exit(0);
                }
              ++i;
            }
          else
            {
              std::vector<int> eachRow;
              for (int c = 0; c < 4; ++c)
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
              /*
                for(std::vector<int>::const_iterator iter = eachRow.begin();
                iter != eachRow.end(); ++iter)
                std::cout << *iter << " ";
                std::cout << std::endl;
              */
              m_Terrain.push_back(eachRow);
            }
        }
    }
  myFile.close();
}
