# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

include ../Makefile_params.common

HARNESS_ENTRY = harness
HARNESS_FILE = attempt_signature_generation_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = mld_attempt_signature_generation

DEFINES +=
INCLUDES +=

REMOVE_FUNCTION_BODY +=
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROJECT_SOURCES += $(SRCDIR)/mldsa/src/sign.c

CHECK_FUNCTION_CONTRACTS=mld_attempt_signature_generation
USE_FUNCTION_CONTRACTS=$(MLD_NAMESPACE)polyvecl_uniform_gamma1 \
                       $(MLD_NAMESPACE)polyvecl_ntt \
                       $(MLD_NAMESPACE)polyvec_matrix_pointwise_montgomery \
                       $(MLD_NAMESPACE)polyveck_reduce \
                       $(MLD_NAMESPACE)polyveck_invntt_tomont \
                       $(MLD_NAMESPACE)polyveck_caddq \
                       $(MLD_NAMESPACE)polyveck_decompose \
                       $(MLD_NAMESPACE)polyveck_pack_w1 \
                       mld_H \
                       $(MLD_NAMESPACE)poly_challenge \
                       $(MLD_NAMESPACE)poly_ntt \
                       $(MLD_NAMESPACE)polyveck_pointwise_poly_montgomery \
                       $(MLD_NAMESPACE)polyveck_sub \
                       $(MLD_NAMESPACE)polyveck_reduce \
                       $(MLD_NAMESPACE)polyveck_chknorm \
                       $(MLD_NAMESPACE)polyveck_add \
                       $(MLD_NAMESPACE)polyveck_make_hint \
                       $(MLD_NAMESPACE)pack_sig_c_h \
                       mld_compute_pack_z
USE_FUNCTION_CONTRACTS+=mld_zeroize

APPLY_LOOP_CONTRACTS=on
USE_DYNAMIC_FRAMES=1

# Disable any setting of EXTERNAL_SAT_SOLVER, and choose SMT backend instead
EXTERNAL_SAT_SOLVER=
CBMCFLAGS=--external-smt2-solver $(PROOF_ROOT)/lib/z3_no_bv_extract --z3
CBMCFLAGS += --slice-formula
CBMCFLAGS += --no-array-field-sensitivity

FUNCTION_NAME = mld_attempt_signature_generation

# If this proof is found to consume huge amounts of RAM, you can set the
# EXPENSIVE variable. With new enough versions of the proof tools, this will
# restrict the number of EXPENSIVE CBMC jobs running at once. See the
# documentation in Makefile.common under the "Job Pools" heading for details.
# EXPENSIVE = true

# This function is large enough to need...
CBMC_OBJECT_BITS = 12

# If you require access to a file-local ("static") function or object to conduct
# your proof, set the following (and do not include the original source file
# ("mldsa/poly.c") in PROJECT_SOURCES).
# REWRITTEN_SOURCES = $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i
# include ../Makefile.common
# $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i_SOURCE = $(SRCDIR)/mldsa/src/poly.c
# $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i_FUNCTIONS = foo bar
# $(PROOFDIR)/<__SOURCE_FILE_BASENAME__>.i_OBJECTS = baz
# Care is required with variables on the left-hand side: REWRITTEN_SOURCES must
# be set before including Makefile.common, but any use of variables on the
# left-hand side requires those variables to be defined. Hence, _SOURCE,
# _FUNCTIONS, _OBJECTS is set after including Makefile.common.

include ../Makefile.common
