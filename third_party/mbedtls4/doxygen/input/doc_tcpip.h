/**
 * \file doc_tcpip.h
 *
 * \brief TCP/IP communication module documentation file.
 */
/*
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/**
 * @addtogroup tcpip_communication_module TCP/IP communication module
 *
 * The TCP/IP communication module provides for a channel of
 * communication for the \link ssltls_communication_module SSL/TLS communication
 * module\endlink to use.
 * In the TCP/IP-model it provides for communication up to the Transport
 * (or Host-to-host) layer.
 * SSL/TLS resides on top of that, in the Application layer, and makes use of
 * its basic provisions:
 * - listening on a port (see \c mbedtls_net_bind()).
 * - accepting a connection (through \c mbedtls_net_accept()).
 * - read/write (through \c mbedtls_net_recv()/\c mbedtls_net_send()).
 * - close a connection (through \c mbedtls_net_close()).
 *
 * This way you have the means to, for example, implement and use an UDP or
 * IPSec communication solution as a basis.
 *
 * This module can be used at server- and clientside to provide a basic
 * means of communication over the internet.
 */
