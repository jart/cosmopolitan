;;; javadown.el --- C/C++ Markdown Javadocs

;; Copyright 2019 Justine Tunney

;; Author: Justine Tunney
;; Version: 0.1

;; Permission to use, copy, modify, and/or distribute this software for any
;; purpose with or without fee is hereby granted, provided that the above
;; copyright notice and this permission notice appear in all copies.
;;
;; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
;; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
;; MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
;; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
;; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
;; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

;;; Commentary:
;;
;; Unintentionally empty.

;;; Code:

(require 'cc-fonts)

(defconst javadown-font-lock-doc-comments
  `(;; e.g. ### Implementation Details
    ("^\\(/\\*\\)?\\(\\s \\|\\*\\)*\\(#+[^\n\r]+\\)"
     3 ,c-doc-markup-face-name prepend nil)
    ;; e.g. {@code param}
    ("{@[a-z]+[^}\n\r]*}"
     0 ,c-doc-markup-face-name prepend nil)
    ;; e.g. @param name does stuff
    ("^\\(/\\*\\)?\\(\\s \\|\\*\\)*\\(@[a-z]+\\)"
     3 ,c-doc-markup-face-name prepend nil)
    ;; e.g. <video src=doge.mp4>
    (,(concat "</?\\sw"
              "\\("
              (concat "\\sw\\|\\s \\|[=\n\r*.:]\\|"
                      "\"[^\"]*\"\\|'[^']*'")
              "\\)*>")
     0 ,c-doc-markup-face-name prepend nil)))

(defconst javadown-font-lock-keywords
  `((,(lambda (limit)
	(c-font-lock-doc-comments "/\\*\\*" limit
	  javadown-font-lock-doc-comments)))))

(provide 'javadown)

;;; javadown.el ends here
