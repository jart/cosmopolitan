(require 'compile)

(define-derived-mode optinfo-mode compilation-mode "Optimization Info"
  (let ((root (locate-dominating-file (buffer-file-name) "Makefile")))
    (when root
      (setq-local default-directory root))))

(auto-compression-mode t)
(add-to-list 'auto-mode-alist '("\\.optinfo\\(\\|\\.gz\\)$" . optinfo-mode))

(provide 'optinfo-mode)
