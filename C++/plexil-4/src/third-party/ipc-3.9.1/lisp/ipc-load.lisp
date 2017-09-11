;;;; -*- Mode: LISP; Syntax: ANSI-COMMON-LISP; Package: CL-USER -*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: IPC (Interprocess Communication) Package
;;;          New Millennium, DS1
;;; 
;;; MODULE: IPC - lisp
;;;
;;; FILE: ipc-load.lisp
;;;
;;; ABSTRACT:
;;; 
;;; IPC - Lisp Load File
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
;;;  $Log: ipc-load.lisp,v $
;;;  Revision 2.3  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.2  2003/02/13 20:39:05  reids
;;;  Updated for ACL6.0
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;

(load (make-pathname :DIRECTORY (pathname-directory *load-truename*)
		     :NAME "ipc-system.lisp"))

(eval-when (eval load)
  #+(and :FRANZ-INC :UNIX (or :DLFCN :SVR4))
  (load (merge-pathnames #+:SVR4    "lib/Solaris-5.8/libipc_lisp.so"
			 #+:LINUX86 "lib/Linux/libipc_lisp.so"
			 *IPC-DIRECTORY*))
  #+(and :FRANZ-INC :UNIX (not :DLFCN))
  (load "" :foreign-files `(,(namestring (merge-pathnames "lib/SunOS/libipc.a"
							  *IPC-DIRECTORY*)))
	#+:SUN4 :UNREFERENCED-LIB-NAMES
	#+:SUN4 `(,(ff::convert-to-lang "IPC_connect")
		  ,(ff::convert-to-lang "IPC_respond")
		  ,(ff::convert-to-lang "IPC_marshall")
		  ,(ff::convert-to-lang "lisp_Data_Flag")))
  )

(operate-on-file-set2 *IPC-FILES* :load)
