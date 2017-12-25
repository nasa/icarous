/* Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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

#include "jni-interface.hh"
#include "ExecTestRunner.hh"
#include <stddef.h> // for NULL

extern "C"
jint PlexilTestExec(JNIEnv *env, jobject /* java_this */, jobjectArray java_argv)
{
  // Get argv length
  int argc = env->GetArrayLength(java_argv);

  // Allocate and initialize argv
  char** argv = new char*[argc + 1];
  for (unsigned int i = 0; i <= argc; i++)
	argv[i] = NULL;
	   
  // Copy the strings
  for (unsigned int i = 0; i < argc; i++) {
	jstring java_string = (jstring) env->GetObjectArrayElement(java_argv, i);
	if (java_string == NULL) {
      delete[] argv;
	  return -1;
	}
	jsize utflen = env->GetStringUTFLength(java_string);
	char* ourString = new char[utflen + 1];
	env->GetStringUTFRegion(java_string, 0, env->GetStringLength(java_string), ourString);
	ourString[utflen] = '\0'; // to be safe
	argv[i] = ourString;
  }

  // *** FIXME: Release the Java arg (is this needed?)

  // Call the TestExec
  int result = PLEXIL::ExecTestRunner::run(argc, argv);

  // Free argv
  for (unsigned int i = 0; i < argc; i++)
	if (argv[i] != NULL) {
	  delete argv[i];
	  argv[i] = NULL;
	}
  delete[] argv;

  // Return result
  return result;
}
