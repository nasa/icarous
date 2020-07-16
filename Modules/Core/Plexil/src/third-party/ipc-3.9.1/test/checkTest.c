/*
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 */
#include <stdio.h>
#include "ipc.h"
int formatsEqual(FORMATTER_PTR format1, FORMATTER_PTR format2);

#define INT_FORMAT "int"
#define STRING_FORMAT "string"
#define SAMPLE_FORMAT "{int, string, double}"
#define MATRIX_LIST_FORMAT "{[float:2,2], string, int, *!}"
#define MATRIX_FORMAT      "[float:2,2]"
#define PTR1_FORMAT "*int"
#define PTR2_FORMAT "*float"
#define T1_FORMAT "{int, {enum : 3}, [double:2,3], double}"
#define T1A_FORMAT "{int, {enum : 3}, [double:3,2], double}"
#define T1B_FORMAT "{int, {enum : 4}, [double:2,3], double}"
#define T2_FORMAT \
"{*int, <{int, float}:2>, {enum WaitVal, SendVal, ReceiveVal, ListenVal}}"
#define T2A_FORMAT \
"{*int, <{int, float}:2>, {enum WaitVal, SendVal, ReceiveVal}}"
#define T2B_FORMAT \
"{*int, <{int, float}:2>, {enum WaitVal, SendVal, ReceiveVal, ListenVal1}}"

static void checkIt (const char *formatString1, const char *formatString2)
{
  fprintf(stderr, "%s: \"%s\" \"%s\"\n", 
	  (formatsEqual(IPC_parseFormat(formatString1),
			IPC_parseFormat(formatString2)) ? "SAME" : "NOT SAME"),
	  formatString1, formatString2);
}

int main (void)
{
  IPC_initialize();

  checkIt(NULL, NULL);
  checkIt("", "");
  checkIt(NULL, "");
  checkIt(INT_FORMAT, INT_FORMAT);
  checkIt(STRING_FORMAT, STRING_FORMAT);
  checkIt(SAMPLE_FORMAT, SAMPLE_FORMAT);
  checkIt(MATRIX_LIST_FORMAT, MATRIX_LIST_FORMAT);
  checkIt(MATRIX_FORMAT, MATRIX_FORMAT);
  checkIt(PTR1_FORMAT, PTR1_FORMAT);
  checkIt(T1_FORMAT, T1_FORMAT);
  checkIt(T2_FORMAT, T2_FORMAT);

  fprintf(stderr, "\n");

  checkIt(INT_FORMAT, STRING_FORMAT);
  checkIt(INT_FORMAT, SAMPLE_FORMAT);
  checkIt(SAMPLE_FORMAT, MATRIX_LIST_FORMAT);
  checkIt(SAMPLE_FORMAT, MATRIX_FORMAT);
  checkIt(PTR1_FORMAT, PTR2_FORMAT);
  checkIt(T1_FORMAT, T1A_FORMAT);
  checkIt(T1_FORMAT, T1B_FORMAT);
  checkIt(T2_FORMAT, T2A_FORMAT);
  checkIt(T2_FORMAT, T2B_FORMAT);
  return 0;
}
