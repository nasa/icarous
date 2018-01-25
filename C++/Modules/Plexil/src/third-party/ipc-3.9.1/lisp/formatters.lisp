;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; PROJECT: New Millennium, DS1
;;;          IPC (Interprocess Communication) Package
;;;
;;; FILE: formatters.lisp
;;;
;;; ABSTRACT: Data Format Routines
;;;           Same as the TCA formatters.lisp file, except in the IPC package.
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
;;;  $Log: formatters.lisp,v $
;;;  Revision 2.3  2009/01/12 15:54:55  reids
;;;  Added BSD Open Source license info
;;;
;;;  Revision 2.2  2003/02/13 20:39:05  reids
;;;  Updated for ACL6.0
;;;
;;;  Revision 2.1.1.1  1999/11/23 19:07:37  reids
;;;  Putting IPC Version 2.9.0 under local (CMU) CVS control.
;;;
;;;  Revision 1.3.4.5  1997/01/25 22:43:57  udo
;;;  ipc_2_6 to r3_dev merge
;;;
;;;  Revision 1.3.4.4.6.3  1997/01/23 16:45:56  reids
;;;  For Lisp, force all format enum values to uppercase; Yields more efficient
;;;    C <=> Lisp conversion of enumerated types.
;;;
;;;  Revision 1.3.4.4.6.2  1996/12/27 19:30:16  reids
;;;  Fixed the way Lisp is passed integer values of various sizes,
;;;  and the way it handles arrays of integers (or various sizes).
;;;
;;;  Revision 1.3.4.4.6.1  1996/12/24 14:27:55  reids
;;;  More efficient way of translating from keyword => enum value (using plist)
;;;
;;;  Revision 1.3.4.4  1996/10/24 16:56:12  reids
;;;  Check that message byte order is the same before transfering all array
;;;     elements at once.
;;;
;;;  Revision 1.3.4.3  1996/10/24 16:13:04  reids
;;;  Better error reporting
;;;
;;;  Revision 1.3.4.2  1996/10/08 14:27:48  reids
;;;  Changes to enable IPC to run under Lispworks on the PPC.
;;;  Main changes due to the fact that Lispworks currently does not support
;;;  "foreign-callable" and that it corrupts the stack when a double is sent
;;;  as the first argument to a foreign (C) function.
;;;
;;;  Revision 1.3.4.1  1996/10/02 20:48:39  reids
;;;  Changes to support LISPWORKS.
;;;
;;;  Revision 1.3  1996/03/12 03:11:46  reids
;;;  Support for the "enum" format in LISP, including automatic conversion
;;;    between integer (C) and keyword (LISP) forms.
;;;
;;;  Revision 1.2  1996/03/04 21:49:57  reids
;;;  Fixed a bug if a formatter is NIL.
;;;
;;;  Revision 1.1  1996/03/03 04:38:53  reids
;;;  First release of IPC files.  Corresponds to LISP version of
;;;  IPC Specifiction 2.2, except that IPC_printData and IPC_readData are
;;;  not yet implemented.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(in-package :IPC)

;; DR 2/98 Revised all the windows *features*

;; Note: All WINDOWS options are added by TNgo, 5/19/97, for PC Allegro

(defconstant STRUCT_ELEM_SIZE	1)
(defconstant ARRAY_ELEM_SIZE	1)
(defconstant LISP_STRUCT_START	1)
(defconstant LISP_ARRAY_START	0)
#+:FRANZ-INC (defvar *LISP-C-GLOBAL-STRING-BUF*)

(defun findSimpleType (format)
  #+MCL (declare (ignore format)) #+MCL NIL
  #-MCL
  (when (eql (formatType format) PrimitiveFMT)
     (funcall (aref TransTable (formatPrimitiveProc format))
	      SimpleType NIL 0 NIL)))

(defun findSimpleTypeSize (format)
  (when (eql (formatType format) PrimitiveFMT)
     (funcall (aref TransTable (formatPrimitiveProc format))
	      SimpleTypeSize NIL 0 NIL)))

(defun Explicit-Format-Error ()
  (error "LISP Version of IPC can only use explicit formats"))

(ff:defforeign 'findNamedFormat
  :entry-point (ff::convert-to-lang "findNamedFormat")
  :arguments   '(integer)
  :return-type :integer)

(ff:defforeign 'enumStringIndex
  :entry-point (ff::convert-to-lang "enumStringIndex")
  :arguments   '(integer #|format|# string)
  :return-type :integer)

;;; Return the integer value associated with the enum symbol
;;; (NIL if not found)
(defun enumSymbolIndex (format enumSymbol)
  (or (get enumSymbol :IPC_ENUM_INDEX)
      (let* ((enumVal (enumStringIndex format (symbol-name enumSymbol))))
	(when (>= enumVal 0) 
	  (setf (get enumSymbol :IPC_ENUM_INDEX) enumVal)
	  enumVal))))

#+(AND ALLEGRO (NOT ALLEGRO-V6.0))
(ff:defforeign 'enumValNameLength
  :entry-point (ff::convert-to-lang "enumValNameLength")
  :arguments   '(integer #|format|# integer #|value|#)
  :return-type :integer #|length|#) 

#+(AND ALLEGRO (NOT ALLEGRO-V6.0))
(ff:defforeign 'enumValNameString
  :entry-point (ff::convert-to-lang "enumValNameString")
  :arguments   '(integer #|format|# integer #|value|#)
  :return-type :lisp #|string|#) 

;;; Get the symbol associated with the enum integer value
;;; (or NIL if outside range, or enum does not have values specified)
;;; Change as of 1/23/97 forces all Lisp enums to uppercase
#+(AND ALLEGRO (NOT ALLEGRO-V6.0))
(defun getNthEnum (format enumVal)
  (let ((length (enumValNameLength format enumVal)))
    (cond ((zerop length) NIL)
	  (T (setq *LISP-C-GLOBAL-STRING-BUF* (make-string length))
	     (multiple-value-bind (index value)
		 (ff:register-value '*LISP-C-GLOBAL-STRING-BUF* 0)
	       (declare (ignore index) (ignore value))
	       (intern (enumValNameString format enumVal) :KEYWORD))))))

#+ (or LISPWORKS MCL ALLEGRO-V6.0)
(ff:defforeign 'enumValString
  :entry-point (ff::convert-to-lang "enumValString")
  :arguments   '(integer #|format|# integer #|value|#)
  :return-type :string #|name|#)

;;; Get the symbol associated with the enum integer value
;;;  (or NIL if outside range, or enum does not have values specified)
#+ (or LISPWORKS MCL ALLEGRO-V6.0)
(defun getNthEnum (format enumVal)
  (let ((name (enumValString format enumVal)))
    (cond ((zerop (length name)) NIL)
	  (T (intern name :KEYWORD)))))

;;;
;;; First element in a FormatArray is the size of the array.
;;; For a FixedArrayFMT (and VarArrayFMT) the second element 
;;; is the formatter the rest of the elements are the dimensions.
;;;
(defun fixedArrayDimensions (formatArray)
  (let (arrayDimensions)
    (doArrayFormat (i formatArray)
       (setq arrayDimensions (cons (formatFormatArrayItem formatArray i) 
				   arrayDimensions)))
    (nreverse arrayDimensions)))

(defun varArrayDimensions (formatArray parentFormat dataStruct dStart)
  (let ((parentStructArray (formatFormatArray parentFormat))
        currentPlace arrayDimensions)
    (do ((place 1 (1+ place))
         (foundPlace NIL (and (eql VarArrayFMT
				   (formatType 
				    (formatFormatArrayItem parentStructArray
							   place)))
                               (eql formatArray
                                    (formatFormatArray
				    (formatFormatArrayItem parentStructArray
							   place))))))
        (foundPlace (setq currentPlace (1- place))))
  
    (doArrayFormat (i formatArray)
       (let ((sizePlace (formatFormatArrayItem formatArray i))
	     (offset 0))
	 (cond ((< currentPlace sizePlace)
		(do ((j currentPlace (1+ j)))
		    ((>= j sizePlace))
		  (incf offset (DataStructureSize
				(formatFormatArrayItem parentStructArray j)))))
	       (T (do ((j sizePlace (1+ j)))
		      ((>= j currentPlace))
		   (decf offset (DataStructureSize 
			(formatFormatArrayItem parentStructArray j))))))
	 (push (aref dataStruct (+ dStart offset)) arrayDimensions)))
      (nreverse arrayDimensions)))

;;; 
;;; TRANSFERRING DATA TO AND FROM ARRAYS TAKES ADVANTAGE OF THE FACT
;;; THAT BOTH LISP AND C STORE ARRAYS IN THE SAME WAY (ROW-MAJOR ORDER).
;;;
;;; Modified by TNgo, 10/16/97, to work with PC Allegro
;;;
(defun displaced_vector (Array)
  (make-array (array-total-size Array)
	      :DISPLACED-TO Array
	      :ELEMENT-TYPE (array-element-type Array)))

(defmacro check-dimensions (expected-dimensions Array)
  `(unless (and (<= (car ,expected-dimensions) (car (array-dimensions ,Array)))
		(equal (cdr ,expected-dimensions)
		       (cdr (array-dimensions ,Array))))
      (error "Mismatch between expected and actual dimensions ~A" ,Array)))

;;;
;;; "ParentFormat" is needed by SelfPtr ("*!") and VarArray ("<..>") 
;;; formatters.
;;; Both these formatters can only be embedded in a Struct format ("{...}".
;;;
(defun BufferSize1 (format dataStruct dStart parentFormat)
  (let ((bufSize 0) (currentData dStart) (type (formatType format)))
    (cond ((eql type LengthFMT) 
	   (Explicit-Format-Error))

          ((eql type PrimitiveFMT)
           (let ((formatProc (aref TransTable (formatPrimitiveProc format))))
	     (if (not (fboundp formatProc))
                 (break "~% Data formatter not implemented for primitive ~A" 
			(formatPrimitiveProc format)))
	     (incf bufSize
		   (funcall formatProc ELength dataStruct currentData NIL))
             (incf currentData
		   (funcall formatProc ALength NIL 0 NIL))))

          ((eql type PointerFMT)
	   (incf bufSize CHAR_SIZE)
	   (when (aref dataStruct currentData)
	     (incf bufSize 
		   (BufferSize1 (formatChoosePtrFormat format parentFormat)
				dataStruct currentData NIL)))
	   (incf currentData STRUCT_ELEM_SIZE))

          ((eql type StructFMT)
           (let ((formatArray (formatFormatArray format))
		 (structPtr (aref dataStruct currentData))
                 (structStart LISP_STRUCT_START))
	     (doStructFormat (i formatArray)
	       (multiple-value-bind (bsize dsize)
		   (BufferSize1 (formatFormatArrayItem formatArray i)
				structPtr structStart format)
		 (incf bufSize bsize)
		 (incf structStart dsize)))
	     (incf currentData STRUCT_ELEM_SIZE)))

          ((eql type FixedArrayFMT)
           (let* ((formatArray (formatFormatArray format))
                  (nextFormat (formatFormatArrayItem formatArray 1))
		  (arrayDimensions (fixedArrayDimensions formatArray))
		  (elementSize (findSimpleTypeSize nextFormat))
		  (array (aref dataStruct currentData)))
	     (cond (elementSize
		    (incf bufSize (* (BufferSize1 nextFormat array 0 NIL)
				     (apply #'* arrayDimensions))))
		   (T (let ((dispVector (displaced_vector array)))
			(check-dimensions arrayDimensions array)
			(dotimes (i (array-total-size dispVector))
			  (incf bufSize 
				(BufferSize1 nextFormat dispVector i NIL))))))
	     (incf currentData STRUCT_ELEM_SIZE)))

          ((eql type VarArrayFMT)
           (let* ((formatArray (formatFormatArray format))
                  (nextFormat (formatFormatArrayItem formatArray 1))
		  (arrayDimensions 
		   (varArrayDimensions formatArray parentFormat
				       dataStruct currentData))
		  (arraySize (apply #'* arrayDimensions))
		  (elementSize (findSimpleTypeSize nextFormat))
		  (array (aref dataStruct currentData)))
             (incf bufSize INT_SIZE)	;for the size of the array

	     (cond ((zerop arraySize))
		   (elementSize (incf bufSize (* (BufferSize1 nextFormat
							      array 0 NIL)
						 arraySize)))
		   (T (let* ((array (aref dataStruct currentData))
			     (dispVector (displaced_vector array)))
			(check-dimensions arrayDimensions array)
			(dotimes (i arraySize)
			  (incf bufSize (BufferSize1 nextFormat
						     dispVector i NIL))))))
             (incf currentData STRUCT_ELEM_SIZE)))

	  ((eql type NamedFMT)
	   (incf bufSize (BufferSize1 (findNamedFormat format) 
				      dataStruct dStart parentFormat))
	   (incf currentData STRUCT_ELEM_SIZE))

	  ((eql type EnumFMT)
	   (incf bufSize INT_SIZE)
	   (incf currentData STRUCT_ELEM_SIZE)))

    (values bufSize (- currentData dStart))))

(defun TransferToBuffer (format dataStruct dStart buffer parentFormat)
  (let ((currentData dStart) (type (formatType format)))
    (cond ((eql type LengthFMT) 
	   (Explicit-Format-Error))

          ((eql type PrimitiveFMT)
           (let ((formatProc (aref TransTable (formatPrimitiveProc format))))
	     (funcall formatProc Encode dataStruct currentData buffer)
	     (incf currentData (funcall formatProc ALength NIL
					LISP_STRUCT_START NIL))))

          ((eql type PointerFMT) 
	   (let ((ptrVal (if (aref dataStruct currentData)
			     ;; Z means data, 0 means NIL
			     (character #\Z)
			   (character 0))))
	     (formatPutChar buffer ptrVal)
	     (if (eql ptrVal (character #\Z))
		 (let ((nextFormat (formatChoosePtrFormat format
							  parentFormat)))
		   (TransferToBuffer nextFormat dataStruct
				     currentData buffer NIL)))
	     (incf currentData STRUCT_ELEM_SIZE)))

          ((eql type StructFMT)
           (let ((formatArray (formatFormatArray format))
		 (structPtr   (aref dataStruct currentData))
		 (structStart LISP_STRUCT_START))
             (doStructFormat (i formatArray)
               (incf structStart (TransferToBuffer
				  (formatFormatArrayItem formatArray i)
				  structPtr structStart buffer format)))
             (incf currentData STRUCT_ELEM_SIZE)))

          ((eql type FixedArrayFMT)
           (let* ((formatArray (formatFormatArray format))
                  (nextFormat (formatFormatArrayItem formatArray 1))
		  (arrayDimensions (fixedArrayDimensions formatArray))
		  (arraySize (apply #'* arrayDimensions))
		  (array (aref dataStruct currentData))
		  (elementSize (findSimpleTypeSize nextFormat))
		  (dispVector (displaced_vector array)))
	     (check-dimensions arrayDimensions array)
	     (cond ((and elementSize
			#+:ACLPC(not (eq (array-element-type dispVector)
						'character))
			(equal (array-element-type dispVector)
			       (findSimpleType nextFormat)))
		    (blockCopyFromArray buffer dispVector 
					(* elementSize arraySize)))
		   (T
		    (dotimes (i arraySize)
		      (TransferToBuffer nextFormat dispVector i buffer NIL))))
	     (incf currentData STRUCT_ELEM_SIZE)))

          ((eql type VarArrayFMT)
           (let* ((formatArray     (formatFormatArray format))
                  (nextFormat      (formatFormatArrayItem formatArray 1))
                  (arrayDimensions (varArrayDimensions formatArray parentFormat
						       dataStruct currentData))
		  (arraySize	   (apply #'* arrayDimensions))
		  (array	   (aref dataStruct currentData)))

	     (formatPutInt buffer arraySize)
	     (cond ((zerop arraySize))
		   (T (let* ((dispVector (displaced_vector array))
			     (elementSize (findSimpleTypeSize nextFormat)))
			(check-dimensions arrayDimensions array)
			(cond ((and elementSize 
				    (equal (array-element-type dispVector)
					   (findSimpleType nextFormat)))
			       (blockCopyFromArray buffer dispVector 
						   (* elementSize arraySize)))
			      (T
			       (dotimes (i arraySize)
				 (TransferToBuffer nextFormat dispVector
						   i buffer NIL)))))))
             (incf currentData STRUCT_ELEM_SIZE)))

	  ((eql type NamedFMT)
	   (transferToBuffer (findNamedFormat format) dataStruct
			     dStart buffer parentFormat)
	   (incf currentData STRUCT_ELEM_SIZE))

	  ((eql type EnumFMT)
	   (let ((enumVal (aref dataStruct dStart)))
	     (when (symbolp enumVal)
	       (setq enumVal (enumSymbolIndex format enumVal)))
	     (unless (numberp enumVal)
	       (error "TransferToBuffer: ~a not valid enumerated value" 
		      (aref dataStruct dStart)))
	     (formatPutInt buffer enumVal))
	   (incf currentData STRUCT_ELEM_SIZE)))

    (- currentData dStart)))

(defun DataStructureSize (format)
  (let ((size 0) (type (formatType format)))
    (cond ((eql type LengthFMT) 
	   (Explicit-Format-Error))
          
          ((eql type PrimitiveFMT)
           (incf size (funcall (aref TransTable (formatPrimitiveProc format))
                               ALength NIL LISP_STRUCT_START NIL)))
          
          ((or (eql type PointerFMT) (eql type VarArrayFMT))
           (incf size STRUCT_ELEM_SIZE))
          
          ((eql type StructFMT)
	   (let ((formatArray (formatFormatArray format)))
	     (doStructFormat (i formatArray)
	       (let ((nextFormat (formatFormatArrayItem formatArray i)))
		 ;; Allow for the future possibility that some
		 ;; primitive format might take up more than one slot
		 ;; in a structure.
		 (incf size (cond ((eql (formatType nextFormat) PrimitiveFMT)
				   (DataStructureSize nextFormat))
				  (T STRUCT_ELEM_SIZE)))))))

          ((eql type FixedArrayFMT)
	   (incf size STRUCT_ELEM_SIZE))

	  ((eql type NamedFMT) 
	   (incf size (dataStructureSize (findNamedFormat format))))

	  ((eql type EnumFMT)
	   (incf size STRUCT_ELEM_SIZE)))

    size))

;;; *************************************************************

(defun make-TC_Struct (size &optional (type :DATA))
  (let ((new_struct (make-array (1+ size))))
    (setf (aref new_struct 0) type)
    new_struct))

(defun get-or-allocate-TC_Struct (size DataStruct DStart)
  (let ((struct (aref DataStruct DStart)))
    (cond ((and (typep struct 'vector) (>= (length struct) (1+ size)))
	   struct) ; already allocated
	  (T (setq struct (make-TC_Struct size))
	     (setf (aref DataStruct DStart) struct)
	     struct))))

;;; Actual array dimensions are "compatible" with the required dimensions 
;;; if all dimensions are equal, except for the first, which may be larger.
;;; ASSUMES ARRAYS ARE STORED WITH LAST DIM
(defun compatible-array (actual-dimensions required-dimensions)
  (and (>= (first actual-dimensions) (first required-dimensions))
       (equal (cdr actual-dimensions) (cdr required-dimensions))))

(defun get-or-allocate-array (array_dimensions element_type DataStruct DStart)
  (let ((array (aref DataStruct DStart)))
    (cond ((and (typep array 'array) 
		(subtypep element_type (array-element-type array))
		(compatible-array (array-dimensions array) array_dimensions))
	   array) ; already allocated
	  (T (setq array
		   (make-array array_dimensions :element-type element_type))
	     (setf (aref DataStruct DStart) array)
	     ;;(format t "Allocate Array ~A ~A~%" array_dimensions element_type)
	     array))))

(defun TransferToDataStructure (format dataStruct dStart buffer parentFormat)
  (let ((currentData DStart) (type (formatType format)))
    (cond ((eql type LengthFMT) 
	   (Explicit-Format-Error))

          ((eql type PrimitiveFMT)
           (let ((formatProc (aref TransTable (formatPrimitiveProc format))))
	     (funcall formatProc Decode dataStruct currentData buffer)
             (incf currentData 
		   (funcall formatProc ALength NIL LISP_STRUCT_START NIL)))) 

          ((eql type PointerFMT)
           (let ((ptrVal (formatGetChar buffer)))
             (cond ((eql ptrVal (character 0))
		    (setf (aref dataStruct currentData) NIL))
		   (T (let ((nextFormat (formatChoosePtrFormat format
							       parentFormat)))
			(TransferToDataStructure nextFormat dataStruct
						 currentData buffer NIL)))))
	   (incf currentData STRUCT_ELEM_SIZE))

	  ((eql type StructFMT)
	   (let ((formatArray (formatFormatArray format))
		 (structStart LISP_STRUCT_START)
		 (new_struct
		  (get-or-allocate-TC_Struct (DataStructureSize format)
					     dataStruct currentData)))
	     (doStructFormat (i formatArray)
	       (incf structStart 
		     (TransferToDataStructure
		      (formatFormatArrayItem formatArray i)
		      new_struct structStart buffer format)))
	     (incf currentData STRUCT_ELEM_SIZE)))

          ((eql type FixedArrayFMT)
           (let* ((formatArray (formatFormatArray format))
                  (nextFormat (formatFormatArrayItem formatArray 1))
                  (arrayDimensions (fixedArrayDimensions formatArray))
		  (arraySize (apply #'* arrayDimensions))
		  (elementType (findSimpleType nextFormat))
		  (elementSize (findSimpleTypeSize nextFormat))
		  (array (get-or-allocate-array arrayDimensions
						(or elementType T)
						dataStruct currentData))
		  (dispVector (displaced_vector array)))
	     (cond ((and elementSize
			 #+:ACLPC(not (eq (array-element-type dispVector)
					  'character))
			 (eql (msgByteOrder) (hostByteOrder))
			 (equal (array-element-type dispVector)
				(findSimpleType nextFormat)))
		    (blockCopyToArray buffer dispVector 
				      (* elementSize arraySize)))
		   (T 
		    (dotimes (i arraySize)
		      (TransferToDataStructure nextFormat dispVector
					       i buffer NIL)))))
	   (incf currentData STRUCT_ELEM_SIZE))

	  ((eql type VarArrayFMT)
           (let* ((formatArray (formatFormatArray format))
                  (nextFormat (formatFormatArrayItem formatArray 1))
		  (arraySize (formatGetInt buffer))
		  arrayDimensions)
	     (cond ((zerop arraySize)
		    (setf (aref dataStruct currentData) NIL))
		   (T (cond ((> (formatFormatArrayMax formatArray) 3)
			     ;; multi-dimensional array
			     (setq arrayDimensions
				   (varArrayDimensions formatArray parentFormat
						       dataStruct currentData))
			     (unless (= arraySize (apply #'* arrayDimensions))
			       (error "variable length array dimensions don't match")))
			    (T (setq arrayDimensions (list arraySize))))

		      (let* ((elementType (findSimpleType nextFormat))
			     (elementSize (findSimpleTypeSize nextFormat))
			     (array (get-or-allocate-array
				     arrayDimensions (or elementType T)
				     dataStruct currentData))
			     (dispVector (displaced_vector array)))
			(cond ((and elementSize
				    (eql (msgByteOrder) (hostByteOrder))
				    (equal (array-element-type dispVector)
					   (findSimpleType nextFormat)))
			       (blockCopyToArray buffer dispVector 
						 (* elementSize arraySize)))
			      (T
			       (dotimes (i arraySize)
				 (TransferToDataStructure nextFormat dispVector
							  i buffer NIL))))))))
	   (incf currentData STRUCT_ELEM_SIZE))

	  ((eql type NamedFMT)
	   (transferToDataStructure (findNamedFormat format) 
				    dataStruct dStart buffer parentFormat)
	   (incf currentData STRUCT_ELEM_SIZE))

	  ((eql type EnumFMT)
	   (let ((enumVal (formatGetInt buffer)))
	     (setf (aref dataStruct dStart) 
		   (or (getNthEnum format enumVal) enumVal))
	     (incf currentData STRUCT_ELEM_SIZE))))

    (- currentData DStart)))

;;;*************************************************************
;;;
;;;  THESE FUNCTIONS FORM THE INTERFACE TO THE REST OF THE SYSTEM
;;;
;;;*************************************************************/

;;; 
;;; For calls to EncodeData, the data must be enclosed within a top-level 
;;; structure (starting at element 1); Similarly for the optional argument
;;; to DecodeData, the data must be enclosed in a top-level structure.  
;;; These routines simplify the creation and management of such enclosing 
;;; structures.
;;;
(defvar enclosing-structs NIL)

;;; Structures sent as data via IPC must be of type "vector" so that formatter
;;;    functions can access their fields without having to know the names of
;;;    their accessor functions (Reid)
#+(OR ALLEGRO-V4.0 :ACLPC :MCL)
(defmacro IPC_defstruct ((name &rest args) &body fields)
  `(progn (defstruct (,name ,@args (:type vector) :named) ,@fields)
	  (deftype ,name () '(or vector NULL))))

#+(AND ALLEGRO (NOT ALLEGRO-V4.0))
(defmacro IPC_defstruct ((name &rest args) &body fields)
  `(defstruct (,name ,@args (:type vector) :named) ,@fields))

#+LISPWORKS
(defmacro IPC_defstruct ((name &rest args) &body fields)
  `(progn (defstruct (,name ,@args (:type vector) )
	    ;; The first field is the "type name"
	    (%%name%% ',name) 
	    ,@fields)
  	  (deftype ,name () '(or vector NULL))))


(IPC_defstruct (encl-struct) element)

(defun enclose-struct (element)
  (cond (enclosing-structs 
	 (let ((enclosing-struct enclosing-structs))
	   (setq enclosing-structs (encl-struct-element enclosing-structs))
	   (setf (encl-struct-element enclosing-struct) element)
	   enclosing-struct))
	(T (make-encl-struct :element element))))

(defun free-enclosing-struct (enclosing-struct)
  (setf (encl-struct-element enclosing-struct) enclosing-structs)
  (setq enclosing-structs enclosing-struct))

;;;*************************************************************

(defun BufferSize (Format DataStruct)
  (cond ((= format 0) 0)
	(T (BufferSize1 Format (enclose-struct DataStruct)
			LISP_STRUCT_START NIL))))

;;;*************************************************************
;;;
;;; (defun DataStructureSize (Format)
;;;  (declare (function DataStructureSize (Formatter) integer))
;;;
;;;  ( .
;;;    . Function is defined (and used) above
;;;    .
;;;  )
;;;
;;;*************************************************************

(defun EncodeData (format dataStruct buffer)
  (TransferToBuffer format (enclose-struct dataStruct)
		    LISP_STRUCT_START buffer NIL))

;;;*************************************************************

(defun DecodeData (format buffer &optional dataStruct)
  (unless dataStruct (setq dataStruct (make-TC_Struct 1)))
  (when format
    ;; no need to allocate decoded struct, done within "TransferToDataStructure"
    (TransferToDataStructure format dataStruct LISP_STRUCT_START Buffer NIL))
  (aref dataStruct LISP_STRUCT_START))

