;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PROJECT: New Millennium, DS1
;;          IPC (Interprocess Communication) Package
;;
;; (c) Copyright 1996 Reid Simmons.  All rights reserved.
;;
;; FILE: module.lisp
;;
;; ABSTRACT: Include file for LISP version of IPC test programs
;;
;; $Revision: 2.2 $
;; $Date: 2009/01/12 15:54:58 $
;; $Author: reids $
;;
;; Copyright (c) 2008, Carnegie Mellon University
;;     This software is distributed under the terms of the 
;;     Simplified BSD License (see ipc/LICENSE.TXT)
;; 
;; REVISION HISTORY
;;
;; $Log: module.lisp,v $
;; Revision 2.2  2009/01/12 15:54:58  reids
;; Added BSD Open Source license info
;;
;; Revision 2.1.1.1  1999/11/23 19:07:38  reids
;; Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;
;; Revision 1.1.2.1  1996/10/02 20:38:44  reids
;; Explicitly label functions as "IPC:"
;; Changes to support LISPWORKS.
;;
;; Revision 1.1  1996/06/17 18:43:34  rouquett
;; ipc test files
;;
;; Revision 1.2  1996/03/12 03:06:06  reids
;; Test programs now illustrate use of "enum" format;
;; Handlers now free data.
;;
;; Revision 1.1  1996/03/06 20:19:23  reids
;; New test programs for passing data between C and LISP modules
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; typedef enum { WaitVal, SendVal, ReceiveVal, ListenVal } STATUS_ENUM;
(defconstant STATUS_ENUM '(:WaitVal :SendVal :ReceiveVal :ListenVal))

(IPC:IPC_defstruct (T1)
 (i1 0 :type integer)
 (status 0 :type (or integer symbol))
 (matrix NIL :type array)
 (d1 0.0 :type double))

(defconstant T1_NAME   "T1")
;;; First form of "enum". 3 is the maximum value -- i.e., the value of WaitVal
(defconstant T1_FORMAT "{int, {enum : 3}, [double:2,3], double}")

(IPC:IPC_defstruct (T2)
 (str1 "" :type string)
 (count 0 :type integer)
 (t1 NIL :type array)
 (status :ReceiveVal :type (or integer symbol)))

(defconstant T2_NAME "T2")
;;; Alternate form of "enum".
(defconstant T2_FORMAT
  "{string, int, <T1:2>, {enum WaitVal, SendVal, ReceiveVal, ListenVal}}")

;;; typedef int MSG1_TYPE, *MSG1_PTR
(defconstant MSG1        "message1")
(defconstant MSG1_FORMAT "int")

;;; typedef char *MSG2_TYPE, **MSG2_PTR;
(defconstant MSG2        "message2")
(defconstant MSG2_FORMAT "string")

;;; typedef T1_TYPE QUERY1_TYPE, *QUERY1_PTR;
(defconstant QUERY1        "query1")
(defconstant QUERY1_FORMAT T1_NAME)

;;; typedef T2_TYPE RESPONSE1_TYPE, *RESPONSE1_PTR;
(defconstant RESPONSE1        "response1")
(defconstant RESPONSE1_FORMAT T2_NAME)

(defconstant MODULE1_NAME "module1")
(defconstant MODULE2_NAME "module2")
(defconstant MODULE3_NAME "module3")
