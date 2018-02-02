#*****************************************************************************
#        $Id: primFmttrs.py,v 1.1 2011/08/16 16:00:37 reids Exp $
# $Revision: 1.1 $
#     $Date: 2011/08/16 16:00:37 $
#   $Author: reids $
#    $State: Exp $
#   $Locker:  $
#
# PROJECT:	NM-DS1
#
# FILE:	primFmttrs.py
#
# DESCRIPTION: Classes for marshalling and unmarshalling IPC byte arrays
#              into Python objects..
#
# $Log: primFmttrs.py,v $
# Revision 1.1  2011/08/16 16:00:37  reids
# Adding Python interface to IPC
#
#
#****************************************************************************/

import _IPC # C functions
import sys

INT_FMT       = 1
BOOLEAN_FMT   = 2
FLOAT_FMT     = 3
DOUBLE_FMT    = 4
BYTE_FMT      = 5
STR_FMT       = 7
UBYTE_FMT     = 9
CHAR_FMT      = 16
SHORT_FMT     = 17
LONG_FMT      = 18
USHORT_FMT    = 28
UINT_FMT      = 29
ULONG_FMT     = 30
MAXFORMATTERS = 31

CHAR_SIZE   = 1
BYTE_SIZE   = 1
SHORT_SIZE  = 2
INT_SIZE    = 4
LONG_SIZE   = 4
FLOAT_SIZE  = 4
DOUBLE_SIZE = 8
PTR_SIZE    = 4

def getNthFieldName (object, n) :
  try :
    fieldName = object._fields[n]
    if (isinstance(fieldName, (tuple, list))) :
      return fieldName[0]
    else :
      return fieldName
  except (AttributeError, IndexError) :
    return "_f%d" % n

def findClass (className, parent=None) :
  split = className.split('.')
  slen = len(split)
  try :
    if (slen > 1) :
      module = sys.modules[split[slen-2]]
    elif (not parent is None) :
      module = sys.modules[parent.__module__]
    else :
      module = sys.modules['__main__']
    return module.__dict__[split[slen-1]]
  except KeyError :
    raise '%s not a valid class name' % className

def getNthFieldClass (object, n) :
  try :
    fieldName = object._fields[n]
    if (isinstance(fieldName, (tuple, list))) :
      fieldName = fieldName[1]
      if (isinstance(fieldName, (tuple, list))) :
        fieldName = fieldName[0]
      if (isinstance(fieldName, str)) :
        fieldName = findClass(fieldName, object)
      return fieldName
    else :
      return None
  except (AttributeError, IndexError) :
    return None

def getField (object, n, theClass=None) :
  fieldName = getNthFieldName(object, n)
  try :
     field = object.__dict__[fieldName]
     if (theClass is None or isinstance(field, theClass)) :
       return field
     else :
       raise "Field %s of %s not of class %s" % (fieldName, object, theClass.__name__)
  except KeyError:
    return None

def setField (object, n, theValue) :
  fieldName = getNthFieldName(object, n);
  object.__dict__[fieldName] = theValue

def setCharField (object, n, theChar) :
  if (isinstance(theChar, str) and len(theChar) == 1) :
    return setField(object, n, theChar)
  else :
    raise "%s is not a single character" % theChar

def getCharField (object, n) :
  theChar = getField(object, n, str)
  if (not theChar is None and len(theChar) == 1) :
    return theChar
  else:
    raise "%s (field %s of %s) is not a single character" % \
          (theChar, getNthFieldName(object, n), object)

def setBooleanField (object, n, theBoolean) :
  if (theBoolean in (0, 1)) :
    if (theBoolean == 1) : return setField(object, n, True)
    else : return setField(object, n, False)
  else :
    raise "%s is not Boolean" % theBoolean

def getBooleanField (object, n) :
  theBoolean = getField(object, n, bool)
  if (isinstance(theBoolean, bool)) :
    if (theBoolean == True) : return 1
    else : return 0
  else:
    raise "%s (field %s of %s) is not Boolean" % \
          (theBoolean, getNthFieldName(object, n), object)

def setByteField (object, n, theByte) :
  if (isinstance(theByte, int) and abs(theByte) <= 0XFF) :
    return setField(object, n, theByte)
  else :
    raise "%s is not a byte" % theByte

def getByteField (object, n) :
  theByte = getField(object, n, int)
  if (not theByte is None and abs(theByte) <= 0XFF) :
    return theByte
  else:
    raise "%s (field %s of %s) is not a byte" % \
          (theByte, getNthFieldName(object, n), object)

def setIntField (object, n, theInt) :
  if (isinstance(theInt, int) and abs(theInt) <= 0XFFFFFFFF) :
    return setField(object, n, theInt)
  else :
    raise "%s is not a int" % theInt

def getIntField (object, n) :
  theInt = getField(object, n, int)
  if (not theInt is None and abs(theInt) <= 0XFFFFFF) :
    return theInt
  else:
    raise "%s (field %s of %s) is not an int" % \
          (theInt, getNthFieldName(object, n), object)

def setShortField (object, n, theShort) :
  if (isinstance(theShort, int) and abs(theShort) <= 0XFFFF) :
    return setField(object, n, theShort)
  else :
    raise "%s is not a short" % theShort

def getShortField (object, n) :
  theShort = getField(object, n, int)
  if (not theShort is None and abs(theShort) <= 0XFFFF) :
    return theShort
  else:
    raise "%s (field %s of %s) is not a short" % \
          (theShort, getNthFieldName(object, n), object)

def setLongField (object, n, theLong) :
  if (isinstance(theLong, int)) :
    return setField(object, n, theLong)
  else :
    raise "%s is not a long" % theLong

def getLongField (object, n) :
  theLong = getField(object, n, int)
  if (not theLong is None) :
    return theLong
  else:
    raise "%s (field %s of %s) is not a long" % \
          (theLong, getNthFieldName(object, n), object)

def setFloatField (object, n, theFloat) :
  if (isinstance(theFloat, float)) :
    return setField(object, n, theFloat)
  else :
    raise "%s is not a float" % theFloat

def getFloatField (object, n) :
  theFloat = getField(object, n, float)
  if (not theFloat is None) :
    return theFloat
  else:
    raise "%s (field %s of %s) is not a float" % \
          (theFloat, getNthFieldName(object, n), object)

def setDoubleField (object, n, theDouble) :
  if (isinstance(theDouble, float)) :
    return setField(object, n, theDouble)
  else :
    raise "%s is not a double" % theDouble

def getDoubleField (object, n) :
  theDouble = getField(object, n, float)
  if (not theDouble is None) :
    return theDouble
  else:
    raise "%s (field %s of %s) is not a double" % \
          (theDouble, getNthFieldName(object, n), object)

def setStringField (object, n, theString) :
  if (isinstance(theString, str)) :
    return setField(object, n, theString)
  else :
    raise "%s is not a string" % theString

def getStringField (object, n) :
  theString = getField(object, n, str)
  if (not theString is None) :
    return theString
  else:
    raise "%s (field %s of %s) is not a string" % \
          (theString, getNthFieldName(object, n), object)

def setObjectField (object, n, theObject) :
  return setField(object, n, theObject)

def getObjectField (object, n) :
  return getField(object, n)

class TransFormat :
  def TransFormat (self, theTypeSize=0) : self.typeSize = theTypeSize
  def Encode (self, dataStruct, dstart, buffer) : pass
  def Decode (self, dataStruct, dstart, buffer) : pass
  def ELength (self, dataStruct, dstart) : return self.typeSize
  def ALength (self) : return 1
  def SimpleType (self) : return True
  def EncodeElement (self, array, index, buffer) : pass
  def DecodeElement (self, array, index, buffer) : pass

class STR_Trans(TransFormat) :
  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutString(buffer, getStringField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setStringField(dataStruct, dstart, _IPC.formatGetString(buffer))

  def ELength (self, dataStruct, dstart) :
    # One int for the size, plus the number of characters (or 1 if empty string)
    if (isinstance(dataStruct, str)) : strlen = len(dataStruct)
    else : strlen = len(getStringField(dataStruct, dstart))
    if (strlen == 0) : strlen = 1
    return INT_SIZE + strlen;

  def SimpleType (self) : return False

  def EncodeElement(self, array, index, buffer) :
    _IPC.formatPutString(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetString(buffer)

class BYTE_Trans(TransFormat) :
  def __init__(self) : self.typeSize = BYTE_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutByte(buffer, getByteField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setByteField(dataStruct, dstart, _IPC.formatGetByte(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutByte(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetByte(buffer)

class UBYTE_Trans(TransFormat) :
  def __init__(self) : self.typeSize = BYTE_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutUByte(buffer, getByteField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setByteField(dataStruct, dstart, _IPC.formatGetUByte(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutUByte(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetUByte(buffer)

class SHORT_Trans(TransFormat) :
  def __init__(self) : self.typeSize = SHORT_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutShort(buffer, getShortField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setShortField(dataStruct, dstart, _IPC.formatGetShort(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutShort(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetShort(buffer)

class INT_Trans(TransFormat) :
  def __init__(self) : self.typeSize = INT_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutInt(buffer, getIntField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setIntField(dataStruct, dstart, _IPC.formatGetInt(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutInt(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetInt(buffer)

class CHAR_Trans(TransFormat) :
  def __init__(self) : self.typeSize = CHAR_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutChar(buffer, getCharField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setCharField(dataStruct, dstart, _IPC.formatGetChar(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutChar(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetChar(buffer)

class FLOAT_Trans(TransFormat) :
  def __init__(self) : self.typeSize = FLOAT_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutFloat(buffer, getFloatField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setFloatField(dataStruct, dstart, _IPC.formatGetFloat(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutFloat(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetFloat(buffer)

class DOUBLE_Trans(TransFormat) :
  def __init__(self) : self.typeSize = DOUBLE_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutDouble(buffer, getDoubleField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setDoubleField(dataStruct, dstart, _IPC.formatGetDouble(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutDouble(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetDouble(buffer)

class BOOLEAN_Trans(TransFormat) :
  def __init__(self) : self.typeSize = INT_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutInt(buffer, getBooleanField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setBooleanField(dataStruct, dstart, _IPC.formatGetInt(buffer))

  def SimpleType (self) : return False

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutBoolean(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetBoolean(buffer)

class USHORT_Trans(TransFormat) :
  def __init__(self) : self.typeSize = SHORT_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutUShort(buffer, getShortField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setShortField(dataStruct, dstart, _IPC.formatGetUShort(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutUShort(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetUShort(buffer)

class UINT_Trans(TransFormat) :
  def __init__(self) : self.typeSize = INT_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    _IPC.formatPutUInt(buffer, getIntField(dataStruct, dstart))

  def Decode (self, dataStruct, dstart, buffer) :
    setIntField(dataStruct, dstart, _IPC.formatGetUInt(buffer))

  def EncodeElement(self, array, index, buffer) : 
    _IPC.formatPutUInt(buffer, array[index])

  def DecodeElement(self, array, index, buffer) :
    array[index] = _IPC.formatGetUInt(buffer)

class LONG_Trans(TransFormat) :
  def __init__(self) : self.typeSize = LONG_SIZE

  def Encode (self, dataStruct, dstart, buffer) :
    theLong = getLongField(dataStruct, dstart)
    if (-0X7FFFFFFF <= theLong <= 0X7FFFFFFF) :
      _IPC.formatPutInt(buffer, theLong)
    # 8 byte longs not implemented, yet
    #elif (0 <= theLong < 0XFFFFFFFFFFFFFFFF) :
    #  _IPC.formatPutLong(buffer, theLong)
    else :
      raise "Will lose precision in transferring long: %d" % theLong

  def Decode (self, dataStruct, dstart, buffer) :
#    setLongField(dataStruct, dstart, _IPC.formatGetLong(buffer))
    setLongField(dataStruct, dstart, _IPC.formatGetInt(buffer))

  def EncodeElement(self, array, index, buffer) : 
    theLong = array[index]
    if (-0X7FFFFFFF <= theLong <= 0X7FFFFFFF) :
      _IPC.formatPutInt(buffer, theLong)
    # 8 byte longs not implemented, yet
    #elif (0 <= theLong < 0XFFFFFFFFFFFFFFFF) :
    #  _IPC.formatPutLong(buffer, theLong)
    else :
      raise "Will lose precision in transferring long: %d" % theLong

  def DecodeElement(self, array, index, buffer) :
#    array[index] = _IPC.formatGetLong(buffer)
    array[index] = _IPC.formatGetInt(buffer)

TransFormatArray = [None]*MAXFORMATTERS
TransFormatArray[STR_FMT]     = STR_Trans()
TransFormatArray[BYTE_FMT]    = BYTE_Trans()
TransFormatArray[UBYTE_FMT]   = UBYTE_Trans()
TransFormatArray[SHORT_FMT]   = SHORT_Trans()
TransFormatArray[INT_FMT]     = INT_Trans()
TransFormatArray[CHAR_FMT]    = CHAR_Trans()
TransFormatArray[FLOAT_FMT]   = FLOAT_Trans()
TransFormatArray[DOUBLE_FMT]  = DOUBLE_Trans()
TransFormatArray[BOOLEAN_FMT] = BOOLEAN_Trans()
TransFormatArray[USHORT_FMT]  = USHORT_Trans()
TransFormatArray[UINT_FMT]    = UINT_Trans()
TransFormatArray[LONG_FMT]    = LONG_Trans()

def pickTrans (type) :
  if (0 <= type < MAXFORMATTERS) :
    fn = TransFormatArray[type];
    if (not fn is None) :
      return fn
  raise "pickTrans: Unhandled format %s" % type


def Encode(type, dataStruct, dstart, buffer) :
  pickTrans(type).Encode(dataStruct, dstart, buffer)

def Decode(type, dataStruct, dstart, buffer) :
  pickTrans(type).Decode(dataStruct, dstart, buffer)

def ELength(type, dataStruct, dstart) :
  return pickTrans(type).ELength(dataStruct, dstart)

def ALength(type) :
  return pickTrans(type).ALength()

def SimpleType(type) :
  return pickTrans(type).SimpleType()

def EncodeElement(type, array, index, buffer) :
  pickTrans(type).EncodeElement(array, index, buffer)

def DecodeElement(type, array, index, buffer) :
  pickTrans(type).DecodeElement(array, index, buffer)
