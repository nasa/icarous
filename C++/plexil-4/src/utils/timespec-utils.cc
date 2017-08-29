/* Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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
// Utilities for timespec arithmetic
//

#include "plexil-config.h"

#ifdef HAVE_SYS_TIME_H 
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <cmath>

const long ONE_BILLION = 1000000000;
const double ONE_BILLION_DOUBLE = 1000000000.0;

//
// General utility for normalizing timespecs after arithmetic
// Assumes |tv_nsec| < 1,999,999,999
// Also assumes time_t is an integer type - which POSIX does not guarantee!
//

//
// what are typical boundary cases?
//  tv_sec = 0,  tv_nsec = -1,999,999,998 (-0.999999999 + -0.999999999) - nsec underflow
//  tv_sec = 1,  tv_nsec = -1 (1 - .000000001)
// is it possible to have over/underflow AND opposing signs from simple add/subtract
// of two valid timespecs?
//

void timespecNormalize(struct timespec& tv)
{
  // check for nsec over/underflow
  if (tv.tv_nsec >= ONE_BILLION) {
    tv.tv_sec += 1;
    tv.tv_nsec -= ONE_BILLION;
  }
  else if (tv.tv_nsec + ONE_BILLION <= 0) {
    tv.tv_sec -= 1;
    tv.tv_nsec += ONE_BILLION;
  }

  // now check that signs are consistent
  if (tv.tv_sec > 0 && tv.tv_nsec < 0) {
    tv.tv_sec -= 1;
    tv.tv_nsec += ONE_BILLION;
  }
  else if (tv.tv_sec < 0 && tv.tv_nsec > 0) {
    tv.tv_sec += 1;
    tv.tv_nsec -= ONE_BILLION;
  }
}

bool operator<(const struct timespec& t1, const struct timespec& t2)
{
  return ((t1.tv_sec < t2.tv_sec) || 
          ((t1.tv_sec == t2.tv_sec) && (t1.tv_nsec < t2.tv_nsec)));
}

bool operator>(const struct timespec& t1, const struct timespec& t2)
{
  return ((t1.tv_sec > t2.tv_sec) || 
          ((t1.tv_sec == t2.tv_sec) && (t1.tv_nsec > t2.tv_nsec)));
}

bool operator==(const struct timespec& t1, const struct timespec& t2)
{
  return (t1.tv_sec == t2.tv_sec) && (t1.tv_nsec == t2.tv_nsec);
}

struct timespec operator+(const struct timespec& t1, const struct timespec& t2)
{
  struct timespec time = {t1.tv_sec + t2.tv_sec,
                          t1.tv_nsec + t2.tv_nsec};
  timespecNormalize(time);
  return time;
}

struct timespec operator-(const struct timespec& t1, const struct timespec& t2)
{
  struct timespec time = {t1.tv_sec - t2.tv_sec,
                          t1.tv_nsec - t2.tv_nsec};
  timespecNormalize(time);
  return time;
}

void doubleToTimespec(double d, timespec& result)
{
  double seconds = 0;
  double fraction = modf(d, &seconds);

  result.tv_sec = (time_t) seconds;
  result.tv_nsec = (long) (fraction * ONE_BILLION_DOUBLE);
}

struct timespec doubleToTimespec(double d)
{
  timespec result;
  doubleToTimespec(d, result);
  return result;
}

double timespecToDouble(const struct timespec& tv)
{
  double result = ((double) tv.tv_nsec / ONE_BILLION_DOUBLE);
  result += (double) tv.tv_sec;
  return result;
}
