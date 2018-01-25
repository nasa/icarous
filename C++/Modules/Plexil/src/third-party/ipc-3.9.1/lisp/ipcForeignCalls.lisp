;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: New Millennium, DS1
;;;          IPC (Interprocess Communication) Package
;;; 
;;; FILE: ipcForeignCalls.lisp
;;;
;;; ABSTRACT: Implement the IPC functions, mainly through foreign-function
;;;             interface to the C-language IPC library.
;;;           Adapted from the tcaForeignCalls.lisp file.
;;;
;;; $Revision: 2.4 $
;;; $Date: 2009/01/12 15:54:55 $
;;; $Author: reids $
;;;
;;; Copyright (c) 2008, Carnegie Mellon University
;;;     This software is distributed under the terms of the 
;;;    Simplified BSD License (see ipc/LICENSE.TXT)

;;; REVISION HISTORY
;;;
;;;  $Log: ipcForeignCalls.lisp,v $
;;;  Revision 2.4  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.3  2003/02/13 20:39:05  reids
;;;  Updated for ACL6.0
;;;
;;;  Revision 2.2  2001/01/10 15:32:08  reids
;;;  Added the function IPC_subscribeData that automatically unmarshalls
;;;    the data before invoking the handler.
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:36  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;  Revision 1.4.2.11  1997/02/26 04:16:54  reids
;;;  Added IPC_isMsgDefined.
;;;
;;;  Revision 1.4.2.10  1997/02/10 15:12:12  reids
;;;  Added IPC_checkMsgFormats to the Lisp version.
;;;
;;;  Revision 1.4.2.9  1997/01/25 22:44:00  udo
;;;  ipc_2_6 to r3_dev merge
;;;
;;;  Revision 1.4.2.7.4.4  1997/01/17 21:18:27  reids
;;;  Fixed the Lispworks version if a handler is aborted (by a throw or error) --
;;;  correctly restore internal state and notify central that the handler ended
;;;
;;;  Revision 1.4.2.7.4.3  1997/01/16 22:34:03  reids
;;;  Took out restriction that Lisp publishing and marshalling was non-reentrant
;;;  (needed for multi-tasking in Lisp).
;;;  Improved error checking for Lisp functions.
;;;
;;;  Revision 1.4.2.7.4.2  1996/12/24 14:27:19  reids
;;;  Add the package name in the generated handler name, so that handlers
;;;  with the same name (but in different packages) can co-exist.
;;;
;;;  Revision 1.4.2.7  1996/12/18 15:13:35  reids
;;;  Fixed bug in Lispworks/VxWorks handling of messages, where a message
;;;  could be "stranded" if it arrived during a blocking query
;;;
;;;  Revision 1.4.2.6  1996/10/28 16:04:14  reids
;;;  Added the error constant IPC_Wrong_Buffer_Length
;;;
;;;  Revision 1.4.2.5  1996/10/28 16:01:02  reids
;;;  Changed IPC_verbosity to IPC_setVerbosity, to be consistent with C.
;;;
;;;  Revision 1.4.2.4  1996/10/24 16:57:53  reids
;;;  Add additional functions: IPC_perror, IPC_initialize, IPC_setVerbosity.
;;;
;;;  Revision 1.4.2.3  1996/10/24 16:12:50  reids
;;;  Forgot to change "directInfo" to "connection"
;;;
;;;  Revision 1.4.2.2  1996/10/08 14:27:52  reids
;;;  Changes to enable IPC to run under Lispworks on the PPC.
;;;  Main changes due to the fact that Lispworks currently does not support
;;;  "foreign-callable" and that it corrupts the stack when a double is sent
;;;  as the first argument to a foreign (C) function.
;;;
;;;  Revision 1.4.2.1  1996/10/02 20:48:42  reids
;;;  Changes to support LISPWORKS.
;;;
;;;  Revision 1.4  1996/05/26 04:12:19  reids
;;;  Added functions IPC_setCapacity and IPC_dataLength
;;;
;;;  Revision 1.3  1996/03/31 23:55:41  reids
;;;  Added code (from Erann) to make it possible to gracefully abort from
;;;  within a message handler.
;;;
;;;  Revision 1.2  1996/03/06 20:25:25  reids
;;;  Version 2.3 adds two new functions: IPC_defineFormat and IPC_isConnected
;;;
;;;  Revision 1.1  1996/03/03 04:38:57  reids
;;;  First release of IPC files.  Corresponds to LISP version of
;;;  IPC Specifiction 2.2, except that IPC_printData and IPC_readData are
;;;  not yet implemented.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(in-package :IPC)

;; DR 2/98 Revised all the windows *features*

;; NOTE: All WINDOWS options are added by TNgo, 5/19/97, for PC Allegro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                INTERNAL DECLARATIONS AND FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defstruct (handler-data)
  name
  proc
  client-data)

#-LISPWORKS_FFI_HACK   (defparameter *LISP-FLAG-GLOBAL* NIL)
#-LISPWORKS_FFI_HACK   (defvar *LISP-ENCODE-VALUE* nil)
#-LISPWORKS_FFI_HACK   (defvar *LISP-DECODE-VALUE* nil)
#-LISPWORKS_FFI_HACK   (defvar *FORMATTER-VALUE* nil)

(defparameter *IPC_HASH_TABLE* (make-hash-table)
  "Hash table of function name, procedure pointer, client data")

(defun sympackage (symbol)
  (let ((package (symbol-package symbol)))
    (or (first (package-nicknames package)) (package-name package))))

(defun ipc-Handler-Name (msgName handler)
  (format nil "HND-~a-~a-~a"
	  (string-upcase msgName) (sympackage handler) handler))

(defun fd-handler-name (fd handler)
  (format NIL "FHND-~d-~a-~a" fd (sympackage handler) handler))

(defun ipc-get-handler-data (handler-name)
  (gethash handler-name *IPC_HASH_TABLE*))

(defun ipc-set-handler-data (handler-name proc client-data)
  (setf (gethash handler-name *IPC_HASH_TABLE*) 
	(make-handler-data :NAME handler-name :PROC proc 
			   :CLIENT-DATA client-data)))

(defun ipc-remove-handler-data (handler-name)
  (remhash handler-name *IPC_HASH_TABLE*))

(defun ipc-handler-data (handler-name)
  (or (ipc-get-handler-data handler-name)
      (ipc-set-handler-data handler-name 0 NIL)))

(defun get-client-data (handler-name)
  (handler-data-client-data (ipc-handler-data handler-name)))

(defun set-client-data (handler-name client-data)
  (setf (handler-data-client-data (ipc-handler-data handler-name))
	client-data))

(defconstant UNKNOWN_LANGUAGE  0)
(defconstant C_LANGUAGE        1)
(defconstant LISP_LANGUAGE     2)
(defconstant RAW_C_LANGUAGE    3)
(defconstant RAW_LISP_LANGUAGE 4)

(ff:defforeign 'lisp_Data_Flag
  :entry-point (ff::convert-to-lang "lisp_Data_Flag")
  :arguments   '()
  :return-type :integer)

(ff:defforeign 'setIntValue
  :entry-point (ff::convert-to-lang "setIntValue")
  :arguments   '(integer integer)
  :return-type :void)

(ff:defforeign 'ipcSetSizeEncodeDecode
  :entry-point (ff::convert-to-lang "x_ipcSetSizeEncodeDecode")
  :arguments   '(integer integer integer integer integer)
  :return-type :void)

;;; if dataTotal is 0 then return null as well -
;;; have c code set format to 0
#-LISPWORKS_FFI_HACK
(ff::defun-c-callable lispDecode ((format :unsigned-long) 
				  (buffer :unsigned-long))
  (format T "Calling lispDecode~%")
  (setf *LISP-DECODE-VALUE*
	(cond ((eql format 0) NIL)
	      (T (DecodeData format buffer)))))

#-LISPWORKS_FFI_HACK
(ff::defun-c-callable lispEncode ((format :unsigned-long)
				  (buffer :unsigned-long))
  (format T "Calling lispEncode~%")
  (EncodeData format *LISP-ENCODE-VALUE* buffer))

#-LISPWORKS_FFI_HACK
(ff::defun-c-callable lispBufferSize ((msgTotal :unsigned-long)
				      (format   :unsigned-long))
  (format T "Calling lispBufferSize~%")
  (let ((x (BufferSize format *LISP-ENCODE-VALUE*)))
    (setIntValue msgTotal x)))

#-LISPWORKS_FFI_HACK
(ff:defun-c-callable lispQueryResponse ((data :unsigned-long)
					(formatter :unsigned-long))
  (setq *LISP-DECODE-VALUE* data)
  (setq *FORMATTER-VALUE* formatter))

#-LISPWORKS_FFI_HACK
(ff::defun-c-callable lispExit ()
  (progn 
    (format T "~% Returning to lisp top level ~%")
    #-(or LISPWORKS MCL ACLPC) (top-level:do-command "reset")
    #+(or LISPWORKS MCL) (abort)
    ))

#+(AND ALLEGRO (NOT ALLEGRO-V6.0))
(ff:defforeign 'ipcRefNameLength
  :entry-point (ff::convert-to-lang "x_ipcRefNameLength")
  :arguments   '(integer)
  :return-type :integer) 

#+(AND ALLEGRO (NOT ALLEGRO-V6.0))
(ff:defforeign 'ipcRefNameLisp
  :entry-point (ff::convert-to-lang "x_ipcRefNameLisp")
  :arguments   '(integer integer)
  :return-type :lisp) 

(ff:defforeign 'ipcSetEncodeBuffer
  :entry-point (ff::convert-to-lang "ipcSetEncodeBuffer")
  :arguments   '(integer #|byteArray|#)
  :return-type :integer #|encode-buffer|#)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                CONSTANTS AND TYPE DECLARATIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defconstant IPC_WAIT_FOREVER -1)

(defconstant IPC_VARIABLE_LENGTH -1)
(defconstant IPC_FIXED_LENGTH    -2)

(defconstant IPC_Error   0)
(defconstant IPC_OK      1)
(defconstant IPC_Timeout 2)

(defconstant IPC_No_Error               0)
(defconstant IPC_Not_Connected          1)
(defconstant IPC_Not_Initialized        2)
(defconstant IPC_Message_Not_Defined    3)
(defconstant IPC_Not_Fixed_Length       4)
(defconstant IPC_Message_Lengths_Differ 5)
(defconstant IPC_Argument_Out_Of_Range  6)
(defconstant IPC_Null_Argument          7)
(defconstant IPC_Illegal_Formatter      8)
(defconstant IPC_Wrong_Buffer_Length    9)
(defconstant IPC_Communication_Error   10)

(defconstant IPC_Silent         0)
(defconstant IPC_Print_Warnings 1)
(defconstant IPC_Print_Errors   2)
(defconstant IPC_Exit_On_Errors 3)

(defstruct (IPC_VARCONTENT)
  (length 0  :type integer)
  (content 0 :type integer)) ;; A C-pointer to a byte array

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                BASIC IPC INTERFACE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#+MCL
(ff:defun-c-callable lisp-printer ((buf :ptr) (n :long))
  (loop for i below n 
        do (write-char (the char (code-char 
                                  (the byte (ccl:%get-byte buf i)))))))

#+MCL
(ff:defforeign 'setup-printer
  :entry-point (ff::convert-to-lang "mclSetupLispPrinter")
  :arguments '(:ptr)
  :return-type :void)

#+MCL
(ff:defforeign 'setup-event-handler
  :entry-point (ff::convert-to-lang "mclSetupEventHandler")
  :arguments '(:ptr)
  :return-type :void)

#+MCL
(ff:defun-c-callable event-handler ((event :ptr))
  (ccl::process-event event)
  )

(defun ipc-lisp-init()

  #+MCL (setup-printer (ff:register-function 'lisp-printer))
  #+MCL (setup-event-handler (ff:register-function 'event-handler))

  (setf (aref TransTable CHAR_FMT) 'CHAR_Trans)
  (setf (aref TransTable BYTE_FMT) 'BYTE_Trans)
  (setf (aref TransTable UBYTE_FMT) 'UBYTE_Trans)
  (setf (aref TransTable SHORT_FMT) 'SHORT_Trans)
  (setf (aref TransTable INT_FMT) 'INT_Trans)
  (setf (aref TransTable LONG_FMT) 'INT_Trans)
  (setf (aref TransTable FLOAT_FMT) 'FLOAT_Trans)
  (setf (aref TransTable DOUBLE_FMT) 'DOUBLE_Trans)
  (setf (aref TransTable BOOLEAN_FMT) 'BOOLEAN_Trans)
  (setf (aref TransTable STR_FMT) 'STR_Trans)

  (setf (aref TransTable USHORT_FMT) 'USHORT_Trans)
  (setf (aref TransTable UINT_FMT) 'UINT_Trans)
  (setf (aref TransTable ULONG_FMT) 'UINT_Trans))

(defun return-error (errno)
  (ipcSetError errno) 
  IPC_Error)

(ff:defforeign 'IPC_initializeC
  :entry-point (ff::convert-to-lang "_IPC_initialize")
  :arguments   '(integer #| isLispModule |#)
  :return-type :integer)

(defun IPC_initialize ()
  (ipc-lisp-init)
  (IPC_initializeC 1))

(ff:defforeign 'IPC_connectC
  :entry-point (ff::convert-to-lang "_IPC_connect")
  :arguments   '(string #|module-name|# string #|server-name|# 
			integer #| isLispModule |#)
  :return-type :integer)

;; Added by TNgo, 5/19/97, since JSC/ARS&D needed this functionality
(defun IPC_connectModule (task-name server-name)
  (ipc-lisp-init)
  (prog1
    (IPC_connectC task-name server-name 1)
    #-LISPWORKS_FFI_HACK (setf *LISP-FLAG-GLOBAL* (lisp_Data_Flag))
    #-LISPWORKS_FFI_HACK
    (ipcSetSizeEncodeDecode (ff:register-function 'lispBufferSize)
			    (ff:register-function 'lispEncode)
			    (ff:register-function 'lispDecode)
			    (ff:register-function 'lispExit)
			    (ff:register-function 'lispQueryResponse))))

(defun IPC_connect (task-name)
  (IPC_connectModule task-name ""))

(ff:defforeign 'IPC_disconnect
  :entry-point (ff::convert-to-lang "IPC_disconnect")
  :arguments   '()
  :return-type :integer)

(ff:defforeign 'IPC_isConnectedC
  :entry-point (ff::convert-to-lang "IPC_isConnected")
  :arguments   '()
  :return-type :integer)

(defun IPC_isConnected ()
  (not (zerop (IPC_isConnectedC))))

(ff:defforeign 'IPC_defineMsgC
  :entry-point (ff::convert-to-lang "IPC_defineMsg")
  :arguments   '(string #|msg-name|# integer #|length|# string #|format-string|#)
  :return-type :integer)

(defun IPC_defineMsg (msg-name length format-string)
  (IPC_defineMsgC msg-name length (or format-string "")))

;; TNgo, 5/19/97, this is new in version 2.7
(ff:defforeign 'IPC_isMsgDefinedC
  :entry-point (ff::convert-to-lang "IPC_isMsgDefined")
  :arguments   '(string #|msg-name|#)
  :return-type :integer)

(defun IPC_isMsgDefined (msg-name)
  (not (zerop (IPC_isMsgDefinedC msg-name))))

(ff:defforeign 'IPC_publishC
  :entry-point (ff::convert-to-lang "IPC_publish")
  :arguments   '(string #|msg-name|# integer #|length|# integer #|content|#)
  :return-type :integer)

;; Not exported -- discourage its use in LISP;
;; Use IPC_publishData, instead
(defun IPC_publish (msg-name length content)
  (IPC_publishC msg-name length (or content 0)))

;; Not exported -- discourage its use in LISP;
;; Use IPC_publishData, instead
(defun IPC_publishVC (msg-name varcontent)
  ;;(declare (type (msg-name string) (varcontent IPC_VARCONTENT)))
  (IPC_publish msg-name (IPC_VARCONTENT-length varcontent)
               (IPC_VARCONTENT-content varcontent)))

;; Not exported -- discourage its use in LISP;
;; Use IPC_publishData, instead
(defun IPC_publishFixed (msg-name content)
  (IPC_publish msg-name IPC_FIXED_LENGTH content))

#+(AND ALLEGRO (NOT ALLEGRO-V6.0))
(defun IPC_msgInstanceName (msg-instance)
  (cond ((or (null msg-instance) (zerop msg-instance)) 
	 (return-error IPC_Null_Argument))
	(T (let ((length (ipcRefNameLength msg-instance)))
	     (cond ((zerop length) NIL)
		   (T (setq *LISP-C-GLOBAL-STRING-BUF* (make-string length))
		      (multiple-value-bind (index value)
			  (ff:register-value '*LISP-C-GLOBAL-STRING-BUF* 0)
			(declare (ignore index) (ignore value))
			(ipcRefNameLisp msg-instance length))))))))

#+(OR LISPWORKS MCL ALLEGRO-V6.0)
(ff:defforeign 'IPC_msgInstanceName
  :entry-point (ff::convert-to-lang "IPC_msgInstanceName")
  :arguments   '(integer #|msg-instance|#)
  :return-type :string)

#+:ACLPC
(ct:defun-dll IPC_msgInstanceNameC ((msgInstance :long))
  :library-name FF::*IPC_LIB*
  :entry-name   "IPC_msgInstanceName"
  :return-type  (:char *))
 
#+:ACLPC
(defun IPC_msgInstanceName (msgInstance)
  (let* ( (LispCvar (ct:ccallocate (:char 125)))
          (Cvar (IPC_msgInstanceNameC msgInstance))
	    myString len)
    (ct::far-peek LispCvar Cvar 0 124)
    (setq myString (subseq LispCvar 0 124))
    (setq len (ct:strlen myString))
    (subseq myString 0 len)))

;;; Create and foreign-register the function, but only once.
;;; Have function always return NIL, since return value not needed.
#-LISPWORKS_FFI_HACK
(defun register-foreign-function (function-name args &rest body)
  (unless (fboundp function-name)
    (eval `(ff::defun-c-callable ,function-name ,args ,@body NIL))))

(defun update-handler-data (function-name client-data
					  #+LISPWORKS_FFI_HACK handler-proc)
  (let ((handler-data (ipc-get-handler-data function-name)))
    (unless handler-data
      (setq handler-data 
	    (ipc-set-handler-data 
	     function-name
	     #-LISPWORKS_FFI_HACK (ff:register-function function-name)
	     #+LISPWORKS_FFI_HACK handler-proc
	     NIL)))
    (setf (handler-data-client-data handler-data) client-data)
    #-LISPWORKS_FFI_HACK (handler-data-proc handler-data)
    #+LISPWORKS_FFI_HACK 1))

(ff:defforeign 'IPC_subscribeC
  :entry-point (ff::convert-to-lang "_IPC_subscribe")
  :arguments   '(string #|msg-name|# string #|hnd-name|#
		 integer #|handler|# integer #|client-data|#
		 integer #|autoUnmarshall|#)
  :return-type :integer)

(defun IPC_subscribe (msg-name handler client-data)
  ;; DR 2/98 These type declarations were wrong
  #-:MCL (declare (type (string msg-name)
		  (symbol handler)
		  (sexpr client-data)))
  (let* ((hnd-name (ipc-handler-name msg-name handler))
	 (function-name (intern hnd-name)))
    ;; Create a handler function that C can call 
    ;; (all arguments to the function are pointers)
    ;; On VxWorks, C cannot call Lispworks functions
    #-LISPWORKS_FFI_HACK
    (register-foreign-function function-name
			       '((msg-ref :signed-long) 
				 (call-data :signed-long)
				 (client-data :signed-long))
			       `(,handler msg-ref call-data 
					  (unless (zerop client-data)
					    (get-client-data ',function-name))))
    ;; Set the client data and get the procedure pointer
    ;; (after the function has been registered)
    (let ((func (update-handler-data function-name client-data
				     #+LISPWORKS_FFI_HACK handler)))
      ;; Subscribe to the message
      (IPC_subscribeC msg-name hnd-name func (if client-data 1 0) 0))))

(defun IPC_subscribeData (msg-name handler client-data)
  #-:MCL (declare (type (string msg-name)
		  (symbol handler)
		  (sexpr client-data)))
  (let* ((hnd-name (ipc-handler-name msg-name handler))
	 (function-name (intern hnd-name)))
    ;; Create a handler function that C can call 
    ;; (all arguments to the function are pointers)
    ;; On VxWorks, C cannot call Lispworks functions
    #-LISPWORKS_FFI_HACK
    (register-foreign-function 
     function-name '((msg-ref :signed-long) 
		     (call-data :signed-long)
		     (client-data :signed-long))
     `(let (lisp-data flagvar)
	(unwind-protect
	    (progn (IPC_unmarshall (IPC_msgInstanceFormatter msg-ref)
				   call-data lisp-data)
		   (IPC_freeByteArray call-data)
		   (,handler msg-ref lisp-data 
			     (unless (zerop client-data)
			       (get-client-data ',function-name)))
		   (setf flagvar T))
	  (unless flagvar (ipc-success msg-ref)))))
    ;; Set the client data and get the procedure pointer
    ;; (after the function has been registered)
    (let ((func (update-handler-data function-name client-data
				     #+LISPWORKS_FFI_HACK handler)))
      ;; Subscribe to the message
      (IPC_subscribeC msg-name hnd-name func (if client-data 1 0) 0))))

(ff:defforeign 'IPC_unsubscribeC
  :entry-point (ff::convert-to-lang "_IPC_unsubscribe")
  :arguments '(string #|msg-name|# string #|hnd-name|#)
  :return-type :integer)

(defun IPC_unsubscribe (msg-name handler &optional clearP)
  ;;(declare (type (msg-name string) (handler symbol)))
  (let ((hnd-name (ipc-handler-name msg-name handler)))
    ;; Clear the client data (or remove data from hash table entirely)
    (cond (clearP (ipc-remove-handler-data (intern hnd-name)))
	  (T (set-client-data (intern hnd-name) NIL)))
    (IPC_unsubscribeC msg-name hnd-name)))

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_subscribeFD_C
  :entry-point (ff::convert-to-lang "IPC_subscribeFD")
  :arguments '(integer #|fd|# integer #|handler|# integer #|client-data|#)
  :return-type :integer)

#-LISPWORKS_FFI_HACK
(defun IPC_subscribeFD (fd handler client-data)
  ;;(declare (type (fd integer) (handler symbol) (client-data sexpr)))
  (let ((function-name (intern (fd-handler-name fd handler))))
    ;; Create a handler function that C can call (both arguments
    ;;  to the function are C-pointers)
    (register-foreign-function function-name
			       '((fd :fixnum) (client-data :signed-long))
			       `(,handler fd
					  (unless (zerop client-data)
					    (get-client-data ',function-name))))
    ;; Set the client data and get the procedure pointer
    ;;  (after the function has been registered)
    (let ((func (update-handler-data function-name client-data)))
      ;; Subscribe to the fd-events
      (IPC_subscribeFD_C fd func (if client-data 1 0)))))

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_unsubscribeFD_C
  :entry-point (ff::convert-to-lang "IPC_unsubscribeFD")
  :arguments '(integer #|fd|# integer #|handler|#)
  :return-type :integer)

#-LISPWORKS_FFI_HACK
(defun IPC_unsubscribeFD (fd handler &optional clearP)
  ;;(declare (type (fd integer) (handler symbol)))
  (let ((function-name (intern (fd-handler-name fd handler))))
    ;; Clear the client data (or remove data from hash table entirely)
    (cond (clearP (ipc-remove-handler-data function-name))
	  (T (set-client-data function-name NIL))))
  ;; Second argument is ignored by the C function
  (IPC_unsubscribeFD_C fd 0))

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_listen
  :entry-point (ff::convert-to-lang "IPC_listen")
  :arguments '(integer #|timeoutMSecs|#)
  :return-type :integer)

#+LISPWORKS_FFI_HACK (defconstant ExecHnd_Idle 0)
#+LISPWORKS_FFI_HACK (defconstant ExecHnd_Pending 1)
#+LISPWORKS_FFI_HACK (defconstant ExecHnd_Handling 2)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-state
  :entry-point (ff::convert-to-lang "execHndState_state")
  :arguments NIL
  :return-type :integer #| EXECHND_STATE_ENUM |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'set-execHndState-state
  :entry-point (ff::convert-to-lang "setExecHndState_state")
  :arguments '(:integer #| EXECHND_STATE_ENUM |#)
  :return-type :void)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-tcaRef
  :entry-point (ff::convert-to-lang "execHndState_tcaRef")
  :arguments NIL
  :return-type :integer #| TCA_REF_PTR |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-hndName
  :entry-point (ff::convert-to-lang "execHndState_hndName")
  :arguments NIL
  :return-type :string #| handler-name  |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-data
  :entry-point (ff::convert-to-lang "execHndState_data")
  :arguments NIL
  :return-type :integer #| Generic Data Pointer |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-clientData
  :entry-point (ff::convert-to-lang "execHndState_clientData")
  :arguments NIL
  :return-type :integer #| 0 or 1 |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-directInfo
  :entry-point (ff::convert-to-lang "execHndState_connection")
  :arguments NIL
  :return-type :integer #| Pointer |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-msg
  :entry-point (ff::convert-to-lang "execHndState_msg")
  :arguments NIL
  :return-type :integer #| MSG_PTR |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-tmpParentRef
  :entry-point (ff::convert-to-lang "execHndState_tmpParentRef")
  :arguments NIL
  :return-type :integer #| ID |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'execHndState-tmpResponse
  :entry-point (ff::convert-to-lang "execHndState_tmpResponse")
  :arguments NIL
  :return-type :integer #| ID |#)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'endExecHandler
  :entry-point (ff::convert-to-lang "endExecHandler")
  :arguments '(integer #|tcaRef|# integer #|directInfo|# integer #|msg|#
	       integer #|tmpParentRef|# integer #|tmpResponse|#)
  :return-type :void)

#+LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_listenC
  :entry-point (ff::convert-to-lang "IPC_listen")
  :arguments '(integer #|timeoutMSecs|#)
  :return-type :integer)

#+LISPWORKS_FFI_HACK
(defun IPC_listen (timeout)
  (let ((retval (cond ((eql (execHndState-state) ExecHnd_Pending) IPC_OK)
		      (T (IPC_listenC timeout)))))
    (when (eql retval IPC_OK)
      ;; In Lispworks, have to dispatch handlers here.
      (let ((handler-data (ipc-get-handler-data 
			   (intern (execHndState-hndName)))))
	(set-execHndState-state ExecHnd_Handling)
	(unwind-protect
	    (cond ((null handler-data) 
		   (ipcSetError IPC_Argument_Out_Of_Range)
		   (setq retval IPC_Error))
		  (T (funcall (handler-data-proc handler-data)
			      (execHndState-tcaRef) (execHndState-data)
			      (unless (zerop (execHndState-clientData))
				(handler-data-client-data handler-data)))))
	  (endExecHandler (execHndState-tcaRef) (execHndState-directInfo)
			  (execHndState-msg) (execHndState-tmpParentRef)
			  (execHndState-tmpResponse)))))
    retval))

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_listenClear
  :entry-point (ff::convert-to-lang "IPC_listenClear")
  :arguments '(integer #|timeoutMSecs|#)
  :return-type :integer)

;;; This is not exactly right, since if it gets a message it waits *another*
;;;  timeout, but it should be good enough until LISPWORKS gets fixed.
#+LISPWORKS_FFI_HACK
(defun IPC_listenClear (timeout)
  (do ((retval (IPC_listen timeout) (IPC_listen timeout)))
      ((not (eql retval IPC_OK)) retval)))

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_dispatch
  :entry-point (ff::convert-to-lang "IPC_dispatch")
  :arguments '()
  :return-type :integer)

#+LISPWORKS_FFI_HACK
(defun IPC_dispatch ()
  (do ((retval (IPC_listen IPC_WAIT_FOREVER) (IPC_listen IPC_WAIT_FOREVER)))
      ((eql retval IPC_Error) retval)))

;; Added by TNgo, 5/29/97, since JSC/ARS&D needed this functionality
(ff:defforeign 'IPC_handleMessage
  :entry-point (ff::convert-to-lang "IPC_handleMessage")
  :arguments '(integer #|timeoutMSecs|#)
  :return-type :integer)

(ff:defforeign 'IPC_setCapacity
  :entry-point (ff::convert-to-lang "IPC_setCapacity")
  :arguments '(integer #|capacity|#)
  :return-type :integer)

(ff:defforeign 'IPC_dataLength
  :entry-point (ff::convert-to-lang "IPC_dataLength")
  :arguments '(integer #|msg-instance|#)
  :return-type :integer)

(ff:defforeign 'IPC_perror
  :entry-point (ff::convert-to-lang "IPC_perror")
  :arguments '(string #|msg|#)
  :return-type :void)

(ff:defforeign 'ipcSetError
  :entry-point (ff::convert-to-lang "ipcSetError")
  :arguments '(integer #|errno|#)
  :return-type :void)

(ff:defforeign 'IPC_setVerbosity
  :entry-point (ff::convert-to-lang "IPC_setVerbosity")
  :arguments '(integer #|verbosity|#)
  :return-type :integer)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                QUERY/RESPONSE FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Not exported -- discourage its use in LISP;
;; Use IPC_respondData, instead
#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_respond
  :entry-point (ff::convert-to-lang "IPC_respond")
  :arguments '(integer #|msg-instance|# string #|msg-name|# 
               integer #|length|# integer #|content|#)
  :return-type :integer)

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_queryNotify_C
  :entry-point (ff::convert-to-lang "IPC_queryNotify")
  :arguments '(string #|msg-name|# integer #|length|# integer #|content|#
               integer #|handler|# integer #|client-data|#)
  :return-type :integer)

;; Not exported -- discourage its use in LISP;
;; Use IPC_queryNotifyData, instead
#-LISPWORKS_FFI_HACK
(defun IPC_queryNotify (msg-name length content handler client-data)
  (let* ((hnd-name (ipc-handler-name msg-name handler))
         (function-name (intern hnd-name)))
    ;; Create a handler function that C can call (all arguments to 
    ;;  the function are pointers)
    (register-foreign-function function-name
			       '((msg-ref     :signed-long)
				 (call-data   :signed-long)
				 (client-data :signed-long))
			       `(,handler msg-ref call-data
					  (unless (zerop client-data)
					    (get-client-data ',function-name))))
    ;; Set the client data and get the procedure pointer
    ;;  (after the function has been registered)
    (let ((func (update-handler-data function-name client-data)))
      ;; Send the notification
      (IPC_queryNotify_C msg-name length content func (if client-data 1 0)))))

#-LISPWORKS_FFI_HACK
(ff:defforeign 'IPC_queryResponse_C
  :entry-point (ff::convert-to-lang "_IPC_queryResponse")
  :arguments '(string #|msg-name|# integer #|length|# integer #|content|#
               integer #|reply-handle|# integer #|reply-formatter|#
	       integer #|timeoutMSecs|#)
  :return-type :integer)

;; Not exported -- discourage its use in LISP;
;; Use IPC_queryResponseData, instead
#-LISPWORKS_FFI_HACK
(defmacro IPC_queryResponse (msg-name length content reply-handle timeoutMSecs)
  `(let ((retVal (IPC_queryResponse_C ,msg-name ,length ,content
				      *LISP-FLAG-GLOBAL* *LISP-FLAG-GLOBAL*
				      ,timeoutMSecs)))
     (setf ,reply-handle (if (eql retVal IPC_OK) *LISP-DECODE-VALUE* NIL))
     retVal))

#-LISPWORKS_FFI_HACK
(defun IPC_respondVC (msg-instance msg-name varcontent)
  (IPC_respond msg-instance msg-name
	       (IPC_VARCONTENT-length varcontent)
	       (IPC_VARCONTENT-content varcontent)))

#-LISPWORKS_FFI_HACK
(defun IPC_queryNotifyVC (msg-name varcontent handler client-data)
  (IPC_queryNotify msg-name (IPC_VARCONTENT-length varcontent) 
		   (IPC_VARCONTENT-content varcontent)
                   handler client-data))

#-LISPWORKS_FFI_HACK
(defmacro IPC_queryResponseVC (msg-name varcontent reply timeoutMSecs)
  `(IPC_queryResponse ,msg-name
		      (IPC_VARCONTENT-length ,varcontent) 
		      (IPC_VARCONTENT-content ,varcontent) 
		      ,reply ,timeoutMSecs))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                MARSHALLING/UNMARSHALLING FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(ff:defforeign 'IPC_createByteArray
  :entry-point (ff::convert-to-lang "IPC_createByteArray")
  :arguments '(integer #|length|#)
  :return-type :integer #|buffer|#)

(ff:defforeign 'IPC_freeByteArray
  :entry-point (ff::convert-to-lang "IPC_freeByteArray")
  :arguments '(integer #|buffer|#)
  :return-type :void)

(ff:defforeign 'IPC_parseFormat
  :entry-point (ff::convert-to-lang "IPC_parseFormat")
  :arguments '(string #|format-string|#)
  :return-type :integer #|formatter|#)

(ff:defforeign 'IPC_msgFormatter
  :entry-point (ff::convert-to-lang "IPC_msgFormatter")
  :arguments '(string #|msg-name|#)
  :return-type :integer #|formatter|#)

(ff:defforeign 'IPC_msgInstanceFormatter
  :entry-point (ff::convert-to-lang "IPC_msgInstanceFormatter")
  :arguments '(integer #|msg-instance|#)
  :return-type :integer #|formatter|#)

(ff:defforeign 'IPC_defineFormat
  :entry-point (ff::convert-to-lang "IPC_defineFormat")
  :arguments '(string #|format-name|# string #|format-string|#)
  :return-type :integer)

(ff:defforeign 'check-marshall-status
  :entry-point (ff::convert-to-lang "checkMarshallStatus")
  :arguments '(integer #|formatter|#)
  :return-type :integer)

(defun ipc-marshall (formatter lisp-data)
  (cond ((eql (check-marshall-status formatter) IPC_Error)
	 (values IPC_Error 0 0))
	(T (let ((length (BufferSize formatter lisp-data))
		 (byte-array 0) buffer)
	     (when (> length 0)
	       (setf byte-array (IPC_createByteArray length))
	       (setf buffer (ipcSetEncodeBuffer byte-array))
	       (EncodeData formatter lisp-data buffer)
	       (IPC_freeByteArray buffer))
	     (values IPC_OK length byte-array)))))

(defun IPC_marshall (formatter lisp-data varcontent)
  (cond ((null varcontent) (return-error IPC_Null_Argument))
	(T (multiple-value-bind (retval length byte-array)
	       (ipc-marshall formatter lisp-data)
	     (when (eql retval IPC_OK)
	       (setf (IPC_VARCONTENT-length varcontent) length)
	       (setf (IPC_VARCONTENT-content varcontent) byte-array))
	     retval))))

(defun ipc-unmarshall (formatter byte-array)
  (cond ((zerop formatter) NIL)
	(T (let ((buffer (ipcSetEncodeBuffer byte-array)))
	     (prog1 (DecodeData formatter buffer)
	       (IPC_freeByteArray buffer))))))

(defmacro IPC_unmarshall (formatter byte-array data)
  `(cond ((eql (check-marshall-status ,formatter) IPC_Error) IPC_Error)
	 (T (setf ,data (ipc-unmarshall ,formatter ,byte-array))
	    IPC_OK)))

(defun IPC_publishData (msg-name lisp-data)
  (multiple-value-bind (retval length byte-array)
      (ipc-marshall (IPC_msgFormatter msg-name) lisp-data)
    (cond ((eql retval IPC_OK)
	   (prog1 (IPC_publish msg-name length byte-array)
	     (IPC_freeByteArray byte-array)))
	  (T (IPC_perror "IPC_publishData")))))

#-LISPWORKS_FFI_HACK
(defun IPC_respondData (msg-instance msg-name lisp-data)
  (multiple-value-bind (retval length byte-array)
      (ipc-marshall (IPC_msgFormatter msg-name) lisp-data)
    (cond ((eql retval IPC_OK)
	   (prog1 (IPC_respond msg-instance msg-name length byte-array)
	     (IPC_freeByteArray byte-array)))
	  (T (IPC_perror "IPC_respondData")))))

#-LISPWORKS_FFI_HACK
(defun IPC_queryNotifyData (msg-name lisp-data handler client-data)
  (multiple-value-bind (retval length byte-array)
      (ipc-marshall (IPC_msgFormatter msg-name) lisp-data)
    (cond ((eql retval IPC_OK)
	   (prog1 (IPC_queryNotify msg-name length byte-array 
				   handler client-data)
	     (IPC_freeByteArray byte-array)))
	  (T (IPC_perror "IPC_queryNotifyData")))))

#-LISPWORKS_FFI_HACK
(defmacro IPC_queryResponseData (msg-name lisp-data reply timeoutMSecs)
  `(let (BYTE-ARRAY*)
     (multiple-value-bind (retVal length byte-array)
	 (ipc-marshall (IPC_msgFormatter ,msg-name) ,lisp-data)
       (cond ((eql retval IPC_OK)
	      (setf retVal (IPC_queryResponse ,msg-name length byte-array
					      BYTE-ARRAY* ,timeoutMSecs))
	      (IPC_freeByteArray byte-array)
	      (cond ((eql retVal IPC_OK)
		     (IPC_unmarshall *FORMATTER-VALUE* BYTE-ARRAY* ,reply)
		     (IPC_freeByteArray BYTE-ARRAY*))
		    (T (IPC_perror "IPC_queryResponseData")
		       (setf ,reply NIL))))
	     (T (IPC_perror "IPC_queryResponseData")))
       retVal)))

(defun IPC_printData (formatter stream lisp-data)
  (declare (ignore formatter stream lisp-data))
  (format T "IPC_printData: Not yet implemented~%"))

(defun IPC_readData (formatter stream variable)
  (declare (ignore formatter stream variable))
  (format T "IPC_printData: Not yet implemented~%"))

;; TNgo, 5/29/97, this is new in version 2.7.
(ff:defforeign 'IPC_checkMsgFormats
  :entry-point (ff::convert-to-lang "IPC_checkMsgFormats")
  :arguments '(string #|msg-name|# string #|format-string|#)
  :return-type :integer)

#-LISPWORKS_FFI_HACK
(ff:defforeign 'ipc-success
  :entry-point (ff::convert-to-lang "x_ipcSuccess")
  :arguments '(integer #|msg-instance|#)
  :return-type :integer)

;;; For Lispworks on VxWorks,
;;; the abort condition is handled within IPC_Listen
#+LISPWORKS_FFI_HACK
(defun ipc-success (msg-instance) msg-instance)

;;; If you abort out of a handler, will notify the IPC server to let it
;;; know that the handler has completed, so additional messages can be sent.
(defmacro IPC_defun_handler (name (msg-instance lisp-data client-data)
				  &rest body)
  (let ((flagvar (gensym "FLAG"))
	decls)
    (when (and (listp body) (listp (car body)) (eq (caar body) 'DECLARE))
      (setq decls (list (first body))
	    body (cdr body)))
    `(defun ,name (,msg-instance BYTE-ARRAY* ,(or client-data 'CLIENT-DATA*))
       ,@decls
       (let (,lisp-data ,flagvar)
	 (unwind-protect
	     (progn (IPC_unmarshall (IPC_msgInstanceFormatter ,msg-instance)
				    BYTE-ARRAY* ,lisp-data)
		    ,@body
		    (IPC_freeByteArray BYTE-ARRAY*)
		    (setf ,flagvar T))
	   (unless ,flagvar (ipc-success ,msg-instance)))))))
