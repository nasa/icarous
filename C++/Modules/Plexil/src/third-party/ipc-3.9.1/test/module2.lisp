;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PROJECT: New Millennium, DS1
;;          IPC (Interprocess Communication) Package
;;
;; (c) Copyright 1996 Reid Simmons.  All rights reserved.
;;
;; FILE: module2.lisp
;;
;; ABSTRACT: Test program for IPC.
;;             Publishes: MSG2
;;             Subscribes to: MSG1, QUERY1
;;             Responds with: RESPONSE1
;;             Behavior: Listens for MSG1 and prints out message data.
;;                       When QUERY1 is received, publishes MSG1 and
;;                       responds to the query with RESPONSE1.
;;                       Exits when 'q' is typed at terminal.
;;                       Should be run in conjunction with module1
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
;; $Log: module2.lisp,v $
;; Revision 2.2  2009/01/12 15:54:58  reids
;; Added BSD Open Source license info
;;
;; Revision 2.1.1.1  1999/11/23 19:07:38  reids
;; Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;
;; Revision 1.1.2.2  1997/01/25 23:18:20  udo
;; ipc_2_6 to r3 merge
;;
;; Revision 1.1.2.1.6.1  1996/12/24 15:25:39  reids
;; Change name of "main" function to enable multiple modules to be loaded
;;   simultaneously
;;
;; Revision 1.1.2.1  1996/10/02 20:38:47  reids
;; Explicitly label functions as "IPC:"
;; Changes to support LISPWORKS.
;;
;; Revision 1.1  1996/06/17 18:43:36  rouquett
;; ipc test files
;;
;; Revision 1.2  1996/03/12 03:06:11  reids
;; Test programs now illustrate use of "enum" format;
;; Handlers now free data.
;;
;; Revision 1.1  1996/03/06 20:19:30  reids
;; New test programs for passing data between C and LISP modules
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;/

;;; Load the common file with all the type and name definitions
(load (make-pathname :DIRECTORY (pathname-directory *LOAD-TRUENAME*)
		     :NAME "module.lisp"))

(IPC:IPC_defun_handler msg1Handler (msgRef msg1Data clientData)
  (format T "msg1Handler: Receiving ~s (~d) [~s]~%" 
	  (IPC:IPC_msgInstanceName msgRef) msg1Data clientData))

(IPC:IPC_defun_handler queryHandler (msgRef queryData clientData)
  (declare (ignore clientData))
  (let ((str1 "Hello, world")
	t2)

  (format T "queryHandler: Receiving ~s [~a]~%" 
	  (IPC:IPC_msgInstanceName msgRef) queryData)

  ;; Publish this message -- all subscribers get it
  (format T "~%  (IPC_publishData ~s, ~s)~%" MSG2 str1)
  (IPC:IPC_publishData MSG2 str1)

  (setq t2 (make-T2 :str1 str1
		    ;; Variable length array of one element
		    :t1 (make-array '(1) :initial-contents (list queryData))
		    :count 1
		    ;; T2 supports symbolic enums, so can use keyword directly
		    :status :ReceiveVal))

  ;; Respond with this message -- only the query handler gets it
  (format T "~%  (IPC_respondData ~d ~s ~a)~%" msgRef RESPONSE1 t2)
  (IPC:IPC_respondData msgRef RESPONSE1 t2)
  ))

(defun stdinHnd (fd clientData)
  (declare (ignore fd))
  (let ((inputLine (read-line)))
    (case (aref inputLine 0)
      ((#\q #\Q) 
       (IPC:IPC_disconnect)  
       #+ALLEGRO (top-level:do-command "reset") #+LISPWORKS (abort)
       )
      (T (format T "stdinHnd [~s]: Received ~s" clientData inputLine)))))

(defun module2 ()

  ;; Connect to the central server
  (format T "~%(IPC_connect ~s)~%" MODULE2_NAME)
  (IPC:IPC_connect MODULE2_NAME)
  
  ;; Define the messages that this module publishes
  (format T "~%(IPC_defineMsg ~s IPC_VARIABLE_LENGTH ~s)~%" MSG2 MSG2_FORMAT)
  (IPC:IPC_defineMsg MSG2 IPC:IPC_VARIABLE_LENGTH MSG2_FORMAT)

  (format T "~%(IPC_defineMsg ~s IPC_VARIABLE_LENGTH ~s)~%"
	  RESPONSE1 RESPONSE1_FORMAT)
  (IPC:IPC_defineMsg RESPONSE1 IPC:IPC_VARIABLE_LENGTH RESPONSE1_FORMAT)

  ;; Subscribe to the messages that this module listens to.
  (format T "~%(IPC_subscribe ~s 'msg1Handler ~s)~%" MSG1 MODULE2_NAME)
  (IPC:IPC_subscribe MSG1 'msg1Handler MODULE2_NAME)

  (format T "~%(IPC_subscribe ~s 'queryHandler ~s)~%" QUERY1 MODULE2_NAME)
  (IPC:IPC_subscribe QUERY1 'queryHandler MODULE2_NAME)

  ;; Subscribe a handler for tty input. Typing "q" will quit the program
  (format T "~%(IPC_subscribeFD ~d 'stdinHnd ~s)~%" 0 MODULE2_NAME)
  (IPC:IPC_subscribeFD 0 'stdinHnd MODULE2_NAME)

  (format T "~%Type 'q' to quit~%")

  (IPC:IPC_dispatch)
  )
