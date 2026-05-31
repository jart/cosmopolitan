# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

######
# To run, see the README.md file
######
.PHONY: all clean

# ISA to optimize for
TARGET_ISA=Arm_AArch64

# MicroArch target to optimize for
# Changing this to Arm_Cortex_A55 results in significantly better performance
# on the Cortex-A55, but may result in worse performance on other CPUs.
TARGET_MICROARCH=Arm_Neoverse_N1_experimental

SLOTHY_EXTRA_FLAGS ?=

SLOTHY_FLAGS=-c sw_pipelining.enabled=true \
             -c inputs_are_outputs \
             -c sw_pipelining.minimize_overlapping=False \
             -c sw_pipelining.allow_post \
             -c variable_size \
             -c constraints.stalls_first_attempt=64 \
             $(SLOTHY_EXTRA_FLAGS)

SLOTHY_FLAGS_SPLIT= -c inputs_are_outputs \
                    -c variable_size \
                    -c constraints.stalls_first_attempt=64 \
                    -c split_heuristic=true \
                    -c split_heuristic_repeat=2 \
                    -c sw_pipelining.enabled=true \
                    -c sw_pipelining.halving_heuristic=True \
                    $(SLOTHY_EXTRA_FLAGS)

# For kernels which stash callee-saved v8-v15 but don't stash callee-saved GPRs x19-x30. 
# Allow SLOTHY to use all V-registers, but only caller-saved GPRs.
RESERVE_X_ONLY_FLAG=-c reserved_regs="[x18--x30,sp]"

# Used for kernels which don't stash callee-saved registers.
# Restrict SLOTHY to caller-saved registers.
RESERVE_ALL_FLAG=-c reserved_regs="[x18--x30,sp,v8--v15]"

all: ntt.S \
     intt.S \
     mld_polyvecl_pointwise_acc_montgomery_l4.S \
     mld_polyvecl_pointwise_acc_montgomery_l5.S \
     mld_polyvecl_pointwise_acc_montgomery_l7.S \
     pointwise_montgomery.S \
     poly_caddq_asm.S \
     poly_chknorm_asm.S \
     poly_decompose_32_asm.S \
     poly_decompose_88_asm.S \
     poly_use_hint_32_asm.S \
     poly_use_hint_88_asm.S \
     polyz_unpack_17_asm.S \
     polyz_unpack_19_asm.S \
     rej_uniform_asm.S \
     rej_uniform_eta2_asm.S \
     rej_uniform_eta4_asm.S

# These units explicitly save and restore registers v8-v15, so SLOTHY can freely use
# those registers.
ntt.S: ../../aarch64_clean/src/ntt.S
	# optimize first loop in one go and write to temp file
	$(eval TMPFILE := $(shell mktemp))
	slothy-cli $(TARGET_ISA) $(TARGET_MICROARCH) $< -o $(TMPFILE) -l ntt_layer123_start $(SLOTHY_FLAGS) $(RESERVE_X_ONLY_FLAG)
	# optimize second loop using split heuristic
	slothy-cli $(TARGET_ISA) $(TARGET_MICROARCH) $(TMPFILE) -o $@ -l ntt_layer45678_start $(SLOTHY_FLAGS_SPLIT) -c split_heuristic_factor=1.5 $(RESERVE_X_ONLY_FLAG)

# Copy remaining files without optimization for now
intt.S: ../../aarch64_clean/src/intt.S
	# optimize first loop in one go and write to temp file
	$(eval TMPFILE := $(shell mktemp))
	slothy-cli $(TARGET_ISA) $(TARGET_MICROARCH) $< -o $(TMPFILE) -l intt_layer5678_start $(SLOTHY_FLAGS) -c reserved_regs="[x0,x18--x30,sp]"
	# optimize second loop using split heuristic
	slothy-cli $(TARGET_ISA) $(TARGET_MICROARCH) $(TMPFILE) -o $@ -l intt_layer1234_start $(SLOTHY_FLAGS_SPLIT) -c split_heuristic_factor=2.5 $(RESERVE_X_ONLY_FLAG)

mld_polyvecl_pointwise_acc_montgomery_l4.S: ../../aarch64_clean/src/mld_polyvecl_pointwise_acc_montgomery_l4.S
	cp $< $@

mld_polyvecl_pointwise_acc_montgomery_l5.S: ../../aarch64_clean/src/mld_polyvecl_pointwise_acc_montgomery_l5.S
	cp $< $@

mld_polyvecl_pointwise_acc_montgomery_l7.S: ../../aarch64_clean/src/mld_polyvecl_pointwise_acc_montgomery_l7.S
	cp $< $@

pointwise_montgomery.S: ../../aarch64_clean/src/pointwise_montgomery.S
	cp $< $@

poly_caddq_asm.S: ../../aarch64_clean/src/poly_caddq_asm.S
	cp $< $@

poly_chknorm_asm.S: ../../aarch64_clean/src/poly_chknorm_asm.S
	cp $< $@

poly_decompose_32_asm.S: ../../aarch64_clean/src/poly_decompose_32_asm.S
	cp $< $@

poly_decompose_88_asm.S: ../../aarch64_clean/src/poly_decompose_88_asm.S
	cp $< $@

poly_use_hint_32_asm.S: ../../aarch64_clean/src/poly_use_hint_32_asm.S
	cp $< $@

poly_use_hint_88_asm.S: ../../aarch64_clean/src/poly_use_hint_88_asm.S
	cp $< $@

polyz_unpack_17_asm.S: ../../aarch64_clean/src/polyz_unpack_17_asm.S
	cp $< $@

polyz_unpack_19_asm.S: ../../aarch64_clean/src/polyz_unpack_19_asm.S
	cp $< $@

rej_uniform_asm.S: ../../aarch64_clean/src/rej_uniform_asm.S
	cp $< $@

rej_uniform_eta2_asm.S: ../../aarch64_clean/src/rej_uniform_eta2_asm.S
	cp $< $@

rej_uniform_eta4_asm.S: ../../aarch64_clean/src/rej_uniform_eta4_asm.S
	cp $< $@

clean:
	-$(RM) -rf *.S
