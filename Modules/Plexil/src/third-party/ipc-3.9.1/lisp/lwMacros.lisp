;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: New Millennium, DS1
;;;          IPC (Interprocess Communication) Package
;;;
;;; FILE: lwMacros.lisp
;;;
;;; ABSTRACT: Macros to allow lispworks to use the Allegro foreign function
;;;           interface that we already have.
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
;;;  $Log: lwMacros.lisp,v $
;;;  Revision 2.2  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;  Revision 1.1.2.2  1996/10/08 14:27:54  reids
;;;  Changes to enable IPC to run under Lispworks on the PPC.
;;;  Main changes due to the fact that Lispworks currently does not support
;;;  "foreign-callable" and that it corrupts the stack when a double is sent
;;;  as the first argument to a foreign (C) function.
;;;
;;;  Revision 1.1.2.1  1996/10/02 20:48:45  reids
;;;  Changes to support LISPWORKS.
;;;
;;;

(defpackage :Foreign-Function
  (:nicknames "FF")
  (:use :foreign :user :lisp)
  (:export defforeign convert-to-lang defun-c-callable register-function)
  )

(in-package "FF")

;;(ff:defforeign 'returnLispFlagGlobal
;;  :entry-point (ff::convert-to-lang "returnLispFlagGlobal")
;;  :arguments '(integer integer)
;;  :return-type :integer)

(defun formatType (type)
  (case type
    (integer :integer)
    (fixnum :fixnum)
    (character :character)
    (string :string)
    (float :float)
    (double-float :double-float)
    ((unsigned-long :unsigned-long) :uinteger)
    ((signed-long :signed-long) :uinteger)
    ((lisp :lisp) :as-is)
    (array :array)
    ((void :void) nil)
    (otherwise 
     type)
    )
  )

(defun formatArgs (args)
  (let ((count 0))
    (mapcar #'(lambda (arg)
		(list (intern (format nil "PARAMETER~D" (incf count)))
		      (formatType arg)))
	    args)))

;; This is the version needed when loading the function to use.
(defmacro defforeign (name &key entry-point arguments return-type)
  `(define-foreign-function
     (,(eval name) ,(eval entry-point))
     ,(formatArgs (eval arguments))
     :RESULT-TYPE ,(formatType (eval return-type))
     :LANGUAGE :ANSI-C)
     )

(defmacro convert-to-lang (string)
#-(and LISPWORKS VXWORKS)
  (format NIL "_~a" string)
#+(and LISPWORKS VXWORKS)
  string
  )

(defmacro register-function (name)
  `(foreign:foreign-symbol-address ,name :ERRORP NIL)
  )

#-LISPWORKS_FFI_HACK
(defun extractLispArgs (args)
  (mapcar #'first args))

#-LISPWORKS_FFI_HACK
(defun typeConvertArgs (args)
  (mapcar #'(lambda (arg) (formatType (second arg))) args))

#-LISPWORKS_FFI_HACK
(defmacro defun-c-callable (name args &body code)
  `(progn
    (defun ,name ,(extractLispArgs args) ,@code)
    (foreign:foreign-callable ,name ,(typeConvertArgs args)
			      :result-type :general)
    )
  )

#-LISPWORKS_FFI_HACK
(ff:defun-c-callable lisp_value ((x :integer))
  (declare (ignore x))
  (format T "Calling lisp_value~%") NIL)
