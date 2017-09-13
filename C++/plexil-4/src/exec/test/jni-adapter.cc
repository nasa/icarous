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

#include "JNIUtils.hh"
#include "ScopedOstreamRedirect.hh"
#include "lifecycle-utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <time.h>

using PLEXIL::JNIUtils;

extern bool stateTransitionTests();

std::string *logFileName(const char* dirname)
{
	// Get current time and decode it in the local timeframe
	time_t now;
	time(&now);
	struct tm nowtm;
	localtime_r(&now, &nowtm);

	std::ostringstream fnameBuilder;
	fnameBuilder << dirname << "/exec-module-test-"
			<< nowtm.tm_year + 1900
			<< '-'
			<< std::setw(2) << std::setfill('0')
			<< nowtm.tm_mon + 1 << '-'
			<< std::setw(2) << nowtm.tm_mday << '_'
			<< std::setw(2) << nowtm.tm_hour << ':'
			<< std::setw(2) << nowtm.tm_min << ':'
			<< std::setw(2) << nowtm.tm_sec
			<< ".log";
	return new std::string(fnameBuilder.str());
}

extern "C"
jint Java_gov_nasa_plexil_android_ExecModuleTest_run(JNIEnv *env, jobject /* java_this */, jstring logDirJstring)
{
  JNIUtils jni(env);
  char* logDir = jni.getJavaStringCopy(logDirJstring);
  if (logDir == NULL)
	return -1;

  // Route cout and cerr to a log file.
  std::string* logName = logFileName(logDir);
  std::ofstream log(logName->c_str());
  if (log.fail())
	return -1;
  PLEXIL::ScopedOstreamRedirect coutRedirect(std::cout, log);
  PLEXIL::ScopedOstreamRedirect cerrRedirect(std::cerr, log);

  stateTransitionTests();
  delete logName;
  delete logDir;
  plexilRunFinalizers();
  return 0;
}

