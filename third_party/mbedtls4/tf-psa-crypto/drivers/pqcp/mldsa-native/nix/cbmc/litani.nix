# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

{ stdenvNoCC
, fetchFromGitHub
, python3Packages
}:

stdenvNoCC.mkDerivation {
  pname = "litani";
  version = "8002c240ef4f424039ed3cc32e076c0234d01768";
  src = fetchFromGitHub {
    owner = "awslabs";
    repo = "aws-build-accumulator";
    rev = "8002c240ef4f424039ed3cc32e076c0234d01768";
    sha256 = "sha256-UwF/B6lpsjpQn8SW+tCfOXTp14pNBr2sRGujJH3iPLk=";
  };
  dontConfigure = true;
  installPhase = ''
    mkdir -p $out/bin
    install -Dm755 litani $out/bin/litani
    cp -r lib $out/bin
    cp -r templates $out/bin
  '';
  dontStrip = true;
  noAuditTmpdir = true;
  propagatedBuildInputs = [
    (python3Packages.python.withPackages
      (pythonPackages: [ pythonPackages.jinja2 ])
    )
  ];

  meta = {
    description = "Litani metabuild system";
    homepage = "https://awslabs.github.io/aws-build-accumulator/";
  };
}
