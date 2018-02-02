;;; Copyright (c) 2008, Carnegie Mellon University
;;;     This software is distributed under the terms of the 
;;;    Simplified BSD License (see ipc/LICENSE.TXT)

(ff:defforeign 'Print_Format
  :entry-point (ff::convert-to-lang "Print_Format")
  :arguments '(integer #|formatter|#)
  :return-type :void)

(ipc:ipc_initialize)

(setq vc (ipc:make-IPC_VARCONTENT))
(setq value NIL)

(setq offset 1)

(setq byte-format (ipc:ipc_parseformat "byte"))
(setq ubyte-format (ipc:ipc_parseformat "ubyte"))

(setq short-format (ipc:ipc_parseformat "short"))
(setq ushort-format (ipc:ipc_parseformat "ushort"))

(setq int-format (ipc:ipc_parseformat "int"))
(setq uint-format (ipc:ipc_parseformat "uint"))

(setq long-format (ipc:ipc_parseformat "long"))
(setq ulong-format (ipc:ipc_parseformat "ulong"))

(setq byte-array-format (ipc:ipc_parseformat "[byte:10]"))
(setq ubyte-array-format (ipc:ipc_parseformat "[ubyte:10]"))

(setq short-array-format (ipc:ipc_parseformat "[short:10]"))
(setq ushort-array-format (ipc:ipc_parseformat "[ushort:10]"))

(setq int-array-format (ipc:ipc_parseformat "[int:10]"))
(setq uint-array-format (ipc:ipc_parseformat "[uint:10]"))

(setq long-array-format (ipc:ipc_parseformat "[long:10]"))
(setq ulong-array-format (ipc:ipc_parseformat "[ulong:10]"))

(setq ubyte-array (make-array 10 :element-type '(unsigned-byte 8) 
			      :initial-element (+ offset (/ 256 2))))
(setq byte-array (make-array 10 :element-type '(signed-byte 8) 
			     :initial-element (+ offset (- (/ 256 2)))))

(setq ushort-array (make-array 10 :element-type '(unsigned-byte 16) 
			      :initial-element (+ offset (/ (* 256 256) 2))))
(setq short-array (make-array 10 :element-type '(signed-byte 16) 
			     :initial-element (+ offset (- (/ (* 256 256) 2)))))

(setq uint-array (make-array 10 :element-type '(unsigned-byte 32) 
			      :initial-element 
			      (+ offset (/ (* 256 256 256 256) 2))))
(setq int-array (make-array 10 :element-type '(signed-byte 32) 
			     :initial-element 
			     (+ offset (- (/ (* 256 256 256 256) 2)))))

(setq ulong-array (make-array 10 :element-type '(unsigned-byte 32) 
			      :initial-element 
			      (+ offset (/ (* 256 256 256 256) 4))))
(setq long-array (make-array 10 :element-type '(signed-byte 32) 
			     :initial-element
			     (+ offset (- (/ (* 256 256 256 256) 4)))))

(defun marshall-value-test (format array)
  (ipc:IPC_marshall format (aref array 0) vc)
  (ipc:IPC_unmarshall format (ipc:ipc_varcontent-content vc) value)
  (format T "~d: ~a ~a ~%"
	  (ipc:ipc_varcontent-length vc) (aref array 0) value))

(defun marshall-array-test (format array)
  (ipc:IPC_marshall format array vc)
  (ipc:IPC_unmarshall format (ipc:ipc_varcontent-content vc) value)
  (format T "~d: ~a ~a ~%"
	  (ipc:ipc_varcontent-length vc) (aref array 0) value))

(defun testit ()
  (marshall-value-test byte-format byte-array)
  (marshall-value-test ubyte-format ubyte-array)
  (marshall-value-test short-format short-array)
  (marshall-value-test ushort-format ushort-array)
  (marshall-value-test int-format int-array)
  (marshall-value-test uint-format uint-array)
  (marshall-value-test long-format long-array)
  (marshall-value-test ulong-format ulong-array))

(defun testit1 ()
  (marshall-value-test ubyte-format byte-array)
  (marshall-value-test byte-format ubyte-array)
  (marshall-value-test ushort-format short-array)
  (marshall-value-test short-format ushort-array)
  (marshall-value-test uint-format int-array)
  (marshall-value-test int-format uint-array)
  (marshall-value-test ulong-format long-array)
  (marshall-value-test long-format ulong-array))

(defun testit2 ()
  (marshall-array-test byte-array-format byte-array)
  (marshall-array-test ubyte-array-format ubyte-array)
  (marshall-array-test short-array-format short-array)
  (marshall-array-test ushort-array-format ushort-array)
  (marshall-array-test int-array-format int-array)
  (marshall-array-test uint-array-format uint-array)
  (marshall-array-test long-array-format long-array)
  (marshall-array-test ulong-array-format ulong-array))

(defun testit3 ()
  (marshall-array-test ubyte-array-format byte-array)
  (marshall-array-test byte-array-format ubyte-array)
  (marshall-array-test ushort-array-format short-array)
  (marshall-array-test short-array-format ushort-array)
  (marshall-array-test uint-array-format int-array)
  (marshall-array-test int-array-format uint-array)
  (marshall-array-test ulong-array-format long-array)
  (marshall-array-test long-array-format ulong-array))

(defun testit4 ()
  (marshall-array-test int-array-format byte-array)
  (marshall-array-test int-array-format ubyte-array)
  (marshall-array-test int-array-format short-array)
  (marshall-array-test int-array-format ushort-array)

  (marshall-array-test byte-array-format short-array)

  (marshall-array-test short-array-format byte-array)
  (marshall-array-test short-array-format ubyte-array)

  (marshall-array-test short-array-format int-array)

  (marshall-array-test uint-array-format ubyte-array)
  (marshall-array-test uint-array-format ushort-array)
  )
