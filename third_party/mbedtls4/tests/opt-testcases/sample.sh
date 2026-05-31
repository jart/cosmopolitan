# Test that SSL sample programs can interoperate with each other
# and with OpenSSL and GnuTLS.

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

: ${PROGRAMS_DIR:=../programs/ssl}

# Disable session tickets for ssl_client1 when potentially using TLS 1.3
# until https://github.com/Mbed-TLS/mbedtls/issues/6640 is resolved
# and (if relevant) implemented in ssl_client1.
run_test    "Sample: ssl_client1, ssl_server2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server2 tickets=0" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

requires_protocol_version tls12
run_test    "Sample: ssl_client1, openssl server, TLS 1.2" \
            -P 4433 \
            "$O_SRV -tls1_2" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -c "Protocol.*TLSv1.2" \
            -S "ERROR" \
            -C "error"

requires_protocol_version tls12
run_test    "Sample: ssl_client1, gnutls server, TLS 1.2" \
            -P 4433 \
            "$G_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -s "Version: TLS1.2" \
            -c "<TD>Protocol version:</TD><TD>TLS1.2</TD>" \
            -S "Error" \
            -C "error"

# Disable session tickets for ssl_client1 when using TLS 1.3
# until https://github.com/Mbed-TLS/mbedtls/issues/6640 is resolved
# and (if relevant) implemented in ssl_client1.
requires_protocol_version tls13
requires_openssl_tls1_3
run_test    "Sample: ssl_client1, openssl server, TLS 1.3" \
            -P 4433 \
            "$O_NEXT_SRV -tls1_3 -num_tickets 0" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -c "New, TLSv1.3, Cipher is" \
            -S "ERROR" \
            -C "error"

# Disable session tickets for ssl_client1 when using TLS 1.3
# until https://github.com/Mbed-TLS/mbedtls/issues/6640 is resolved
# and (if relevant) implemented in ssl_client1.
requires_protocol_version tls13
requires_gnutls_tls1_3
run_test    "Sample: ssl_client1, gnutls server, TLS 1.3" \
            -P 4433 \
            "$G_NEXT_SRV --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.3 --noticket" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -s "Version: TLS1.3" \
            -c "<TD>Protocol version:</TD><TD>TLS1.3</TD>" \
            -S "Error" \
            -C "error"

# The server complains of extra data after it closes the connection
# because the client keeps sending data, so the server receives
# more application data when it expects a new handshake. We consider
# the test a success if both sides have sent and received application
# data, no matter what happens afterwards.
run_test    "Sample: dtls_client, ssl_server2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server2 dtls=1 server_addr=localhost" \
            "$PROGRAMS_DIR/dtls_client" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -C "error"

# The dtls_client program connects to localhost. This test case fails on
# systems where the name "localhost" resolves to an IPv6 address, but
# the IPv6 connection is not possible. Possible reasons include:
# * OpenSSL is too old (IPv6 support was added in 1.1.0).
# * OpenSSL was built without IPv6 support.
# * A firewall blocks IPv6.
#
# To facilitate working with this test case, have it run with $OPENSSL_NEXT
# which is at least 1.1.1a. At the time it was introduced, this test case
# passed with OpenSSL 1.0.2g on an environment where IPv6 is disabled.
requires_protocol_version dtls12
run_test    "Sample: dtls_client, openssl server, DTLS 1.2" \
            -P 4433 \
            "$O_NEXT_SRV -dtls1_2" \
            "$PROGRAMS_DIR/dtls_client" \
            0 \
            -s "Echo this" \
            -c "Echo this" \
            -c "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -S "ERROR" \
            -C "error"

requires_protocol_version dtls12
run_test    "Sample: dtls_client, gnutls server, DTLS 1.2" \
            -P 4433 \
            "$G_SRV -u --echo --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2" \
            "$PROGRAMS_DIR/dtls_client" \
            0 \
            -s "Server listening" \
            -s "[1-9][0-9]* bytes command:" \
            -c "Echo this" \
            -c "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -S "Error" \
            -C "error"

run_test    "Sample: ssl_server, ssl_client2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server" \
            "$PROGRAMS_DIR/ssl_client2" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

run_test    "Sample: ssl_client1 with ssl_server" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

requires_protocol_version tls12
run_test    "Sample: ssl_server, openssl client, TLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server" \
            "$O_CLI -tls1_2" \
            0 \
            -s "Successful connection using: TLS-" \
            -c "Protocol.*TLSv1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls12
run_test    "Sample: ssl_server, gnutls client, TLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server" \
            "$G_CLI --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2 localhost" \
            0 \
            -s "Successful connection using: TLS-" \
            -c "Description:.*TLS1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls13
requires_openssl_tls1_3
run_test    "Sample: ssl_server, openssl client, TLS 1.3" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server" \
            "$O_NEXT_CLI -tls1_3" \
            0 \
            -s "Successful connection using: TLS1-3-" \
            -c "New, TLSv1.3, Cipher is" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls13
requires_gnutls_tls1_3
run_test    "Sample: ssl_server, gnutls client, TLS 1.3" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_server" \
            "$G_NEXT_CLI --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.3 localhost" \
            0 \
            -s "Successful connection using: TLS1-3-" \
            -c "Description:.*TLS1.3" \
            -S "error" \
            -C "ERROR"

run_test    "Sample: ssl_fork_server, ssl_client2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_fork_server" \
            "$PROGRAMS_DIR/ssl_client2" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

run_test    "Sample: ssl_client1 with ssl_fork_server" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_fork_server" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

requires_protocol_version tls12
run_test    "Sample: ssl_fork_server, openssl client, TLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_fork_server" \
            "$O_CLI -tls1_2" \
            0 \
            -s "Successful connection using: TLS-" \
            -c "Protocol.*TLSv1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls12
run_test    "Sample: ssl_fork_server, gnutls client, TLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_fork_server" \
            "$G_CLI --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2 localhost" \
            0 \
            -s "Successful connection using: TLS-" \
            -c "Description:.*TLS1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls13
requires_openssl_tls1_3
run_test    "Sample: ssl_fork_server, openssl client, TLS 1.3" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_fork_server" \
            "$O_NEXT_CLI -tls1_3" \
            0 \
            -s "Successful connection using: TLS1-3-" \
            -c "New, TLSv1.3, Cipher is" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls13
requires_gnutls_tls1_3
run_test    "Sample: ssl_fork_server, gnutls client, TLS 1.3" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_fork_server" \
            "$G_NEXT_CLI --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.3 localhost" \
            0 \
            -s "Successful connection using: TLS1-3-" \
            -c "Description:.*TLS1.3" \
            -S "error" \
            -C "ERROR"

run_test    "Sample: ssl_pthread_server, ssl_client2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_pthread_server" \
            "$PROGRAMS_DIR/ssl_client2" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

run_test    "Sample: ssl_client1 with ssl_pthread_server" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_pthread_server" \
            "$PROGRAMS_DIR/ssl_client1" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

requires_protocol_version tls12
run_test    "Sample: ssl_pthread_server, openssl client, TLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_pthread_server" \
            "$O_CLI -tls1_2" \
            0 \
            -s "Successful connection using: TLS-" \
            -c "Protocol.*TLSv1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls12
run_test    "Sample: ssl_pthread_server, gnutls client, TLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_pthread_server" \
            "$G_CLI --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2 localhost" \
            0 \
            -s "Successful connection using: TLS-" \
            -c "Description:.*TLS1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls13
requires_openssl_tls1_3
run_test    "Sample: ssl_pthread_server, openssl client, TLS 1.3" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_pthread_server" \
            "$O_NEXT_CLI -tls1_3" \
            0 \
            -s "Successful connection using: TLS1-3-" \
            -c "New, TLSv1.3, Cipher is" \
            -S "error" \
            -C "ERROR"

requires_protocol_version tls13
requires_gnutls_tls1_3
run_test    "Sample: ssl_pthread_server, gnutls client, TLS 1.3" \
            -P 4433 \
            "$PROGRAMS_DIR/ssl_pthread_server" \
            "$G_NEXT_CLI --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.3 localhost" \
            0 \
            -s "Successful connection using: TLS1-3-" \
            -c "Description:.*TLS1.3" \
            -S "error" \
            -C "ERROR"

run_test    "Sample: dtls_client with dtls_server" \
            -P 4433 \
            "$PROGRAMS_DIR/dtls_server" \
            "$PROGRAMS_DIR/dtls_client" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

run_test    "Sample: ssl_client2, dtls_server" \
            -P 4433 \
            "$PROGRAMS_DIR/dtls_server" \
            "$PROGRAMS_DIR/ssl_client2 dtls=1" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "[1-9][0-9]* bytes read" \
            -c "[1-9][0-9]* bytes written" \
            -S "error" \
            -C "error"

requires_protocol_version dtls12
run_test    "Sample: dtls_server, openssl client, DTLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/dtls_server" \
            "$O_CLI -dtls1_2" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "Protocol.*TLSv1.2" \
            -S "error" \
            -C "ERROR"

requires_protocol_version dtls12
run_test    "Sample: dtls_server, gnutls client, DTLS 1.2" \
            -P 4433 \
            "$PROGRAMS_DIR/dtls_server" \
            "$G_CLI -u --priority=NORMAL:-VERS-TLS-ALL:+VERS-TLS1.2 localhost" \
            0 \
            -s "[1-9][0-9]* bytes read" \
            -s "[1-9][0-9]* bytes written" \
            -c "Description:.*DTLS1.2" \
            -S "error" \
            -C "ERROR"
