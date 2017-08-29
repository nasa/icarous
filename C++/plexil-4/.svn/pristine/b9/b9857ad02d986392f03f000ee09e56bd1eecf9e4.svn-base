/;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PROJECT: New Millennium, DS1
;;          IPC (Interprocess Communication) Package
;;
;; (c) Copyright 1996 Reid Simmons.  All rights reserved.
;;
;; FILE: module3.lisp
;;
;; ABSTRACT: Test program for IPC.
;;             Publishes: MSG2
;;             Subscribes to: MSG1, QUERY1
;;             Responds with: RESPONSE1
;;             Behavior: Prints out the message data received.
;;                       Exits when 'q' is typed at terminal
;;                       Should be run in conjunction with module1 and module2
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
;; $Log: module3.lisp,v $
;; Revision 2.2  2009/01/12 15:54:58  reids
;; Added BSD Open Source license info
;;
;; Revision 2.1.1.1  1999/11/23 19:07:38  reids
;; Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;
;; Revision 1.1.2.2  1997/01/25 23:18:21  udo
;; ipc_2_6 to r3 merge
;;
;; Revision 1.1.2.1.6.1  1996/12/24 15:25:40  reids
;; Change name of "main" function to enable multiple modules to be loaded
;;   simultaneously
;;
;; Revision 1.1.2.1  1996/10/02 20:38:49  reids
;; Explicitly label functions as "IPC:"
;; Changes to support LISPWORKS.
;;
;; Revision 1.1  1996/06/17 18:43:36  rouquett
;; ipc test files
;;
;; Revision 1.1  1996/03/06 20:19:33  reids
;; New test programs for passing data between C and LISP modules
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;/

;;; Load the common file with all the type and name definitions
(load (make-pathname :DIRECTORY (pathname-directory *LOAD-TRUENAME*)
		     :NAME "module.lisp"))

;; This is copied from module2.c;  The "right" way to do it is to define
;;   it once, in a separate file, and link both modules with the same function
(IPC:IPC_defun_handler msg1Handler (msgRef msg1Data clientData)
  (format T "msg1Handler: Receiving ~s (~d) [~s]~%" 
	  (IPC:IPC_msgInstanceName msgRef) msg1Data clientData))

;; This is copied from module1.c;  The "right" way to do it is to define
;;   it once, in a separate file, and link both modules with the same function
(IPC:IPC_defun_handler msg2Handler (msgRef lispData clientData)
  (format T "msg2Handler: Receiving ~s (~s) [~s]~%" 
	  (IPC:IPC_msgInstanceName msgRef) lispData clientData))

(defun stdinHnd (fd clientData)
  (declare (ignore fd))
  (let ((inputLine (read-line)))
    (case (aref inputLine 0)
      ((#\q #\Q) 
       (IPC:IPC_disconnect)
       #+ALLEGRO (top-level:do-command "reset") #+LISPWORKS (abort)
       )
      (T (format T "stdinHnd [~s]: Received ~s" clientData inputLine)))))

(defun module3 ()

  ;; Connect to the central server
  (format T "~%(IPC_connect ~s)~%" MODULE3_NAME)
  (IPC:IPC_connect MODULE3_NAME)

  ;; Subscribe to the messages that this module listens to.
  (format T "~%(IPC_subscribe ~s 'msg1Handler ~s)~%" MSG1 MODULE3_NAME)
  (IPC:IPC_subscribe MSG1 'msg1Handler MODULE3_NAME)

  (format T "~%(IPC_subscribe ~s 'msg2Handler ~s)~%" MSG2 MODULE3_NAME)
  (IPC:IPC_subscribe MSG2 'msg2Handler MODULE3_NAME)

  ;; Subscribe a handler for tty input. Typing "q" will quit the program
  (format T "~%(IPC_subscribeFD ~d 'stdinHnd ~s)~%" 0 MODULE3_NAME)
  (IPC:IPC_subscribeFD 0 'stdinHnd MODULE3_NAME)

  (format T "~%Type 'q' to quit~%")

  (IPC:IPC_dispatch)
  )
