"""Mbed TLS build tree information and manipulation.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import os
import inspect
import re
from typing import Optional

def looks_like_tf_psa_crypto_root(path: str) -> bool:
    """Whether the given directory looks like the root of the PSA Crypto source tree."""
    try:
        with open(os.path.join(path, 'scripts', 'project_name.txt'), 'r') as f:
            return f.read() == "TF-PSA-Crypto\n"
    except FileNotFoundError:
        return False

def looks_like_mbedtls_root(path: str) -> bool:
    """Whether the given directory looks like the root of the Mbed TLS source tree."""
    try:
        with open(os.path.join(path, 'scripts', 'project_name.txt'), 'r') as f:
            return f.read() == "Mbed TLS\n"
    except FileNotFoundError:
        return False

def looks_like_root(path: str) -> bool:
    return looks_like_tf_psa_crypto_root(path) or looks_like_mbedtls_root(path)

def crypto_core_directory(root: Optional[str] = None, relative: Optional[bool] = False) -> str:
    """
    Return the path of the directory containing the PSA crypto core
    for either TF-PSA-Crypto or Mbed TLS.

    Returns either the full path or relative path depending on the
    "relative" boolean argument.
    """
    if root is None:
        root = guess_project_root()
    if looks_like_tf_psa_crypto_root(root):
        if relative:
            return "core"
        return os.path.join(root, "core")
    elif looks_like_mbedtls_root(root):
        if is_mbedtls_3_6():
            path = "library"
        else:
            path = "tf-psa-crypto/core"
        if relative:
            return path
        return os.path.join(root, path)
    else:
        raise Exception('Neither Mbed TLS nor TF-PSA-Crypto source tree found')

def crypto_library_filename(root: Optional[str] = None) -> str:
    """Return the crypto library filename for either TF-PSA-Crypto or Mbed TLS."""
    if root is None:
        root = guess_project_root()
    if looks_like_tf_psa_crypto_root(root):
        return "tfpsacrypto"
    elif looks_like_mbedtls_root(root):
        return "mbedcrypto"
    else:
        raise Exception('Neither Mbed TLS nor TF-PSA-Crypto source tree found')

def check_repo_path():
    """Check that the current working directory is the project root, and throw
    an exception if not.
    """
    if not all(os.path.isdir(d) for d in ["include", "library", "tests"]):
        raise Exception("This script must be run from Mbed TLS root")

def chdir_to_root() -> None:
    """Detect the root of the Mbed TLS or TF-PSA-Crypto source tree and change to it.

    The current directory must be up to two levels deep inside an Mbed TLS or
    TF-PSA-Crypto source tree.
    """
    for d in [os.path.curdir,
              os.path.pardir,
              os.path.join(os.path.pardir, os.path.pardir)]:
        if looks_like_root(d):
            os.chdir(d)
            return
    raise Exception('Mbed TLS or TF-PSA-Crypto source tree not found')

def guess_project_root():
    """Guess project source code directory.

    Return the first possible project root directory.
    """
    dirs = set({})
    for frame in inspect.stack():
        path = os.path.dirname(frame.filename)
        for d in ['.', os.path.pardir] \
                 + [os.path.join(*([os.path.pardir]*i)) for i in range(2, 10)]:
            d = os.path.abspath(os.path.join(path, d))
            if d in dirs:
                continue
            dirs.add(d)
            if looks_like_root(d):
                return d
    raise Exception('Neither Mbed TLS nor TF-PSA-Crypto source tree found')

def guess_mbedtls_root(root: Optional[str] = None) -> str:
    """Guess Mbed TLS source code directory.

    Return the first possible Mbed TLS root directory.
    Raise an exception if we are not in Mbed TLS.
    """
    if root is None:
        root = guess_project_root()
    if looks_like_mbedtls_root(root):
        return root
    else:
        raise Exception('Mbed TLS source tree not found')

def guess_tf_psa_crypto_root(root: Optional[str] = None) -> str:
    """Guess TF-PSA-Crypto source code directory.

    Return the first possible TF-PSA-Crypto root directory.
    Raise an exception if we are not in TF-PSA-Crypto.
    """
    if root is None:
        root = guess_project_root()
    if looks_like_tf_psa_crypto_root(root):
        return root
    else:
        raise Exception('TF-PSA-Crypto source tree not found')

def framework_root(root: Optional[str] = None) -> str:
    """Return the path to the framework directory for this project."""
    if root is None:
        root = guess_project_root()
    return os.path.join(root, 'framework')

def is_mbedtls_3_6() -> bool:
    """Whether the working tree is an Mbed TLS 3.6 one or not

    Return false if we are in TF-PSA-Crypto or in Mbed TLS but with a version
    different from 3.6.x.
    Raise an exception if we are neither in Mbed TLS nor in TF-PSA-Crypto.
    """
    root = guess_project_root()
    if not looks_like_mbedtls_root(root):
        return False
    with open(os.path.join(root, 'include', 'mbedtls', 'build_info.h'), 'r') as f:
        return re.search(r"#define MBEDTLS_VERSION_NUMBER.*0x0306", f.read()) is not None
