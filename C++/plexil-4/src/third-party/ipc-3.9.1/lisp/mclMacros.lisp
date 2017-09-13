;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: New Millennium, DS1
;;;          IPC (Interprocess Communication) Package
;;;
;;; FILE: mclMacros.lisp
;;;
;;; ABSTRACT: Macros to allow MCL to use the Allegro foreign function
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
;;;  $Log: mclMacros.lisp,v $
;;;  Revision 2.2  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;

(eval-when (compile load eval)
  (ccl::add-to-shared-library-search-path "IPC.lisp.shlib"))

(defpackage :Foreign-Function
  (:nicknames "FF")
  (:use :common-lisp :ccl)
  (:export defforeign convert-to-lang defun-c-callable register-function)
  )

(in-package "FF")

(defun format-Type (type)
  (case type
    ((short :short) :short)
    ((integer :integer) :long)
    ((long :long) :long)
    ((fixnum :fixnum) :long)
    ((character :character) :character)
    ((string :string) :string)
    (float :singlefloat)
    ((double-float :double-float) :double-float)
    ((unsigned-long :unsigned-long) :long)
    ((signed-long :signed-long) :signed-long)
    ((void :void) nil)
    (:ptr :pointer)
    (otherwise (format T "format-Type: ~a~%" type)
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
        (ret-type (eval return-type))
        (fn-name (eval name))
        (entry (eval entry-point)))
    (cond ((not (or (pointer-arg-type ret-type)
                    (some #'pointer-arg-type args)))
           `(define-entry-point (,fn-name ("IPC.lisp.shlib" ,entry))
                ,(format-Args args) ,(format-Type ret-type)))
          ;; Complicated since MCL does not handle pointers very transparently
          (T (let* ((args-and-types (format-args args))
                    (formals (mapcar #'first args-and-types))
                    (fn-args (copy-list formals))
                    (lib-fn-name (intern (format nil "~A-LIB-FN" fn-name)))
                    macptrs setqs mformal)
               (do ((rest-formals args-and-types (cdr rest-formals))
                    (rest-fn-args fn-args (cdr rest-fn-args)))
                   ((null rest-formals))
                 (let ((formal (car rest-formals)))
                   (when (pointer-arg-type (second formal))
                     (setq mformal (intern (format nil "MPTR-~a" (first formal))))
                     (rplacd formal '(:POINTER))
                     (rplaca rest-fn-args mformal)
                     (push `(,mformal (1+ (length ,(first formal)))) macptrs)
                     (push `(%put-cstring ,mformal ,(first formal)) setqs))))
               (let ((fn-call `(,lib-fn-name ,@fn-args)))
                 (when (pointer-arg-type ret-type)
                   (setq fn-call `(%get-cstring ,fn-call)))
                 `(progn (define-entry-point (,lib-fn-name ("IPC.lisp.shlib" ,entry))
                           ,args-and-types ,(format-Type ret-type))
                         (defun ,fn-name ,formals
                           (%stack-block ,macptrs 
                             ,@setqs ,fn-call)))))))))

(defmacro convert-to-lang (string)
  string
  )

(defmacro register-function (name)
  `(%ptr-to-int 
    (ccl:pref (ccl:pref (eval ,name) :RoutineDescriptor.RoutineRecords)
              :RoutineRecord.ProcDescriptor))
  )

(defun foreign-lisp-args (args)
  (mapcan #'(lambda (arg) (list (format-type (second arg)) (first arg))) args))

(defmacro defun-c-callable (name args &body code)
  `(defccallable ,name ,(foreign-Lisp-Args args) ,@code)
  )

(ff:defun-c-callable lisp_value ((x :integer))
  (declare (ignore x))
  (format T "Calling lisp_value~%") NIL)
