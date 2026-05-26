# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

{ hol_light, fetchFromGitHub, writeText, ocamlPackages, ledit, ... }:
hol_light.overrideAttrs (old: {
  setupHook = writeText "setup-hook.sh" ''
    export HOLDIR="$1/lib/hol_light"
    export HOLLIGHT_DIR="$1/lib/hol_light"
    export PATH="$1/lib/hol_light:$PATH"
  '';
  version = "unstable-2025-09-22";
  src = fetchFromGitHub {
    owner = "jrh13";
    repo = "hol-light";
    rev = "bed58fa74649fa74015176f8f90e77f7af5cf8e3";
    hash = "sha256-QDubbUUChvv04239BdcKPSU+E2gdSzqAWfAETK2Xtg0=";
  };
  patches = [
    ./0005-Configure-hol-sh-for-mldsa-native.patch
    ./0006-Add-findlib-to-ocaml-hol.patch
  ];
  propagatedBuildInputs = old.propagatedBuildInputs ++ old.nativeBuildInputs ++ [ ocamlPackages.pcre2 ledit ];
  buildPhase = ''
    HOLLIGHT_USE_MODULE=1 make hol.sh
    patchShebangs hol.sh
    HOLLIGHT_USE_MODULE=1 make
  '';
  installPhase = ''
    mkdir -p "$out/lib/hol_light"
    cp -a . $out/lib/hol_light
    sed "s^__DIR__^$out/lib/hol_light^g; s^__USE_MODULE__^1^g" hol_4.14.sh > hol.sh
    mv hol.sh $out/lib/hol_light/
  '';
})
