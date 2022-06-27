;;; ld-script.el --- GNU linker script editing mode for Emacs

;; Copyright (C) 2001-2018 Free Software Foundation, Inc.

;; Author: Masatake YAMATO<jet@gyve.org>
;; Keywords: languages, faces

;; This file is part of GNU Emacs.

;; GNU Emacs is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs.  If not, see <https://www.gnu.org/licenses/>.

;;; Commentary:

;; Major mode for editing GNU linker (ld) scripts.

;;; Code:

;; Custom
(defgroup ld-script nil
  "GNU linker script code editing commands for Emacs."
  :prefix "ld-script-"
  :group 'languages)

(defvar ld-script-location-counter-face 'ld-script-location-counter)
(defface ld-script-location-counter
  '((t :weight bold :inherit font-lock-builtin-face))
  "Face for location counter in GNU ld script."
  :group 'ld-script)

;; Syntax rules
(defvar ld-script-mode-syntax-table
  (let ((st (make-syntax-table)))
    (modify-syntax-entry ?\  "-"   st)
    (modify-syntax-entry ?{ "(}" st)
    (modify-syntax-entry ?} "){" st)
    (modify-syntax-entry ?\( "()" st)
    (modify-syntax-entry ?\) ")(" st)
    (modify-syntax-entry ?\[ "(]" st)
    (modify-syntax-entry ?\] ")[" st)
    (modify-syntax-entry ?_ "_"   st)
    (modify-syntax-entry ?. "_"   st)
    (modify-syntax-entry ?\\  "\\" st)
    (modify-syntax-entry ?: "." st)
    (modify-syntax-entry ?, "." st)
    (modify-syntax-entry ?? "." st)
    (modify-syntax-entry ?= "." st)
    (modify-syntax-entry ?* ". 23" st)
    (modify-syntax-entry ?/ ". 14" st)
    (modify-syntax-entry ?+ "." st)
    (modify-syntax-entry ?- "." st)
    (modify-syntax-entry ?! "." st)
    (modify-syntax-entry ?~ "." st)
    (modify-syntax-entry ?% "." st)
    (modify-syntax-entry ?< "." st)
    (modify-syntax-entry ?> "." st)
    (modify-syntax-entry ?& "." st)
    (modify-syntax-entry ?| "." st)
    (modify-syntax-entry ?\" "\"" st)
    st)
  "Syntax table used while in `ld-script-mode'.")

;; Font lock keywords
;; (The section number comes from ld's info.)
(defvar ld-script-keywords
  '(
    ;; 3.4.1 Setting the Entry Point
    "ENTRY"
    ;; 3.4.2 Commands Dealing with Files
    "INCLUDE" "INPUT" "GROUP" "AS_NEEDED" "OUTPUT" "SEARCH_DIR" "STARTUP"
    ;; 3.4.3 Commands Dealing with Object File Formats
    "OUTPUT_FORMAT" "TARGET"
    ;; 3.4.4 Assign alias names to memory regions
    "REGION_ALIAS"
    ;; 3.4.5 Other Linker Script Commands
    "ASSERT" "EXTERN" "FORCE_COMMON_ALLOCATION"
    "INHIBIT_COMMON_ALLOCATION" "INSERT" "AFTER" "BEFORE"
    "NOCROSSREFS" "NOCROSSREFS_TO" "OUTPUT_ARCH" "LD_FEATURE"
    ;; 3.5.2 HIDDEN
    "HIDDEN"
    ;; 3.5.3 PROVIDE
    "PROVIDE"
    ;; 3.5.4 PROVIDE_HIDDEN
    "PROVIDE_HIDDEN"
    ;; 3.6 SECTIONS Command
    "SECTIONS"
    ;; 3.6.4.2 Input Section Wildcard Patterns
    "SORT" "SORT_NONE" "SORT_BY_NAME" "SORT_BY_ALIGNMENT"
    "SORT_BY_INIT_PRIORITY"
    ;; 3.6.4.3 Input Section for Common Symbols
    "COMMON"
    ;; 3.6.4.4 Input Section and Garbage Collection
    "KEEP"
    ;; 3.6.5 Output Section Data
    "BYTE" "SHORT" "LONG" "QUAD" "SQUAD" "FILL"
    ;; 3.6.6 Output Section Keywords
    "CREATE_OBJECT_SYMBOLS" "CONSTRUCTORS"
    "__CTOR_LIST__" "__CTOR_END__" "__DTOR_LIST__" "__DTOR_END__"
    ;; 3.6.7 Output Section Discarding
    ;; See `ld-script-font-lock-keywords'
    ;; 3.6.8.1 Output Section Type
    "NOLOAD" "DSECT" "COPY" "INFO" "OVERLAY"
    ;; 3.6.8.2 Output Section LMA
    "AT"
    ;; 3.6.8.4 Forced Input Alignment
    "SUBALIGN"
    ;; 3.6.8.5 Output Section Constraint
    "ONLY_IF_RO" "ONLY_IF_RW"
    ;; 3.6.8.7 Output Section Phdr
    ":PHDR"
    ;; 3.7 MEMORY Command
    "MEMORY"
    ;; 3.8 PHDRS Command
    "PHDRS" "FILEHDR" "FLAGS"
    "PT_NULL" "PT_LOAD" "PT_DYNAMIC" "PT_INTERP" "PT_NOTE"
    "PT_SHLIB" "PT_PHDR" "PT_GNU_STACK" "PT_TLS"
    ;; 3.9 VERSION Command
    "VERSION")
  "Keywords used of GNU ld script.")


;; 3.10.2 Symbolic Constants
;; 3.10.9 Builtin Functions
(defvar ld-script-builtins
  '("CONSTANT"
    "MAXPAGESIZE"
    "COMMONPAGESIZE"
    "ABSOLUTE"
    "ADDR"
    "ALIGN"
    "ALIGNOF"
    "BLOCK"
    "DATA_SEGMENT_ALIGN"
    "DATA_SEGMENT_END"
    "DATA_SEGMENT_RELRO_END"
    "DEFINED"
    "LENGTH" "len" "l"
    "LOADADDR"
    "LOG2CEIL"
    "MAX"
    "MIN"
    "NEXT"
    "ORIGIN" "org" "o"
    "SEGMENT_START"
    "SIZEOF"
    "SIZEOF_HEADERS"
    "sizeof_headers")
  "Builtin functions of GNU ld script.")

(defvar ld-script-font-lock-keywords
  (append
   `((,(concat "\\_<" (regexp-opt ld-script-keywords) "\\_>")
      0 font-lock-keyword-face)
     (,(concat "\\_<" (regexp-opt ld-script-builtins) "\\_>")
      0 font-lock-builtin-face)
     ;; 3.6.7 Output Section Discarding
     ;; 3.6.4.1 Input Section Basics
     ;; 3.6.8.7 Output Section Phdr
     ("/DISCARD/\\|EXCLUDE_FILE\\|:NONE" . font-lock-warning-face)
     ("\\W\\(\\.\\)\\W" 1 ld-script-location-counter-face)
     )
   cpp-font-lock-keywords)
  "Default font-lock-keywords for `ld-script-mode'.")

;;;###autoload
(define-derived-mode ld-script-mode prog-mode "LD-Script"
   "A major mode to edit GNU ld script files"
  (set (make-local-variable 'comment-start) "/* ")
  (set (make-local-variable 'comment-end)   " */")
  (set (make-local-variable 'font-lock-defaults)
       '(ld-script-font-lock-keywords nil)))

(provide 'ld-script)

;;; ld-script.el ends here
