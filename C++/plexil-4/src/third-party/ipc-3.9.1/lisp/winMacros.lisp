;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: New Millennium, DS1
;;;          IPC (Interprocess Communication) Package
;;;
;;; FILE: winMacros.lisp
;;;
;;; ABSTRACT: Macros to allow Windows lisp to use the Allegro foreign function
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
;;;  $Log: winMacros.lisp,v $
;;;  Revision 2.2  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;

(defpackage :Foreign-Function
  (:nicknames "FF")
  (:use :common-lisp)
  (:export defforeign convert-to-lang defun-c-callable register-function)
  )

(in-package "FF")

(defparameter *IPC_LIB* USER::*IPC_LIB*)

(defun format-Type (type)
  (case type
    ((integer :integer) :long)
    ((:signed-long)     :long)
    ((:unsigned-long)   :unsigned-long)
    ((:fixnum)          :long)
    ((:double-float double-float) :double-float)
    ((array)            :unsigned-long)
    ((string)          '(:char *))
    ((void :void)       :void)
    (otherwise (format T "~%format-Type: ~S~%" type)
     type)
    )
  )

(defun pointer-arg-type (arg)
  (member (format-type arg) '(:array :string)))

(defun format-Args (args)
  (let ((count 0))
    (mapcar #'(lambda (arg)
		(list (intern (format nil "PARAMETER~D" (incf count)))
		      (format-Type arg)))
	    args)))

;; This is the version needed when loading the function to use.
(defmacro defforeign (name &key entry-point arguments return-type)
  (let ((args (eval arguments))
        (ret-type (format-type (eval return-type)))
        (fn-name (eval name))
        (entry (eval entry-point)))
    `(CT:DEFUN-DLL ,fn-name ,(format-args args)
       :LIBRARY-NAME *IPC_LIB*
       :ENTRY-NAME ,entry
       :RETURN-TYPE ,ret-type)))

(defmacro convert-to-lang (string)
  string
  )

(defmacro register-function (name)
  `(CT:GET-CALLBACK-PROCINST ,name)
  )

(defun foreign-lisp-args (args)
  (mapcar #'(lambda (arg) (list (first arg) (format-type (second arg)))) args))

(defmacro defun-c-callable (name args &body code)
  `(CT:DEFUN-C-CALLBACK ,name ,(foreign-lisp-args args) ,@code))

(ff:defun-c-callable lisp_value ((x :integer))
  (declare (ignore x))
  (format T "Calling lisp_value~%") NIL)

