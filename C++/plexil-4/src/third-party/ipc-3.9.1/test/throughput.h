/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: throughput.h
 *
 * ABSTRACT: Test the throughput of IPC for various sized messages.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:59 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: throughput.h,v $
 * Revision 2.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2000/07/03 17:03:38  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/01/27 20:47:11  reids
 * Removed compiler warnings for RedHat Linux
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/06/17 18:43:38  rouquett
 * ipc test files
 *
 * Revision 1.1  1996/05/09 01:07:53  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 *
 * Adapted from X_IPC/TCX test code by Terry Fong (NASA Ames)
 *****************************************************************************/
#ifndef THROUGHPUT_H
#define THROUGHPUT_H

#if !defined(__sgi) && !defined(WIN32) && !defined(REDHAT_52)
#define UNUSED_PRAGMA
#endif

#define MAX_DATA_SIZE	((sizeof(float)*256*1024))

#define NSEND (200)

#define NULL_MSG      "NullMsg"
#define FLOAT4_MSG    "Float4Msg"
#define FLOAT16_MSG   "Float16Msg"
#define FLOAT64_MSG   "Float64Msg"
#define FLOAT256_MSG  "Float256Msg"
#define FLOAT1K_MSG   "Float1KMsg"
#define FLOAT4K_MSG   "Float4KMsg"
#define FLOAT16K_MSG  "Float16KMsg"
#define FLOAT64K_MSG  "Float64KMsg"
#define FLOAT256K_MSG "Float256KMsg"

#define DONE_MSG      "I_done"

#endif /* THROUGHPUT_H */
