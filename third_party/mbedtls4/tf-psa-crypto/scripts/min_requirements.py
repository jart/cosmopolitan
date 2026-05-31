#!/usr/bin/env python3
"""Install all the required Python packages, with the minimum Python version.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import os
import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import min_requirements

# The default file is located in the same folder as this script.
DEFAULT_REQUIREMENTS_FILE = 'ci.requirements.txt'

min_requirements.main(os.path.join(os.path.dirname(__file__),
                                   DEFAULT_REQUIREMENTS_FILE))
