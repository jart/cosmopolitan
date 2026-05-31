# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

# HOL Light server for programmatic communication
# Based on https://github.com/monadius/hol_server
{ fetchFromGitHub, runCommand, hol_light' }:

let
  hol_server_src = fetchFromGitHub {
    owner = "monadius";
    repo = "hol_server";
    rev = "vscode";
    hash = "sha256-o98ule5uuazm36+ppsvX2KCbtVbVwzHxGboUhbbrPCQ=";
  };

  hol_server_start = runCommand "hol-server" { } ''
    mkdir -p $out/bin
    substitute ${./hol-server.sh} $out/bin/hol-server \
      --replace-fail "@hol_light@" "${hol_light'}" \
      --replace-fail "@hol_server_src@" "${hol_server_src}"
    chmod +x $out/bin/hol-server
  '';

in
{
  inherit hol_server_src hol_server_start;
}
