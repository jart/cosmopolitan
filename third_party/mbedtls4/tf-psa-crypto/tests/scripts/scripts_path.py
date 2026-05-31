"""Add our Python library directories to the module search path.

Usage:

    import scripts_path # pylint: disable=unused-import
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__),
                             os.path.pardir, os.path.pardir,
                             'scripts'))
sys.path.append(os.path.join(os.path.dirname(__file__),
                             os.path.pardir, os.path.pardir,
                             'framework', 'scripts'))
