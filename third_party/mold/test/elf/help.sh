// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

./mold --help | grep -q Usage
