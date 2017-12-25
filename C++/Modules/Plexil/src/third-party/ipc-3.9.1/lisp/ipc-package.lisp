;;;; -*- Mode: LISP; Syntax: ANSI-COMMON-LISP; Package: CL-USER -*-

;;; Copyright (c) 2008, Carnegie Mellon University
;;;     This software is distributed under the terms of the 
;;;    Simplified BSD License (see ipc/LICENSE.TXT)

;; DR 2/98 Added package def as separate file to aid in cleaning up
;; the IPC loading process.

#+(AND LISPWORKS VXWORKS)
(eval-when (eval compile load) (pushnew :LISPWORKS_FFI_HACK *features*))

(in-package #-:MCL :user #+:MCL :cl-user)

(eval-when (eval compile load)
(defpackage :IPC
  (:use :common-lisp-user :common-lisp)
  (:export
   ;; Return Values
   IPC_Error
   IPC_OK
   IPC_Timeout

   ;; Error Values
   IPC_No_Error
   IPC_Not_Connected
   IPC_Message_Not_Defined
   IPC_Not_Fixed_Length
   IPC_Message_Lengths_Differ
   IPC_Argument_Out_Of_Range
   IPC_Null_Argument
   IPC_Illegal_Formatter
   IPC_Wrong_Buffer_Length
   IPC_Communication_Error

   ;; Verbosity Levels
   IPC_Silent
   IPC_Print_Warnings
   IPC_Print_Errors
   IPC_Exit_On_Errors

   ;; Constants
   IPC_WAIT_FOREVER
   IPC_VARIABLE_LENGTH
   IPC_FIXED_LENGTH

   ;; Type declarations
   IPC_VARCONTENT
   make-IPC_VARCONTENT
   IPC_VARCONTENT-length
   IPC_VARCONTENT-content

   ;; External variables
   IPC_errno

   ;; Basic IPC Interface (not all IPC functions are exported, 
   ;; just useful ones)
   IPC_initialize		#| Args   : NIL
				   Returns: IPC_RETURN_TYPE |#
   IPC_connect			#| Args   : task-name;
				   Returns: IPC_RETURN_TYPE |#
   IPC_connectModule		#| Added by TNgo, 5/19/97
				   Args   : task-name, server-name;
				   Returns: IPC_RETURN_TYPE |#
   IPC_disconnect		#| Args   : NIL;
				   Returns: IPC_RETURN_TYPE |#
   IPC_isConnected		#| Args   : NIL;
				   Returns: T/NIL |#
   IPC_defineMsg		#| Args   : msg-name, length, format-string;
				   Returns: IPC_RETURN_TYPE |#
   IPC_isMsgDefined		#| Args   : msg-name;
				   Returns: T/NIL |#
   IPC_msgInstanceName		#| Args   : msg-instance;
				   Returns: string |#
   IPC_subscribe		#| Args   : msg-name, handler-symbol, client-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_subscribeData		#| Args   : msg-name, handler-symbol, client-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_unsubscribe		#| Args   : msg-name, handler-symbol;
				   Returns: IPC_RETURN_TYPE |#
   IPC_subscribeFD		#| Args   : fd, handler-symbol, client-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_unsubscribeFD		#| Args   : fd, handler-symbol;
				   Returns: IPC_RETURN_TYPE |#
   IPC_listen			#| Args   : timeoutMSecs;
				   Returns: IPC_RETURN_TYPE |#
   IPC_listenClear		#| Args   : timeoutMSecs;
				   Returns: IPC_RETURN_TYPE |#
   IPC_dispatch			#| Args   : NIL;
				   Returns: IPC_RETURN_TYPE |#
   IPC_handleMessage		#| Added by TNgo, 5/29/97
				   Args   : timeoutMSecs;
				   Returns: IPC_RETURN_TYPE |#
   IPC_setCapacity		#| Args   : capacity;
				   Returns: IPC_RETURN_TYPE |#
   IPC_dataLength		#| Args   : msg-instance;
				   Returns: data length (integer) |#
   IPC_perror			#| Args   : msg;
				   Returns: none |#
   IPC_setVerbosity		#| Args   : verbosity;
				   Returns: IPC_RETURN_TYPE |#

   ;; Query/response functions (not all IPC functions are exported, 
   ;; just useful ones)
   IPC_respondVC		#| Args   : msg-instance, msg-name, varcontent;
				   Returns: IPC_RETURN_TYPE |#
   IPC_queryNotifyVC		#| Args   : msg-name, varcontent, handler, client-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_queryResponseVC		#| Args   : msg-name, varcontent, reply, timeoutMSecs;
				   Returns: IPC_RETURN_TYPE |#

   ;; Marshalling functions
   IPC_parseFormat		#| Args   : format-string;
				   Returns: FORMATTER_PTR |#
   IPC_msgFormatter		#| Args   : msg-name;
				   Returns: FORMATTER_PTR |#
   IPC_msgInstanceFormatter	#| Args   : msg-instance;
					   Returns: IPC_RETURN_TYPE |#
   IPC_defineFormat		#| Args   : format-name, format-string;
				   Returns: IPC_RETURN_TYPE |#
   IPC_marshall			#| Args   : formatter, lisp-data, varcontent;
				   Returns: IPC_RETURN_TYPE |#
   IPC_unmarshall		#| Args   : formatter, byte-array, data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_publishData		#| Args   : msg-name, lisp-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_respondData		#| Args   : msg-instance, msg-name, lisp-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_queryNotifyData		#| Args   : msg-name, lisp-data, handler, client-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_queryResponseData	#| Args   : msg-name, lisp-data, reply, timeoutMSecs;
				   Returns: IPC_RETURN_TYPE |#
   IPC_printData		#| Args   : formatter, stream, lisp-data;
				   Returns: IPC_RETURN_TYPE |#
   IPC_readData			#| Args   : formatter, stream, variable;
				   Returns: IPC_RETURN_TYPE |#
   IPC_checkMsgFormats		#| Args   : msg-name, format-string;
				   Returns: IPC_RETURN_TYPE |#
   IPC_freeByteArray		#| Args   : C-byte-array;
				   Returns: void |#

   ;; All structures that IPC needs to marshall/unmarshall must be declared
   ;; using IPC_defstruct
   IPC_defstruct

   ;; Macro to define a handler that automatically unmarshalls the data
   IPC_defun_handler
   ))
)
