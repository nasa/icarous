/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

/*
 * DynamicLoader.h
 *
 * Convenience wrapper for loading dynamic libraries and initializing them.
 *
 *  Created on: Jan 27, 2010
 *      Author: jhogins
 */

#ifndef DYNAMICLOADER_H_
#define DYNAMICLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // for NULL
    
  /**
   * @brief Dynamically load the shared library containing the module name, using the library name if provided.
   * @param moduleName The name of the module
   * @param libPath The library name containing the module; defaults to NULL.
   * @return true if successful, false otherwise.
   * @note If libPath is not provided, attempts to load 'lib<moduleName><LIB_EXT>'.
   * @note Expects to call init<moduleName>() with no args to initialize the freshly loaded module.
   */
  
  bool dynamicLoadModule(const char* moduleName, 
                         const char* libPath);
  
#ifdef __cplusplus
}
#endif

#endif /* DYNAMICLOADER_H_ */
