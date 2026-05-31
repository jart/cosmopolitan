#!/usr/bin/env bash
# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

# Helper script to query META.yml
#
# Arguments
# - Scheme to query: ML-DSA-44, ML-DSA-65, ML-DSA-87
# - Field to query, e.g. "kat-sha256"
#
# Optional:
# - Value to compare against

META=META.yml

# Manual extraction of metadata with basic cmd line tools
VAL=$(cat $META |
  grep "name\|$2" |
  grep $1 -A 1 |
  grep $2 |
  cut -d ":" -f 2 | tr -d ' ')

# More robust extraction using yq
if (which yq 2>&1 >/dev/null); then
  QUERY=".implementations | .[] | select(.name==\"$1\") | .\"$2\""
  echo "cat $META | yq "$QUERY" -r"
  VAL_JQ=$(cat $META | yq "$QUERY" -r)

  if [[ $VAL_JQ != $VAL ]]; then
    echo "ERROR parsing metadata file $META"
    exit 1
  fi
fi

INPUT=$3
if [[ $INPUT != "" ]]; then
  if [[ $INPUT != "$VAL" ]]; then
    echo "$META $1 $2: FAIL ($VAL != $INPUT)"
    exit 1
  else
    echo "$META $1 $2: OK"
    exit 0
  fi
else
  echo $VAL
fi
