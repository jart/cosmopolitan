#!/usr/bin/env python3

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

"""Module generating EC and RSA keys to be used in test_suite_pk instead of
generating the required key at run time. This helps speeding up testing."""

from typing import Iterator, List, Tuple
import re
import argparse
from mbedtls_framework.asymmetric_key_data import ASYMMETRIC_KEY_DATA
from mbedtls_framework import build_tree

BYTES_PER_LINE = 16

def c_byte_array_literal_content(array_name: str, key_data: bytes) -> Iterator[str]:
    """Return C code that defines array_name as a byte array with the given content."""
    yield 'static const unsigned char '
    yield array_name
    yield '[] = {'
    for index in range(0, len(key_data), BYTES_PER_LINE):
        yield '\n   '
        for b in key_data[index:index + BYTES_PER_LINE]:
            yield ' {:#04x},'.format(b)
    yield '\n};'

def convert_der_to_c(array_name: str, key_data: bytes) -> str:
    return ''.join(c_byte_array_literal_content(array_name, key_data))

def get_key_type(key_type: str) -> str:
    """Short name for a PSA key type."""
    if key_type.startswith('PSA_KEY_TYPE_ECC_'):
        return "ec"
    elif key_type.startswith('PSA_KEY_TYPE_ML_DSA_'):
        return "mldsa"
    elif key_type.startswith('PSA_KEY_TYPE_ML_KEM_'):
        return "mlkem"
    elif key_type.startswith('PSA_KEY_TYPE_RSA_'):
        return "rsa"
    elif key_type.startswith('PSA_KEY_TYPE_SLH_DSA_'):
        return "slhdsa"
    else:
        raise Exception(f"Unhandled key type {key_type}")

def get_ec_key_family(key: str) -> str:
    """Extract "PSA_ECC_xxx" from "PSA_KEY_TYPE_ECC_ttt(PSA_ECC_xxx)"."""
    match = re.search(r'.*\((.*)\)', key)
    if match is None:
        raise Exception("Unable to get EC family from {}".format(key))
    return match.group(1)

# Legacy EC group ID do not support all the key types that PSA does, so the
# following dictionaries are used for:
# - getting prefix/suffix for legacy curve names
# - understand if the curve is supported in legacy symbols (MBEDTLS_ECP_DP_...)
EC_NAME_CONVERSION = {
    'PSA_ECC_FAMILY_SECP_K1': {
        192: ('secp', 'k1'),
        256: ('secp', 'k1')
    },
    'PSA_ECC_FAMILY_SECP_R1': {
        192: ('secp', 'r1'),
        224: ('secp', 'r1'),
        256: ('secp', 'r1'),
        384: ('secp', 'r1'),
        521: ('secp', 'r1')
    },
    'PSA_ECC_FAMILY_BRAINPOOL_P_R1': {
        256: ('bp', 'r1'),
        384: ('bp', 'r1'),
        512: ('bp', 'r1')
    },
    'PSA_ECC_FAMILY_MONTGOMERY': {
        255: ('curve', '19'),
        448: ('curve', '')
    }
}

def get_ec_curve_name(priv_key: str, bits: int) -> str:
    """Short name for an elliptic curve key type."""
    ec_family = get_ec_key_family(priv_key)
    try:
        prefix = EC_NAME_CONVERSION[ec_family][bits][0]
        suffix = EC_NAME_CONVERSION[ec_family][bits][1]
    except KeyError:
        return ""
    return prefix + str(bits) + suffix

def get_slh_dsa_family(key_type: str) -> str:
    """Short name from an SLH-DSA family."""
    m = re.search(r'PSA_SLH_FAMILY_(\w+)', key_type)
    assert m
    return m.group(1).replace('_', '').lower()

def get_look_up_table_entry(key_type: str, group_id_or_keybits: str,
                            priv_array_name: str, pub_array_name: str) -> Iterator[str]:
    """Yield C code lines for the definition of a key pair and its matching public key."""
    if key_type == "ec":
        yield "    {{ {}, 0,\n".format(group_id_or_keybits)
    else:
        yield "    {{ 0, {},\n".format(group_id_or_keybits)
    yield "      {0}, sizeof({0}),\n".format(priv_array_name)
    yield "      {0}, sizeof({0}) }},".format(pub_array_name)


def write_output_file(output_file_name: str, arrays: str, look_up_table: str):
    """Write generated content to the output file"""
    with open(output_file_name, 'wt') as output:
        output.write("""\
/*********************************************************************************
 * This file was automatically generated from framework/scripts/generate_test_keys.py.
 * Please do not edit it manually.
 *********************************************************************************/

#ifndef TEST_TEST_KEYS_H
#define TEST_TEST_KEYS_H

#if TF_PSA_CRYPTO_VERSION_MAJOR >= 1
#include <tf_psa_crypto_common.h>
#include <mbedtls/private/ecp.h>
#else
#include <common.h>
#include <mbedtls/ecp.h>
#endif

""")
        output.write(arrays)
        output.write("""
struct predefined_key_element {{
    int group_id;  // EC group ID; 0 for RSA keys
    int keybits;  // bits size of RSA key; 0 for EC keys
    const unsigned char *priv_key;
    size_t priv_key_len;
    const unsigned char *pub_key;
    size_t pub_key_len;
}};

MBEDTLS_MAYBE_UNUSED static struct predefined_key_element predefined_keys[] = {{
{}
}};

#endif /* TEST_TEST_KEYS_H */

/* End of generated file */
""".format(look_up_table))

def collect_keys() -> Tuple[str, str]:
    """"
    This function reads key data from ASYMMETRIC_KEY_DATA and, only for the
    keys supported in legacy ECP/RSA modules, it returns 2 strings:
    - the 1st contains C arrays declaration of these keys and
    - the 2nd contains the final look-up table for all these arrays.
    """
    arrays = []
    look_up_table = []

    # Get a list of private keys only in order to get a single item for every
    # (key type, key bits) pair. We know that ASYMMETRIC_KEY_DATA
    # contains also the public counterpart.
    priv_keys = [key for key in ASYMMETRIC_KEY_DATA if '_KEY_PAIR' in key]
    priv_keys = sorted(priv_keys)

    for priv_key in priv_keys:
        key_type = get_key_type(priv_key)
        pub_key = re.sub('_KEY_PAIR', '_PUBLIC_KEY', priv_key)

        for bits in ASYMMETRIC_KEY_DATA[priv_key]:
            if key_type == "ec":
                curve = get_ec_curve_name(priv_key, bits)
                # Ignore EC curves unsupported in legacy symbols
                if curve == "":
                    continue
            # Create output array name
            if key_type == "ec":
                array_name_base = "_".join(["test", key_type, curve])
            elif key_type == "slhdsa":
                family = get_slh_dsa_family(priv_key)
                array_name_base = "_".join(["test", key_type, family, str(bits)])
            else:
                array_name_base = "_".join(["test", key_type, str(bits)])
            array_name_priv = array_name_base + "_priv"
            array_name_pub = array_name_base + "_pub"
            # Convert bytearray to C array
            c_array_priv = convert_der_to_c(array_name_priv, ASYMMETRIC_KEY_DATA[priv_key][bits])
            c_array_pub = convert_der_to_c(array_name_pub, ASYMMETRIC_KEY_DATA[pub_key][bits])
            # Write the C array to the output file
            arrays.append(''.join(["\n", c_array_priv, "\n", c_array_pub, "\n"]))
            # Update the lookup table
            if key_type == "ec":
                group_id_or_keybits = "MBEDTLS_ECP_DP_" + curve.upper()
            else:
                group_id_or_keybits = str(bits)
            look_up_table.append(''.join(get_look_up_table_entry(key_type, group_id_or_keybits,
                                                                 array_name_priv, array_name_pub)))

    return ''.join(arrays), '\n'.join(look_up_table)

def main() -> None:
    """Command line entry point."""
    default_output_path = build_tree.guess_project_root() + "/tests/include/test/test_keys.h"

    argparser = argparse.ArgumentParser()
    argparser.add_argument("--output", help="Output file", default=default_output_path)
    args = argparser.parse_args()

    output_file = args.output

    # Support for 224 bit EC curves (secp224r1 and secp224k1) was removed from
    # tf-psa-crypto. It only remains available for 3.6 LTS branch.
    if not build_tree.is_mbedtls_3_6():
        del EC_NAME_CONVERSION['PSA_ECC_FAMILY_SECP_R1'][224]
        del EC_NAME_CONVERSION['PSA_ECC_FAMILY_SECP_R1'][192]
        del EC_NAME_CONVERSION['PSA_ECC_FAMILY_SECP_K1'][192]

    arrays, look_up_table = collect_keys()

    write_output_file(output_file, arrays, look_up_table)

if __name__ == '__main__':
    main()
