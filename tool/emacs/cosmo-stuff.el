;; ╔──────────────────────────────────────────────────────────────────╗
;; │ To the extent possible under law, Justine Tunney has waived      │
;; │ all copyright and related or neighboring rights to this file,    │
;; │ as it is written in the following disclaimers:                   │
;; │   • http://unlicense.org/                                        │
;; │   • http://creativecommons.org/publicdomain/zero/1.0/            │
;; ╚──────────────────────────────────────────────────────────────────╝
;; Hodgepodge of copypasta from Justine's Emacs config intended to be
;; helpful to those wanting to configure their own Emacs editor to work
;; pretty well as a Cosmopolitan IDE.

;;; Code:

(require 'asm-mode)
(require 'cc-mode)
(require 'fortran)
(require 'cosmo-c-types)
(require 'cosmo-c-keywords)
(require 'cosmo-c-builtins)
(require 'cosmo-c-constants)
(require 'cosmo-cpp-constants)
(require 'cosmo-platform-constants)
(require 'dired)
(require 'javadown)
(require 'ld-script)
(require 'make-mode)

(setq cosmo-arch
      (let ((arch (string-trim-right
                   (shell-command-to-string
                    "uname -m"))))
        (cond ((string= arch "amd64") "x86_64")
              ((string= arch "arm64") "aarch64")
              (t arch))))

(setq cosmo-default-mode
      (cond ((string= cosmo-arch "aarch64") "aarch64")
            (t "")))

;; TODO(jart): How do we get GCC to do only dead code elimination?
(setq cosmo-dbg-mode
      (cond ((string= cosmo-arch "aarch64") "aarch64")
            (t "dbg")))

(setq c-doc-comment-style 'javadown)

(add-to-list 'auto-mode-alist '("\\.x$" . c-mode))  ;; -aux-info


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Support Macros

(defmacro setql (var val)
  `(set (make-local-variable ',var) ,val))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Support Functions

(defun cosmo-contains (x s &optional icase)
  (declare (pure t) (side-effect-free t))
  (let ((case-fold-search icase))
    (not (null (string-match-p (regexp-quote x) s)))))

(defun cosmo-startswith (x s &optional icase)
  (declare (pure t) (side-effect-free t))
  (not (null (string-prefix-p x s icase))))

(defun cosmo-endswith (x s &optional icase)
  (declare (pure t) (side-effect-free t))
  (let ((p (- (length s) (length x))))
    (and (>= p 0)
         (eq t (compare-strings x nil nil
                                s p nil icase)))))

(defun cosmo-replace (x y s)
  (declare (pure t) (side-effect-free t))
  (let ((case-fold-search nil))
    (replace-regexp-in-string (regexp-quote x) y s t t)))

(defun cosmo-join (s l)
  (declare (pure t) (side-effect-free t))
  (mapconcat 'identity l s))

(defun cosmo-lchop (p s)
  (declare (pure t) (side-effect-free t))
  (if (and (>= (length s) (length p))
           (string= p (substring s 0 (length p))))
      (substring s (length p))
    s))

(defun cosmo-first-that (predicate list)
  (declare (pure t))
  (when list
    (if (funcall predicate (car list))
        (car list)
      (cosmo-first-that predicate (cdr list)))))

(defun cosmo-file-name-sans-extensions (filename)
  (save-match-data
    (let (directory
          (file (file-name-sans-versions
                 (file-name-nondirectory filename))))
      (if (and (string-match "[.-].*\\'" file)
	       (not (eq 0 (match-beginning 0))))
	  (if (setq directory (file-name-directory filename))
	      (concat directory (substring file 0 (match-beginning 0)))
	    (substring file 0 (match-beginning 0)))
	filename))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; C-c .     Jump to symbol definition
;; C-c ,     Jump back to where we were
;; C-c C-i   Show references to symbol

;; TODO(jart): Why doesnt the default-directory variable work?
(defun project-current (&optional maybe-prompt dir)
  (expand-file-name
   (file-name-directory
    (locate-dominating-file
     (buffer-name) "Makefile"))))

(defun cosmo-xref-find-references ()
  (interactive)
  (let ((case-fold-search nil))
    (xref-find-references (format "%S" (symbol-at-point)))))

(defun cosmo-symbol-at-point (&optional no-properties)
  (let ((stab (copy-syntax-table)))
    (with-syntax-table stab
      (modify-syntax-entry ?+ " ")
      (modify-syntax-entry ?* " ")
      (let ((thing (thing-at-point 'symbol no-properties)))
        (when thing
          (intern thing))))))

(defun cosmo-xref-find-definitions ()
  (interactive)
  (let ((case-fold-search nil))
    (xref-find-definitions (format "%S" (cosmo-symbol-at-point)))))

(global-set-key (kbd "M-,") 'xref-pop-marker-stack)
(global-set-key (kbd "M-.") 'cosmo-xref-find-definitions)
(global-set-key (kbd "C-c TAB") 'cosmo-xref-find-references)

(defun stop-asking-questions-etags ()
  (set (make-local-variable 'tags-file-name)
       (format "%s/TAGS"
               (or (locate-dominating-file (buffer-name) "Makefile")
                   (file-name-directory (buffer-name))))))
(add-hook 'c-mode-common-hook 'stop-asking-questions-etags)
(add-hook 'c++-mode-common-hook 'stop-asking-questions-etags)
(setq tags-revert-without-query t)
(setq kill-buffer-query-functions ;; disable kill buffer w/ process question
      (delq 'process-kill-buffer-query-function kill-buffer-query-functions))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Compile buffer and run associated test and/or display assembly
;;       C-c C-c   Compile
;;   M-1 C-c C-c   Compile w/ MODE=tiny
;;   M-2 C-c C-c   Compile w/ MODE=opt
;;   M-3 C-c C-c   Compile w/ MODE=rel
;;   M-4 C-c C-c   Compile w/ MODE=dbg
;;   M-5 C-c C-c   Compile w/ MODE=""
;;   M-7 C-c C-c   Compile w/ MODE=aarch64
;;   M-8 C-c C-c   Compile w/ MODE=aarch64-tiny
;;   M-9 C-c C-c   Compile w/ chibicc

(defun cosmo-intest (&optional file-name)
  (let (path root pkg)
    (and (setq path (or (buffer-file-name) dired-directory))
         (setq root (locate-dominating-file path "Makefile"))
         (setq pkg (file-relative-name path root))
         (cosmo-startswith "test/" pkg))))

(defun cosmo--make-mode (arg &optional default)
  (cond ((eq arg 1)
         (cond ((string= cosmo-arch "aarch64") "aarch64-tiny")
               (t "tiny")))
        ((eq arg 2) "opt")
        ((eq arg 3) "rel")
        ((eq arg 4)
         cosmo-dbg-mode)
        ((eq arg 5)
         (cond ((string= cosmo-arch "aarch64") "aarch64")
               (t cosmo-default-mode)))
        ((eq arg 6) "llvm")
        ((eq arg 7)
         (cond ((string= cosmo-arch "aarch64") "x86_64")
               (t "aarch64")))
        ((eq arg 8)
         (cond ((string= cosmo-arch "aarch64") "tiny")
               (t "aarch64-tiny")))
        (default default)
        ((cosmo-intest) cosmo-dbg-mode)
        (t cosmo-default-mode)))

(defun cosmo--make-suffix (arg)
  (cond ((eq arg 9) ".chibicc")
        (t "")))

(defun cosmo--make-objdump-flags (arg)
  (cond ((eq arg 9) "-x")
        (t "")))

(defun cosmo--compile-command (this root kind mode suffix objdumpflags runsuffix)
  (let* ((ext (file-name-extension this))       ;; e.g. "c"
         (dir (file-name-directory this))       ;; e.g. "/home/jart/daisy/libc/"
         (dots (file-relative-name root dir))   ;; e.g. "../"
         (file (file-relative-name this root))  ;; e.g. "libc/crc32c.c"
         (name (file-name-sans-extension file)) ;; e.g. "libc/crc32c"
         (buddy (format "test/%s_test.c" name))
         (runs (format "o/$m/%s.com%s V=5 TESTARGS=-b" name runsuffix))
         (buns (format "o/$m/test/%s_test.com%s V=5 TESTARGS=-b" name runsuffix)))
    (cond ((not (member ext '("c" "cc" "cpp" "s" "S" "rl" "f")))
           (format "m=%s; make -j32 MODE=$m o/$m/%s"
                   mode
                   (directory-file-name
                    (or (file-name-directory
                         (file-relative-name this root))
                        ""))))
          ((eq kind 'run-win10)
           (format
            (cosmo-join
             " && "
             `("m=%s; f=o/$m/%s.com"
               ,(concat "make -j32 $f MODE=$m")
               "scp $f $f.dbg win10:; ssh win10 ./%s.com"))
            mode name (file-name-nondirectory name)))
          ((eq kind 'run-xnu)
           (format
            (cosmo-join
             " && "
             `("m=%s; f=o/$m/%s.com"
               ,(concat "make -j32 $f MODE=$m")
               "scp $f $f.dbg xnu:"
               "ssh xnu ./%s.com"))
            mode name (file-name-nondirectory name)))
          ((and (equal suffix "")
                (cosmo-contains "_test." (buffer-file-name)))
           (format "m=%s; make -j32 MODE=$m %s"
                   mode runs))
          ((and (equal suffix "")
                (file-exists-p (format "%s" buddy)))
           (format (cosmo-join
                    " && "
                    '("m=%s; n=%s; make -j32 o/$m/$n%s.o MODE=$m"
                      ;; "bloat o/$m/%s.o | head"
                      ;; "nm -C --size o/$m/%s.o | sort -r"
                      "echo"
                      "size -A o/$m/$n.o | grep '^[.T]' | grep -v 'debug\\|command.line\\|stack' | sort -rnk2"
                      "build/objdump %s -wzCd o/$m/$n%s.o | c++filt"))
                   mode name suffix objdumpflags suffix))
          ((eq kind 'run)
           (format
            (cosmo-join
             " && "
             `("m=%s; f=o/$m/%s.com"
               ,(concat "make -j32 $f MODE=$m")
               "build/run ./$f"))
            mode name))
          ((eq kind 'test)
           (format `"m=%s; f=o/$m/%s.com.ok && make -j32 $f MODE=$m" mode name))
          ((and (file-regular-p this)
                (file-executable-p this))
           (format "build/run ./%s" file))
          ('t
           (format
            (cosmo-join
             " && "
             `("m=%s; f=o/$m/%s%s.o"
               ,(concat "make -j32 $f MODE=$m")
               ;; "nm -C --size $f | sort -r"
               "echo"
               "size -A $f | grep '^[.T]' | grep -v 'debug\\|command.line\\|stack' | sort -rnk2"
               "build/objdump %s -wzCd $f | c++filt"))
            mode name suffix objdumpflags)))))

(defun cosmo-compile (arg)
  (interactive "P")
  (let* ((this (or (buffer-file-name) dired-directory))
         (root (locate-dominating-file this "Makefile")))
    (when root
      (let* ((mode (cosmo--make-mode arg))
             (suffix (cosmo--make-suffix arg))
             (objdumpflags (cosmo--make-objdump-flags arg))
             (compilation-scroll-output nil)
             (default-directory root)
             (compile-command (cosmo--compile-command this root nil mode suffix objdumpflags ".runs")))
        (compile compile-command)))))

(defun cosmo-compile-hook ()
  (local-set-key (kbd "C-c C-c") 'cosmo-compile))

(progn
  (add-hook 'makefile-mode-hook 'cosmo-compile-hook)
  (add-hook 'asm-mode-hook 'cosmo-compile-hook)
  (add-hook 'ld-script-mode-hook 'cosmo-compile-hook)
  (add-hook 'dired-mode-hook 'cosmo-compile-hook)
  (add-hook 'c-mode-common-hook 'cosmo-compile-hook)
  (add-hook 'c++-mode-common-hook 'cosmo-compile-hook)
  (add-hook 'fortran-mode-hook 'cosmo-compile-hook)
  (add-hook 'protobuf-mode-hook 'cosmo-compile-hook))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Display assembly for C/C++ buffer
;;
;;       C-c C-a   Compile and show assembly, with a native optimized build
;;       C-c C-b   Compile and show assembly, with a balanced build
;;
;; The ALT key may be used to override the default mode.
;;
;;   M-1 C-c C-a   Compile w/ MODE=tiny
;;   M-2 C-c C-a   Compile w/ MODE=opt
;;   M-3 C-c C-a   Compile w/ MODE=rel
;;   M-4 C-c C-a   Compile w/ MODE=dbg
;;   M-5 C-c C-a   Compile w/ MODE=""
;;

(defvar cosmo--assembly-root nil)
(defvar cosmo--assembly-pending nil)

(defun cosmo--scrub (regex replace)
  (replace-regexp regex replace nil (point-min) (point-max)))

(defun cosmo--fixup-asm ()
  (cosmo--scrub
   ;; Code like this
   ;;
   ;;   .section "GNU!GNU!GNU!.must.be.in.every.asm.or.mprotect.breaks"
   ;;
   ;; Isn't needed since the underlying issue is addressed by ape/ape.S
   ;; which generates executable structure.
   "	.section	.note.GNU-stack,.*,@progbits\n"
   "")
  (cosmo--scrub
   ;; Code like this
   ;;
   ;;   and $2047, %edx
   ;;
   ;; Should be this
   ;;
   ;;   and $2047,%edx
   ;;
   ;; Per our favored coding style.
   ", "
   ",")
  (cosmo--scrub
   ;; Code like this
   ;;
   ;;   .ascii "foo"
   ;;   .zero  1
   ;;
   ;; Should be this
   ;;
   ;;   .ascii "foo"
   ;;   .zero  1
   ;;
   ;; @see -frecord-gcc-switches
   "\t\\.ascii\t\"\\([^\"]*\\)\"\n\t\\.zero\t1\n"
   "\t.asciz\t\"\\1\"\n")
  (cosmo--scrub
   ;; Code like this
   ;;
   ;;   movl %eax,a
   ;;   movb b,%bl
   ;;   movb $1,c
   ;;   incb d
   ;;   movq %xmm0,%rax
   ;;
   ;; Should be this
   ;;
   ;;   mov  %eax,a
   ;;   mov  %bl,b
   ;;   movb $1,c
   ;;   incb d
   ;;   movq %xmm0,%rax  # ;_;
   ;;
   ;; Because we dislike redundancy and don't want the
   ;; ibmicroborlandtel crowd feeling too uncomfortable.
   (let ((names-a-register
          "%[A-Za-z]")
         (normal-arithmetic-ops
          (regexp-opt
           (list
            "rcr"  "rcl"  "ror"  "rol"  "hlt"  "cmc"  "div"  "sar"
            "mov"  "shl"  "shr"  "lea"  "cmp"  "adc"  "sbb"  "inc"
            "sub"  "xor"  "add"  "and"  "or"   "not"  "btc"  "dec"
            "mul"  "neg"  "bt"   "bsf"  "bsr"  "int"  "test")))
         (prefixes
          (regexp-opt
           (list
            "es"       "cs"       "ss"      "ds"      "lock"
            "rep"      "repnz"    "repne"   "repz"    "repe"
            "rex.b"    "rex.x"    "rex.xb"  "rex.r"   "rex.rb"   "rex.rx"
            "rex.rxb"  "rex.w"    "rex.wb"  "rex.wx"  "rex.wxb"  "rex.wr"
            "rex.trb"  "rex.wrx"  "rex.wrxb"))))
     (concat "\\("
             "^[ \t]*"
             "\\(?:" prefixes " \\|\\)"
             normal-arithmetic-ops
             "\\)"
             "[bwlq]"
             "\\("
             "[ \t]+"
             "\\(?:"
             "%[A-Wa-w][^\n]+"
             "\\|"
             "[^%][^\n,]*,%[A-Za-z][^\n]+"
             "\\)"
             "\\)"))
   "\\1\\2")
  ;; Scroll first function to top of display.
  (goto-char (point-min))
  (when (search-forward-regexp
         "\t\\.type[^\n]*@function" nil t)
    (recenter-top-bottom 'top)))

(defun cosmo--assembly-compilation-finished (compilebuf msg)
  (when cosmo--assembly-pending
    (let ((asm-gcc (car cosmo--assembly-pending))
          (asm-clang (cdr cosmo--assembly-pending))
          width
          asm-gcc-buffer
          asm-gcc-window
          asm-clang-buffer
          asm-clang-window)
      (setq cosmo--assembly-pending nil)
      (when (not (cosmo-contains "finished" msg))
        (error "making assembly failed: %s" msg))
      (let ((f (format "%s/%s" cosmo--assembly-root asm-gcc)))
        (when (or (null asm-gcc) (not (file-exists-p f)))
          (error "asm-gcc not found: %s" f)))
      (let ((f (format "%s/%s" cosmo--assembly-root asm-clang)))
        (when (or (null asm-gcc) (not (file-exists-p f)))
          (error "asm-gcc not found: %s" f)))
      (delete-other-windows)
      (setq width (window-total-width))
      (setq asm-gcc-buffer (find-file-noselect asm-gcc t nil nil))
      (setq asm-clang-buffer (find-file-noselect asm-clang t nil nil))
      (setq asm-clang-window (split-window-right (- width (/ width 4))))
      (setq asm-gcc-window (split-window-right (- (- width (/ width 4)) (/ width 4))))
      (set-window-buffer asm-gcc-window asm-gcc-buffer)
      (set-window-buffer asm-clang-window asm-clang-buffer)
      (other-window 1)
      (cosmo--fixup-asm)
      (save-buffer)
      (bury-buffer (current-buffer))
      (other-window 1)
      (cosmo--fixup-asm)
      (save-buffer)
      (bury-buffer (current-buffer))
      (other-window 1)
      (bury-buffer compilebuf))))

(defun cosmo--purge-file (path)
  (let ((b (find-file-noselect path t nil nil)))
    (when b
      (with-current-buffer b
        (save-buffer))
      (when (not (kill-buffer b))
        (error "couldn't kill asm buffer"))))
  (delete-file path t))

(defun cosmo--assembly (arg extra-make-flags)
  (let* ((this (expand-file-name (or (buffer-file-name) dired-directory)))
         (root (expand-file-name (locate-dominating-file this "Makefile"))))
    (when root
      (let* ((mode (cosmo--make-mode arg "opt"))
             (ext (file-name-extension this))
             (dir (file-name-directory this))
             (dots (file-relative-name root dir))
             (file (file-relative-name this root))
             (name (file-name-sans-extension file))
             (asm-gcc (format "o/%s/%s-gcc.asm" mode name))
             (asm-clang (format "o/%s/%s-clang.asm" mode name)))
        (when (not (member ext '("c" "cc" "f")))
          (error "don't know how to show assembly for non c/c++ source file"))
        (let* ((default-directory root)
               (compile-command
                (format "make %s -j32 MODE=%s %s %s"
                        (or extra-make-flags "") mode asm-gcc asm-clang)))
          (save-buffer)
          (set-visited-file-modtime (current-time))
          (cosmo--purge-file asm-gcc)
          (cosmo--purge-file asm-clang)
          (save-buffer)
          (setq cosmo--assembly-root root)
          (setq cosmo--assembly-pending (cons asm-gcc asm-clang))
          (let ((errormsg (shell-command-to-string (format "touch %s" file))))
            (when (not (equal "" errormsg))
              (error errormsg)))
          (revert-buffer :ignore-auto :noconfirm)
          (compile compile-command))))))

(defun cosmo-assembly (arg)
  (interactive "P")
  (setq arg (or arg 2))
  ;; -ffast-math -funsafe-math-optimizations -fsched2-use-superblocks -fjump-tables
  (cond ((eq arg 9)
         (cosmo--assembly 1
                          "V=1 OVERRIDE_COPTS='-w -fverbose-asm -fsanitize=undefined -fno-sanitize=null -fno-sanitize=alignment -fno-sanitize=pointer-overflow'"))
        ;; ((not (eq 0 (logand 8 arg)))
        ;;  (cosmo--assembly (setq arg (logand (lognot 8)))
        ;;                   "V=1 OVERRIDE_COPTS='-w -fverbose-asm -fsanitize=address'"))
        (t (cosmo--assembly arg "V=1 OVERRIDE_COPTS='-w ' CPPFLAGS=''"))))

(defun cosmo-assembly-native (arg)
  (interactive "P")
  (setq arg (or arg 3))
  (cond ((not (eq 0 (logand 8 arg)))
         (cosmo--assembly
          (setq arg (logand (lognot 8)))
          "V=1 CCFLAGS=--verbose COPTS='$(IEEE_MATH)' CPPFLAGS='' TARGET_ARCH='-march=k8'"))   ;; znver2
        (t
         (cosmo--assembly
          arg
          "V=1 CCFLAGS=--verbose COPTS='$(MATHEMATICAL) -O3' CPPFLAGS='' TARGET_ARCH='-march=k8'"))))  ;; znver2

(defun cosmo-assembly-icelake (arg)
  (interactive "P")
  (setq arg (or arg 3))
  (cond ((not (eq 0 (logand 8 arg)))
         (cosmo--assembly
          (setq arg (logand (lognot 8)))
          "V=1 CCFLAGS=--verbose COPTS='$(MATHEMATICAL) -O3' CPPFLAGS='' TARGET_ARCH='-march=icelake-client'"))
        (t
         (cosmo--assembly
          arg
          "V=1 CCFLAGS=--verbose COPTS='$(MATHEMATICAL) -O3' CPPFLAGS='' TARGET_ARCH='-march=icelake-client'"))))

(defun cosmo-assembly-balanced (arg)
  (interactive "P")
  (cosmo--assembly (or arg 5) "CFLAGS='-O2 -ftrapv' CPPFLAGS=''"))

(defun cosmo-mca (arg)
  (interactive "P")
  (let (code
        delete
        cleanup
        (inhibit-message t)
        (inhibit-read-only t)
        (term (getenv "TERM"))
        (prog (executable-find "llvm-mca-10"))
        (buf1 (get-buffer-create "*mca*"))
        (buf2 (generate-new-buffer "*mca*")))
    (setenv "TERM" "xterm-256color")
    (setq cleanup
          (lambda ()
            (setenv term)
            (kill-buffer buf2)
            (when delete (delete-file delete))))
    (condition-case exc
        (progn
          (if (not (buffer-modified-p))
              (setq code (buffer-file-name))
            (setq code (make-temp-file "mca.s"))
            (write-region nil nil code)
            (setq delete code))
          (with-current-buffer buf2
            (insert "\n")
            (setq rc (call-process prog nil t nil
                                   "-mcpu=skylake"
                                   "-mtriple=x86_64-pc-linux-gnu"
                                   "--bottleneck-analysis"
                                   "-instruction-info"
                                   "-iterations=8"
                                   "-all-stats"
                                   "-all-views"
                                   "-timeline"
                                   code)))
          (with-current-buffer buf1
            (replace-buffer-contents buf2)
            (if (eq rc 0)
                (fundamental-mode)
              (compilation-mode))
            (xterm-color-colorize-buffer)
            (display-buffer (current-buffer))))
      ('error
       (funcall cleanup)
       (error exc)))
    (funcall cleanup)))

(defun cosmo-assemble-hook ()
  (add-to-list 'compilation-finish-functions
               'cosmo--assembly-compilation-finished)
  (local-set-key (kbd "C-c C-a") 'cosmo-assembly)
  (local-set-key (kbd "C-c C-b") 'cosmo-assembly-balanced)
  (local-set-key (kbd "C-c C-n") 'cosmo-assembly-native)
  (local-set-key (kbd "C-c C-i") 'cosmo-assembly-icelake))

(defun cosmo-mca-hook ()
  ;; (local-set-key (kbd "C-c C-h") 'cosmo-mca)
  )

(progn
  (add-hook 'asm-mode-hook 'cosmo-mca-hook)
  (add-hook 'makefile-mode-hook 'cosmo-assemble-hook)
  (add-hook 'asm-mode-hook 'cosmo-assemble-hook)
  (add-hook 'ld-script-mode-hook 'cosmo-assemble-hook)
  (add-hook 'dired-mode-hook 'cosmo-assemble-hook)
  (add-hook 'c-mode-common-hook 'cosmo-assemble-hook)
  (add-hook 'fortran-mode-hook 'cosmo-assemble-hook)
  (add-hook 'protobuf-mode-hook 'cosmo-assemble-hook))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Run buffer.
;;       C-c C-r   Run
;;   M-1 C-c C-r   Run w/ MODE=tiny
;;   M-2 C-c C-r   Run w/ MODE=opt
;;   M-3 C-c C-r   Run w/ MODE=rel
;;   M-4 C-c C-r   Run w/ MODE=dbg
;;   M-5 C-c C-r   Run w/ MODE=""

(defun cosmo-run (arg)
  (interactive "P")
  (let* ((this (or (buffer-file-name) dired-directory))
         (proj (locate-dominating-file this "Makefile"))
         (root (or proj default-directory))
         (file (file-relative-name this root)))
    (when root
      (let ((default-directory root))
        (save-buffer)
        (cond ((save-excursion
                 (goto-char (point-min))
                 (looking-at "#!"))
               (compile (format "sh -c %s" file)))
              ((file-executable-p file)
               (compile (if (cosmo-contains "/" file)
                            file
                          (format "build/run ./%s" file))))
              ((memq major-mode '(c-mode c++-mode asm-mode fortran-mode))
               (let* ((mode (cosmo--make-mode arg))
                      (compile-command (cosmo--compile-command this root 'run mode "" "" ".runs")))
                 (compile compile-command)))
              ((eq major-mode 'sh-mode)
               (compile (format "sh -c %s" file)))
              ((eq major-mode 'lua-mode)
               (let* ((mode (cosmo--make-mode arg)))
                 (compile (format "make -j32 MODE=%s o/%s/tool/net/redbean.com && build/run o/%s/tool/net/redbean.com -i %s" mode mode mode file))))
              ((and (eq major-mode 'python-mode)
                    (cosmo-startswith "third_party/python/Lib/test/" file))
               (let ((mode (cosmo--make-mode arg)))
                 (compile (format "make -j32 MODE=%s PYHARNESSARGS=-vv PYTESTARGS=-v o/%s/%s.py.runs"
                                  mode mode (file-name-sans-extension file)))))
              ((eq major-mode 'python-mode)
               (compile (format "python.com %s" file)))
              ('t
               (error "cosmo-run: unknown major mode")))))))

(defun cosmo-run-test (arg)
  (interactive "P")
  (let* ((this (or (buffer-file-name) dired-directory))
         (proj (locate-dominating-file this "Makefile"))
         (root (or proj default-directory))
         (file (file-relative-name this root)))
    (when root
      (let ((default-directory root))
        (save-buffer)
        (cond ((memq major-mode '(c-mode c++-mode asm-mode fortran-mode))
               (let* ((mode (cosmo--make-mode arg cosmo-default-mode))
                      (compile-command (cosmo--compile-command this root 'test mode "" "" ".ok")))
                 (compile compile-command)))
              ('t
               (error "cosmo-run: unknown major mode")))))))

(defun cosmo-run-win10 (arg)
  (interactive "P")
  (let* ((this (or (buffer-file-name) dired-directory))
         (proj (locate-dominating-file this "Makefile"))
         (root (or proj default-directory))
         (file (file-relative-name this root)))
    (when root
      (let ((default-directory root))
        (save-buffer)
        (cond ((memq major-mode '(c-mode c++-mode asm-mode fortran-mode))
               (let* ((mode (cosmo--make-mode arg cosmo-default-mode))
                      (compile-command (cosmo--compile-command this root 'run-win10 mode "" "" "")))
                 (compile compile-command)))
              ('t
               (error "cosmo-run: unknown major mode")))))))

(progn
  (define-key asm-mode-map (kbd "C-c C-r") 'cosmo-run)
  (define-key c-mode-base-map (kbd "C-c C-r") 'cosmo-run)
  (define-key fortran-mode-map (kbd "C-c C-r") 'cosmo-run)
  (define-key sh-mode-map (kbd "C-c C-r") 'cosmo-run)
  (define-key lua-mode-map (kbd "C-c C-r") 'cosmo-run)
  (define-key python-mode-map (kbd "C-c C-r") 'cosmo-run)
  (define-key c-mode-map (kbd "C-c C-s") 'cosmo-run-test)
  (define-key c++-mode-map (kbd "C-c C-s") 'cosmo-run-test)
  (define-key c-mode-map (kbd "C-c C-_") 'cosmo-run-win7)
  (define-key c-mode-map (kbd "C-c C-_") 'cosmo-run-win10)
  (define-key c++-mode-map (kbd "C-c C-_") 'cosmo-run-win10))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Debug buffer.
;;       C-c C-d   Run in GDB/GUD
;;   M-1 C-c C-d   Run in GDB/GUD w/ MODE=tiny
;;   M-2 C-c C-d   Run in GDB/GUD w/ MODE=opt
;;   M-3 C-c C-d   Run in GDB/GUD w/ MODE=rel
;;   M-4 C-c C-d   Run in GDB/GUD w/ MODE=dbg
;;   M-5 C-c C-d   Run in GDB/GUD w/ MODE=""

(defun cosmo-debug (arg)
  (interactive "P")
  (let* ((this (or (buffer-file-name) dired-directory))
         (root (locate-dominating-file this "Makefile")))
    (when root
      (let* ((mode (cosmo--make-mode arg cosmo-dbg-mode))
             (name (file-relative-name this root))
             (next (file-name-sans-extension name))
             (exec (format "o/%s/%s.com.dbg" mode next))
             (default-directory root)
             (compile-command (cosmo--compile-command this root nil mode "" "" ".runs")))
        (compile compile-command)
        (gdb (format "gdb -q -nh -i=mi %s -ex run" exec))))))

(progn
  (define-key asm-mode-map (kbd "C-c C-d") 'cosmo-debug)
  (define-key c-mode-base-map (kbd "C-c C-d") 'cosmo-debug))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; C-c C-t  Toggle buffer between source file and unit test file.

(defun cosmo-toggle-buddy ()
  (interactive)
  (let* ((this (or (buffer-file-name) dired-directory))
         (root (locate-dominating-file this "Makefile")))
    (when root
      (let* ((name (file-relative-name this root))
             (dir (file-name-directory this))
             (pkgname (file-name-nondirectory (substring dir 0 -1)))
             (dots (file-relative-name root dir))
             (notest (cosmo-file-name-sans-extensions
                      (cosmo-replace "_test" "" (cosmo-lchop "test/" name))))
             (buddy
              (cond ((and (cosmo-startswith "test/" dir)
                          (cosmo-endswith "/test.mk" name))
                     (message (format "%s/%s.mk" (substring dir 5) pkgname))
                     (format "%s/%s.mk" (substring dir 5) pkgname))
                    ((cosmo-startswith "test/" name)
                     (cosmo-first-that
                      'file-exists-p
                      (list (concat dots notest ".s")
                            (concat dots notest ".S")
                            (concat dots notest ".f")
                            (concat dots notest ".F")
                            (concat dots notest ".c")
                            (concat dots notest ".cc")
                            (concat dots notest ".rl")
                            (concat dots notest ".cpp")
                            (concat dots notest ".greg.c")
                            (concat dots notest ".ncabi.c")
                            (concat dots notest ".hookabi.c")
                            (concat dots notest ".h"))))
                    (t
                     (format "%stest/%s_test.c"
                             dots (cosmo-file-name-sans-extensions name))))))
        (when buddy
          (find-file buddy))))))

(progn
  (global-set-key (kbd "C-c C-t") 'cosmo-toggle-buddy))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; C-c C-h  Add Include Line

(defun cosmo-add-include ()
  (interactive)
  (let* ((no-whine t)
         (tag-file "HTAGS")
         (this (buffer-name))
         (case-fold-search nil)
         (search (thing-at-point 'symbol))
         (buffer (find-file-noselect (format "%s/%s"
                                             (locate-dominating-file
                                              this tag-file)
                                             tag-file)
                                     no-whine))
         (header (with-current-buffer buffer
                   (revert-buffer :ignore-auto :noconfirm)
                   (save-excursion
                     (goto-char 0)
                     (when (re-search-forward
                            (concat "\177" search "\001") nil t)
                       (when (re-search-backward "\f\n\\([^,]*\\)," nil t)
                         (match-string 1))))))
         (root (locate-dominating-file this "Makefile"))
         (name (file-relative-name this root)))
    (when header
      (when (not (equal header name))
        (save-excursion
          (goto-char 0)
          (re-search-forward "#include" nil t)
          (re-search-forward "^$")
          (re-search-backward "#include" nil t)
          (beginning-of-line)
          (insert (concat "#include \"" header "\"\n"))))
      (message header))))

(progn
  (define-key prog-mode-map (kbd "C-c C-h") 'cosmo-add-include)
  (define-key asm-mode-map (kbd "C-c C-h") 'cosmo-add-include)
  (define-key c-mode-base-map (kbd "C-c C-h") 'cosmo-add-include)
  (define-key c++-mode-map (kbd "C-c C-h") 'cosmo-add-include))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; C-c C-o  Show Optimization Report

(defun cosmo-show-optinfo (arg)
  (interactive "P")
  (let* ((mode (cosmo--make-mode arg "opt"))
         (this (or (buffer-file-name) dired-directory))
         (root (locate-dominating-file this "Makefile")))
    (when root
      (let* ((name (file-relative-name this root))
             (buddy
              (format "%s/o/%s/%s.optinfo.gz"
                      root mode (cosmo-file-name-sans-extensions name))))
        (when buddy
          (find-file buddy))))))

(defun cosmo-lisp-is-the-best ()
  (define-key c-mode-base-map (kbd "C-c C-o") 'cosmo-show-optinfo))
(add-hook 'c-mode-common-hook 'cosmo-lisp-is-the-best)

(defun cosmo-lisp-is-the-best++ ()
  (define-key c++-mode-base-map (kbd "C-c C-o") 'cosmo-show-optinfo))
(add-hook 'c++-mode-common-hook 'cosmo-lisp-is-the-best++)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Cosmopolitan Extended Language Keyword Definitions

(defun cosmo-c-keywords-hook ()
  (font-lock-add-keywords
   nil `((,cosmo-c-keywords-regex . font-lock-keyword-face)
         (,cosmo-c-builtins-regex . font-lock-builtin-face)
         (,cosmo-platform-constants-regex . font-lock-builtin-face)
         (,cosmo-cpp-constants-regex . font-lock-constant-face)
         (,cosmo-c-constants-regex . font-lock-constant-face)
         (,cosmo-c-types-regex . font-lock-type-face))))

(defun cosmo-asm-keywords-hook ()
  (font-lock-add-keywords
   nil `((,cosmo-cpp-constants-regex . font-lock-constant-face)
         (,cosmo-platform-constants-regex . font-lock-constant-face))))

(add-hook 'c-mode-common-hook 'cosmo-c-keywords-hook)
(add-hook 'c++-mode-common-hook 'cosmo-c-keywords-hook)
(add-hook 'asm-mode-hook 'cosmo-asm-keywords-hook)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Symbol naming convention conversion

;; e.g. STARTF_USESHOWWINDOW -> kNtStartfUseshowwindow
(defun cosmo--ms-to-google-const (s)
  (declare (pure t) (side-effect-free t))
  (let ((d (downcase (substring s 1))))
    (cosmo-replace
     "kNtNt" "kNt"
     (concat
      "kNt"
      (substring s 0 1)
      (replace-regexp-in-string
       "_\\([A-Za-z]\\)"
       (lambda (m)
         (upcase (match-string 1 m)))
       d)))))

;; e.g. kNtStartfUseshowwindow -> STARTF_USESHOWWINDOW
(defun cosmo--google-to-ms-const (s)
  (declare (pure t) (side-effect-free t))
  (upcase (replace-regexp-in-string
           "\\(.\\)\\([A-Z]\\)"
           (lambda (m)
             (upcase (concat
                      (match-string 1 m) "_"
                      (match-string 2 m))))
           (substring s 3)
           t)))

(defun cosmo-toggle-ms-const ()
  (interactive)
  (let* ((case-fold-search nil)
         (bounds (if (use-region-p)
                        (cons (region-beginning) (region-end))
                      (bounds-of-thing-at-point 'symbol)))
         (text (buffer-substring-no-properties (car bounds) (cdr bounds))))
    (when bounds
      (let ((fn (if (or (cosmo-contains "_" text)
                        (equal text (upcase text)))
                    'cosmo--ms-to-google-const
                  'cosmo--google-to-ms-const)))
        (delete-region (car bounds) (cdr bounds))
        (insert (funcall fn text))))))

;; (define-key c-mode-base-map (kbd "C-c C-l") 'cosmo-toggle-ms-const)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CP/M EOF for the lulz

(defun cosmo-before-save ()
  (cond ((memq major-mode '(c-mode asm-mode))
         (set (make-local-variable 'require-final-newline)
              (not (equal (buffer-substring-no-properties
                           (- (point-max) 1) (point-max))
                          "\x1a"))))))

(add-hook 'before-save-hook 'cosmo-before-save)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Cosmopolitan Logger Integration

;; Cosmopolitan logger
;;
;; W2022-03-23T15:58:19.102930:tool/build/runit.c:274:runit:20719] hello
;; warn  date   time    micros file              line prog  pid   message
;;
;; I2022-03-23T15:58:19.+00033:tool/build/runit.c:274:runit:20719] there
;; info date   time     delta file              line prog  pid    message
;;

(defvar cosmo-compilation-regexps
  (list (cosmo-join
         ""
         '("^[FEWIVDNT]"                                ;; level
           "[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]" ;; date
           "T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]"          ;; time
           "[+.][0-9][0-9][0-9][0-9][0-9][0-9]"         ;; micros
           ":\\([^:]+\\)"                               ;; file
           ":\\([0-9]+\\)"))                            ;; line
        1 2))

(eval-after-load 'compile
  '(progn
     (add-to-list 'compilation-error-regexp-alist-alist
                  (cons 'cosmo cosmo-compilation-regexps))
     (add-to-list 'compilation-error-regexp-alist 'cosmo)))

(defvar cosmo-gcc123-compilation-regexps
  (list (cosmo-join
         ""
         '("inlined from '[^']*' at "
           "\\([^:]+\\)"                ;; file
           ":\\([0-9]+\\)"))            ;; line
        1 2))

(eval-after-load 'compile
  '(progn
     (add-to-list 'compilation-error-regexp-alist-alist
                  (cons 'cosmo cosmo-gcc123-compilation-regexps))
     (add-to-list 'compilation-error-regexp-alist 'cosmo)))

(provide 'cosmo-stuff)

;;; cosmo-stuff.el ends here
