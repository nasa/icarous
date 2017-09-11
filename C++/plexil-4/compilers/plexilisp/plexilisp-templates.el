;;; plexilisp-templates --- Tempo-based templates for plexilisp
;;; Copyright (c) 2010 Robert P. Goldman and SIFT, LLC

;;; This module is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published
;;; by the Free Software Foundation; either version 2, or (at your
;;; option) any later version.

;;; This module is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;;; General Public License for more details.

;;; You should have received a copy of the GNU General Public License
;;; along with GNU Emacs; see the file COPYING.  If not, write to the
;;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;;; Boston, MA 02111-1307, USA.

;;; To load this file, simply (require 'plexilisp-templates)
;;; typically this should be done in a plexilisp-mode-hook

(require 'tempo)
(require 'easymenu)
(require 'plexilisp "plexil")

(tempo-define-template
 "plexil-plan"
 (list
  "(plexil-plan " 'n>
  "(global-declarations )" 'n> 'n>
  ")" 'n))

;;;---------------------------------------------------------------------------
;;; Start node templates
;;;---------------------------------------------------------------------------

(tempo-define-template
 "plexil-list-node"
 (list
  "(list-node "
  "\"" '(p "List node name? " node-name) "\"" 'n> 
  "(list " 'n>
  'r ; if we're this around a previously-existing node, that
                                        ; node goes here
  ")" 'n>
  ") ; end list-node " '(s node-name) 'n))

(tempo-define-template
 "plexil-empty-node"
 (list
  "(empty-node "
  "\"" '(p "Empty node name? " node-name) "\"" 'n> 
  ") ; end empty-node " '(s node-name)))

;;; command nodes
(defvar *plexil-template-command-takes-return*)

(tempo-define-template
 "plexil-command-node"
 (list
  "(command-node "
  "\"" '(p "Command node name? " node-name) "\"" 'n>
  '(cond ((y-or-n-p "Does this command return a value?")
         (setq *plexil-template-command-takes-return* t)
         '(l "(command-with-return "
           (p "Variable to which to assign return value?" retval nil)))
        (t
         (setq *plexil-template-command-takes-return* nil)
         "(command "))
  '(p "Command name?" command-name)
  '(when *plexil-template-command-takes-return*
    '(s retval))
  '(p "Argument list?")
  ")) ; end command-node " '(s node-name)))

;;; End command nodes

(tempo-define-template
 "plexil-assignment-node"
 (list
  "(assignment-node "
  "\"" '(p "Assignment node name? " node-name) "\"" 'n>
  "(assignment "
  '(p "Variable to assign?")
  '(p "RHS?")
  ")) ; end assignment-node " '(s node-name)))

;;;---------------------------------------------------------------------------
;;; End of node templates
;;;---------------------------------------------------------------------------


;;;---------------------------------------------------------------------------
;;; Node conditions
;;;---------------------------------------------------------------------------

(defconst +plexil-conditions+
  '(
    postcondition
    end-condition
    skip-condition
    precondition
    repeat-condition
    start-condition
    invariant-condition))

(dolist (x +plexil-conditions+)
  (eval `(tempo-define-template ,(concat "plexil-" (symbol-name x))
            (list "(" ,(symbol-name x) " " 'p ")"))))

;;;---------------------------------------------------------------------------
;;; End of node conditions
;;;---------------------------------------------------------------------------

;;;---------------------------------------------------------------------------
;;; Declarations
;;;---------------------------------------------------------------------------
(tempo-define-template
 "plexil-define-lookup"
 (list
  "(state-declaration "
  "\"" '(p "state variable name? ") "\""
  "(return "
  '(completing-read "Return value type (boolean, integer, real, string)?"
                   '("boolean" "integer" "real" "string")
                   nil t)
  ")"
  '(loop for param = (read-string "Parameter name (or nothing)?")
        while (not (equal param ""))
        collect (concat "(parameter \"" param "\")") into results
        finally (when results (return (cons 'l results))))
  ")"))



;;;---------------------------------------------------------------------------
;;; end of declarations
;;;---------------------------------------------------------------------------

;;;---------------------------------------------------------------------------
;;; Miscellaneous items
;;;---------------------------------------------------------------------------
;;; FIXME: should permit adding parameters to lookup-now, but we don't
;;; ever use these. [2010/03/11:rpg]
(tempo-define-template
 "plexil-lookup-now"
 (list
  "(lookup-now "
  "\"" '(p "state variable name?") "\""  ")"))

(tempo-define-template
 "plexil-variable-declarations"
 (list
  "(declare-variables " 'p 'n>
  ")"))  

;;;---------------------------------------------------------------------------
;;; Menu setup 
;;;---------------------------------------------------------------------------
(easy-menu-define plexil-template-map plexilisp-mode-map "Plexilisp template menu"
		      '("Plexilisp templates"
                        ["Plexil plan" tempo-template-plexil-plan t] 
                        ("Nodes"
                         ["List node" tempo-template-plexil-list-node t]
                         ["Empty node" tempo-template-plexil-empty-node t]
                         ["Command node" tempo-template-plexil-command-node t]
                         ["Assignment node" tempo-template-plexil-assignment-node t]
                         )
                        ("Node conditions"
                         ["Postcondition" tempo-template-plexil-postcondition t]
                         ["End condition" tempo-template-plexil-end-condition t]
                         ["Skip condition" tempo-template-plexil-skip-condition t]
                         ["Precondition" tempo-template-plexil-precondition t]
                         ["Repeat condition" tempo-template-plexil-repeat-condition t]
                         ["Start condition" tempo-template-plexil-start-condition t]
                         ["Invariant condition" tempo-template-plexil-invariant-condition t]
                         )
                        ("Global declarations"
                         ["Lookup (state) declaration"
                          tempo-template-plexil-define-lookup t]
                         )
                        ["Variable declarations"
                         tempo-template-plexil-variable-declarations t]
			["Lookup now" tempo-template-plexil-lookup-now t]))

;;; on xemacs we need to additionally do
(when (featurep 'xemacs) 
  (easy-menu-add plexil-template-map plexilisp-mode-map))

;;;---------------------------------------------------------------------------
;;; Highlighting for Plexilisp
;;; only partially complete
;;;---------------------------------------------------------------------------
(defvar plexilisp-mode-node-keywords
  (append 
          '(ListNode list-node)
          '(CommandNode command-node)
          '(CommandWithReturn command-with-return)
          '(UpdateNode update-node)
          '(AssignmentNode assignment-node)
          '(LibraryCallNode library-call-node)
          '(EmptyNode empty-node)))

(defvar plexilisp-mode-structure-keywords
  (append
   '(plexil-plan PlexilPlan)
   '(GlobalDeclarations global-declarations Declarations declarations)))

(defvar plexilisp-declaration-keywords
  (append
   '(LibraryNodeDeclaration library-node-declaration)
   '(StateDeclaration state-declaration)
   '(CommandDeclaration command-declaration)))

(defvar plexilisp-font-lock-keywords
  (list
   (cons (regexp-opt
          (mapcar 'symbol-name plexilisp-mode-node-keywords) t)
         'font-lock-keyword-face)
   (cons (regexp-opt
          (mapcar 'symbol-name plexilisp-mode-structure-keywords) t)
         'font-lock-builtin-face)
   ))


(add-hook 'plexilisp-mode-hook
          '(lambda ()
            (set (make-local-variable 'font-lock-defaults)
             '(plexilisp-font-lock-keywords))))

(provide 'plexilisp-templates)
