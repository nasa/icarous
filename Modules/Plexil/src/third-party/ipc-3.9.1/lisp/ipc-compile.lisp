;;;; -*- Mode: LISP; Syntax: ANSI-COMMON-LISP; Package: CL-USER -*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: IPC (Interprocess Communication) Package
;;;          New Millennium, DS1
;;; 
;;; MODULE: IPC - lisp
;;;
;;; FILE: ipc-compile.lisp
;;;
;;; ABSTRACT:
;;; 
;;; IPC - Lisp Compile File
;;;
;;; $Revision: 2.2 $
;;; $Date: 2009/01/12 15:54:55 $
;;; $Author: reids $
;;;
;;; Copyright (c) 2008, Carnegie Mellon University
;;;     This software is distributed under the terms of the 
;;;    Simplified BSD License (see ipc/LICENSE.TXT)

;;; REVISION HISTORY
;;;
;;;  $Log: ipc-compile.lisp,v $
;;;  Revision 2.2  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;

(load (make-pathname :DIRECTORY (pathname-directory *load-truename*)
		     :NAME "ipc-system.lisp"))

(operate-on-file-set2 *IPC-FILES* :compile)

