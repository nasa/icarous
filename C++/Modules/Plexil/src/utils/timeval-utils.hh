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

//
// Utilities for timeval arithmetic
//

struct timeval;

//* @brief Returns true if the first argument is less than the second, false otherwise.
bool operator<(const struct timeval& t1, const struct timeval& t2);

//* @brief Returns true if the first argument is greater than the second, false otherwise.
bool operator>(const struct timeval& t1, const struct timeval& t2);

//* @brief Returns true if the first argument is identical to the second, false otherwise.
bool operator==(const struct timeval& t1, const struct timeval& t2);

//* @brief Returns the arithmetic sum of its two arguments.
struct timeval operator+ (const struct timeval& t1, const struct timeval& t2);

//* @brief Returns the first argument minus the second argument.
struct timeval operator- (const struct timeval& t1, const struct timeval& t2);

//* @brief Conversion operator
struct timeval doubleToTimeval(double d);

//* @brief Conversion operator
void doubleToTimeval(double d, struct timeval& result);

//* @brief Conversion operator
double timevalToDouble(const struct timeval& tv);
