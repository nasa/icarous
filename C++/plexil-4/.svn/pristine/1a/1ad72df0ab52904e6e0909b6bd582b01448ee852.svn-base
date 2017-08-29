;;;/***************************************************************************
;;; * PROJECT: New Millennium, DS1
;;; *          IPC (Interprocess Communication) Package
;;; *
;;; * (c) Copyright 1996 Reid Simmons.  All rights reserved.
;;; *
;;; * FILE: latency2.lisp
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
;;; * $Log: latency2.lisp,v $
;;; * Revision 2.2  2009/01/12 15:54:58  reids
;;; * Added BSD Open Source license info
;;; *
;;; * Revision 2.1.1.1  1999/11/23 19:07:38  reids
;;; * Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;; *
;;; * Revision 1.1.4.2  1997/01/25 23:18:16  udo
;;; * ipc_2_6 to r3 merge
;;; *
;;; * Revision 1.1.2.1  1996/12/24 14:04:42  reids
;;; * Test the latency between Lisp and C programs running on PPC
;;; *
;;; **************************************************************************/

;;; Load the common file with all the type and name definitions
(load (make-pathname :DIRECTORY (pathname-directory *LOAD-TRUENAME*)
		     :NAME "latency.lisp"))

#|
(foreign:define-foreign-type timeval (:structure (sec :long)(usec :long)))
|#

(ff:defforeign 'gettimeofday
  :entry-point (ff::convert-to-lang "gettimeofday")
  :arguments '(integer #|time pointer|# integer #|pointer|#)
  :return-type :void)

(ff:defforeign 'taskIdSelf
  :entry-point (ff::convert-to-lang "taskIdSelf")
  :arguments '()
  :return-type :integer)

(ff:defforeign 'taskPrioritySet
  :entry-point (ff::convert-to-lang "taskPrioritySet")
  :arguments '(integer #|taskId|# integer #|priority|#)
  :return-type :void)

(defun latency2 ()

  ;; The producer needs to run at a lower priority, else it swamps
  ;;   central and the consumer
  ;; Assumes that Lisp is running at priority 100 (the default)
  (taskPrioritySet (taskIdSelf) (+ 100 10))

  (IPC:IPC_connect "sender")

  (IPC:IPC_defineMsg MSG_NAME 8 MSG_FORMAT)

  ;; This is a gross hack, but it seems to work.
  ;; "timeval" is a struct of 2 longs (4 bytes each)
  (let ((now (IPC::IPC_createByteArray 8)))
    (dotimes (j NSEND)
      (gettimeofday now 0)
      (IPC::IPC_publish MSG_NAME IPC:IPC_FIXED_LENGTH 
			now
			;; (foreign::lisp-alien-address now)
			)
      )
    )
 
  (IPC:IPC_disconnect)
  )
