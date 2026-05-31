#!/usr/bin/env python3

# translate_ciphers.py
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

"""
Translate standard ciphersuite names to GnuTLS, OpenSSL and Mbed TLS standards.

To test the translation functions run:
python3 -m unittest translate_cipher.py
"""

import re
import argparse
import unittest

class TestTranslateCiphers(unittest.TestCase):
    """
    Ensure translate_ciphers.py translates and formats ciphersuite names
    correctly
    """
    def test_translate_all_cipher_names(self):
        """
        Translate standard ciphersuite names to GnuTLS, OpenSSL and
        Mbed TLS counterpart. Use only a small subset of ciphers
        that exercise each step of the translation functions
        """
        ciphers = [
            ("TLS_ECDHE_ECDSA_WITH_NULL_SHA",
             "+ECDHE-ECDSA:+NULL:+SHA1",
             "ECDHE-ECDSA-NULL-SHA",
             "TLS-ECDHE-ECDSA-WITH-NULL-SHA"),
            ("TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256",
             "+ECDHE-ECDSA:+AES-128-GCM:+AEAD",
             "ECDHE-ECDSA-AES128-GCM-SHA256",
             "TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256"),
            ("TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA",
             "+DHE-RSA:+3DES-CBC:+SHA1",
             "EDH-RSA-DES-CBC3-SHA",
             "TLS-DHE-RSA-WITH-3DES-EDE-CBC-SHA"),
            ("TLS_RSA_WITH_AES_256_CBC_SHA",
             "+RSA:+AES-256-CBC:+SHA1",
             "AES256-SHA",
             "TLS-RSA-WITH-AES-256-CBC-SHA"),
            ("TLS_PSK_WITH_3DES_EDE_CBC_SHA",
             "+PSK:+3DES-CBC:+SHA1",
             "PSK-3DES-EDE-CBC-SHA",
             "TLS-PSK-WITH-3DES-EDE-CBC-SHA"),
            ("TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256",
             None,
             "ECDHE-ECDSA-CHACHA20-POLY1305",
             "TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256"),
            ("TLS_ECDHE_ECDSA_WITH_AES_128_CCM",
             "+ECDHE-ECDSA:+AES-128-CCM:+AEAD",
             None,
             "TLS-ECDHE-ECDSA-WITH-AES-128-CCM"),
            ("TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384",
             None,
             "ECDHE-ARIA256-GCM-SHA384",
             "TLS-ECDHE-RSA-WITH-ARIA-256-GCM-SHA384"),
        ]

        for s, g_exp, o_exp, m_exp in ciphers:

            if g_exp is not None:
                g = translate_gnutls(s)
                self.assertEqual(g, g_exp)

            if o_exp is not None:
                o = translate_ossl(s)
                self.assertEqual(o, o_exp)

            if m_exp is not None:
                m = translate_mbedtls(s)
                self.assertEqual(m, m_exp)

def translate_gnutls(s_cipher):
    """
    Translate s_cipher from standard ciphersuite naming convention
    and return the GnuTLS naming convention
    """

    # Replace "_" with "-" to handle ciphersuite names based on Mbed TLS
    # naming convention
    s_cipher = s_cipher.replace("_", "-")

    s_cipher = re.sub(r'\ATLS-', '+', s_cipher)
    s_cipher = s_cipher.replace("-WITH-", ":+")
    s_cipher = s_cipher.replace("-EDE", "")

    # SHA in Mbed TLS == SHA1 GnuTLS,
    # if the last 3 chars are SHA append 1
    if s_cipher[-3:] == "SHA":
        s_cipher = s_cipher+"1"

    # CCM or CCM-8 should be followed by ":+AEAD"
    # Replace "GCM:+SHAxyz" with "GCM:+AEAD"
    if "CCM" in s_cipher or "GCM" in s_cipher:
        s_cipher = re.sub(r"GCM-SHA\d\d\d", "GCM", s_cipher)
        s_cipher = s_cipher+":+AEAD"

    # Replace the last "-" with ":+"
    else:
        index = s_cipher.rindex("-")
        s_cipher = s_cipher[:index] + ":+" + s_cipher[index+1:]

    return s_cipher

def translate_ossl(s_cipher):
    """
    Translate s_cipher from standard ciphersuite naming convention
    and return the OpenSSL naming convention
    """

    # Replace "_" with "-" to handle ciphersuite names based on Mbed TLS
    # naming convention
    s_cipher = s_cipher.replace("_", "-")

    s_cipher = re.sub(r'^TLS-', '', s_cipher)
    s_cipher = s_cipher.replace("-WITH", "")

    # Remove the "-" from "ABC-xyz"
    s_cipher = s_cipher.replace("AES-", "AES")
    s_cipher = s_cipher.replace("CAMELLIA-", "CAMELLIA")
    s_cipher = s_cipher.replace("ARIA-", "ARIA")

    # Remove "RSA" if it is at the beginning
    s_cipher = re.sub(r'^RSA-', r'', s_cipher)

    # For all circumstances outside of PSK
    if "PSK" not in s_cipher:
        s_cipher = s_cipher.replace("-EDE", "")
        s_cipher = s_cipher.replace("3DES-CBC", "DES-CBC3")

        # Remove "CBC" if it is not prefixed by DES
        s_cipher = re.sub(r'(?<!DES-)CBC-', r'', s_cipher)

    # ECDHE-RSA-ARIA does not exist in OpenSSL
    s_cipher = s_cipher.replace("ECDHE-RSA-ARIA", "ECDHE-ARIA")

    # POLY1305 should not be followed by anything
    if "POLY1305" in s_cipher:
        index = s_cipher.rindex("POLY1305")
        s_cipher = s_cipher[:index+8]

    # If DES is being used, Replace DHE with EDH
    if "DES" in s_cipher and "DHE" in s_cipher and "ECDHE" not in s_cipher:
        s_cipher = s_cipher.replace("DHE", "EDH")

    return s_cipher

def translate_mbedtls(s_cipher):
    """
    Translate s_cipher from standard ciphersuite naming convention
    and return Mbed TLS ciphersuite naming convention
    """

    # Replace "_" with "-"
    s_cipher = s_cipher.replace("_", "-")

    return s_cipher

def format_ciphersuite_names(mode, names):
    t = {"g": translate_gnutls,
         "o": translate_ossl,
         "m": translate_mbedtls
        }[mode]
    return " ".join(c + '=' + t(c) for c in names)

def main(target, names):
    print(format_ciphersuite_names(target, names))

if __name__ == "__main__":
    PARSER = argparse.ArgumentParser()
    PARSER.add_argument('target', metavar='TARGET', choices=['o', 'g', 'm'])
    PARSER.add_argument('names', metavar='NAMES', nargs='+')
    ARGS = PARSER.parse_args()
    main(ARGS.target, ARGS.names)
