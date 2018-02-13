;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: New Millennium, DS1
;;;          IPC (Interprocess Communication) Package
;;; 
;;; FILE: primFmttrs.lisp
;;;
;;; ABSTRACT: Primitive Data Formatters
;;;           Same as the TCA primFmttrs.lisp file, except in the IPC package.
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
;;;  $Log: primFmttrs.lisp,v $
;;;  Revision 2.2  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;  Revision 1.2.4.4  1997/01/25 22:44:02  udo
;;;  ipc_2_6 to r3_dev merge
;;;
;;;  Revision 1.2.4.3.6.1  1996/12/27 19:30:18  reids
;;;  Fixed the way Lisp is passed integer values of various sizes,
;;;  and the way it handles arrays of integers (or various sizes).
;;;
;;;  Revision 1.2.4.3  1996/10/29 04:42:45  reids
;;;  Fixed input argument type for formatPutInt.
;;;
;;;  Revision 1.2.4.2  1996/10/24 17:00:30  reids
;;;  Add additional primitive formats (byte, ubyte, short).
;;;
;;;  Revision 1.2.4.1  1996/10/08 14:27:56  reids
;;;  Changes to enable IPC to run under Lispworks on the PPC.
;;;  Main changes due to the fact that Lispworks currently does not support
;;;  "foreign-callable" and that it corrupts the stack when a double is sent
;;;  as the first argument to a foreign (C) function.
;;;
;;;  Revision 1.2  1996/03/12 03:11:47  reids
;;;  Support for the "enum" format in LISP, including automatic conversion
;;;    between integer (C) and keyword (LISP) forms.
;;;
;;;  Revision 1.1  1996/03/03 04:38:58  reids
;;;  First release of IPC files.  Corresponds to LISP version of
;;;  IPC Specifiction 2.2, except that IPC_printData and IPC_readData are
;;;  not yet implemented.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; DR 2/98 Revised all the windows *features*

;; NOTE: All WINDOWS options are added by TNgo, 5/19/97, for PC Allegro

(in-package :IPC)

#-MCL
(ff:defforeign 'blockCopyToArray
  :entry-point (ff::convert-to-lang "blockCopyToArray")
  :arguments   '(integer array integer)
  :return-type :void)

#+MCL
(defun blockCopyToArray (int1 array int2)
  (declare (ignore int1 array int2))
  (format T "Calling blockCopyToArray~%")
  (abort))

#-MCL
(ff:defforeign 'blockCopyFromArray
  :entry-point (ff::convert-to-lang "blockCopyFromArray")
  :arguments   '(integer array integer)
  :return-type :void)

#+MCL 
(defun blockCopyFromArray (int1 array int2)
  (declare (ignore int1 array int2))
  (format T "Calling blockCopyFromArray~%")
  (abort))

(ff:defforeign 'formatPrimitiveProc
  :entry-point (ff::convert-to-lang "formatPrimitiveProc")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'formatType
  :entry-point (ff::convert-to-lang "formatType")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'formatChoosePtrFormatC
  :entry-point (ff::convert-to-lang "formatChoosePtrFormat")
  :arguments   '(integer integer)
  :return-type :integer)

(defun formatChoosePtrFormat (format parentFormat)
  (if (null parentFormat) 
      (formatChoosePtrFormatC format 0) 
    (formatChoosePtrFormatC format parentFormat) ))

(ff:defforeign 'formatFormatArray
  :entry-point (ff::convert-to-lang "formatFormatArray")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'formatFormatArrayMax
  :entry-point (ff::convert-to-lang "formatFormatArrayMax")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'formatFormatArrayItem
  :entry-point (ff::convert-to-lang "formatFormatArrayItem")
  :arguments   '(integer integer)
  :return-type :integer)

(ff:defforeign 'formatGetByte
  :entry-point (ff::convert-to-lang "formatGetByte")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'formatGetUByte
  :entry-point (ff::convert-to-lang "formatGetUByte")
  :arguments   '(integer)
  :return-type :integer)

#-:ACLPC
(ff:defforeign 'formatGetShort
  :entry-point (ff::convert-to-lang "formatGetShort")
  :arguments   '(integer)
  :return-type :integer)

#+:ACLPC
(ct:defun-dll formatGetShort ((buffer :unsigned-long))
  :library-name FF::*IPC_LIB*
  :entry-name   "_formatGetShort"
  :return-type  :short)

(ff:defforeign 'formatGetInt
  :entry-point (ff::convert-to-lang "formatGetInt")
  :arguments   '(integer)
  :return-type :integer)

#-:ACLPC
(ff:defforeign 'formatGetChar
  :entry-point (ff::convert-to-lang "formatGetChar")
  :arguments   '(integer)
  :return-type :character)

#+:ACLPC
(ct:defun-dll formatGetCharC ((buffer :unsigned-long))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatGetChar"
  :return-type  :long)

#+:ACLPC
(defun formatGetChar (buffer)
  (let ((lisp-char (formatGetCharC buffer)))
    (character lisp-char)))

;; "single-float" arguments do not seem to work with
;; the gcc or acc compiler
#-:ACLPC
(ff:defforeign 'formatGetFloat
  :entry-point (ff::convert-to-lang "formatGetFloat")
  :arguments   '(integer)
  :return-type :double-float)

#+:ACLPC
(ct:defun-dll formatGetFloatC ((buffer  :unsigned-long)
			       (retval (:single-float 1)))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatGetFloat"
  :return-type  :void)

#+:ACLPC
(defun formatGetFloat (buffer)
  (let ( (floatVal (ct:ccallocate (:single-float 1))) )
    (formatGetFloatC buffer floatVal)
    (ct:cref (:single-float 1) floatVal 0)))

#-:ACLPC
(ff:defforeign 'formatGetDouble
  :entry-point (ff::convert-to-lang "formatGetDouble")
  :arguments   '(integer)
  :return-type :double-float)

#+:ACLPC
;; PC Allegro forbids the return of double-float. Bug???
(ct:defun-dll formatGetDoubleC ((buffer  :unsigned-long)
			        (retval (:double-float 1)))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatGetDouble"
  :return-type  :void)

#+:ACLPC
(defun formatGetDouble (buffer)
  (let ( (doubleVal (ct:ccallocate (:double-float 1))) )
    (formatGetDoubleC buffer doubleVal)
    (ct:cref (:double-float 1) doubleVal 0)))

#-:ACLPC
(ff:defforeign 'formatGetUShort
  :entry-point (ff::convert-to-lang "formatGetUShort")
  :arguments   '(integer)
  :return-type :integer)

#+:ACLPC
(ct:defun-dll formatGetUShort ((buffer :unsigned-long))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatGetUShort"
  :return-type  :short)

(ff:defforeign 'formatGetUInt
  :entry-point (ff::convert-to-lang "formatGetUInt")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'formatPutByte
  :entry-point (ff::convert-to-lang "formatPutByte")
  :arguments   '(integer integer)
  :return-type :void)

(ff:defforeign 'formatPutUByte
  :entry-point (ff::convert-to-lang "formatPutUByte")
  :arguments   '(integer integer)
  :return-type :void)

(ff:defforeign 'formatPutShort
  :entry-point (ff::convert-to-lang "formatPutShort")
  :arguments   '(integer integer)
  :return-type :void)

(ff:defforeign 'formatPutInt
  :entry-point (ff::convert-to-lang "formatPutInt")
  :arguments   '(integer integer)
  :return-type :void)

#-:ACLPC
(ff:defforeign 'formatPutChar
  :entry-point (ff::convert-to-lang "formatPutChar")
  :arguments   '(integer character)
  :return-type :void)

#+:ACLPC
(ct:defun-dll formatPutCharC ((buffer :unsigned-long)
			      (achar  :long))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatPutChar"
  :return-type  :void)

#+:ACLPC
(defun formatPutChar (buffer lisp-char)
  (if (characterp lisp-char)
      (formatPutCharC buffer (char-int lisp-char))
      (formatPutCharC buffer lisp-char)))

#-:ACLPC
;; "single-float" arguments do not seem to work with
;; the gcc or acc compiler
(ff:defforeign 'formatPutFloat
  :entry-point (ff::convert-to-lang "formatPutFloat")
  :arguments   '(integer double-float)
  :return-type :void)

#+:ACLPC
(ct:defun-dll formatPutFloat ((buffer :unsigned-long)
                              (afloat :single-float))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatPutFloat"
  :return-type  :void)

(ff:defforeign 'formatPutDouble
  :entry-point (ff::convert-to-lang "formatPutDouble")
  :arguments   '(integer double-float)
  :return-type :void)

#-:ACLPC
(ff:defforeign 'formatPutUShort
  :entry-point (ff::convert-to-lang "formatPutUShort")
  :arguments   '(integer integer)
  :return-type :void)

#+:ACLPC
(ct:defun-dll formatPutUShort ((buffer  :unsigned-long)
                               (aushrt  :short))
  :library-name FF::*IPC_LIB*
  :entry-name   "formatPutUShort"
  :return-type  :void)

(ff:defforeign 'formatPutUInt
  :entry-point (ff::convert-to-lang "formatPutUInt")
  :arguments   '(integer integer)
  :return-type :void)

(ff:defforeign 'msgByteOrder
  :entry-point (ff::convert-to-lang "msgByteOrder")
  :arguments   '()
  :return-type :integer)

(ff:defforeign 'hostByteOrder
  :entry-point (ff::convert-to-lang "hostByteOrder")
  :arguments   '()
  :return-type :integer)

;;; must follow c - enum
(defconstant Encode		0)
(defconstant Decode		1)
(defconstant ELength		2)
(defconstant ALength		3)
(defconstant RLength		4)
(defconstant SimpleType		5)
(defconstant DPrint		6)
(defconstant DFree		7)
(defconstant SimpleTypeSize	8)

;;;*********************************

;; SimpleTypeSize is the amount of storage to byte copy in C per element
;; if a byte copy of some array is possible, otherwise if byte copy is
;; not possible for this element type NIL is returned.

;;;*********************************

;;; IMPORTANT!! Needs to match C translation functions
(defconstant MAXFORMATTERS	31)

(defconstant INT_FMT		1)
(defconstant BOOLEAN_FMT	2)
(defconstant FLOAT_FMT		3)
(defconstant DOUBLE_FMT		4)
(defconstant BYTE_FMT		5)
;;(defconstant TWOBYTE_FMT	6)
(defconstant STR_FMT		7)
;;(defconstant FORMAT_FMT	8)
(defconstant UBYTE_FMT		9)
;;(defconstant CMAT_FMT		10)
;;(defconstant SMAT_FMT		11)
;;(defconstant IMAT_FMT		12)
;;(defconstant LMAT_FMT		13)
;;(defconstant FMAT_FMT		14)
;;(defconstant DMAT_FMT		15)
(defconstant CHAR_FMT		16)
(defconstant SHORT_FMT		17)
(defconstant LONG_FMT		18)
;;(defconstant UCMAT_FMT	19)

(defconstant USHORT_FMT		28)
(defconstant UINT_FMT		29)
(defconstant ULONG_FMT		30)

;;; IMPORTANT!! Must follow C-Enum
(defconstant PrimitiveFMT	0)
(defconstant LengthFMT		1)
(defconstant StructFMT		2)
(defconstant PointerFMT		3)
(defconstant FixedArrayFMT	4)
(defconstant VarArrayFMT	5)
(defconstant BadFormatFMT	6)
(defconstant NamedFMT		7)
(defconstant EnumFMT		8)

(defconstant CHAR_SIZE		1)
(defconstant BYTE_SIZE		1)
(defconstant SHORT_SIZE		2)
(defconstant INT_SIZE		4)
(defconstant FLOAT_SIZE		4) 
(defconstant DOUBLE_SIZE	8)
(defconstant PTR_SIZE		4)
(defconstant DATAPTR_SIZE	1)

(defconstant BYTE_TYPE	`(signed-byte ,(* 8 BYTE_SIZE)))
(defconstant SHORT_TYPE	`(signed-byte ,(* 8 SHORT_SIZE)))
(defconstant INT_TYPE	`(signed-byte ,(* 8 INT_SIZE)))
;;(defconstant INT_TYPE	`integer)
(defconstant UBYTE_TYPE	`(unsigned-byte ,(* 8 BYTE_SIZE)))
(defconstant USHORT_TYPE	`(unsigned-byte ,(* 8 SHORT_SIZE)))
(defconstant UINT_TYPE	`(unsigned-byte ,(* 8 INT_SIZE)))

(defvar TransTable (make-array (1+ MAXFORMATTERS) :INITIAL-ELEMENT NIL))

;; DR 2/98 added the eval-when to force def in compilation envmnt
(eval-when (compile load eval)
(defmacro doStructFormat ((index formatArray) &body body)
  `(do ((,index 1 (1+ ,index))
	(formatArraySize (formatFormatArrayMax ,formatArray)))
       ((>= ,index formatArraySize))
     ,@body))
)

;; DR 2/98 added the eval-when to force def in compilation envmnt
(eval-when (compile load eval)
(defmacro doArrayFormat ((index formatArray) &body body)
  `(do ((,index 2 (1+ ,index))
	(formatArraySize (formatFormatArrayMax ,formatArray)))
       ((>= ,index formatArraySize))
     ,@body))
)

;;;
;;; STR_Trans (Op DataStruct DStart Buffer BStart)
;;;
;;; "String" is an array of characters.
;;; Decodes to: length of string, Character-List
;;;

(defun STR_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode)
         (let* ((length (formatGetInt buffer))
		(string (make-string length)))
	   (cond ((zerop length) (formatGetChar buffer))
		 (T (dotimes (i length) 
		      (setf (aref string i) (formatGetChar buffer)))
		    (setf (aref dataStruct dStart) string)))))
	
	((eql op Encode)
	 (let ((length (length (aref dataStruct dStart)))
	       (string (aref dataStruct dStart))
	       (nullString #\Z))
	   (formatPutInt buffer length)
	   (cond ((zerop length) (formatPutChar buffer nullString))
		 (T (dotimes (i length)
		      (formatPutChar buffer (aref string i)))))))

        ((eql op ELength)
	 (let ((str_length (length (aref dataStruct dStart))))
	   ( + (if (zerop str_length) CHAR_SIZE str_length) INT_SIZE)))

        ((eql op ALength) 1)

        ((eql op SimpleType) NIL)
	((eql op SimpleTypeSize) NIL)

        (T (error "STR_Trans: undefined op: ~d" Op))))

;;;
;;; BYTE_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun BYTE_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 (setf (aref dataStruct dStart) (formatGetByte buffer)))
	((eql op Encode)
	 (formatPutByte buffer (aref dataStruct dStart)))
	((eql op Elength) BYTE_SIZE)
	((eql op Alength) 1)

	((eql op SimpleType) BYTE_TYPE)
	((eql op SimpleTypeSize) BYTE_SIZE)
	
	(T (error "BYTE_Trans ERROR ~%"))))

;;;
;;; UBYTE_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun UBYTE_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 (setf (aref dataStruct dStart) (formatGetUByte buffer)))
	((eql op Encode)
	 (formatPutUByte buffer (aref dataStruct dStart)))
	((eql op Elength) BYTE_SIZE)
	((eql op Alength) 1)

	((eql op SimpleType) UBYTE_TYPE)
	((eql op SimpleTypeSize) BYTE_SIZE)
	
	(T (error "BYTE_Trans ERROR ~%"))))

;;;
;;; SHORT_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun SHORT_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 (setf (aref dataStruct dStart) (formatGetShort buffer)))
	((eql op Encode)
	 (formatPutShort buffer (aref dataStruct dStart)))
	((eql op Elength) SHORT_SIZE)
	((eql op Alength) 1)

	((eql op SimpleType) SHORT_TYPE)
	((eql op SimpleTypeSize) SHORT_SIZE)
	
	(T (error "SHORT_Trans ERROR ~%"))))

;;;
;;; INT_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun INT_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 (setf (aref dataStruct dStart) (formatGetInt buffer)))
	((eql op Encode)
	 (formatPutInt buffer (aref dataStruct dStart)))
	((eql op Elength) INT_SIZE)
	((eql op Alength) 1)

	((eql op SimpleType) INT_TYPE)
	((eql op SimpleTypeSize) INT_SIZE)
	
	(T (error "INT_Trans ERROR ~%"))))

;;;
;;; CHAR_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun CHAR_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 (setf (aref dataStruct dStart) (formatGetChar buffer)))
	((eql op Encode)
	 (formatPutChar buffer (aref dataStruct dStart)))
	((eql op Elength) CHAR_SIZE)
	((eql op Alength) 1)

        ((eql op SimpleType) 'character)
	((eql op SimpleTypeSize) CHAR_SIZE)
	
	(T (error "CHAR_Trans ERROR ~%"))))

;;;
;;; FLOAT_Trans (Op DataStruct DStart Buffer BStart)
;;;
(defun FLOAT_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode)
	 ;; "single-float" arguments do not seem to work with
	 ;; the gcc or acc compiler
	 (setf (aref dataStruct dStart)
	       (coerce (formatGetFloat buffer) 'SINGLE-FLOAT)))
	((eql op Encode)
	 ;; "single-float" arguments do not seem to work
	 ;; with the gcc or acc compiler
	 (formatPutFloat buffer
			 (coerce (aref dataStruct dStart) 'DOUBLE-FLOAT)))
	((eql op ELength) FLOAT_SIZE)
	((eql op Alength) 1)

        ((eql op SimpleType) 'single-float)
	((eql op SimpleTypeSize) FLOAT_SIZE)
	
	(T (error "FLOAT_Trans ERROR ~%"))))

;;;
;;; DOUBLE_Trans (Op DataStruct DStart Buffer BStart)
;;;
(defun DOUBLE_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode)
	 (setf (aref dataStruct dStart) (formatGetDouble buffer)))
	((eql op Encode)
	 (formatPutDouble buffer (aref dataStruct dStart)))
	((eql op ELength) DOUBLE_SIZE)
	((eql op Alength) 1)

        ((eql op SimpleType) 'double-float)
	((eql op SimpleTypeSize) DOUBLE_SIZE)
	
	(T (error "DOUBLE_Trans ERROR ~%"))))

;;;
;;; BOOLEAN_Trans (Op DataStruct DStart Buffer BStart)
;;;
(defun BOOLEAN_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode)
	 (let ((item (formatGetInt buffer)))
	   (cond ((eql item 0) (setf (aref dataStruct dStart) NIL))
		 ((eql item 1) (setf (aref dataStruct dStart) T))
		 (T (error "BOOLEAN_Trans: not TRUE or FALSE")))))
	((eql op Encode)
	 (cond ((NULL (aref dataStruct dStart))	(formatPutInt buffer 0))
	       (T (formatPutInt buffer 1))))
	((eql op ELength) INT_SIZE)
	((eql op Alength) 1)

        ((eql op SimpleType) NIL)
	((eql op SimpleTypeSize) NIL)
	
	(T (error "BOOLEAN_Trans ERROR ~%"))))

;;;
;;; USHORT_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun USHORT_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 (setf (aref dataStruct dStart) (formatGetUShort buffer)))
	((eql op Encode)
	 (formatPutUShort buffer (aref dataStruct dStart)))
	((eql op Elength) SHORT_SIZE)
	((eql op Alength) 1)

	((eql op SimpleType) USHORT_TYPE)
	((eql op SimpleTypeSize) SHORT_SIZE)

	(T (error "USHORT_Trans ERROR ~%"))))

;;;
;;; UINT_Trans (Op DataStruct DStart Buffer BStart)
;;;

(defun UINT_Trans (op dataStruct dStart buffer)
  (cond ((eql op Decode) 
	 ;; Allegro does not do the right thing wrt
	 ;; large unsigned integers
	 (let ((intval (formatGetUInt buffer)))
	   (setf (aref dataStruct dStart)
		 (if (>= intval 0) intval (+ #x100000000 intval)))))
	((eql op Encode)
	 (formatPutUInt buffer (aref dataStruct dStart)))
	((eql op Elength) INT_SIZE)
	((eql op Alength) 1)

	((eql op SimpleType) UINT_TYPE)
	((eql op SimpleTypeSize) INT_SIZE)

	(T (error "INT_Trans ERROR ~%"))))
