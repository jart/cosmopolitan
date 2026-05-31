#!/usr/bin/env python3
# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

# ACVP client for ML-DSA
# See https://pages.nist.gov/ACVP/draft-celi-acvp-ml-dsa.html and
# https://github.com/usnistgov/ACVP-Server/tree/master/gen-val/json-files
# Invokes `acvp_mldsa{lvl}` under the hood.

import argparse
import hashlib
import os
import json
import sys
import subprocess
import urllib.request
from pathlib import Path

# Check if we need to use a wrapper for execution (e.g. QEMU)
exec_prefix = os.environ.get("EXEC_WRAPPER", "")
exec_prefix = exec_prefix.split(" ") if exec_prefix != "" else []


def download_acvp_files(version):
    """Download ACVP test files for the specified version if not present."""
    base_url = f"https://raw.githubusercontent.com/usnistgov/ACVP-Server/{version}/gen-val/json-files"

    # Files we need to download for ML-KEM
    files_to_download = [
        "ML-DSA-keyGen-FIPS204/prompt.json",
        "ML-DSA-keyGen-FIPS204/expectedResults.json",
        "ML-DSA-sigGen-FIPS204/prompt.json",
        "ML-DSA-sigGen-FIPS204/expectedResults.json",
        "ML-DSA-sigVer-FIPS204/prompt.json",
        "ML-DSA-sigVer-FIPS204/expectedResults.json",
    ]

    # Create directory structure
    data_dir = Path(f"test/acvp/.acvp-data/{version}/files")
    data_dir.mkdir(parents=True, exist_ok=True)

    for file_path in files_to_download:
        local_file = data_dir / file_path
        local_file.parent.mkdir(parents=True, exist_ok=True)

        if not local_file.exists():
            url = f"{base_url}/{file_path}"
            print(f"Downloading {file_path}...", file=sys.stderr)
            try:
                urllib.request.urlretrieve(url, local_file)
                # Verify the file is valid JSON
                with open(local_file, "r") as f:
                    json.load(f)
            except json.JSONDecodeError as e:
                print(
                    f"Error: Downloaded file {file_path} is not valid JSON: {e}",
                    file=sys.stderr,
                )
                local_file.unlink(missing_ok=True)
                return False
            except Exception as e:
                print(f"Error downloading {file_path}: {e}", file=sys.stderr)
                local_file.unlink(missing_ok=True)
                return False

    return True


def loadAcvpData(prompt, expectedResults):
    with open(prompt, "r") as f:
        promptData = json.load(f)
    expectedResultsData = None
    if expectedResults is not None:
        with open(expectedResults, "r") as f:
            expectedResultsData = json.load(f)

    return (prompt, promptData, expectedResults, expectedResultsData)


def loadDefaultAcvpData(version):
    data_dir = f"test/acvp/.acvp-data/{version}/files"
    acvp_jsons_for_version = [
        (
            f"{data_dir}/ML-DSA-keyGen-FIPS204/prompt.json",
            f"{data_dir}/ML-DSA-keyGen-FIPS204/expectedResults.json",
        ),
        (
            f"{data_dir}/ML-DSA-sigGen-FIPS204/prompt.json",
            f"{data_dir}/ML-DSA-sigGen-FIPS204/expectedResults.json",
        ),
        (
            f"{data_dir}/ML-DSA-sigVer-FIPS204/prompt.json",
            f"{data_dir}/ML-DSA-sigVer-FIPS204/expectedResults.json",
        ),
    ]
    acvp_data = []
    for prompt, expectedResults in acvp_jsons_for_version:
        acvp_data.append(loadAcvpData(prompt, expectedResults))
    return acvp_data


def err(msg, **kwargs):
    print(msg, file=sys.stderr, **kwargs)


def info(msg, **kwargs):
    print(msg, **kwargs)


def get_acvp_binary(tg):
    """Convert JSON dict for ACVP test group to suitable ACVP binary."""
    parameterSetToLevel = {
        "ML-DSA-44": 44,
        "ML-DSA-65": 65,
        "ML-DSA-87": 87,
    }
    level = parameterSetToLevel[tg["parameterSet"]]
    basedir = f"./test/build/mldsa{level}/bin"
    acvp_bin = f"acvp_mldsa{level}"
    return f"{basedir}/{acvp_bin}"


def run_keyGen_test(tg, tc):
    info(f"Running keyGen test case {tc['tcId']} ... ", end="")

    results = {"tcId": tc["tcId"]}
    acvp_bin = get_acvp_binary(tg)
    assert tg["testType"] == "AFT"
    acvp_call = exec_prefix + [
        acvp_bin,
        "keyGen",
        f"seed={tc['seed']}",
    ]
    result = subprocess.run(acvp_call, encoding="utf-8", capture_output=True)
    if result.returncode != 0:
        err("FAIL!")
        err(f"{acvp_call} failed with error code {result.returncode}")
        err(result.stderr)
        exit(1)
    # Extract results
    for l in result.stdout.splitlines():
        (k, v) = l.split("=")
        results[k] = v
    info("done")
    return results


def compute_hash(msg, alg):
    msg_bytes = bytes.fromhex(msg)

    if alg == "SHA2-224":
        return hashlib.sha224(msg_bytes).hexdigest()
    elif alg == "SHA2-256":
        return hashlib.sha256(msg_bytes).hexdigest()
    elif alg == "SHA2-384":
        return hashlib.sha384(msg_bytes).hexdigest()
    elif alg == "SHA2-512":
        return hashlib.sha512(msg_bytes).hexdigest()
    elif alg == "SHA2-512/224":
        return hashlib.new("sha512_224", msg_bytes).hexdigest()
    elif alg == "SHA2-512/256":
        return hashlib.new("sha512_256", msg_bytes).hexdigest()
    elif alg == "SHA3-224":
        return hashlib.sha3_224(msg_bytes).hexdigest()
    elif alg == "SHA3-256":
        return hashlib.sha3_256(msg_bytes).hexdigest()
    elif alg == "SHA3-384":
        return hashlib.sha3_384(msg_bytes).hexdigest()
    elif alg == "SHA3-512":
        return hashlib.sha3_512(msg_bytes).hexdigest()
    elif alg == "SHAKE-128":
        return hashlib.shake_128(msg_bytes).hexdigest(32)
    elif alg == "SHAKE-256":
        return hashlib.shake_256(msg_bytes).hexdigest(64)
    else:
        raise ValueError(f"Unsupported hash algorithm: {alg}")


def run_sigGen_test(tg, tc):
    info(f"Running sigGen test case {tc['tcId']} ... ", end="")
    results = {"tcId": tc["tcId"]}
    acvp_bin = get_acvp_binary(tg)

    assert tg["testType"] == "AFT"

    is_deterministic = tg["deterministic"] is True
    if "preHash" in tg and tg["preHash"] == "preHash":
        assert len(tc["context"]) <= 2 * 255

        # Use specialized SHAKE256 function that computes hash internally
        if tc["hashAlg"] == "SHAKE-256":
            target = (
                "sigGenPreHashShake256Deterministic"
                if is_deterministic
                else "sigGenPreHashShake256"
            )
            acvp_call = exec_prefix + [
                acvp_bin,
                target,
                f"message={tc['message']}",
                f"context={tc['context']}",
                f"sk={tc['sk']}",
            ]
        else:
            ph = compute_hash(tc["message"], tc["hashAlg"])
            target = (
                "sigGenPreHashDeterministic" if is_deterministic else "sigGenPreHash"
            )
            acvp_call = exec_prefix + [
                acvp_bin,
                target,
                f"ph={ph}",
                f"context={tc['context']}",
                f"sk={tc['sk']}",
                f"hashAlg={tc['hashAlg']}",
            ]
    elif tg["signatureInterface"] == "external":
        assert "hashAlg" not in tc or tc["hashAlg"] == "none"
        assert len(tc["context"]) <= 2 * 255
        assert len(tc["message"]) <= 2 * 65536

        target = "sigGenDeterministic" if is_deterministic else "sigGen"
        acvp_call = exec_prefix + [
            acvp_bin,
            target,
            f"message={tc['message']}",
            f"sk={tc['sk']}",
            f"context={tc['context']}",
        ]
    else:  # signatureInterface=internal
        assert "hashAlg" not in tc or tc["hashAlg"] == "none"
        externalMu = 0
        if tg["externalMu"] is True:
            externalMu = 1
            assert len(tc["mu"]) == 2 * 64
            msg = tc["mu"]
        else:
            assert len(tc["message"]) <= 2 * 65536
            msg = tc["message"]

        target = "sigGenInternalDeterministic" if is_deterministic else "sigGenInternal"
        acvp_call = exec_prefix + [
            acvp_bin,
            target,
            f"message={msg}",
            f"sk={tc['sk']}",
            f"externalMu={externalMu}",
        ]

    # Append rnd argument for randomized (non-deterministic) variant
    if not is_deterministic:
        acvp_call.append(f"rnd={tc['rnd']}")

    result = subprocess.run(acvp_call, encoding="utf-8", capture_output=True)
    if result.returncode != 0:
        err("FAIL!")
        err(f"{acvp_call} failed with error code {result.returncode}")
        err(result.stderr)
        exit(1)
    # Extract results
    for l in result.stdout.splitlines():
        (k, v) = l.split("=")
        results[k] = v
    info("done")
    return results


def run_sigVer_test(tg, tc):
    info(f"Running sigVer test case {tc['tcId']} ... ", end="")
    results = {"tcId": tc["tcId"]}
    acvp_bin = get_acvp_binary(tg)

    if "preHash" in tg and tg["preHash"] == "preHash":
        assert len(tc["context"]) <= 2 * 255

        # Use specialized SHAKE256 function that computes hash internally
        if tc["hashAlg"] == "SHAKE-256":
            acvp_call = exec_prefix + [
                acvp_bin,
                "sigVerPreHashShake256",
                f"message={tc['message']}",
                f"context={tc['context']}",
                f"signature={tc['signature']}",
                f"pk={tc['pk']}",
            ]
        else:
            ph = compute_hash(tc["message"], tc["hashAlg"])
            acvp_call = exec_prefix + [
                acvp_bin,
                "sigVerPreHash",
                f"ph={ph}",
                f"context={tc['context']}",
                f"signature={tc['signature']}",
                f"pk={tc['pk']}",
                f"hashAlg={tc['hashAlg']}",
            ]
    elif tg["signatureInterface"] == "external":
        assert "hashAlg" not in tc or tc["hashAlg"] == "none"
        assert len(tc["context"]) <= 2 * 255
        assert len(tc["message"]) <= 2 * 65536

        acvp_call = exec_prefix + [
            acvp_bin,
            "sigVer",
            f"message={tc['message']}",
            f"context={tc['context']}",
            f"signature={tc['signature']}",
            f"pk={tc['pk']}",
        ]
    else:  # signatureInterface=internal
        assert "hashAlg" not in tc or tc["hashAlg"] == "none"
        externalMu = 0
        if tg["externalMu"] is True:
            externalMu = 1
            assert len(tc["mu"]) == 2 * 64
            msg = tc["mu"]
        else:
            assert len(tc["message"]) <= 2 * 65536
            msg = tc["message"]

        acvp_call = exec_prefix + [
            acvp_bin,
            "sigVerInternal",
            f"message={msg}",
            f"signature={tc['signature']}",
            f"pk={tc['pk']}",
            f"externalMu={externalMu}",
        ]

    result = subprocess.run(acvp_call, encoding="utf-8", capture_output=True)
    # Extract results
    results["testPassed"] = result.returncode == 0
    info("done")
    return results


def runTestSingle(promptName, prompt, expectedResultName, expectedResult, output):
    info(f"Running ACVP tests for {promptName}")

    assert expectedResult is not None or output is not None

    # The ACVTS data structure is very slightly different from the sample files
    # in the usnistgov/ACVP-Server Github repository:
    # The prompt consists of a 2-element list, where the first element is
    # solely consisting of {"acvVersion": "1.0"} and the second element is
    # the usual prompt containing the test values.
    # See https://pages.nist.gov/ACVP/draft-celi-acvp-ml-dsa.txt for details.
    # We automatically detect that case here and extract the second element
    isAcvts = False
    if type(prompt) is list:
        isAcvts = True
        assert len(prompt) == 2
        acvVersion = prompt[0]
        assert len(acvVersion) == 1
        prompt = prompt[1]

    assert prompt["algorithm"] == "ML-DSA"
    assert (
        prompt["mode"] == "keyGen"
        or prompt["mode"] == "sigGen"
        or prompt["mode"] == "sigVer"
    )

    # copy top level fields into the results
    results = prompt.copy()

    results["testGroups"] = []
    for tg in prompt["testGroups"]:
        tgResult = {
            "tgId": tg["tgId"],
            "tests": [],
        }
        results["testGroups"].append(tgResult)
        for tc in tg["tests"]:
            if prompt["mode"] == "keyGen":
                result = run_keyGen_test(tg, tc)
            elif prompt["mode"] == "sigGen":
                result = run_sigGen_test(tg, tc)
            elif prompt["mode"] == "sigVer":
                result = run_sigVer_test(tg, tc)
            tgResult["tests"].append(result)

    # In case the testvectors are from the ACVTS server, it is expected
    # that the acvVersion is included in the output results.
    # See note on ACVTS data structure above.
    if isAcvts is True:
        results = [acvVersion, results]

    # Compare to expected results
    if expectedResult is not None:
        info(f"Comparing results with {expectedResultName}")
        # json.dumps() is guaranteed to preserve insertion order (since Python 3.7)
        # Enforce strictly the same order as in the expected Result
        if json.dumps(results) != json.dumps(expectedResult):
            err("FAIL!")
            err(f"Mismatching result for {promptName}")
            exit(1)
        info("OK")
    else:
        info(
            "Results could not be validated as no expected resulted were provided to --expected"
        )

    # Write results to file
    if output is not None:
        info(f"Writing results to {output}")
        with open(output, "w") as f:
            json.dump(results, f)


def runTest(data, output):
    # if output is defined we expect only one input
    assert output is None or len(data) == 1

    for promptName, prompt, expectedResultName, expectedResult in data:
        runTestSingle(promptName, prompt, expectedResultName, expectedResult, output)
    info("ALL GOOD!")


def test(prompt, expected, output, version):
    assert (
        prompt is not None or output is None
    ), "cannot produce output if there is no input"

    assert prompt is None or (
        output is not None or expected is not None
    ), "if there is a prompt, either output or expectedResult required"

    # if prompt is passed, use it
    if prompt is not None:
        data = [loadAcvpData(prompt, expected)]
    else:
        # load data from downloaded files
        data = loadDefaultAcvpData(version)

    runTest(data, output)


parser = argparse.ArgumentParser()
parser.add_argument(
    "-p", "--prompt", help="Path to prompt file in json format", required=False
)
parser.add_argument(
    "-e",
    "--expected",
    help="Path to expectedResults file in json format",
    required=False,
)
parser.add_argument(
    "-o", "--output", help="Path to output file in json format", required=False
)
parser.add_argument(
    "--version",
    "-v",
    default="v1.1.0.41",
    help="ACVP test vector version (default: v1.1.0.41)",
)
args = parser.parse_args()

if args.prompt is None:
    print(f"Using ACVP test vectors version {args.version}", file=sys.stderr)

    # Download files if needed
    if not download_acvp_files(args.version):
        print("Failed to download ACVP test files", file=sys.stderr)
        sys.exit(1)

test(args.prompt, args.expected, args.output, args.version)
