#!/usr/bin/env python3
#
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: MIT-0


import logging
import pathlib
import shutil
import subprocess


_TOOLS = [
    "cadical",
    "cbmc",
    "cbmc-viewer",
    "litani",
]


def _format_versions(table):
    lines = [
        "<table>",
        '<tr><td colspan="2" style="font-weight: bold">Tool Versions</td></tr>',
    ]
    for tool, version in table.items():
        if version:
            v_str = f'<code><pre style="margin: 0">{version}</pre></code>'
        else:
            v_str = "<em>not found</em>"
        lines.append(
            f'<tr><td style="font-weight: bold; padding-right: 1em; '
            f'text-align: right;">{tool}:</td>'
            f"<td>{v_str}</td></tr>"
        )
    lines.append("</table>")
    return "\n".join(lines)


def _get_tool_versions():
    ret = {}
    for tool in _TOOLS:
        err = f"Could not determine version of {tool}: "
        ret[tool] = None
        if not shutil.which(tool):
            logging.error("%s'%s' not found on $PATH", err, tool)
            continue
        cmd = [tool, "--version"]
        proc = subprocess.Popen(cmd, text=True, stdout=subprocess.PIPE)
        try:
            out, _ = proc.communicate(timeout=10)
        except subprocess.TimeoutExpired:
            logging.error("%s'%s --version' timed out", err, tool)
            continue
        if proc.returncode:
            logging.error(
                "%s'%s --version' returned %s", err, tool, str(proc.returncode)
            )
            continue
        ret[tool] = out.strip()
    return ret


def main():
    exe_name = pathlib.Path(__file__).name
    logging.basicConfig(format=f"{exe_name}: %(message)s")

    table = _get_tool_versions()
    out = _format_versions(table)
    print(out)


if __name__ == "__main__":
    main()
