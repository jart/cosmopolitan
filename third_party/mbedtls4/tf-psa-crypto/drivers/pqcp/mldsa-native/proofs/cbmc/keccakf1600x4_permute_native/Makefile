# Copyright (c) The mldsa-native project authors
# Copyright (c) The mlkem-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

include ../Makefile_params.common

HARNESS_ENTRY = harness
HARNESS_FILE = keccakf1600x4_permute_native_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = keccakf1600x4_permute_native

DEFINES += -DMLD_CONFIG_USE_NATIVE_BACKEND_FIPS202 -DMLD_CONFIG_FIPS202_BACKEND_FILE="\"dummy_backend_fips202_x4.h\""
INCLUDES +=

REMOVE_FUNCTION_BODY +=
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROJECT_SOURCES += $(SRCDIR)/mldsa/src/fips202/keccakf1600.c

CHECK_FUNCTION_CONTRACTS=$(MLD_NAMESPACE)keccakf1600x4_permute
USE_FUNCTION_CONTRACTS=mld_keccak_f1600_x4_native
APPLY_LOOP_CONTRACTS=on
USE_DYNAMIC_FRAMES=1

# Disable any setting of EXTERNAL_SAT_SOLVER, and choose SMT backend instead
EXTERNAL_SAT_SOLVER=
CBMCFLAGS=--smt2

# For this proof we tell CBMC to
#  - not decompose arrays into their individual cells
#  - to slice constraints that are not in the cone of influence of the proof obligations
# These options simplify them modelling of arrays and produce much more compact
# SMT files, leaving all array-type reasoning to the SMT solver.
#
# For functions that use large and multi-dimensional arrays, this yields
# a substantial improvement in proof performance.
CBMCFLAGS += --no-array-field-sensitivity
CBMCFLAGS += --slice-formula

FUNCTION_NAME = mld_keccakf1600x4_permute_native

# If this proof is found to consume huge amounts of RAM, you can set the
# EXPENSIVE variable. With new enough versions of the proof tools, this will
# restrict the number of EXPENSIVE CBMC jobs running at once. See the
# documentation in Makefile.common under the "Job Pools" heading for details.
# EXPENSIVE = true

# This function is large enough to need...
CBMC_OBJECT_BITS = 8

# If you require access to a file-local ("static") function or object to conduct
# your proof, set the following (and do not include the original source file
# ("mlkem/poly.c") in PROJECT_SOURCES).
# REWRITTEN_SOURCES = $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i
# include ../Makefile.common
# $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i_SOURCE = $(SRCDIR)/mlkem/poly.c
# $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i_FUNCTIONS = foo bar
# $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i_OBJECTS = baz
# Care is required with variables on the left-hand side: REWRITTEN_SOURCES must
# be set before including Makefile.common, but any use of variables on the
# left-hand side requires those variables to be defined. Hence, _SOURCE,
# _FUNCTIONS, _OBJECTS is set after including Makefile.common.

include ../Makefile.common
