# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

include ../Makefile_params.common

HARNESS_ENTRY = harness
HARNESS_FILE = sign_verify_internal_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = sign_verify_internal

DEFINES +=
INCLUDES +=

REMOVE_FUNCTION_BODY +=
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROJECT_SOURCES += $(SRCDIR)/mldsa/src/sign.c

CHECK_FUNCTION_CONTRACTS=$(MLD_NAMESPACE)verify_internal
USE_FUNCTION_CONTRACTS=$(MLD_NAMESPACE)unpack_pk
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)unpack_sig
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyvecl_chknorm
USE_FUNCTION_CONTRACTS+=mld_H
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)poly_challenge
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyvec_matrix_expand
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyvecl_ntt
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyvec_matrix_pointwise_montgomery
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)poly_ntt
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_shiftl
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_ntt
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_pointwise_poly_montgomery
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_sub
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_reduce
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_invntt_tomont
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_caddq
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_use_hint
USE_FUNCTION_CONTRACTS+=$(MLD_NAMESPACE)polyveck_pack_w1
USE_FUNCTION_CONTRACTS+=mld_zeroize
USE_FUNCTION_CONTRACTS+=mld_ct_memcmp

APPLY_LOOP_CONTRACTS=on
USE_DYNAMIC_FRAMES=1

# Disable any setting of EXTERNAL_SAT_SOLVER, and choose SMT backend instead
EXTERNAL_SAT_SOLVER=
CBMCFLAGS=--smt2
CBMCFLAGS += --slice-formula

FUNCTION_NAME = sign_verify_internal

# If this proof is found to consume huge amounts of RAM, you can set the
# EXPENSIVE variable. With new enough versions of the proof tools, this will
# restrict the number of EXPENSIVE CBMC jobs running at once. See the
# documentation in Makefile.common under the "Job Pools" heading for details.
# EXPENSIVE = true

# This function is large enough to need...
CBMC_OBJECT_BITS = 10

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
