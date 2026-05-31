"""Auxiliary definitions used in type annotations.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

from typing import Any

# The typing_extensions module is necessary for type annotations that are
# checked with mypy. It is only used for type annotations or to define
# things that are themselves only used for type annotations. It is not
# available on a default Python installation. Therefore, try loading
# what we need from it for the sake of mypy (which depends on, or comes
# with, typing_extensions), and if not define substitutes that lack the
# static type information but are good enough at runtime.
try:
    from typing_extensions import Protocol #pylint: disable=import-error
except ImportError:
    class Protocol: #type: ignore
        #pylint: disable=too-few-public-methods
        pass

class Writable(Protocol):
    """Abstract class for typing hints."""
    # pylint: disable=no-self-use,too-few-public-methods,unused-argument
    def write(self, text: str) -> Any:
        ...
