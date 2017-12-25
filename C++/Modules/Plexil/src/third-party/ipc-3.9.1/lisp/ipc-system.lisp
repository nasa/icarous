;;;; -*- Mode: LISP; Syntax: ANSI-COMMON-LISP; Package: CL-USER -*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: IPC (Interprocess Communication) Package
;;;          New Millennium, DS1
;;; 
;;; MODULE: IPC - lisp
;;;
;;; FILE: ipc-system.lisp
;;;
;;; ABSTRACT:
;;; 
;;; IPC - Lisp System Definition File
;;;
;;; $Revision: 2.3 $
;;; $Date: 2009/01/12 15:54:55 $
;;; $Author: reids $
;;;
;;; Copyright (c) 2008, Carnegie Mellon University
;;;     This software is distributed under the terms of the 
;;;    Simplified BSD License (see ipc/LICENSE.TXT)

;;; REVISION HISTORY
;;;
;;;  $Log: ipc-system.lisp,v $
;;;  Revision 2.3  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.2  2003/02/13 20:39:05  reids
;;;  Updated for ACL6.0
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:36  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;
;;;  DR 2/98 Broke out the IPC package def into its own file and
;;;  cleaned the loading process up.

(in-package #-:MCL :user #+:MCL :cl-user)

(eval-when (eval load execute)
  (pushnew :IPC *FEATURES*))


;; set up these file type defaults
(unless (boundp '*SOURCE*)
  (defconstant *SOURCE* "lisp"))
(unless (boundp '*EXECUTABLE*)
  (defconstant *EXECUTABLE*
    #-(and :ACLPC CLISP) "fasl" #+:ACLPC "fsl" #+CLISP "fas"))

;; Pathname object for IPC Lisp directory
;; DR 3/98 recall that I used to have this wrapped in
;; (unless (boundp '*IPC-LISP-DIRECTORY*))
(defparameter *IPC-LISP-DIRECTORY*
  (make-pathname :directory (pathname-directory *LOAD-TRUENAME*)
		 :host (pathname-host *LOAD-TRUENAME*)
		 :device (pathname-device *LOAD-TRUENAME*)))

(defparameter *IPC-LISP-BIN-DIRECTORY*
  (make-pathname
   :directory (append (pathname-directory *IPC-LISP-DIRECTORY*)
		      (list "bin"
			    #+(and :FRANZ-INC :UNIX
				   :DLFCN (not :LINUX86)) "Solaris"
			    #+(and :FRANZ-INC :UNIX (not :DLFCN)) "SunOS"
			    #+(and :FRANZ-INC :UNIX :LINUX86) "Linux"
			    #+(and :FRANZ-INC :ACLNT) "w95"
			    #+(and :FRANZ-INC :ACLPC (not :ACLNT)) "w95"
			    #+(and :FRANZ-INC :MSWINDOWS) "wnt"))
   :host (pathname-host *IPC-LISP-DIRECTORY*)
   :device (pathname-device *IPC-LISP-DIRECTORY*)))

;; Pathname object for IPC directory
(defparameter *IPC-DIRECTORY*
  (make-pathname :directory (butlast (pathname-directory *IPC-LISP-DIRECTORY*))
		 :host (pathname-host *IPC-LISP-DIRECTORY*)
		 :device (pathname-device *IPC-LISP-DIRECTORY*)))

(defconstant *IPC-FILES*
  '(("ipc-package"	        *IPC-LISP-DIRECTORY*)
    #+:ACLPC("winMacros"	*IPC-LISP-DIRECTORY*)
    #+:MCL("mclMacros"		*IPC-LISP-DIRECTORY*)
    #+:ALLEGRO-V6.0("acl60Macros" *IPC-LISP-DIRECTORY*)
    ("primFmttrs"		*IPC-LISP-DIRECTORY*)
    ("formatters"		*IPC-LISP-DIRECTORY*)
    ("ipcForeignCalls"		*IPC-LISP-DIRECTORY*)))


;; DR 3/98 Modified so its not the same as operate-on-file-set
(defun operate-on-file-set2 (file-set op)
  ; (break "inside operate-on-file-set2")
  (dolist (item file-set)
    (let* ((filename (or (and (stringp (first item)) (first item))
			 (string-downcase (string (first item)))))
	   (source-directory (symbol-value (second item)))
	   (binary-directory *IPC-LISP-BIN-DIRECTORY*)
	   (source (probe-file (merge-pathnames
				(format nil "~A.~A" filename *SOURCE*)
				source-directory)))
	   (executable-path (merge-pathnames
			     (format nil "~A.~A" filename *EXECUTABLE*)
			     binary-directory))
	   (executable (probe-file executable-path)))
      (unless (or (and (eq op :load) (or executable source))
		  (and (eq op :compile) source))
	(error "Missing file: ~A from source-directory ~A" filename source-directory))
      (if (eq op :load)
	  (load (or executable source))
	(progn (compile-file source :output-file executable-path)
	       (load (probe-file executable-path)))))))

#+:ACLPC
(defparameter *IPC_LIB* 
  (merge-pathnames #+(and :DLFCN (not :LINUX86)) "lib/Solaris/libipc.so"
		   #+(and :DLFCN :LINUX86)	 "lib/Linux/libipc.a"
		   #+:ACLNT			 "lib/wnt/ipc.dll"
		   #+(and :ACLPC (not :ACLNT))	 "lib/w95/ipcw95.dll"
		   #+:MSWINDOWS			 "lib/wnt/ipc.dll"
		   *IPC-DIRECTORY*))
