"""Add the consuming repository's scripts to the module search path.

Usage:

    import project_scripts # pylint: disable=unused-import
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__),
                             os.path.pardir, os.path.pardir,
                             'scripts'))
