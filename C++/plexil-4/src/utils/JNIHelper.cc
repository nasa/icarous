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

#include "JNIHelper.hh"
#include "Debug.hh"
#include "Error.hh"

// Initialization for the static instance pointer
JNIHelper* JNIHelper::s_instance = NULL;

JNIHelper::JNIHelper(JNIEnv* env, jobject java_this) 
  : m_env(env),
	m_java_this(java_this),
	m_previous(s_instance),
	m_classClass(NULL),
	m_isArrayMethod(NULL),
	m_getNameMethod(NULL)
{
  debugMsg("JNIHelper",
		   " constructor, "
		   << (m_previous == NULL ? "no previous instance" : " reentrant call"));
  s_instance = this;
}

JNIHelper::~JNIHelper()
{
  assertTrue_2(s_instance == this, "JNIHelper destructor: this != s_instance");

  // Restore static instance pointer from reentrant calls
  debugMsg("JNIHelper",
		   " destructor, "
		   << (m_previous == NULL ? "stack empty" : " restoring previous instance"));
  s_instance = m_previous;

  // Clean up local references
  if (m_classClass != NULL)
	m_env->DeleteLocalRef(m_classClass);
}

JNIEnv* JNIHelper::getJNIEnv()
{
  assert(s_instance != NULL);
  return s_instance->m_env;
}

jobject JNIHelper::getJavaThis()
{
  assert(s_instance != NULL);
  return s_instance->m_java_this;
}

jclass JNIHelper::getClassClass()
{
  if (m_classClass == NULL) {
	debugMsg("JNIHelper:getClassClass", " fetching Class class");
	m_classClass = m_env->FindClass("java/lang/Class");
	assertTrue_2(m_classClass != NULL, "JNIHelper::getClassClass failed");
  }
  return m_classClass;
}

/**
 * @brief Return true if the object is an array, false otherwise.
 */
bool JNIHelper::isArray(jobject object)
{
  jclass objectClass = m_env->GetObjectClass(object);
  assertTrue_2(objectClass != NULL, "JNIHelper::isArray: GetObjectClass() returned NULL");
  bool result = isArrayClass(objectClass);
  m_env->DeleteLocalRef(objectClass);
  return result;
}

/**
 * @brief Return true if the class is an array class, false otherwise.
 */
bool JNIHelper::isArrayClass(jclass klass)
{
  if (m_isArrayMethod == NULL) {
	debugMsg("JNIHelper:isArrayClass", " fetching Class.isArray() method");
	m_isArrayMethod = m_env->GetMethodID(getClassClass(), "isArray", "()Z");
	assertTrue_2(m_isArrayMethod != NULL, "JNIHelper::isArrayClass: Failed to fetch Class.isArray() method");
  }
  debugMsg("JNIHelper:isArrayClass", " calling Class.isArray()");
  bool result = m_env->CallBooleanMethod(klass, m_isArrayMethod);
  debugMsg("JNIHelper:isArrayClass", " returning " << (result ? "true" : "false"));
  return result;
}


/**
 * @brief Get the class's name from the JNI.
 * @return Freshly allocated copy of the class name.
 * @note Caller must check result for NULL.
 */
char* JNIHelper::getClassName(jclass klass)
{
  if (m_getNameMethod == NULL) {
	debugMsg("JNIHelper:getClassName", " fetching Class.getName() method");
	m_getNameMethod = m_env->GetMethodID(getClassClass(), "getName", "()Ljava/lang/String;");
	assertTrue_2(m_getNameMethod != NULL, "JNIHelper::getClassName: Failed to fetch Class.getName() method");
  }
  jstring name = (jstring) m_env->CallObjectMethod(klass, m_getNameMethod);
  if (name == NULL) 
	return NULL;
  jsize nameLen = m_env->GetStringUTFLength(name);
  char* nameCopy = new char[nameLen + 1];
  m_env->GetStringUTFRegion(name, 0, m_env->GetStringUTFLength(name), nameCopy);
  nameCopy[nameLen] = '\0'; // to be safe
  m_env->DeleteLocalRef(name);
  return nameCopy;
}

