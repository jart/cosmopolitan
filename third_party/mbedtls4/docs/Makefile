# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help clean apidoc breathe_apidoc Makefile

# Intercept the 'clean' target so we can do the right thing for apidoc as well
clean:
	@# Clean the apidoc
	$(MAKE) -C .. apidoc_clean
	@# Clean the breathe-apidoc generated files
	rm -rf ./api
	@# Clean the sphinx docs
	@$(SPHINXBUILD) -M clean "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

apidoc:
	@# Generate doxygen from source using the main Makefile
	$(MAKE) -C .. apidoc

breathe_apidoc: apidoc
	@# Remove existing files - breathe-apidoc skips them if they're present
	rm -rf ./api
	@# Generate RST file structure with breathe-apidoc
	breathe-apidoc -o ./api ../apidoc/xml

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile breathe_apidoc
	@# Build the relevant target with sphinx
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
