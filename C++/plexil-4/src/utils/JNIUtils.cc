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

#include "JNIUtils.hh"
#include "Error.hh"
#include "Debug.hh"
#include <stddef.h> // for NULL

//
// Utilities for accessing Java data
//

namespace PLEXIL 
{

  JNIUtils::JNIUtils(JNIEnv* env)
    : m_env(env),
      m_stringClass(NULL)
  {
    assertTrueMsg(m_env != NULL,
                  "JNIUtils constructor: JNI environment is NULL");
  }

  JNIUtils::JNIUtils(const JNIUtils& other)
    : m_env(other.m_env)
  {
    assertTrueMsg(m_env != NULL,
                  "JNIUtils constructor: JNI environment is NULL");
  }

  JNIUtils::~JNIUtils()
  {
    // Clean up JNI ref
    if (m_stringClass != NULL)
      m_env->DeleteLocalRef(m_stringClass);
  }

  JNIUtils& JNIUtils::operator=(const JNIUtils& orig)
  {
    m_env = orig.m_env;
    assertTrueMsg(m_env != NULL,
                  "JNIUtils::operator=: JNI environment is NULL");
    return *this;
  }


  /**
   * @brief Returns a freshly allocated copy of the Java string.
   * @param javaString The JNI string object.
   * @return A freshly allocated copy of the Java string, or NULL.
   */
  char* JNIUtils::getJavaStringCopy(jstring javaString)
  {
    if (javaString == NULL)
      return NULL;

    jsize utflen = m_env->GetStringUTFLength(javaString);
    char* ourString = new char[utflen + 1];
    m_env->GetStringUTFRegion(javaString, 0, m_env->GetStringLength(javaString), ourString);
    ourString[utflen] = '\0'; // to be safe
    return ourString;
  }

  /**
   * @brief Extract the strings from a Java string array in argc/argv format.
   * @param javaArgv The JNI string array.
   * @param argcReturn A reference to an int variable to hold the argument count.
   * @param argvReturn A reference to a char** variable to hold the newly allocated argument vector.
   * @return true if the operation was successful, false otherwise.
   * @note Both the strings and the string array are freshly allocated and must be deleted by the caller.
   */
  bool JNIUtils::getArgcArgv(jobjectArray javaArgv, int &argcReturn, char** &argvReturn)
  {
    // Get argv length
    int argc = m_env->GetArrayLength(javaArgv);

    // Allocate and initialize argv
    char** argv = new char*[argc + 1];
    for (unsigned int i = 0; i <= argc; i++)
      argv[i] = NULL;
       
    // Copy the strings
    for (unsigned int i = 0; i < argc; i++) {
      jstring java_string = (jstring) m_env->GetObjectArrayElement(javaArgv, i);
      if (java_string == NULL) {
        debugMsg("JNIUtils:getArgcArgv", "GetObjectArrayElement returned NULL");
        // FIXME: clean up all allocated structures here
        return false;
      }
      argv[i] = getJavaStringCopy(java_string);
    }

    argcReturn = argc;
    argvReturn = argv;
    return true;
  }


  /**
   * @brief Constructs a Java String object from the given C char* object.
   * @param cstr Pointer to a C character string.
   * @return A freshly allocated Java jstring object.
   */
  jstring JNIUtils::makeJavaString(const char* cstr)
  {
    return m_env->NewStringUTF(cstr);
  }


  /**
   * @brief Constructs a Java array of String objects.
   * @param size The number of elements in the array.
   * @return A freshly allocated Java string array object.
   */
  jobjectArray JNIUtils::makeJavaStringArray(jsize size)
  {
    if (m_stringClass == NULL)
      m_stringClass = m_env->FindClass("java/lang/String");
    assertTrueMsg(m_stringClass != NULL,
                  "FindClass failed to find Java string class");

    return m_env->NewObjectArray(size, m_stringClass, NULL);
  }

  /**
   * @brief Given a Java String[], return an array of char*.
   * @param The Java String[] object.
   * @return A freshly allocated vector of strings.
   */
  std::vector<std::string>* JNIUtils::getJavaStringArray(jobjectArray ary)
  {
    assertTrue_1(ary != NULL);

    jsize n = m_env->GetArrayLength(ary);
    std::vector<std::string>* result = new std::vector<std::string>(n);
    for (jsize i = 0; i < n; i++) {
      jstring javastr = (jstring) m_env->GetObjectArrayElement(ary, i);
      if (javastr != NULL) {
        const char* utf = m_env->GetStringUTFChars(javastr, NULL);
        (*result)[i] = utf;
        m_env->ReleaseStringUTFChars(javastr, utf);
      }
      m_env->DeleteLocalRef(javastr);
    }
    return result;
  }

}
