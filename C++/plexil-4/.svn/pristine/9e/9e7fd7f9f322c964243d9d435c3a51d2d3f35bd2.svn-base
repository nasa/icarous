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

#ifndef PLEXIL_COMMAND_HANDLE_VARIABLE_HH
#define PLEXIL_COMMAND_HANDLE_VARIABLE_HH

#include "CommandHandle.hh"
#include "GetValueImpl.hh"
#include "NotifierImpl.hh"

namespace PLEXIL
{
  // Forward reference
  class Command;

  class CommandHandleVariable :
    public GetValueImpl<uint16_t>,
    public NotifierImpl
  {
  public:
    /**
     * @brief Constructor.
     */
    CommandHandleVariable(Command const &cmd);

    /**
     * @brief Destructor.
     */
    ~CommandHandleVariable();

    char const *getName() const;

    char const *exprName() const
    {
      return "CommandHandleVariable";
    }

    /**
     * @brief Get the type of the expression's value.
     */
    ValueType valueType() const
    {
      return COMMAND_HANDLE_TYPE;
    }

    bool isKnown() const;

    /**
     * @brief Get the current value of the command handle.
     */
    bool getValue(uint16_t &) const;

    // Command notifies variable when value changes
    void valueChanged();

    /**
     * @brief Print the expression's value to the given stream.
     * @param s The output stream.
     */
    void printValue(std::ostream& s) const;

    void setName(const std::string &);

  private:

    // Not implemented
    CommandHandleVariable();
    CommandHandleVariable(const CommandHandleVariable &);
    CommandHandleVariable &operator=(const CommandHandleVariable &);

    Command const & m_command;
    char const *m_name;
  };

} // namespace PLEXIL

#endif // PLEXIL_COMMAND_HANDLE_VARIABLE_HH
