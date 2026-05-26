#!/usr/bin/env python3
#
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: MIT-0

import argparse
import asyncio
import json
import logging
import math
import os
import pathlib
import re
import subprocess
import sys
import tempfile
import uuid

from lib.summarize import print_proof_results, export_result_json


DESCRIPTION = "Configure and run all CBMC proofs in parallel"

# Keep the epilog hard-wrapped at 70 characters, as it gets printed
# verbatim in the terminal. 70 characters stops here --------------> |
EPILOG = """
This tool automates the process of running `make report` in each of
the CBMC proof directories. The tool calculates the dependency graph
of all tasks needed to build, run, and report on all the proofs, and
executes these tasks in parallel.

The tool is roughly equivalent to doing this:

        litani init --project "my-cool-project";

        find . -name Makefile | while read -r proof; do
            pushd $(dirname ${proof});

            # The `make _report` rule adds a single proof to litani
            # without running it
            make _report;

            popd;
        done

        litani run-build;

except that it is much faster and provides some convenience options.
The CBMC CI runs this script with no arguments to build and run all
proofs in parallel. The value of "my-cool-project" is taken from the
PROJECT_NAME variable in Makefile-project-defines.

The --no-standalone argument omits the `litani init` and `litani
run-build`; use it when you want to add additional proof jobs, not
just the CBMC ones. In that case, you would run `litani init`
yourself; then run `run-cbmc-proofs --no-standalone`; add any
additional jobs that you want to execute with `litani add-job`; and
finally run `litani run-build`.

The litani dashboard will be written under the `output` directory; the
cbmc-viewer reports remain in the `$PROOF_DIR/report` directory. The
HTML dashboard from the latest Litani run will always be symlinked to
`output/latest/html/index.html`, so you can keep that page open in
your browser and reload the page whenever you re-run this script.
"""
# 70 characters stops here ----------------------------------------> |


def get_project_name():
    cmd = [
        "make",
        "--no-print-directory",
        "-f",
        "Makefile.common",
        "echo-project-name",
    ]
    logging.debug(" ".join(cmd))
    proc = subprocess.run(
        cmd, universal_newlines=True, stdout=subprocess.PIPE, check=False
    )
    if proc.returncode:
        logging.critical("could not run make to determine project name")
        sys.exit(1)
    if not proc.stdout.strip():
        logging.warning(
            "project name has not been set; using generic name instead. "
            "Set the PROJECT_NAME value in Makefile-project-defines to "
            "remove this warning"
        )
        return "<PROJECT NAME HERE>"
    return proc.stdout.strip()


def get_args():
    pars = argparse.ArgumentParser(
        description=DESCRIPTION,
        epilog=EPILOG,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    for arg in [
        {
            "flags": ["-j", "--parallel-jobs"],
            "type": int,
            "metavar": "N",
            "help": "run at most N proof jobs in parallel",
        },
        {
            "flags": ["--fail-on-proof-failure"],
            "action": "store_true",
            "help": "exit with return code `10' if any proof failed"
            " (default: exit 0)",
        },
        {
            "flags": ["--no-standalone"],
            "action": "store_true",
            "help": "only configure proofs: do not initialize nor run",
        },
        {
            "flags": ["-p", "--proofs"],
            "nargs": "+",
            "metavar": "DIR",
            "help": "only run proof in directory DIR (can pass more than one)",
        },
        {
            "flags": ["--project-name"],
            "metavar": "NAME",
            "default": get_project_name(),
            "help": "project name for report. Default: %(default)s",
        },
        {
            "flags": ["--marker-file"],
            "metavar": "FILE",
            "default": "Makefile",
            "help": (
                "name of file that marks proof directories. Default: " "%(default)s"
            ),
        },
        {
            "flags": ["--no-memory-profile"],
            "action": "store_true",
            "help": "disable memory profiling, even if Litani supports it",
        },
        {
            "flags": ["--no-expensive-limit"],
            "action": "store_true",
            "help": "do not limit parallelism of 'EXPENSIVE' jobs",
        },
        {
            "flags": ["--expensive-jobs-parallelism"],
            "metavar": "N",
            "default": 1,
            "type": int,
            "help": (
                "how many proof jobs marked 'EXPENSIVE' to run in parallel. "
                "Default: %(default)s"
            ),
        },
        {
            "flags": ["--verbose"],
            "action": "store_true",
            "help": "verbose output",
        },
        {
            "flags": ["--debug"],
            "action": "store_true",
            "help": "debug output",
        },
        {
            "flags": ["--summarize"],
            "action": "store_true",
            "help": "summarize proof results with two tables on stdout",
        },
        {
            "flags": ["--version"],
            "action": "version",
            "version": "CBMC starter kit 2.10",
            "help": "display version and exit",
        },
        {
            "flags": ["--no-coverage"],
            "action": "store_true",
            "help": "do property checking without coverage checking",
        },
        {
            "flags": ["--per-proof-timeout"],
            "type": int,
            "metavar": "SECONDS",
            "default": 1800,
            "help": "timeout for each individual proof in seconds (default: 1800)",
        },
        {
            "flags": ["--output-result-json"],
            "metavar": "FILE",
            "help": "path to export result JSON",
        },
    ]:
        flags = arg.pop("flags")
        pars.add_argument(*flags, **arg)
    return pars.parse_args()


def set_up_logging(verbose):
    if verbose:
        level = logging.DEBUG
    else:
        level = logging.WARNING
    logging.basicConfig(format="run-cbmc-proofs: %(message)s", level=level)


def task_pool_size():
    ret = os.cpu_count()
    if ret is None or ret < 3:
        return 1
    return ret - 2


def print_counter(counter):
    # pylint: disable=consider-using-f-string
    print(
        "\rConfiguring CBMC proofs: "
        "{complete:{width}} / {total:{width}}".format(**counter),
        end="",
        file=sys.stderr,
    )


def get_proof_dirs(proof_root, proof_list, marker_file):
    if proof_list is not None:
        proofs_remaining = list(proof_list)
    else:
        proofs_remaining = []

    for root, _, fyles in os.walk(proof_root):
        proof_name = str(pathlib.Path(root).name)
        if root != str(proof_root) and ".litani_cache_dir" in fyles:
            pathlib.Path(f"{root}/.litani_cache_dir").unlink()
        if proof_list and proof_name not in proof_list:
            continue
        if proof_list and proof_name in proofs_remaining:
            proofs_remaining.remove(proof_name)
        if marker_file in fyles:
            yield root

    if proofs_remaining:
        logging.critical(
            "The following proofs were not found: %s", ", ".join(proofs_remaining)
        )
        sys.exit(1)


def run_build(litani, jobs, fail_on_proof_failure, summarize, output_result_json=None):
    cmd = [str(litani), "run-build"]
    if jobs:
        cmd.extend(["-j", str(jobs)])
    if fail_on_proof_failure:
        cmd.append("--fail-on-pipeline-failure")
    if summarize:
        out_file = pathlib.Path(tempfile.gettempdir(), "run.json").resolve()
        cmd.extend(["--out-file", str(out_file)])

    logging.debug(" ".join(cmd))
    proc = subprocess.run(cmd, check=False)

    if proc.returncode and not fail_on_proof_failure:
        logging.critical("Failed to run litani run-build")
        sys.exit(1)

    if summarize:
        export_result_json(output_result_json, out_file)
        print_proof_results(out_file)
        out_file.unlink()

    if proc.returncode:
        logging.error("One or more proofs failed")
        sys.exit(10)


def get_litani_path(proof_root):
    cmd = [
        "make",
        "--no-print-directory",
        f"PROOF_ROOT={proof_root}",
        "-f",
        "Makefile.common",
        "litani-path",
    ]
    logging.debug(" ".join(cmd))
    proc = subprocess.run(
        cmd, universal_newlines=True, stdout=subprocess.PIPE, check=False
    )
    if proc.returncode:
        logging.critical("Could not determine path to litani")
        sys.exit(1)
    return proc.stdout.strip()


def get_litani_capabilities(litani_path):
    cmd = [litani_path, "print-capabilities"]
    proc = subprocess.run(
        cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=False
    )
    if proc.returncode:
        return []
    try:
        return json.loads(proc.stdout)
    except RuntimeError:
        logging.warning("Could not load litani capabilities: '%s'", proc.stdout)
        return []


def check_uid_uniqueness(proof_dir, proof_uids):
    with (pathlib.Path(proof_dir) / "Makefile").open() as handle:
        for line in handle:
            match = re.match(r"^PROOF_UID\s*=(?P<uid>[^#]+)", line)
            if not match:
                continue
            uid = match["uid"].strip()
            if uid not in proof_uids:
                proof_uids[uid] = proof_dir
                return

            logging.critical(
                "The Makefile in directory '%s' should have a different "
                "PROOF_UID than the Makefile in directory '%s'",
                proof_dir,
                proof_uids[match["uid"]],
            )
            sys.exit(1)

    logging.critical(
        "The Makefile in directory '%s' should contain a line like", proof_dir
    )
    logging.critical("PROOF_UID = ...")
    logging.critical("with a unique identifier for the proof.")
    sys.exit(1)


def should_enable_memory_profiling(litani_caps, args):
    if args.no_memory_profile:
        return False
    return "memory_profile" in litani_caps


def should_enable_pools(litani_caps, args):
    if args.no_expensive_limit:
        return False
    return "pools" in litani_caps


async def configure_proof_dirs(  # pylint: disable=too-many-arguments
    queue,
    counter,
    proof_uids,
    enable_pools,
    enable_memory_profiling,
    report_target,
    debug,
    timeout,
):
    while True:
        print_counter(counter)
        path = str(await queue.get())

        check_uid_uniqueness(path, proof_uids)

        pools = ["ENABLE_POOLS=true"] if enable_pools else []
        profiling = ["ENABLE_MEMORY_PROFILING=true"] if enable_memory_profiling else []

        # Set up environment with CBMC_TIMEOUT
        env = os.environ.copy()
        env["CBMC_TIMEOUT"] = str(timeout)

        # delete old reports
        proc = await asyncio.create_subprocess_exec(
            "make",
            "veryclean",
            cwd=path,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        # Allow interactive tasks to preempt proof configuration
        proc = await asyncio.create_subprocess_exec(
            "nice",
            "-n",
            "15",
            "make",
            *pools,
            *profiling,
            "-B",
            report_target,
            "" if debug else "--quiet",
            cwd=path,
            env=env,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )
        stdout, stderr = await proc.communicate()
        logging.debug("returncode: %s", str(proc.returncode))
        logging.debug("stdout:")
        for line in stdout.decode().splitlines():
            logging.debug(line)
        logging.debug("stderr:")
        for line in stderr.decode().splitlines():
            logging.debug(line)

        counter["fail" if proc.returncode else "pass"].append(path)
        counter["complete"] += 1

        print_counter(counter)
        queue.task_done()


def add_tool_version_job():
    cmd = [
        "litani",
        "add-job",
        "--command",
        "./lib/print_tool_versions.py .",
        "--description",
        "printing out tool versions",
        "--phony-outputs",
        str(uuid.uuid4()),
        "--pipeline-name",
        "print_tool_versions",
        "--ci-stage",
        "report",
        "--tags",
        "front-page-text",
    ]
    proc = subprocess.run(cmd)
    if proc.returncode:
        logging.critical("Could not add tool version printing job")
        sys.exit(1)


async def main():  # pylint: disable=too-many-locals
    args = get_args()
    set_up_logging(args.verbose)

    proof_root = pathlib.Path(os.getcwd())
    litani = get_litani_path(proof_root)

    litani_caps = get_litani_capabilities(litani)
    enable_pools = should_enable_pools(litani_caps, args)
    init_pools = (
        ["--pools", f"expensive:{args.expensive_jobs_parallelism}"]
        if enable_pools
        else []
    )

    if not args.no_standalone:
        cmd = [
            str(litani),
            "init",
            *init_pools,
            "--project",
            args.project_name,
            "--no-print-out-dir",
        ]

        if "output_directory_flags" in litani_caps:
            out_prefix = proof_root / "output"
            out_symlink = out_prefix / "latest"
            out_index = out_symlink / "html" / "index.html"
            cmd.extend(
                [
                    "--output-prefix",
                    str(out_prefix),
                    "--output-symlink",
                    str(out_symlink),
                ]
            )
            print(
                "\nFor your convenience, the output of this run will be symbolically linked to ",
                out_index,
                "\n",
            )

        logging.debug(" ".join(cmd))
        proc = subprocess.run(cmd, check=False)
        if proc.returncode:
            logging.critical("Failed to run litani init")
            sys.exit(1)

    proof_dirs = list(get_proof_dirs(proof_root, args.proofs, args.marker_file))
    if not proof_dirs:
        logging.critical("No proof directories found")
        sys.exit(1)

    proof_queue = asyncio.Queue()
    for proof_dir in proof_dirs:
        proof_queue.put_nowait(proof_dir)

    counter = {
        "pass": [],
        "fail": [],
        "complete": 0,
        "total": len(proof_dirs),
        "width": int(math.log10(len(proof_dirs))) + 1,
    }

    proof_uids = {}
    tasks = []

    enable_memory_profiling = should_enable_memory_profiling(litani_caps, args)
    report_target = "_report_no_coverage" if args.no_coverage else "_report"

    for _ in range(task_pool_size()):
        task = asyncio.create_task(
            configure_proof_dirs(
                proof_queue,
                counter,
                proof_uids,
                enable_pools,
                enable_memory_profiling,
                report_target,
                args.debug,
                args.per_proof_timeout,
            )
        )
        tasks.append(task)

    await proof_queue.join()

    add_tool_version_job()

    print_counter(counter)
    print("", file=sys.stderr)

    if counter["fail"]:
        logging.critical(
            "Failed to configure the following proofs:\n%s",
            "\n".join([str(f) for f in counter["fail"]]),
        )
        sys.exit(1)

    if not args.no_standalone:
        run_build(
            litani,
            args.parallel_jobs,
            args.fail_on_proof_failure,
            args.summarize,
            args.output_result_json,
        )


if __name__ == "__main__":
    asyncio.run(main())
