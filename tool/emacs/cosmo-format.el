;;; cosmo-format.el --- Cosmopolitan Clang-Format Integration

;; Author: Justine Tunney <jtunney@gmail.com>
;; Version: 0.1.0
;; License: Public Domain
;; Keywords: c c++ clang

;; To the extent possible under law, Justine Tunney has waived all
;; copyright and related or neighboring rights to this file, as it is
;; written in the following disclaimers: <http://unlicense.org/> and
;; <http://creativecommons.org/publicdomain/zero/1.0/>

;;; Commentary:
;;
;; This module automates indentation, whitespace, and other stylistic
;; concerns while editing C/C++ source files. The clang-format program,
;; if present on the system, is run each time a buffer is saved.

;;; Installation:
;;
;; Put the following in your .emacs.d/init.el file:
;;
;;     (require 'cosmo-format)
;;
;; Put this file in the root of your project:
;;
;;     printf '---\nBasedOnStyle: Google\n...\n' >.clang-format
;;
;; Any buffer whose pathname matches `cosmo-format-path-regex' will
;; be formatted automatically on save if:
;;
;;   1. It's able to find the clang-format program, or
;;      `cosmo-format-bin' is customized.
;;
;;   2. There's a .clang-format file up the directory tree, or
;;      `cosmo-format-arg' is customized; in which case, it is
;;      recommended that it be customized buffer locally.
;;
;; For all other cases, there are no latency penalties (i.e. superfluous
;; i/o syscalls) or risks to leaving this enabled globally.

;;; Code:

(defcustom cosmo-format-bin nil
  "Explicit command or pathname of clang-format program."
  :type 'string
  :group 'cosmo-format)

(defcustom cosmo-format-arg nil
  "Explicit argument to clang-format program."
  :type 'string
  :group 'cosmo-format)

(defcustom cosmo-format-modes '(c-mode
                                c++-mode
                                java-mode
                                protobuf-mode)
  "List of major-modes that need clang-format."
  :type '(repeat symbol)
  :group 'cosmo-format)

(defcustom cosmo-format-exts '("c" "cc" "h" "inc"  ;; c/c++
                               "hh" "cpp" "hpp"    ;; ms c/c++
                               "rl"                ;; ragel
                               "proto")            ;; protobuf
  "List of pathname extensions that need clang-format."
  :type '(repeat string)
  :group 'cosmo-format)

(defcustom cosmo-format-blacklist '("quickjs.c" "ggml.c")
  "List of files to ignore, matched by basename."
  :type '(repeat string)
  :group 'cosmo-format)

(defvar cosmo--clang-format-bin)

(defmacro cosmo-memoize (var mvar form)
  "Return VAR or evaluate FORM memoized locally to MVAR."
  `(cond (,var ,var)
         ((fboundp (quote ,mvar))
          (cond ((eq ,mvar 'null) nil)
                (t ,mvar)))
         (t (let ((res ,form))
              (setq-local ,mvar (or res 'null))
              res))))

(defun cosmo--find-clang-format-bin ()
  (cosmo-memoize cosmo-format-bin
                 cosmo--clang-format-bin
                 (or (executable-find "clang-format-10")
                     (executable-find "clang-format-9")
                     (executable-find "clang-format-8")
                     (executable-find "clang-format-7")
                     (executable-find "clang-format"))))

(defun cosmo-format ()
  "Beautifies source code in current buffer."
  (interactive)
  (when (and (memq major-mode cosmo-format-modes)
             (member (file-name-extension (buffer-file-name))
                     cosmo-format-exts)
             (not (member (file-name-nondirectory (buffer-name))
                          cosmo-format-blacklist))
             (not (save-excursion
                    (beginning-of-buffer)
                    (looking-at "/\\* clang-format off \\*/"))))
    (let* ((bin (cosmo--find-clang-format-bin))
           (this (buffer-file-name))
           (root (locate-dominating-file this ".clang-format")))
      (when (and bin root)
        (let ((p (point))
              (tmp (make-temp-file "cosmo-format"))
              (arg1 (concat "--assume-filename="
                            (file-relative-name this root)))
              (arg2 (or cosmo-format-arg
                        "-style=file")))
          (when arg2
            (write-region nil nil tmp)
            (let ((buf (get-buffer-create "*clang-format*"))
                  (exe (cosmo--find-clang-format-bin)))
              (with-current-buffer buf
                (call-process exe tmp t nil arg1 arg2))
              (replace-buffer-contents buf)
              (kill-buffer buf)
              (delete-file tmp nil))))))))

;; Emacs 26.3+ needed for replace-buffer-contents; so worth it!!
(unless (version-list-< (version-to-list emacs-version) '(26 3))
  (add-hook 'before-save-hook 'cosmo-format))

(provide 'cosmo-format)

;;; cosmo-format.el ends here
