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

#ifndef IPC_DATA_FORMATS_H
#define IPC_DATA_FORMATS_H

#include "plexil-config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#elif defined(__VXWORKS__)
#include <vxWorks.h>
#endif

#include <limits.h>

/*
 * Data formats used by IpcAdapter
 */

struct PlexilMsgBase
{
  uint16_t msgType;
  uint16_t count;   /* either # of msgs to follow, or index of this msg in the sequence */
  uint32_t serial;  /* uniquely IDs this message sequence from this sender */
  const char* senderUID;  /* uniquely IDs the sender's IP and process */
};

#define MSG_BASE "PlexilMessageBase"
#define MSG_BASE_FORMAT "{ushort, ushort, uint, string}"

/* something is whacked with UINT16_MAX! */
#ifndef UINT16_MAX
#define UINT16_MAX (65535)
#endif

#define MSG_COUNT_CMD_ACK UINT16_MAX

/*
 * Used for return values
 * Followed by count data messages
 */

struct PlexilReturnValuesMsg
{
  struct PlexilMsgBase header;
  uint32_t requestSerial;
  const char* requesterUID;
};

#define RETURN_VALUE_MSG "PlexilReturnValueMessage"
#define RETURN_VALUE_MSG_FORMAT "{ushort, ushort, uint, string, uint, string}"

/*
 * Unknown carries no value
 */

struct PlexilUnknownValueMsg
{
  struct PlexilMsgBase header;
};

#define UNKNOWN_VALUE_MSG "PlexilUnknownValue"
#define UNKNOWN_VALUE_MSG_FORMAT MSG_BASE_FORMAT

/*
 * Used for command handle (acknowledge) value
 */

struct PlexilCommandHandleValueMsg
{
  struct PlexilMsgBase header;
  uint16_t commandHandleValue;
};

#define COMMAND_HANDLE_VALUE_MSG "PlexilCommandHandleValue"
#define COMMAND_HANDLE_VALUE_MSG_FORMAT "{ushort, ushort, uint, string, ushort}"

/*
 * Used for Boolean argument or return values
 */

struct PlexilBooleanValueMsg
{
  struct PlexilMsgBase header;
  unsigned char boolValue;
};

#define BOOLEAN_VALUE_MSG "PlexilBooleanValue"
#define BOOLEAN_VALUE_MSG_FORMAT "{ushort, ushort, uint, string, ubyte}"

/*
 * Used for Integer argument or return values
 */

struct PlexilIntegerValueMsg
{
  struct PlexilMsgBase header;
  int32_t intValue;
};

#define INTEGER_VALUE_MSG "PlexilIntegerValue"
#define INTEGER_VALUE_MSG_FORMAT "{ushort, ushort, uint, string, int}"

/*
 * Used for Real argument or return values
 */

struct PlexilRealValueMsg
{
  struct PlexilMsgBase header;
  double doubleValue;
};

#define REAL_VALUE_MSG "PlexilRealValue"
#define REAL_VALUE_MSG_FORMAT "{ushort, ushort, uint, string, double}"

/*
 * When used for commands, lookups:
 *  followed by count parameter values
 * When used for planner updates: 
 *  string value is node name, followed by count name/value pairs
 * When used for messages, plans, plan files, libraries, library files: 
 *  stands alone, count ignored
 * When used as telemetry value leader:
 *  string value is state name, followed by count value messages
 * When used for numeric argument or return values:
 *  preceded by corresponding message type, 
 *  count indicates position in sequence
 */

struct PlexilStringValueMsg
{
  struct PlexilMsgBase header;
  const char* stringValue;
};

#define STRING_VALUE_MSG "PlexilStringValue"
#define STRING_VALUE_MSG_FORMAT "{ushort, ushort, uint, string, string}"

/*
 * Arrays and other argument types here (NYI)
 */

struct PlexilStringArrayMsg
{
  ~PlexilStringArrayMsg() {
    delete [] stringArray;
  }
  struct PlexilMsgBase header;
  uint32_t arraySize;
  const char** stringArray;
};

#define STRING_ARRAY_MSG "PlexilStringArray"
#define STRING_ARRAY_MSG_FORMAT "{ushort, ushort, uint, string, int, <string:5>}"

/*
 * Used for Real array argument or return values
 */

struct PlexilRealArrayMsg
{
  struct PlexilMsgBase header;
  uint32_t arraySize;
  double* doubleArray;

  ~PlexilRealArrayMsg()
  {
    delete doubleArray;
  }
};

#define REAL_ARRAY_MSG "PlexilRealArray"
#define REAL_ARRAY_MSG_FORMAT "{ushort, ushort, uint, string, int, <double:5>}"

/*
 * Used for Integer array argument or return values
 */

struct PlexilIntegerArrayMsg
{
  struct PlexilMsgBase header;
  uint32_t arraySize;
  int32_t* intArray;

  ~PlexilIntegerArrayMsg()
  {
    delete intArray;
  }
};

#define INTEGER_ARRAY_MSG "PlexilIntegerArray"
#define INTEGER_ARRAY_MSG_FORMAT "{ushort, ushort, uint, string, int, <int:5>}"

/*
 * Used for Boolean array argument or return values
 */

struct PlexilBooleanArrayMsg
{
  struct PlexilMsgBase header;
  uint32_t arraySize;
  unsigned char* boolArray;

  ~PlexilBooleanArrayMsg()
  {
    delete boolArray;
  }
};

#define BOOLEAN_ARRAY_MSG "PlexilBooleanArray"
#define BOOLEAN_ARRAY_MSG_FORMAT "{ushort, ushort, uint, string, int, <ubyte:5>}"

struct PairHeader
{
  struct PlexilMsgBase datumHeader;
  const char* pairName;
};

struct BooleanPair
{
  struct PairHeader pairHeader;
  unsigned char pairBoolValue;
};

#define BOOLEAN_PAIR_MSG "PlexilBooleanPair"
#define BOOLEAN_PAIR_MSG_FORMAT "{ushort, ushort, uint, string, string, ubyte}"

struct IntegerPair
{
  struct PairHeader pairHeader;
  int32_t pairIntValue;
};

#define INTEGER_PAIR_MSG "PlexilIntegerPair"
#define INTEGER_PAIR_MSG_FORMAT "{ushort, ushort, uint, string, string, int}"

struct RealPair
{
  struct PairHeader pairHeader;
  double pairDoubleValue;
};

#define REAL_PAIR_MSG "PlexilRealPair"
#define REAL_PAIR_MSG_FORMAT "{ushort, ushort, uint, string, string, double}"

struct StringPair
{
  struct PairHeader pairHeader;
  const char *pairStringValue;
};

#define STRING_PAIR_MSG "PlexilStringPair"
#define STRING_PAIR_MSG_FORMAT "{ushort, ushort, uint, string, string, string}"

typedef enum {
  PlexilMsgType_uninited=0,

  /* PlexilMsgBase - 
   * These messages are complete unto themselves */
  PlexilMsgType_NotifyExec,

  /* PlexilStringValueMsg -
   * These have an operation (or state) name and an argument count */
  PlexilMsgType_Command,
  PlexilMsgType_Message,
  PlexilMsgType_LookupNow,
  PlexilMsgType_TelemetryValues,

  /* PlexilReturnValuesMsg - 
   * These have a unique ID of the requested operation and count of values (which may be 0) */
  PlexilMsgType_ReturnValues,

  /* PlexilStringValueMsg -
   * These have one (non-empty?) string datum, the plan or file name
   * Count must be 0 */
  PlexilMsgType_AddPlan,
  PlexilMsgType_AddPlanFile,
  PlexilMsgType_AddLibrary,
  PlexilMsgType_AddLibraryFile,

  /* PlexilStringValueMsg -
   * These have a node name and a pair count (which may be 0?) */
  PlexilMsgType_PlannerUpdate,

  /* PlexilUnknownValueMsg - 
   * no datum
   * Count indicates position in sequence */
  PlexilMsgType_UnknownValue,

  /* PlexilCommandHandleValueMsg -
   * Enumeration datum
   * Count indicates position in sequence */
  PlexilMsgType_CommandHandleValue,

  /* PlexilBooleanValueMsg -
   * Simple bool datum
   * Count indicates position in sequence */
  PlexilMsgType_BooleanValue,
  PlexilMsgType_BooleanArray,

  /* PlexilIntegerValueMsg -
   * A simple integer datum
   * Count indicates position in sequence */
  PlexilMsgType_IntegerValue,
  PlexilMsgType_IntegerArray,

  /* PlexilRealValueMsg -
   * A simple real datum
   * Count indicates position in sequence */
  PlexilMsgType_RealValue,
  PlexilMsgType_RealArray,

  /* PlexilStringValueMsg -
   * A simple string datum
   * Count indicates position in sequence */
  PlexilMsgType_StringValue,
  PlexilMsgType_StringArray,

  /* BooleanPair -
   * A pair of a name and a boolean value
   * Count indicates position in sequence */
  PlexilMsgType_PairBoolean,

  /* IntegerPair -
   * A pair of a name and a integer value
   * Count indicates position in sequence */
  PlexilMsgType_PairInteger,

  /* RealPair -
   * A pair of a name and a real value
   * Count indicates position in sequence */
  PlexilMsgType_PairReal,

  /* StringPair -
   * A pair of a name and a string value
   * Count indicates position in sequence */
  PlexilMsgType_PairString,

  PlexilMsgType_limit
}
  PlexilMsgType;

#ifdef __cplusplus
}
#endif

#endif /* IPC_DATA_FORMATS_H */
