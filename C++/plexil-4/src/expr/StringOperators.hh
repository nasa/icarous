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

#ifndef PLEXIL_STRING_OPERATORS_HH
#define PLEXIL_STRING_OPERATORS_HH

#include "Expression.hh"
#include "OperatorImpl.hh"

namespace PLEXIL
{
  class StringConcat : public OperatorImpl<String>
  {
  public:
    ~StringConcat();

    bool checkArgCount(size_t count) const;

    bool checkArgTypes(Function const *ev) const;

    bool operator()(String &result) const;

    bool operator()(String &result, Expression const *arg) const;

    bool operator()(String &result,
                    Expression const *argA,
                    Expression const *argB) const;

    bool operator()(String &result, Function const &f) const;

    DECLARE_OPERATOR_STATIC_INSTANCE(StringConcat, String);

  private:
    StringConcat();

    // Disallow copy, assignment
    StringConcat(const StringConcat &other);
    StringConcat &operator=(const StringConcat& other);
  };

  class StringLength : public OperatorImpl<Integer>
  {
  public:
    ~StringLength();

    bool checkArgCount(size_t count) const;

    bool checkArgTypes(Function const *ev) const;

    bool operator()(Integer &result, Expression const *arg) const;

    DECLARE_OPERATOR_STATIC_INSTANCE(StringLength, Integer);

  private:
    StringLength();

    // Disallow copy, assignment
    StringLength(const StringLength &other);
    StringLength &operator=(const StringLength& other);
  };

  // more to come

} // namespace PLEXIL

#endif // PLEXIL_STRING_OPERATORS_HH
