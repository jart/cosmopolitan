#!/usr/bin/env bash

#
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT-0
#

#
# Compile proof script into executable binary
#

# This file is derived from s2n-bignum's tools/build-proof.sh
# Notable changes:
# - Modify HOL-Light's inline_load.ml to allow for a 3rd include path,
#   in addition to S2N_BIGNUM_DIR and HOLLIGHT_DIR
# - Removal of s2n-bignum specific code that is not relevant for
#   the mldsa-native proofs.

ROOT="$(realpath "$(dirname "$0")"/../)"

if [ "$#" -ne 3 ]; then
  echo "${ROOT}/build-proof.sh <.ml file path> <hol.sh> <output .native path>"
  echo "This script builds HOL Light proof using OCaml native compiler and puts the "
  echo "output binary at <output .native path>."
  exit 1
fi

# Return the exit code if any statement fails
set -e

ml_path="$1"
hol_sh_cmd=$2
output_path=$3
output_dir=$(dirname "$output_path")
[ -d "$output_dir" ] || mkdir -p "$output_dir"

export HOLLIGHT_DIR="$(dirname ${hol_sh_cmd})"
if [ ! -f "${HOLLIGHT_DIR}/hol_lib.cmxa" ]; then
  echo "hol_lib.cmxa does not exist in HOLLIGHT_DIR('${HOLLIGHT_DIR}')."
  echo "Did you compile HOL Light with HOLLIGHT_USE_MODULE set to 1?"
  exit 1
fi

template_ml="$(mktemp).ml"
echo "Generating a template .ml that loads the file...: ${template_ml}"

(
  echo 'let proof_start_time = Unix.time();;'
  echo "loadt \"${ml_path}\";;"
  echo "check_axioms ();;"
  echo 'let proof_end_time = Unix.time();;'
  echo 'Printf.printf "Running time: %f sec, Start unixtime: %f, End unixtime: %f\n" (proof_end_time -. proof_start_time) proof_start_time proof_end_time;;'
) >>${template_ml}

inlined_prefix="$(mktemp)"
inlined_ml="${inlined_prefix}.ml"
inlined_cmx="${inlined_prefix}.cmx"
(cd "${S2N_BIGNUM_DIR}" && HOLLIGHT_LOAD_PATH=${ROOT} ocaml ${HOLLIGHT_DIR}/inline_load.ml "${template_ml}" "${inlined_ml}")

# Give a large stack size.
OCAMLRUNPARAM=l=2000000000 \
  ocamlopt.byte -pp "$(${hol_sh_cmd} -pp)" -I "${HOLLIGHT_DIR}" -I +unix -c \
  hol_lib.cmxa ${inlined_ml} -o ${inlined_cmx} -w -a
ocamlfind ocamlopt -package zarith,unix -linkpkg hol_lib.cmxa \
  -I "${HOLLIGHT_DIR}" ${inlined_cmx} \
  -o "${output_path}"

# Remove the intermediate files to save disk space
rm -f ${inlined_cmx} ${template_ml} ${inlined_ml}
