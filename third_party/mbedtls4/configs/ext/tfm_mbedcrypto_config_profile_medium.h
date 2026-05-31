/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2023, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef PROFILE_M_MBEDTLS_CONFIG_H
#define PROFILE_M_MBEDTLS_CONFIG_H

#include "config_tfm.h"

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

/**
 * \name SECTION: General configuration options
 *
 * This section contains Mbed TLS build settings that are not associated
 * with a particular module.
 *
 * \{
 */

/**
 * \def MBEDTLS_CONFIG_FILE
 *
 * If defined, this is a header which will be included instead of
 * `"mbedtls/mbedtls_config.h"`.
 * This header file specifies the compile-time configuration of Mbed TLS.
 * Unlike other configuration options, this one must be defined on the
 * compiler command line: a definition in `mbedtls_config.h` would have
 * no effect.
 *
 * This macro is expanded after an <tt>\#include</tt> directive. This is a popular but
 * non-standard feature of the C language, so this feature is only available
 * with compilers that perform macro expansion on an <tt>\#include</tt> line.
 *
 * The value of this symbol is typically a path in double quotes, either
 * absolute or relative to a directory on the include search path.
 */
//#define MBEDTLS_CONFIG_FILE "mbedtls/mbedtls_config.h"

/**
 * \def MBEDTLS_USER_CONFIG_FILE
 *
 * If defined, this is a header which will be included after
 * `"mbedtls/mbedtls_config.h"` or #MBEDTLS_CONFIG_FILE.
 * This allows you to modify the default configuration, including the ability
 * to undefine options that are enabled by default.
 *
 * This macro is expanded after an <tt>\#include</tt> directive. This is a popular but
 * non-standard feature of the C language, so this feature is only available
 * with compilers that perform macro expansion on an <tt>\#include</tt> line.
 *
 * The value of this symbol is typically a path in double quotes, either
 * absolute or relative to a directory on the include search path.
 */
//#define MBEDTLS_USER_CONFIG_FILE "/dev/null"

/** \} name SECTION: General configuration options */

#endif /* PROFILE_M_MBEDTLS_CONFIG_H */
