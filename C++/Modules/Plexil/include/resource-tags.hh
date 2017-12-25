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

#ifndef RESOURCE_TAGS_HH
#define RESOURCE_TAGS_HH

namespace PLEXIL
{
  // Used only by XML parser
  extern const char *RESOURCE_TAG; //  = "Resource";
  extern const char *RESOURCE_LIST_TAG; // = "ResourceList";
  // Used by XML parser and resource arbiter interface
  extern const char *RESOURCE_NAME_TAG; // = "ResourceName";
  extern const char *RESOURCE_PRIORITY_TAG; // = "ResourcePriority";
  // Used only by resource arbiter (at present)
  extern const char *RESOURCE_LOWER_BOUND_TAG; // ="ResourceLowerBound";
  extern const char *RESOURCE_UPPER_BOUND_TAG; // = "ResourceUpperBound";
  extern const char *RESOURCE_RELEASE_AT_TERMINATION_TAG; // = "ResourceReleaseAtTermination";
}

#endif // RESOURCE_TAGS_HH
