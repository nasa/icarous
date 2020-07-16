/*
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 */
#include "ipc.h"

IPC_VARCONTENT_TYPE vc;
FORMATTER_PTR byte_format, ubyte_format, short_format, ushort_format;
FORMATTER_PTR int_format, uint_format, long_format, ulong_format;
signed char byte_array[10], *bytePtr;
unsigned char ubyte_array[10], *ubytePtr;
short short_array[10], *shortPtr;
unsigned short ushort_array[10], *ushortPtr;
int int_array[10], *intPtr;
unsigned int uint_array[10], *uintPtr;
long long_array[10], *longPtr;
unsigned long ulong_array[10], *ulongPtr;

#ifdef VXWORKS
#define STATIC
#else
#define STATIC static
#endif

STATIC void initialize (void)
{
  int i, offset = 1;
  IPC_initialize();

  IPC_setVerbosity(IPC_Print_Errors);

  byte_format = IPC_parseFormat("byte");
  ubyte_format = IPC_parseFormat("ubyte");

  short_format = IPC_parseFormat("short");
  ushort_format = IPC_parseFormat("ushort");

  int_format = IPC_parseFormat("int");
  uint_format = IPC_parseFormat("uint");

  long_format = IPC_parseFormat("long");
  ulong_format = IPC_parseFormat("ulong");

  for (i=0; i<10; i++) {
    ubyte_array[i] = (256/2) + offset;
    byte_array[i] = -(256/2) + offset;
    ushort_array[i] = (256*256/2) + offset;
    short_array[i] = -(256*256/2) + offset;
    uint_array[i] = ((unsigned)(256*256/2)*(unsigned)(256*256)) + offset;
    int_array[i] = -(256*256)*(256*256/2) + offset;
    ulong_array[i] = (256*256)*(256*256/4) + offset;
    long_array[i] = -(256*256/4)*(256*256) + offset;
  }
}

STATIC void marshall_value_test (FORMATTER_PTR format, void *elementPtr,
				 void **valuePtr)
{
  IPC_marshall(format, elementPtr, &vc);
  IPC_unmarshall(format, vc.content, valuePtr);
}

STATIC void testit (void)
{
  marshall_value_test(byte_format, &byte_array[0], (void **)(void *)&bytePtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, byte_array[0], *bytePtr);
  IPC_printData(byte_format, stderr, bytePtr);

  marshall_value_test(ubyte_format, &ubyte_array[0],
		      (void **)(void *)&ubytePtr);
  fprintf(stderr, "%d: %u %u\n", vc.length, ubyte_array[0], *ubytePtr);
  IPC_printData(ubyte_format, stderr, ubytePtr);

  marshall_value_test(short_format, &short_array[0],
		      (void **)(void *)&shortPtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, short_array[0], *shortPtr);
  IPC_printData(short_format, stderr, shortPtr);

  marshall_value_test(ushort_format, &ushort_array[0],
		      (void **)(void *)&ushortPtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, ushort_array[0], *ushortPtr);
  IPC_printData(ushort_format, stderr, ushortPtr);

  marshall_value_test(int_format, &int_array[0], (void **)(void *)&intPtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, int_array[0], *intPtr);
  IPC_printData(int_format, stderr, intPtr);

  marshall_value_test(uint_format, &uint_array[0], (void **)(void *)&uintPtr);
  fprintf(stderr, "%d: %u %u\n", vc.length, uint_array[0], *uintPtr);
  IPC_printData(uint_format, stderr, uintPtr);

  marshall_value_test(long_format, &long_array[0], (void **)(void *)&longPtr);
  fprintf(stderr, "%d: %ld %ld\n", vc.length, long_array[0], *longPtr);
  IPC_printData(long_format, stderr, longPtr);

  marshall_value_test(ulong_format, &ulong_array[0],
		      (void **)(void *)&ulongPtr);
  fprintf(stderr, "%d: %lu %lu\n", vc.length, ulong_array[0], *ulongPtr);
  IPC_printData(ulong_format, stderr, ulongPtr);
}

STATIC void testit1 (void)
{
  marshall_value_test(ubyte_format, &byte_array[0], (void **)(void *)&bytePtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, byte_array[0], *bytePtr);
  IPC_printData(byte_format, stderr, bytePtr);

  marshall_value_test(byte_format, &ubyte_array[0],
		      (void **)(void *)&ubytePtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, ubyte_array[0], *ubytePtr);
  IPC_printData(ubyte_format, stderr, ubytePtr);

  marshall_value_test(ushort_format, &short_array[0],
		      (void **)(void *)&shortPtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, short_array[0], *shortPtr);
  IPC_printData(short_format, stderr, shortPtr);

  marshall_value_test(short_format, &ushort_array[0],
		      (void **)(void *)&ushortPtr);
  fprintf(stderr, "%d: %u %u\n", vc.length, ushort_array[0], *ushortPtr);
  IPC_printData(ushort_format, stderr, ushortPtr);

  marshall_value_test(uint_format, &int_array[0], (void **)(void *)&intPtr);
  fprintf(stderr, "%d: %d %d\n", vc.length, int_array[0], *intPtr);
  IPC_printData(int_format, stderr, intPtr);

  marshall_value_test(int_format, &uint_array[0], (void **)(void *)&uintPtr);
  fprintf(stderr, "%d: %u %u\n", vc.length, uint_array[0], *uintPtr);
  IPC_printData(uint_format, stderr, uintPtr);

  marshall_value_test(ulong_format, &long_array[0], (void **)(void *)&longPtr);
  fprintf(stderr, "%d: %ld %ld\n", vc.length, long_array[0], *longPtr);
  IPC_printData(long_format, stderr, longPtr);

  marshall_value_test(long_format, &ulong_array[0],
		      (void **)(void *)&ulongPtr);
  fprintf(stderr, "%d: %lu %lu\n", vc.length, ulong_array[0], *ulongPtr);
  IPC_printData(ulong_format, stderr, ulongPtr);
}

#ifndef VXWORKS
int main (void)
{
  initialize();
  testit();
  testit1();
  return 0;
}
#endif
