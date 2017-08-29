;;;/***************************************************************************
;;; * PROJECT: New Millennium, DS1
;;; *          IPC (Interprocess Communication) Package
;;; *
;;; * (c) Copyright 1996 Reid Simmons.  All rights reserved.
;;; *
;;; * FILE: latency.lisp
;;; *
;;; * ABSTRACT: Test the latency of IPC for various sized messages.
;;; *
;;; * $Revision: 2.2 $
;;; * $Date: 2009/01/12 15:54:58 $
;;; * $Author: reids $
;;; *
;;; * Copyright (c) 2008, Carnegie Mellon University
;;; *     This software is distributed under the terms of the 
;;; *     Simplified BSD License (see ipc/LICENSE.TXT)
;;; *
;;; * REVISION HISTORY
;;; *
;;; * $Log: latency.lisp,v $
;;; * Revision 2.2  2009/01/12 15:54:58  reids
;;; * Added BSD Open Source license info
;;; *
;;; * Revision 2.1.1.1  1999/11/23 19:07:38  reids
;;; * Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;; *
;;; * Revision 1.1.4.2  1997/01/25 23:18:12  udo
;;; * ipc_2_6 to r3 merge
;;; *
;;; * Revision 1.1.2.1  1996/12/24 14:04:40  reids
;;; * Test the latency between Lisp and C programs running on PPC
;;; *
;;; **************************************************************************/

(defconstant NSEND 1000)

(defconstant MSG_NAME    "LatencyMsg")
(defconstant MSG_FORMAT  "{long, long}")
