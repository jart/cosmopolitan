#!/usr/bin/env python3
"""
Purpose

This script dumps comb table of ec curve. When you add a new ec curve, you
can use this script to generate codes to define `<curve>_T` in ecp_curves.c
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import os
import subprocess
import sys
import tempfile

HOW_TO_ADD_NEW_CURVE = """
If you are trying to add new curve, you can follow these steps:

1. Define curve parameters (<curve>_p, <curve>_gx, etc...) in ecp_curves.c.
2. Add a macro to define <curve>_T to NULL following these parameters.
3. Build mbedcrypto
4. Run this script with an argument of new curve
5. Copy the output of this script into ecp_curves.c and replace the macro added
   in Step 2
6. Rebuild and test if everything is ok

Replace the <curve> in the above with the name of the curve you want to add."""

CC = os.getenv('CC', 'cc')
MBEDTLS_LIBRARY_PATH = os.getenv('MBEDTLS_LIBRARY_PATH', "library")

SRC_DUMP_COMB_TABLE = r'''
#include <stdio.h>
#include <stdlib.h>
#include "mbedtls/ecp.h"
#include "mbedtls/error.h"

static void dump_mpi_initialize( const char *name, const mbedtls_mpi *d )
{
    uint8_t buf[128] = {0};
    size_t olen;
    uint8_t *p;

    olen = mbedtls_mpi_size( d );
    mbedtls_mpi_write_binary_le( d, buf, olen );
    printf("static const mbedtls_mpi_uint %s[] = {\n", name);
    for (p = buf; p < buf + olen; p += 8) {
        printf( "    BYTES_TO_T_UINT_8( 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X ),\n",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );
    }
    printf("};\n");
}

static void dump_T( const mbedtls_ecp_group *grp )
{
    char name[128];

    printf( "#if MBEDTLS_ECP_FIXED_POINT_OPTIM == 1\n" );

    for (size_t i = 0; i < grp->T_size; ++i) {
        snprintf( name, sizeof(name), "%s_T_%zu_X", CURVE_NAME, i );
        dump_mpi_initialize( name, &grp->T[i].X );

        snprintf( name, sizeof(name), "%s_T_%zu_Y", CURVE_NAME, i );
        dump_mpi_initialize( name, &grp->T[i].Y );
    }
    printf( "static const mbedtls_ecp_point %s_T[%zu] = {\n", CURVE_NAME, grp->T_size );
    size_t olen;
    for (size_t i = 0; i < grp->T_size; ++i) {
        int z;
        if ( mbedtls_mpi_cmp_int(&grp->T[i].Z, 0) == 0 ) {
            z = 0;
        } else if ( mbedtls_mpi_cmp_int(&grp->T[i].Z, 1) == 0 ) {
            z = 1;
        } else {
            fprintf( stderr, "Unexpected value of Z (i = %d)\n", (int)i );
            exit( 1 );
        }
        printf( "    ECP_POINT_INIT_XY_Z%d(%s_T_%zu_X, %s_T_%zu_Y),\n",
                z,
                CURVE_NAME, i,
                CURVE_NAME, i
        );
    }
    printf("};\n#endif\n\n");
}

int main()
{
    int rc;
    mbedtls_mpi m;
    mbedtls_ecp_point R;
    mbedtls_ecp_group grp;

    mbedtls_ecp_group_init( &grp );
    rc = mbedtls_ecp_group_load( &grp, CURVE_ID );
    if (rc != 0) {
        char buf[100];
        mbedtls_strerror( rc, buf, sizeof(buf) );
        fprintf( stderr, "mbedtls_ecp_group_load: %s (-0x%x)\n", buf, -rc );
        return 1;
    }
    grp.T = NULL;
    mbedtls_ecp_point_init( &R );
    mbedtls_mpi_init( &m);
    mbedtls_mpi_lset( &m, 1 );
    rc = mbedtls_ecp_mul( &grp, &R, &m, &grp.G, NULL, NULL );
    if ( rc != 0 ) {
        char buf[100];
        mbedtls_strerror( rc, buf, sizeof(buf) );
        fprintf( stderr, "mbedtls_ecp_mul: %s (-0x%x)\n", buf, -rc );
        return 1;
    }
    if ( grp.T == NULL ) {
        fprintf( stderr, "grp.T is not generated. Please make sure"
                         "MBEDTLS_ECP_FIXED_POINT_OPTIM is enabled in mbedtls_config.h\n" );
        return 1;
    }
    dump_T( &grp );
    return 0;
}
'''

SRC_DUMP_KNOWN_CURVE = r'''
#include <stdio.h>
#include <stdlib.h>
#include "mbedtls/ecp.h"

int main() {
    const mbedtls_ecp_curve_info *info = mbedtls_ecp_curve_list();
    mbedtls_ecp_group grp;

    mbedtls_ecp_group_init( &grp );
    while ( info->name != NULL ) {
        mbedtls_ecp_group_load( &grp, info->grp_id );
        if ( mbedtls_ecp_get_type(&grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS ) {
            printf( " %s", info->name );
        }
        info++;
    }
    printf( "\n" );
    return 0;
}
'''


def join_src_path(*args):
    return os.path.normpath(os.path.join(os.path.dirname(__file__), "..", *args))


def run_c_source(src, cflags):
    """
    Compile and run C source code
    :param src: the c language code to run
    :param cflags: additional cflags passing to compiler
    :return:
    """
    binname = tempfile.mktemp(prefix="mbedtls")
    fd, srcname = tempfile.mkstemp(prefix="mbedtls", suffix=".c")
    srcfile = os.fdopen(fd, mode="w")
    srcfile.write(src)
    srcfile.close()
    args = [CC,
            *cflags,
            '-I' + join_src_path("include"),
            "-o", binname,
            '-L' + MBEDTLS_LIBRARY_PATH,
            srcname,
            '-lmbedcrypto']

    p = subprocess.run(args=args, check=False)
    if p.returncode != 0:
        return False
    p = subprocess.run(args=[binname], check=False, env={
        'LD_LIBRARY_PATH': MBEDTLS_LIBRARY_PATH
    })
    if p.returncode != 0:
        return False
    os.unlink(srcname)
    os.unlink(binname)
    return True


def compute_curve(curve):
    """compute comb table for curve"""
    r = run_c_source(
        SRC_DUMP_COMB_TABLE,
        [
            '-g',
            '-DCURVE_ID=MBEDTLS_ECP_DP_%s' % curve.upper(),
            '-DCURVE_NAME="%s"' % curve.lower(),
        ])
    if not r:
        print("""\
Unable to compile and run utility.""", file=sys.stderr)
        sys.exit(1)


def usage():
    print("""
Usage: python %s <curve>...

Arguments:
    curve       Specify one or more curve names (e.g secp256r1)

All possible curves: """ % sys.argv[0])
    run_c_source(SRC_DUMP_KNOWN_CURVE, [])
    print("""
Environment Variable:
    CC          Specify which c compile to use to compile utility.
    MBEDTLS_LIBRARY_PATH
                Specify the path to mbedcrypto library. (e.g. build/library/)

How to add a new curve: %s""" % HOW_TO_ADD_NEW_CURVE)


def run_main():
    shared_lib_path = os.path.normpath(os.path.join(MBEDTLS_LIBRARY_PATH, "libmbedcrypto.so"))
    static_lib_path = os.path.normpath(os.path.join(MBEDTLS_LIBRARY_PATH, "libmbedcrypto.a"))
    if not os.path.exists(shared_lib_path) and not os.path.exists(static_lib_path):
        print("Warning: both '%s' and '%s' are not exists. This script will use "
              "the library from your system instead of the library compiled by "
              "this source directory.\n"
              "You can specify library path using environment variable "
              "'MBEDTLS_LIBRARY_PATH'." % (shared_lib_path, static_lib_path),
              file=sys.stderr)

    if len(sys.argv) <= 1:
        usage()
    else:
        for curve in sys.argv[1:]:
            compute_curve(curve)


if __name__ == '__main__':
    run_main()
