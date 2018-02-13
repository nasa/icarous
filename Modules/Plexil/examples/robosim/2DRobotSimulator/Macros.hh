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

#ifndef MACROS_HH
#define MACROS_HH

#include <cstdlib>
#include <iostream>

#define PI 3.14159265

#define EUCLIDEAN_DISTANCE(r1, c1, r2, c2) sqrt((double) (pow(abs(r1-r2), 2) + pow(abs(c1-c2), 2)))
#define RANDOM_NUMBER_INT(minVal, maxVal) minVal + (int) ((double) (maxVal - minVal) * \
                                                      ((double) rand() / (double) RAND_MAX) \
                                                      + 0.5)
#define CHECK_READ_DATA(pos, lastPos, myFile) if ((std::string::npos == pos) && (std::string::npos == lastPos)) \
                                                {\
                                                  std::cerr << "Error reading element [" << lastPos << ", " << pos \
                                                            << "] in string: " << dataStr << std::endl;\
                                                  myFile.close();\
                                                  exit(0);       \
                                                }

#endif // MACROS_HH
