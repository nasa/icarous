/*****************************************************************************
 *       $Id: formatters.java,v 1.6 2009/09/04 19:11:20 reids Exp $
 * $Revision: 1.6 $
 *     $Date: 2009/09/04 19:11:20 $
 *   $Author: reids $
 *    $State: Exp $
 *   $Locker:  $
 *
 * PROJECT:	NM-DS1
 *
 * FILE:	formatters.java
 *
 * DESCRIPTION: JAVA class for marshalling and unmarshalling IPC byte arrays
 *              into JAVA objects..
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * $Log: formatters.java,v $
 * Revision 1.6  2009/09/04 19:11:20  reids
 * IPC Java is now in its own package
 *
 * Revision 1.5  2009/02/07 18:54:45  reids
 * Updates for use on 64 bit machines
 *
 * Revision 1.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2002/01/11 22:35:23  reids
 * Improved error handling for Java version
 *
 * Revision 1.2  2002/01/02 21:10:03  reids
 * Fixed implementation of decoding enumerated types.
 * Added debugging function (printByteArray).
 *
 * Revision 1.1  2002/01/02 17:40:17  reids
 * Initial (and more, or less, complete) release of Java version of IPC.
 *
 *
 *****************************************************************************/

package ipc.java;

import java.lang.reflect.Field;
import java.lang.reflect.Array;

public class formatters {
  public static class VARCONTENT {
    public int length;
    public long byteArray;
  }

  public static final int PrimitiveFMT  = 0;
  public static final int LengthFMT     = 1;
  public static final int StructFMT     = 2;
  public static final int PointerFMT    = 3;
  public static final int FixedArrayFMT = 4;
  public static final int VarArrayFMT   = 5;
  public static final int BadFormatFMT  = 6;
  public static final int NamedFMT      = 7;
  public static final int EnumFMT       = 8;

  private native static int formatType(long formatter);
  private native static int formatPrimitiveProc(long formatter);
  private native static long formatChoosePtrFormat(long formatter, 
						   long parentFormat);
  private native static long formatFormatArray(long formatter);
  private native static int formatFormatArrayMax(long formatArray);
  private native static long formatFormatArrayItemPtr(long formatArray, int n);
  private native static int formatFormatArrayItemInt(long formatArray, int n);
  private native static long findNamedFormat(long format);
  private native static boolean checkMarshallStatus(long formatter);
  private native static long createBuffer(long byteArray);
  private native static void freeBuffer(long buffer);
  private native static int bufferLength(long buffer);
  private native static long createByteArray(int length);
  // For debugging purposes
  private native static void rewindBuffer(long buffer);
  private native static void printBuffer(long buffer);
  private native static void printByteArray(long byteArray, int length);
  private native static int parseFormat(String format);

  private static boolean isSimpleType (long format) throws Exception {
    return (formatType(format) == PrimitiveFMT &&
	    primFmttrs.SimpleType(formatPrimitiveProc(format)));
  }

  private static long fixedArrayNumElements (long formatArray) {
    int i, numElements = 1;
    int n = formatFormatArrayMax(formatArray);

    for (i=2; i<n; i++)
      numElements *= formatFormatArrayItemInt(formatArray, i);
    return numElements;
  }

  private static int varArrayDimSize (int dim, long formatArray,
				      Object dataStruct) throws Exception {
    int sizePlace = formatFormatArrayItemInt(formatArray, dim);
    int size = primFmttrs.getIntField(dataStruct, sizePlace-1);
    return size;
  }

  private static int varArrayNumElements (long formatArray, Object dataStruct)
    throws Exception {
    int i, numElements = 1;
    int n = formatFormatArrayMax(formatArray);

    for (i=2; i<n; i++)
      numElements *= varArrayDimSize(i, formatArray, dataStruct);
    return numElements;
  }

  /* dataStruct is non-null if this is a variable length array */
  private static int arrayBufferSize (Object[] array, int dim, int max,
				      long formatArray, long arrayFormat,
				      Object dataStruct) 
    throws Exception {
    int bufSize = 0;
    int len = (dataStruct == null ? formatFormatArrayItemInt(formatArray, dim)
	       : varArrayDimSize(dim, formatArray, dataStruct));
    for (int i=0; i<len; i++) {
      bufSize += (dim == max ? bufferSize1(arrayFormat, array[i], 0, 0, true)
		  : arrayBufferSize((Object[])array[i], dim+1, max, 
				    formatArray, arrayFormat, dataStruct));
    }
    return bufSize;
  }

  /* dataStruct is non-null if this is a variable length array */
  private static void arrayTransferToBuffer (Object array, long buffer,
					     int dim, int max, 
					     boolean isSimple,
					     long formatArray, long arrayFormat,
					     Object dataStruct) 
    throws Exception {
      int i, len = (dataStruct == null
		    ? formatFormatArrayItemInt(formatArray, dim)
		    : varArrayDimSize(dim, formatArray, dataStruct));
    for (i=0; i<len; i++) {
      if (dim != max) {
	arrayTransferToBuffer(((Object[])array)[i], buffer, dim+1, max,
			      isSimple, formatArray, arrayFormat, dataStruct);
      } else if (isSimple) {
	primFmttrs.EncodeElement(formatPrimitiveProc(arrayFormat), 
				 array, i, buffer);
      } else {
	transferToBuffer(arrayFormat, ((Object[])array)[i], 0, buffer, 0, 
			 true);
      }
    }
  }

  /* dataStruct is non-null if this is a variable length array */
  private static void
  arrayTransferToDataStructure (Object array, long buffer, int dim, int max, 
				int len, boolean isSimple, long formatArray,
				long arrayFormat, Object dataStruct) 
    throws Exception {
    int i, nextLen=0;

    if (dim != max) 
	nextLen = (dataStruct == null
		   ? formatFormatArrayItemInt(formatArray, dim+1)
		   : varArrayDimSize(dim+1, formatArray, dataStruct));
    for (i=0; i<len; i++) {
      if (dim != max) {
	Object[] arrayObject = ((Object[])array);
	Array.set(arrayObject, i,
		  validateArrayObject(arrayObject[i], nextLen, array, -1));
	arrayTransferToDataStructure(arrayObject[i], buffer, dim+1, max,
				     nextLen, isSimple, formatArray,
				     arrayFormat, dataStruct);
      } else if (isSimple) {
	primFmttrs.DecodeElement(formatPrimitiveProc(arrayFormat), 
				 array, i, buffer);
      } else {
	Array.set(array, i, validateObject(((Object[])array)[i], array, -1));
	transferToDataStructure(arrayFormat, ((Object[])array)[i], 0,
				buffer, 0, true);
      }
    }
  }

  private static Object validateObject (Object object, Object parentObject,
					int index) throws Exception {
    if (object == null) {
      Class pclass = parentObject.getClass();
      Class oclass = (index < 0 ? pclass.getComponentType()
		      : pclass.getFields()[index].getType());
      object = oclass.newInstance();
    }
    return object;
  }

  private static String invalidStructFormat(Class oclass, int n) {
    return ("Data structure \""+ oclass.getName()
	    +"\" does not match format -- have "+
	    oclass.getFields().length +" public fields; need " + (n-1));
  }

  private static String invalidStructFormat(Object object, int n) {
    return invalidStructFormat(object.getClass(), n);
  }

  private static String invalidArrayFormat(Class oclass, int index) {
    return ("Data structure \""+ oclass.getName()
	    +"\" does not match format -- field \""+
	    oclass.getFields()[index].getName() +"\" needs to be an array");
  }

  private static String invalidArrayFormat(Object object, int index) {
    return invalidArrayFormat(object.getClass(), index);
  }

  private static Object validateArrayObject (Object arrayObject, int dim,
					     Object object, int index) 
    throws Exception {
      if (arrayObject != null && !arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(object, index));
      } else if (arrayObject == null || Array.getLength(arrayObject) != dim) {
      Class oclass = object.getClass();
      Class aclass = (index < 0 ? oclass.getComponentType()
		      : oclass.getFields()[index].getType());
      arrayObject = Array.newInstance(aclass.getComponentType(), dim);
    }
    return arrayObject;
  }

  private static boolean feasibleToDecodeVarArray(int size, long formatArray,
						  int dStart) {
    int max = formatFormatArrayMax(formatArray);

    if (max > 3) { // Number of dimensions is max-2
      for (int i=2; i<max; i++) {
	if (formatFormatArrayItemInt(formatArray, i) > dStart) {
	  return false;
	}
      }
    }
    return true;
  }

  private static int bufferSize1 (long format, Object dataStruct, 
				  int dStart, long parentFormat, 
				  boolean isTopLevelStruct) 
    throws Exception {
    int bufSize = 0;
    Object struct;

    switch (formatType(format)) {
    case LengthFMT:
      throw new Exception("JAVA version of IPC can only use explicit formats");

    case PrimitiveFMT:
      int primType = formatPrimitiveProc(format);
      bufSize += primFmttrs.ELength(primType, dataStruct, dStart);
      break;

    case PointerFMT:
      bufSize += 1;
      if (primFmttrs.getObjectField(dataStruct, dStart) != null)
	bufSize += bufferSize1(formatChoosePtrFormat(format, parentFormat),
			       dataStruct, dStart, 0, isTopLevelStruct);
      break;

    case StructFMT: {
      long formatArray = formatFormatArray(format);
      int i, structStart = 0, n = formatFormatArrayMax(formatArray);

      struct = (isTopLevelStruct ? dataStruct
		: primFmttrs.getObjectField(dataStruct, dStart));
      if (struct.getClass().getFields().length < n-1) {
	throw new Exception(invalidStructFormat(struct, n));
      }
      for (i=1; i<n; i++) {
	bufSize += bufferSize1(formatFormatArrayItemPtr(formatArray, i),
			       struct, structStart, format, false);
	structStart++;
      }
      break;
    }

    case FixedArrayFMT: {
      long formatArray = formatFormatArray(format);
      long nextFormat = formatFormatArrayItemPtr(formatArray, 1);
      Object arrayObject = (isTopLevelStruct ? dataStruct :
			    primFmttrs.getObjectField(dataStruct, dStart));

      if (!arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(dataStruct, dStart));
      } else if (isSimpleType(nextFormat)) {
	bufSize += (bufferSize1(nextFormat, arrayObject, 0, 0, false) *
		    fixedArrayNumElements(formatArray));
      } else {
	bufSize += arrayBufferSize((Object[])arrayObject, 2, 
				   formatFormatArrayMax(formatArray)-1,
				   formatArray, nextFormat, null);
      }
      break;
    }

    case VarArrayFMT: {
      long formatArray = formatFormatArray(format);
      long nextFormat = formatFormatArrayItemPtr(formatArray, 1);
      Object arrayObject = (isTopLevelStruct ? dataStruct :
			    primFmttrs.getObjectField(dataStruct, dStart));

      /* For the size of the array */
      bufSize += primFmttrs.ELength(primFmttrs.INT_FMT, null, 0);
      
      if (!arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(dataStruct, dStart));
      } else if (isSimpleType(nextFormat)) {
	bufSize += (bufferSize1(nextFormat, arrayObject, 0, 0, false) *
		    varArrayNumElements(formatArray, dataStruct));
      } else {
	bufSize += arrayBufferSize((Object[])arrayObject, 2, 
				   formatFormatArrayMax(formatArray)-1,
				   formatArray, nextFormat, dataStruct);
      }
      break;
    }

    case NamedFMT:
      bufSize += bufferSize1(findNamedFormat(format), dataStruct, dStart,
			     parentFormat, isTopLevelStruct);
      break;

    case EnumFMT:
      bufSize += primFmttrs.ELength(primFmttrs.INT_FMT, null, 0);
      break;
    }
    return bufSize;
  }

  private static int bufferSize (long formatter, Object object,
				 boolean isTopLevelStruct) 
      throws Exception {
    int val = (formatter == 0 ? 0 : bufferSize1(formatter, object, 0, 0,
						isTopLevelStruct));
    return val;
  }

  private static void transferToBuffer (long format, Object dataStruct, 
					int dStart, long buffer, 
					long parentFormat, 
					boolean isTopLevelStruct) 
    throws Exception {
    Object struct;

    switch (formatType(format)) {
    case LengthFMT:
      throw new Exception("JAVA version of IPC can only use explicit formats");

    case PrimitiveFMT:
      primFmttrs.Encode(formatPrimitiveProc(format), dataStruct, dStart,
			buffer);
      break;

    case PointerFMT:
      Object object = primFmttrs.getObjectField(dataStruct, dStart);
      /* 'Z' means data, 0 means NIL */
      primFmttrs.formatPutChar(buffer, (object != null ? 'Z' : '\0'));
      if (object != null)
	transferToBuffer(formatChoosePtrFormat(format, parentFormat),
			 dataStruct, dStart, buffer, 0, isTopLevelStruct);
      break;

    case StructFMT: {
      long formatArray = formatFormatArray(format);
      int i, structStart = 0, n = formatFormatArrayMax(formatArray);

      struct = (isTopLevelStruct ? dataStruct
		: primFmttrs.getObjectField(dataStruct, dStart));
      if (struct.getClass().getFields().length < n-1) {
	throw new Exception(invalidStructFormat(struct, n));
      }
      for (i=1; i<n; i++) {
	transferToBuffer(formatFormatArrayItemPtr(formatArray, i),
			 struct, structStart, buffer, format, false);
	structStart++;
      }
      break;
    }

    case FixedArrayFMT: {
      long formatArray = formatFormatArray(format);
      long  nextFormat = formatFormatArrayItemPtr(formatArray, 1);
      Object arrayObject = (isTopLevelStruct ? dataStruct :
			    primFmttrs.getObjectField(dataStruct, dStart));

      if (!arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(dataStruct, dStart));
      } else {
	arrayTransferToBuffer(arrayObject, buffer, 2, 
			      formatFormatArrayMax(formatArray)-1,
			      isSimpleType(nextFormat),
			      formatArray, nextFormat, null);
      }
      break;
    }

    case VarArrayFMT: {
      long formatArray = formatFormatArray(format);
      long nextFormat = formatFormatArrayItemPtr(formatArray, 1);
      Object arrayObject = (isTopLevelStruct ? dataStruct :
			    primFmttrs.getObjectField(dataStruct, dStart));

      /* For the size of the array */
      primFmttrs.formatPutInt(buffer, varArrayNumElements(formatArray,
							  dataStruct));
      
      if (!arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(dataStruct, dStart));
      } else {
	arrayTransferToBuffer(arrayObject, buffer, 2, 
			      formatFormatArrayMax(formatArray)-1,
			      isSimpleType(nextFormat),
			      formatArray, nextFormat, dataStruct);
      }
      break;
    }

    case NamedFMT:
      transferToBuffer(findNamedFormat(format), dataStruct, dStart, buffer,
		       parentFormat, isTopLevelStruct);
      break;

    case EnumFMT:
      primFmttrs.Encode(primFmttrs.INT_FMT, dataStruct, dStart, buffer);
      break;
    }
  }

  private static void transferToDataStructure (long format, Object dataStruct, 
					       int dStart, long buffer,
					       long parentFormat, 
					       boolean isTopLevelStruct) 
    throws Exception {
    Object struct;

    switch (formatType(format)) {
    case LengthFMT:
      throw new Exception("JAVA version of IPC can only use explicit formats");

    case PrimitiveFMT:
      primFmttrs.Decode(formatPrimitiveProc(format), dataStruct, dStart,
			buffer);
      break;

    case PointerFMT: {
      char c = primFmttrs.formatGetChar(buffer);
      if (c == '\0') {
	primFmttrs.setObjectField(dataStruct, dStart, null);
      } else {
	transferToDataStructure(formatChoosePtrFormat(format, parentFormat),
				dataStruct, dStart, buffer, 0,
				isTopLevelStruct);
      }
      break;
    }
    case StructFMT: {
      long formatArray = formatFormatArray(format);
      int i, structStart = 0, n = formatFormatArrayMax(formatArray);

      if (isTopLevelStruct) {
	struct = dataStruct;
      } else {
	Object struct1 = primFmttrs.getObjectField(dataStruct, dStart);
	struct = validateObject(struct1, dataStruct, dStart);
	if (struct != struct1)
	  primFmttrs.setObjectField(dataStruct, dStart, struct);
      }

      if (struct.getClass().getFields().length < n-1) {
	throw new Exception(invalidStructFormat(struct, n));
      }
      for (i=1; i<n; i++) {
	transferToDataStructure(formatFormatArrayItemPtr(formatArray, i),
				struct, structStart, buffer, format, false);
	structStart++;
      }
      break;
    }

    case FixedArrayFMT: {
      long formatArray = formatFormatArray(format);
      long nextFormat = formatFormatArrayItemPtr(formatArray, 1);
      int size = formatFormatArrayItemInt(formatArray, 2);
      Object arrayObject;

      if (isTopLevelStruct) {
	arrayObject = dataStruct;
      } else {
	arrayObject = primFmttrs.getObjectField(dataStruct, dStart);
	arrayObject = validateArrayObject(arrayObject, size,
					  dataStruct, dStart);
	primFmttrs.setObjectField(dataStruct, dStart, arrayObject);
      }

      if (!arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(dataStruct, dStart));
      } else {
	arrayTransferToDataStructure(arrayObject, buffer, 2, 
				     formatFormatArrayMax(formatArray)-1,
				     size, isSimpleType(nextFormat),
				     formatArray, nextFormat, null);
      }
      break;
    }

    case VarArrayFMT: {
      long formatArray = formatFormatArray(format);
      long nextFormat = formatFormatArrayItemPtr(formatArray, 1);
      /* The total size of the array is the stored first */
      int size = primFmttrs.formatGetInt(buffer);
      int numDims = formatFormatArrayMax(formatArray)-2;
      Object arrayObject;

      if (numDims > 1) size = varArrayDimSize(2, formatArray, dataStruct);

      if (!feasibleToDecodeVarArray(size, formatArray, dStart)) {
	throw new Exception("JAVA version of IPC cannot decode "+
			    "multi-dimensional variable length arrays unless "+
			    "the size variables appear BEFORE the array "+
			    "in the enclosing structure");
      } else if (isTopLevelStruct) {
	  arrayObject = dataStruct;
      } else {
	arrayObject = primFmttrs.getObjectField(dataStruct, dStart);
	arrayObject = validateArrayObject(arrayObject, size,
					  dataStruct, dStart);
	primFmttrs.setObjectField(dataStruct, dStart, arrayObject);
      }

      if (!arrayObject.getClass().isArray()) {
	throw new Exception(invalidArrayFormat(dataStruct, dStart));
      } else {
	arrayTransferToDataStructure(arrayObject, buffer, 2,
				     numDims+1, size, isSimpleType(nextFormat),
				     formatArray, nextFormat, dataStruct);
      }
      break;
    }

    case NamedFMT:
      transferToDataStructure(findNamedFormat(format), dataStruct, dStart,
			      buffer, parentFormat, isTopLevelStruct);
      break;

    case EnumFMT:
      primFmttrs.Decode(primFmttrs.INT_FMT, dataStruct, dStart, buffer);
      break;
    }
  }

  private static void encodeData (long formatter, Object object, long buffer)
      throws Exception {
    transferToBuffer(formatter, object, 0, buffer, 0, true);
  }

  private static void decodeData (long formatter, long buffer, Object object) 
    throws Exception {
    transferToDataStructure(formatter, object, 0, buffer, 0, true);
  }

  public static void checkDataClass (long format, Class oclass)
      throws Exception {
   switch (formatType(format)) {
    case LengthFMT:
      throw new Exception("JAVA version of IPC can only use explicit formats");

    case PrimitiveFMT:
      boolean matches = true;
      String neededType = null;
      switch (formatPrimitiveProc(format)) {
      case primFmttrs.INT_FMT:
      case primFmttrs.UINT_FMT:
	neededType = "int";
	matches = (oclass == int.class || oclass == Integer.class); break;
      case primFmttrs.BOOLEAN_FMT:
	neededType = "boolean";
	matches = (oclass == boolean.class || oclass == Boolean.class); break;
      case primFmttrs.FLOAT_FMT:
	neededType = "float";
	matches = (oclass == float.class || oclass == Float.class); break;
      case primFmttrs.DOUBLE_FMT:
	neededType = "double";
	matches = (oclass == double.class || oclass == Double.class); break;
      case primFmttrs.BYTE_FMT:
      case primFmttrs.UBYTE_FMT:
	neededType = "byte";
	matches = (oclass == byte.class || oclass == Byte.class); break;
      case primFmttrs.STR_FMT:
	neededType = "String";
	matches = (oclass == String.class); break;
      case primFmttrs.CHAR_FMT:
	neededType = "char";
	matches = (oclass == char.class); break;
      case primFmttrs.SHORT_FMT:
      case primFmttrs.USHORT_FMT:
	neededType = "short";
	matches = (oclass == short.class || oclass == Short.class); break;
      case primFmttrs.LONG_FMT:
      case primFmttrs.ULONG_FMT:
	neededType = "long";
	matches = (oclass == long.class || oclass == Long.class); break;
      }
      if (!matches) {
	throw new Exception("\""+ oclass.getName()
			    +"\" does not match format -- needs to be "+
			    neededType);
      }
      break;

    case PointerFMT: {
      checkDataClass(formatChoosePtrFormat(format, 0), oclass);
      break;
    }
    case StructFMT: {
      int n = formatFormatArrayMax(formatFormatArray(format));
      if (oclass.getFields().length < n-1) {
	throw new Exception(invalidStructFormat(oclass, n));
      }
      break;
    }

    case VarArrayFMT:
    case FixedArrayFMT: {
      if (!oclass.isArray()) {
	throw new Exception("Data structure \""+ oclass.getName()
			    +"\" does not match format -- needs to be array");
      }
      break;
    }

    case NamedFMT:
      checkDataClass(findNamedFormat(format), oclass);
      break;

    case EnumFMT:
      if (!(oclass == int.class || oclass == Integer.class)) {
	throw new Exception("\""+ oclass.getName() +"\" does not match format"
			    +" -- needs to be int (enum)");
      }
      break;
    }
  }

  /* Marshalls the object into a byte array.
     Fills in the VARCONTENT structure with the length and byteArray.
     "formatter" is a C pointer. Returns any error conditions. */
  public static int marshall (long formatter, Object object, 
			      VARCONTENT varcontent) throws Exception {
    if (!checkMarshallStatus(formatter)) {
      return IPC.IPC_Error;
    } else {
      varcontent.length = bufferSize(formatter, object, true);
      varcontent.byteArray = 0;
      if (varcontent.length > 0) {
	varcontent.byteArray = createByteArray(varcontent.length);
	long buffer = createBuffer(varcontent.byteArray);
	encodeData(formatter, object, buffer);
	if (bufferLength(buffer) != varcontent.length)
	  throw new Exception("Mismatch between buffer size ("+
			      varcontent.length+") and encoded data ("+
			      bufferLength(buffer)+")");
	freeBuffer(buffer);
      }
      return IPC.IPC_OK;
    }
  }

  /* Fills in the slots of the object according to the formatter.
     "byteArray" and "formatter" are both C pointers.
     Returns any error conditions. */
  public static int unmarshall (long formatter, long byteArray, Object object) 
      throws Exception {
    if (!checkMarshallStatus(formatter)) {
      return IPC.IPC_Error;
    } else {
      if (formatter != 0) {
	long buffer = createBuffer(byteArray);
	decodeData(formatter, buffer, object);
	freeBuffer(buffer);
      }
      return IPC.IPC_OK;
    }
  }

  public static Object createFixedArray(Class arrayClass, long formatter) 
      throws Exception {
    int size = formatFormatArrayItemInt(formatFormatArray(formatter), 2);
    return Array.newInstance(arrayClass, size);
  }

  public static class IPCPrim {
    public Object coerce () { return null; }
    public String toString() { return "IPCPrim"; }
  }

  public static class IPCChar extends IPCPrim {
    public IPCChar () {}
    public IPCChar (char theChar) { value = theChar; }
    public Object coerce () { return this; }
    public String toString() { return new String() + value; }
    public char value;
  }

  public static class IPCBoolean extends IPCPrim {
    public IPCBoolean () {}
    public IPCBoolean (boolean theBoolean) { value = theBoolean; }
    public Object coerce () { return new Boolean(value); }
    public String toString() { return (value ? "true" : "false"); }
    public boolean value;
  }

  public static class IPCByte extends IPCPrim {
    public IPCByte () {}
    public IPCByte (byte theByte) { value = theByte; }
    public Object coerce () { return new Byte(value); }
    public String toString() { return Byte.toString(value); }
    public byte value;
  }

  public static class IPCShort extends IPCPrim {
    public IPCShort () {}
    public IPCShort (short theShort) { value = theShort; }
    public Object coerce () { return new Short(value); }
    public String toString() { return Short.toString(value); }
    public short value;
  }

  public static class IPCInteger extends IPCPrim {
    public IPCInteger () {}
    public IPCInteger (int theInt) { value = theInt; }
    public Object coerce () { return new Integer(value); }
    public String toString() { return Integer.toString(value); }
    public int value;
  }

  public static class IPCLong extends IPCPrim {
    public IPCLong () {}
    public IPCLong (long theLong) { value = theLong; }
    public Object coerce () { return new Long(value); }
    public String toString() { return Long.toString(value); }
    public long value;
  }

  public static class IPCFloat extends IPCPrim {
    public IPCFloat () {}
    public IPCFloat (float theFloat) { value = theFloat; }
    public Object coerce () { return new Float(value); }
    public String toString() { return Float.toString(value); }
    public float value;
  }

  public static class IPCDouble extends IPCPrim {
    public IPCDouble () {}
    public IPCDouble (double theDouble) { value = theDouble; }
    public Object coerce () { return new Double(value); }
    public String toString() { return Double.toString(value); }
    public double value;
  }

  public static class IPCString extends IPCPrim {
    public IPCString () {}
    public IPCString (String theString) { value = theString; }
    public Object coerce () { return value; }
    public String toString() { return value; }
    public String value;
  }
}
