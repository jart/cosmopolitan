# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

.PHONY: func kat acvp stack unit alloc rng_fail \
	func_44 kat_44 acvp_44 stack_44 unit_44 alloc_44 rng_fail_44 \
	func_65 kat_65 acvp_65 stack_65 unit_65 alloc_65 rng_fail_65 \
	func_87 kat_87 acvp_87 stack_87 unit_87 alloc_87 rng_fail_87 \
	run_func run_kat run_acvp run_stack run_unit run_alloc run_rng_fail \
	run_func_44 run_kat_44 run_stack_44 run_unit_44 run_alloc_44 run_rng_fail_44 \
	run_func_65 run_kat_65 run_stack_65 run_unit_65 run_alloc_65 run_rng_fail_65 \
	run_func_87 run_kat_87 run_stack_87 run_unit_87 run_alloc_87 run_rng_fail_87 \
	bench_44 bench_65 bench_87 bench \
	run_bench_44 run_bench_65 run_bench_87 run_bench \
	bench_components_44 bench_components_65 bench_components_87 bench_components \
	run_bench_components_44 run_bench_components_65 run_bench_components_87 run_bench_components \
	build test all \
	clean quickcheck check-defined-CYCLES \
	size_44 size_65 size_87 size \
	run_size_44 run_size_65 run_size_87 run_size \
	host_info

SHELL := /usr/bin/env bash
.DEFAULT_GOAL := build

all: build

# Extra Makefile to include, e.g., for baremetal targets
ifneq ($(EXTRA_MAKEFILE),)
include $(EXTRA_MAKEFILE)
endif

W := $(EXEC_WRAPPER)

BUILD_DIR ?= test/build

# Skip includes for clean target
ifneq ($(MAKECMDGOALS),clean)
include test/mk/config.mk
include test/mk/auto.mk
include test/mk/components.mk
include test/mk/rules.mk
endif

quickcheck: test

build: func kat acvp
	$(Q)echo "  Everything builds fine!"

test: run_kat run_func run_acvp run_unit run_alloc run_rng_fail
	$(Q)echo "  Everything checks fine!"

# Detect available SHA256 command
SHA256SUM := $(shell command -v shasum >/dev/null 2>&1 && echo "shasum -a 256" || (command -v sha256sum >/dev/null 2>&1 && echo "sha256sum" || echo ""))
ifeq ($(SHA256SUM),)
$(error Neither 'shasum' nor 'sha256sum' found. Please install one of these tools.)
endif

run_kat_44: kat_44
	set -o pipefail; $(W) $(MLDSA44_DIR)/bin/gen_KAT44 | $(SHA256SUM) | cut -d " " -f 1 | xargs ./META.sh ML-DSA-44 kat-sha256
run_kat_65: kat_65
	set -o pipefail; $(W) $(MLDSA65_DIR)/bin/gen_KAT65 | $(SHA256SUM) | cut -d " " -f 1 | xargs ./META.sh ML-DSA-65 kat-sha256
run_kat_87: kat_87
	set -o pipefail; $(W) $(MLDSA87_DIR)/bin/gen_KAT87 | $(SHA256SUM) | cut -d " " -f 1 | xargs ./META.sh ML-DSA-87 kat-sha256
run_kat: run_kat_44 run_kat_65 run_kat_87

run_func_44: func_44
	$(W) $(MLDSA44_DIR)/bin/test_mldsa44
run_func_65: func_65
	$(W) $(MLDSA65_DIR)/bin/test_mldsa65
run_func_87: func_87
	$(W) $(MLDSA87_DIR)/bin/test_mldsa87
run_func: run_func_44 run_func_65 run_func_87

run_unit_44: unit_44
	$(W) $(MLDSA44_DIR)/bin/test_unit44
run_unit_65: unit_65
	$(W) $(MLDSA65_DIR)/bin/test_unit65
run_unit_87: unit_87
	$(W) $(MLDSA87_DIR)/bin/test_unit87
run_unit: run_unit_44 run_unit_65 run_unit_87

run_acvp: acvp
	EXEC_WRAPPER="$(EXEC_WRAPPER)" python3 ./test/acvp/acvp_client.py $(if $(ACVP_VERSION),--version $(ACVP_VERSION))

func_44: $(MLDSA44_DIR)/bin/test_mldsa44
	$(Q)echo "  FUNC       ML-DSA-44:   $^"
func_65: $(MLDSA65_DIR)/bin/test_mldsa65
	$(Q)echo "  FUNC       ML-DSA-65:   $^"
func_87: $(MLDSA87_DIR)/bin/test_mldsa87
	$(Q)echo "  FUNC       ML-DSA-87:  $^"
func: func_44 func_65 func_87

unit_44:  $(MLDSA44_DIR)/bin/test_unit44
	$(Q)echo "  UNIT       ML-DSA-44:   $^"
unit_65:  $(MLDSA65_DIR)/bin/test_unit65
	$(Q)echo "  UNIT       ML-DSA-65:   $^"
unit_87: $(MLDSA87_DIR)/bin/test_unit87
	$(Q)echo "  UNIT       ML-DSA-87:  $^"
unit: unit_44 unit_65 unit_87

kat_44: $(MLDSA44_DIR)/bin/gen_KAT44
	$(Q)echo "  KAT        ML-DSA-44:   $^"
kat_65: $(MLDSA65_DIR)/bin/gen_KAT65
	$(Q)echo "  KAT        ML-DSA-65:   $^"
kat_87: $(MLDSA87_DIR)/bin/gen_KAT87
	$(Q)echo "  KAT        ML-DSA-87:  $^"
kat: kat_44 kat_65 kat_87

acvp_44:  $(MLDSA44_DIR)/bin/acvp_mldsa44
	$(Q)echo "  ACVP       ML-DSA-44:   $^"
acvp_65:  $(MLDSA65_DIR)/bin/acvp_mldsa65
	$(Q)echo "  ACVP       ML-DSA-65:   $^"
acvp_87: $(MLDSA87_DIR)/bin/acvp_mldsa87
	$(Q)echo "  ACVP       ML-DSA-87:  $^"
acvp: acvp_44 acvp_65 acvp_87

ifeq ($(HOST_PLATFORM),Linux-aarch64)
# valgrind does not work with the AArch64 SHA3 extension
# Use armv8-a as the target architecture, overwriting a
# potential earlier addition of armv8.4-a+sha3.
$(MLDSA44_DIR)/bin/test_stack44:   CFLAGS += -march=armv8-a
$(MLDSA65_DIR)/bin/test_stack65:   CFLAGS += -march=armv8-a
$(MLDSA87_DIR)/bin/test_stack87: CFLAGS += -march=armv8-a
endif

stack_44: $(MLDSA44_DIR)/bin/test_stack44
	$(Q)echo "  STACK      ML-DSA-44:   $^"
stack_65: $(MLDSA65_DIR)/bin/test_stack65
	$(Q)echo "  STACK      ML-DSA-65:   $^"
stack_87: $(MLDSA87_DIR)/bin/test_stack87
	$(Q)echo "  STACK      ML-DSA-87:  $^"
stack: stack_44 stack_65 stack_87

run_stack_44: stack_44
	$(Q)python3 scripts/stack $(MLDSA44_DIR)/bin/test_stack44 --build-dir $(MLDSA44_DIR) $(STACK_ANALYSIS_FLAGS)
run_stack_65: stack_65
	$(Q)python3 scripts/stack $(MLDSA65_DIR)/bin/test_stack65 --build-dir $(MLDSA65_DIR) $(STACK_ANALYSIS_FLAGS)
run_stack_87: stack_87
	$(Q)python3 scripts/stack $(MLDSA87_DIR)/bin/test_stack87 --build-dir $(MLDSA87_DIR) $(STACK_ANALYSIS_FLAGS)
run_stack: run_stack_44 run_stack_65 run_stack_87

alloc_44: $(MLDSA44_DIR)/bin/test_alloc44
	$(Q)echo "  ALLOC   ML-DSA-44:   $^"
alloc_65: $(MLDSA65_DIR)/bin/test_alloc65
	$(Q)echo "  ALLOC   ML-DSA-65:   $^"
alloc_87: $(MLDSA87_DIR)/bin/test_alloc87
	$(Q)echo "  ALLOC   ML-DSA-87:  $^"
alloc: alloc_44 alloc_65 alloc_87

run_alloc_44: alloc_44
	$(W) $(MLDSA44_DIR)/bin/test_alloc44
run_alloc_65: alloc_65
	$(W) $(MLDSA65_DIR)/bin/test_alloc65
run_alloc_87: alloc_87
	$(W) $(MLDSA87_DIR)/bin/test_alloc87
run_alloc: run_alloc_44 run_alloc_65 run_alloc_87

rng_fail_44: $(MLDSA44_DIR)/bin/test_rng_fail44
	$(Q)echo "  RNG_FAIL   ML-DSA-44:   $^"
rng_fail_65: $(MLDSA65_DIR)/bin/test_rng_fail65
	$(Q)echo "  RNG_FAIL   ML-DSA-65:   $^"
rng_fail_87: $(MLDSA87_DIR)/bin/test_rng_fail87
	$(Q)echo "  RNG_FAIL   ML-DSA-87:  $^"
rng_fail: rng_fail_44 rng_fail_65 rng_fail_87

run_rng_fail_44: rng_fail_44
	$(W) $(MLDSA44_DIR)/bin/test_rng_fail44
run_rng_fail_65: rng_fail_65
	$(W) $(MLDSA65_DIR)/bin/test_rng_fail65
run_rng_fail_87: rng_fail_87
	$(W) $(MLDSA87_DIR)/bin/test_rng_fail87
run_rng_fail: run_rng_fail_44 run_rng_fail_65 run_rng_fail_87

lib: $(BUILD_DIR)/libmldsa.a $(BUILD_DIR)/libmldsa44.a $(BUILD_DIR)/libmldsa65.a $(BUILD_DIR)/libmldsa87.a

# Enforce setting CYCLES make variable when
# building benchmarking binaries
check_defined = $(if $(value $1),, $(error $2))
check-defined-CYCLES:
	@:$(call check_defined,CYCLES,CYCLES undefined. Benchmarking requires setting one of NO PMU PERF MAC)

bench_44: check-defined-CYCLES \
	$(MLDSA44_DIR)/bin/bench_mldsa44
bench_65: check-defined-CYCLES \
	$(MLDSA65_DIR)/bin/bench_mldsa65
bench_87: check-defined-CYCLES \
	$(MLDSA87_DIR)/bin/bench_mldsa87
bench: bench_44 bench_65 bench_87

run_bench_44: bench_44
	$(W) $(MLDSA44_DIR)/bin/bench_mldsa44
run_bench_65: bench_65
	$(W) $(MLDSA65_DIR)/bin/bench_mldsa65
run_bench_87: bench_87
	$(W) $(MLDSA87_DIR)/bin/bench_mldsa87
run_bench: bench
	$(W) $(MLDSA44_DIR)/bin/bench_mldsa44
	$(W) $(MLDSA65_DIR)/bin/bench_mldsa65
	$(W) $(MLDSA87_DIR)/bin/bench_mldsa87

bench_components_44: check-defined-CYCLES \
	$(MLDSA44_DIR)/bin/bench_components_mldsa44
bench_components_65: check-defined-CYCLES \
	$(MLDSA65_DIR)/bin/bench_components_mldsa65
bench_components_87: check-defined-CYCLES \
	$(MLDSA87_DIR)/bin/bench_components_mldsa87
bench_components: bench_components_44 bench_components_65 bench_components_87

run_bench_components_44: bench_components_44
	$(W) $(MLDSA44_DIR)/bin/bench_components_mldsa44
run_bench_components_65: bench_components_65
	$(W) $(MLDSA65_DIR)/bin/bench_components_mldsa65
run_bench_components_87: bench_components_87
	$(W) $(MLDSA87_DIR)/bin/bench_components_mldsa87
run_bench_components: bench_components
	$(W) $(MLDSA44_DIR)/bin/bench_components_mldsa44
	$(W) $(MLDSA65_DIR)/bin/bench_components_mldsa65
	$(W) $(MLDSA87_DIR)/bin/bench_components_mldsa87


size_44: $(BUILD_DIR)/libmldsa44.a
size_65: $(BUILD_DIR)/libmldsa65.a
size_87: $(BUILD_DIR)/libmldsa87.a
size: size_44 size_65 size_87

run_size_44: size_44
	$(Q)echo "size $(BUILD_DIR)/libmldsa44.a"
	$(Q)$(SIZE) $(BUILD_DIR)/libmldsa44.a | (read header; echo "$$header"; awk '$$5 != 0' | sort -k5 -n -r)

run_size_65: size_65
	$(Q)echo "size $(BUILD_DIR)/libmldsa65.a"
	$(Q)$(SIZE) $(BUILD_DIR)/libmldsa65.a | (read header; echo "$$header"; awk '$$5 != 0' | sort -k5 -n -r)

run_size_87: size_87
	$(Q)echo "size $(BUILD_DIR)/libmldsa87.a"
	$(Q)$(SIZE) $(BUILD_DIR)/libmldsa87.a | (read header; echo "$$header"; awk '$$5 != 0' | sort -k5 -n -r)


run_size: \
	run_size_44 \
	run_size_65 \
	run_size_87

# Display host and compiler feature detection information
# Shows which architectural features are supported by both the compiler and host CPU
# Usage: make host_info [AUTO=0|1] [CROSS_PREFIX=...]
host_info:
	@echo "=== Host and Compiler Feature Detection ==="
	@echo "Host Platform: $(HOST_PLATFORM)"
	@echo "Target Architecture: $(ARCH)"
	@echo "Compiler: $(CC)"
	@echo "Cross Prefix: $(if $(CROSS_PREFIX),$(CROSS_PREFIX),<none>)"
	@echo "AUTO: $(AUTO)"
	@echo ""
ifeq ($(ARCH),x86_64)
	@echo "=== x86_64 Feature Support ==="
	@echo "AVX2: Host $(if $(filter 1,$(MK_HOST_SUPPORTS_AVX2)),✅,❌) Compiler $(if $(filter 1,$(MK_COMPILER_SUPPORTS_AVX2)),✅,❌)"
	@echo "SSE2: Host $(if $(filter 1,$(MK_HOST_SUPPORTS_SSE2)),✅,❌) Compiler $(if $(filter 1,$(MK_COMPILER_SUPPORTS_SSE2)),✅,❌)"
	@echo "BMI2: Host $(if $(filter 1,$(MK_HOST_SUPPORTS_BMI2)),✅,❌) Compiler $(if $(filter 1,$(MK_COMPILER_SUPPORTS_BMI2)),✅,❌)"
else ifeq ($(ARCH),aarch64)
	@echo "=== AArch64 Feature Support ==="
	@echo "SHA3: Host $(if $(filter 1,$(MK_HOST_SUPPORTS_SHA3)),✅,❌) Compiler $(if $(filter 1,$(MK_COMPILER_SUPPORTS_SHA3)),✅,❌)"
else
	@echo "=== Architecture Not Supported ==="
	@echo "No specific feature detection available for $(ARCH)"
endif

EXAMPLE_DIRS := \
	examples/bring_your_own_fips202 \
	examples/bring_your_own_fips202_static \
	examples/custom_backend \
	examples/basic \
	examples/basic_deterministic \
	examples/basic_lowram \
	examples/monolithic_build \
	examples/monolithic_build_native \
	examples/monolithic_build_multilevel \
	examples/monolithic_build_multilevel_native \
	examples/multilevel_build \
	examples/multilevel_build_native

EXAMPLE_CLEAN_TARGETS := $(EXAMPLE_DIRS:%=clean-%)

.PHONY: $(EXAMPLE_CLEAN_TARGETS)

$(EXAMPLE_CLEAN_TARGETS): clean-%:
	@echo "  CLEAN   $*"
	-@$(MAKE) clean -C $* >/dev/null

clean: $(EXAMPLE_CLEAN_TARGETS)
	@echo "  RM      $(BUILD_DIR)"
	-@$(RM) -rf $(BUILD_DIR)
