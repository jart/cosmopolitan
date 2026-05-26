# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

include ../Makefile_params.common

HARNESS_ENTRY = harness
HARNESS_FILE = prepare_domain_separation_prefix_harness

PROOF_UID = mld_prepare_domain_separation_prefix

DEFINES +=
INCLUDES +=

REMOVE_FUNCTION_BODY +=
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROJECT_SOURCES += $(SRCDIR)/mldsa/src/sign.c

CHECK_FUNCTION_CONTRACTS=$(MLD_NAMESPACE)prepare_domain_separation_prefix
USE_FUNCTION_CONTRACTS=
APPLY_LOOP_CONTRACTS=on
USE_DYNAMIC_FRAMES=1

FUNCTION_NAME = prepare_domain_separation_prefix

EXTERNAL_SAT_SOLVER=
CBMCFLAGS=--smt2
CBMCFLAGS += --slice-formula

CBMC_OBJECT_BITS = 8

include ../Makefile.common
