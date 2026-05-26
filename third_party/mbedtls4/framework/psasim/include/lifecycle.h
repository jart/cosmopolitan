/* PSA lifecycle states used by psasim. */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#define PSA_LIFECYCLE_PSA_STATE_MASK (0xff00u)
#define PSA_LIFECYCLE_IMP_STATE_MASK (0x00ffu)
#define PSA_LIFECYCLE_UNKNOWN (0x0000u)
#define PSA_LIFECYCLE_ASSEMBLY_AND_TEST (0x1000u)
#define PSA_LIFECYCLE_PSA_ROT_PROVISIONING (0x2000u)
#define PSA_LIFECYCLE_SECURED (0x3000u)
#define PSA_LIFECYCLE_NON_PSA_ROT_DEBUG (0x4000u)
#define PSA_LIFECYCLE_RECOVERABLE_PSA_ROT_DEBUG (0x5000u)
#define PSA_LIFECYCLE_DECOMMISSIONED (0x6000u)
#define psa_rot_lifecycle_state(void) PSA_LIFECYCLE_UNKNOWN
