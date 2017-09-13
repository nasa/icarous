/*
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 */
#include "ipc.h"

/* JSM Added for Windows 98 */
#include "libc.h"

#ifdef _WINSOCK_
#undef BOOLEAN		/* Need int_32 sized booleans instead of char size */
#define BOOLEAN int	/* This gets typedef'd in the MSC includes */
#endif
#include "basics.h"
#include "tca.h"
#include "formatters.h"
#include "parseFmttrs.h"

static void testSize (int size, char *formatString)
{
  int formatSize;
  
  formatSize = x_ipc_dataStructureSize(ParseFormatString(formatString));
  if (size != formatSize) 
    printf("SIZES DIFFER: ");
  printf("Size of %s -- Structure: %d, Format: %d\n", 
	 formatString, size, formatSize);
  fflush(stdout);
}

void sizesTest(void);
void sizesTest(void)
{
  IPC_initialize();
  
  testSize(sizeof(int), "int");
  testSize(sizeof(short), "short");
  testSize(sizeof(long), "long");
  testSize(sizeof(char), "char");
  testSize(sizeof(float), "float");
  testSize(sizeof(double), "double");
  
#ifndef __sgi
  testSize(sizeof(struct{int a;char b;double c;}), "{int, char, double}");
  testSize(sizeof(struct{int a;double c;char b;}), "{int, double, char}");
  testSize(sizeof(struct{int a;char b;}), "{int, char}");
  testSize(sizeof(struct{char b; int a;}), "{char, int}");
  testSize(sizeof(struct{int a;struct{int a; char b;} b;double c;}),
	   "{int, {int, char}, double}");
  
  testSize(sizeof(struct{int a;char b;float c;}), "{int, char, float}");
  testSize(sizeof(struct{int a;float c;char b;}), "{int, float, char}");
  testSize(sizeof(struct{int a;struct{int a; char b;} b;float c;}),
	   "{int, {int, char}, float}");
  
  testSize(sizeof(struct{int a;char b;double c;}), "{int, char, double}");
  testSize(sizeof(struct{double a; char b;}), "{double, char}");
  testSize(sizeof(struct{int a;char b,d;double c;}),
	   "{int, char, char, double}");
  testSize(sizeof(struct{int a;char b,d,e;double c;}),
	   "{int, char, char, char, double}");
  
  testSize(sizeof(struct{int a;struct{int a; char b; double c;} b;float c;}),
	   "{int, {int, char, double}, float}");
  testSize(sizeof(struct{int a;struct{int a; char b,d; double c;} b;float c;}),
	   "{int, {int, char, char, double}, float}");
  testSize(sizeof(struct{int a;struct{int a; char b,d,e; double c;}
			 b;float c;}),
	   "{int, {int, char, char, char, double}, float}");
  
  testSize(sizeof(struct{char b;}), "{char}");  
  testSize(sizeof(struct{char b,c;}), "{char, char}");  
  testSize(sizeof(struct{char b,c,d;}), "{char, char, char}");  
  testSize(sizeof(struct{char b,c,d,e;}), "{char, char, char, char}");  
  testSize(sizeof(struct{char b,c,d,e,f;}),
	   "{char, char, char, char, char}");  
  testSize(sizeof(struct{char b,c,d,e,f,g;}),
	   "{char, char, char, char, char, char}");  
  testSize(sizeof(struct{int a;struct{char b;} b;float c;}),
	   "{int, {char}, float}");
  
  testSize(sizeof(struct { char a; struct { char a, b, c;} b; int c; }),
	   "{char, {char, char, char}, int}");
  testSize(sizeof(struct { char a; char b[3]; int c; }),
	   "{char, [char:3], int}");

  testSize(sizeof(short), "short");
  testSize(sizeof(struct{short a;}), "{short}");
  testSize(sizeof(struct{short a,b;}), "{short, short}");
  testSize(sizeof(struct{short a,b,c;}), "{short, short, short}");
  testSize(sizeof(struct{short a,b,c; char d;}),
	   "{short, short, short, char}");
  
  testSize(sizeof(long), "long");
  testSize(sizeof(struct{long a;}), "{long}");
  testSize(sizeof(struct{long a,b;}), "{long, long}");
  testSize(sizeof(struct{long a,b,c;}), "{long, long, long}");
  testSize(sizeof(struct{long a,b,c; char d;}), "{long, long, long, char}");
  
  testSize(sizeof(struct{short a; struct{char a,b;} b; short c;}),
	   "{short, {char, char}, short}");
  testSize(sizeof(struct{short a; struct{char a,b;} b; float c;}),
	   "{short, {char, char}, float}");
  testSize(sizeof(struct{double a; struct{char a,b;} b; short c;}),
	   "{double, {char, char}, short}");
  testSize(sizeof(struct{double a; struct{char a,b;} b; int c;}),
	   "{double, {char, char}, int}");
  testSize(sizeof(struct{char a; struct{char a,b;} b; short c;}),
	   "{char, {char, char}, short}");
  testSize(sizeof(struct{char a; struct{char a,b,c,d,e;} b; short c;}),
	   "{char, {char, char, char, char, char}, short}");
  testSize(sizeof(struct{char a,d,e; struct{char a,b,c;} b; short c;}),
	   "{char, char, char, {char, char, char}, short}");
  testSize(sizeof(struct{char a; short b;}), "{char, short}");
  testSize(sizeof(struct{char a; struct{char a; short b;} b; short c;}),
	   "{char, {char, short}, short}");
  testSize(sizeof(struct{char a; 
			 struct{char a; short b; struct {int a;} c;} b;
			 short c;}),
	   "{char, {char, short, {int}}, short}");
  
  testSize(sizeof(struct{int a[5];}), "{[int:5]}");
  testSize(sizeof(struct{double a[5];}), "{[double:5]}");
  testSize(sizeof(struct{char a[5];}), "{[char:5]}");
  
  testSize(sizeof(struct{char b; int a;}), "{char, int}");
  testSize(sizeof(struct{char b,c; int a;}), "{char, char, int}");
  testSize(sizeof(struct{int a; char b,c;}), "{int, char, char}");
  testSize(sizeof(struct{double a; char b,c;}), "{double, char, char}");
  testSize(sizeof(struct{char b; struct{char c; int a;} c;}),
	   "{char, {char, int}}");
  testSize(sizeof(struct{char b; struct{char c; int a;} c; char d;}),
	   "{char, {char, int}, char}");
  testSize(sizeof(struct{struct {int a; char b;} a; char c;}), 
	   "{{int, char}, char}");
  testSize(sizeof(struct{struct {double a; char b;} a; char c;}), 
	   "{{double, char}, char}");
  
  /* added for alpha testing */
  testSize(sizeof(struct {
    int silly;
    float *x;
    float *y;
    int n;
  }), 
	   "{int, <float:4>, <float:4>, int}");

  testSize(sizeof(struct {
    int silly;
    float *x;
    float *y;
    int n;
  }), 
	   "{int, *float, *float, int}");
  
  testSize(sizeof(struct {
    int refCount;
    char *classData;
    char *msgData;
    int classTotal;
    int msgTotal;
    int parentRef;
    int intent;
    int classId;
    int dispatchRef;
    int msgRef;
  }),
	   "{int,*char,*char,int,int,int,int,int,int,int}");

  testSize(sizeof(enum { Ea0=0, Ea1=1}), "{enum Ea0, Ea1}");
  testSize(sizeof(enum { Eb0=0, Eb1000 = 1000}), "{enum : 1000}");
  testSize(sizeof(enum { Ec0=0, EcBig = 16777000}), "{enum : 16777000}");

  testSize(sizeof(struct{
    int i1;
    enum { Ed0=0, EdBig = 16777000} enumD;
    enum { Ee0=0, Ee1000 = 1000} enumE;
    enum { Ef0=0, Ef1=1} enumF;
  }),
	   "{int, {enum : 16777000}, {enum : 1000}, {enum Ef0, Ef1}}");

  /* JSM added to test version message return from server */
  testSize(sizeof(struct { struct {int a; int b;} c; BOOLEAN d;}),
    "{{int, int}, boolean}");

#endif
}

#ifndef VXWORKS
int main (void)
{
  sizesTest();
  return 0;
}
#endif
