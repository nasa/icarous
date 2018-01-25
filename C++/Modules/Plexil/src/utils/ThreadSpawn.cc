/* Copyright (c) 2006-2013, Universities Space Research Association (USRA).
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

#include "ThreadSpawn.hh"
#include <iostream>
#include <cerrno>

bool threadSpawn(void* (*threadFunc)(void*), void *arg, pthread_t& thread_id) 
{
  const size_t stacksize = 0x10000;
  pthread_attr_t pthread_attr;
  int nRet;

  // Initialize the pthread_attr_t struct
  switch ((nRet = pthread_attr_init(&pthread_attr))) {
  case 0: break;
  case ENOMEM: 
    std::cerr << "Insufficient memory exists to create the thread attribute object" << std::endl;
    return false;
  case EINVAL:
    std::cerr << "&pthread_attr does not point to writeable memory" << std::endl;
    return false;
  case EFAULT:
    std::cerr << "&pthread_attr is an invalid pointer" << std::endl;
    return false;
  default:
    std::cerr << "unknown error " << nRet << std::endl; 
    return false;
  }

  // Set the stack size in the pthread_attr_t struct
  nRet = pthread_attr_setstacksize(&pthread_attr, stacksize);
  switch (nRet) {
  case 0: 
    break;
  case EINVAL: 
    std::cerr << "setstacksize invalid" << std::endl; 
    return false;
  default: 
    std::cerr << "unknown error " << nRet << std::endl; 
    return false;
  } 
  
  //  pthread_t thread_id;
  nRet = pthread_create(&thread_id,                  // Thread id
                        &pthread_attr,               // Use default attributes
                        (THREAD_FUNC_PTR)threadFunc, // Thread function
                        arg                          // Argument to thread function
                        );
  if (nRet !=0)
    std::cerr << "Error " << nRet << " occurred while spawning thread" << std::endl;
  //  else
  //    pthread_detach(thread_id); // TODO: do we need this

  return (nRet == 0);
}
