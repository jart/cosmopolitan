# Miscellaneous tests of TLS 1.3 features.

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3: PSK: No valid ciphersuite. G->m" \
            "$P_SRV tls13_kex_modes=all debug_level=5 $(get_srv_psk_list)" \
            "$G_NEXT_CLI -d 10 --priority NORMAL:-VERS-ALL:-CIPHER-ALL:+AES-256-GCM:+AEAD:+SHA384:-KX-ALL:+ECDHE-PSK:+DHE-PSK:+PSK:+VERS-TLS1.3 \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70 \
                         localhost" \
            1 \
            -s "found psk key exchange modes extension" \
            -s "found pre_shared_key extension" \
            -s "Found PSK_EPHEMERAL KEX MODE" \
            -s "Found PSK KEX MODE" \
            -s "No matched ciphersuite"

requires_openssl_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3: PSK: No valid ciphersuite. O->m" \
            "$P_SRV tls13_kex_modes=all debug_level=5 $(get_srv_psk_list)" \
            "$O_NEXT_CLI -tls1_3 -msg -allow_no_dhe_kex -ciphersuites TLS_AES_256_GCM_SHA384\
                         -psk_identity Client_identity -psk 6162636465666768696a6b6c6d6e6f70" \
            1 \
            -s "found psk key exchange modes extension" \
            -s "found pre_shared_key extension" \
            -s "Found PSK_EPHEMERAL KEX MODE" \
            -s "Found PSK KEX MODE" \
            -s "No matched ciphersuite"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: Multiple PSKs: valid ticket, reconnect with ticket" \
         "$P_SRV tls13_kex_modes=psk_ephemeral debug_level=5 psk_identity=Client_identity psk=6162636465666768696a6b6c6d6e6f70 tickets=8" \
         "$P_CLI tls13_kex_modes=psk_ephemeral debug_level=5 psk_identity=Client_identity psk=6162636465666768696a6b6c6d6e6f70 reco_mode=1 reconnect=1" \
         0 \
         -c "Pre-configured PSK number = 2" \
         -s "sent selected_identity: 0" \
         -s "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -S "key exchange mode: ephemeral$" \
         -S "ticket is not authentic"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: Multiple PSKs: invalid ticket, reconnect with PSK" \
         "$P_SRV tls13_kex_modes=psk_ephemeral debug_level=5 psk_identity=Client_identity psk=6162636465666768696a6b6c6d6e6f70 tickets=8 dummy_ticket=1" \
         "$P_CLI tls13_kex_modes=psk_ephemeral debug_level=5 psk_identity=Client_identity psk=6162636465666768696a6b6c6d6e6f70 reco_mode=1 reconnect=1" \
         0 \
         -c "Pre-configured PSK number = 2" \
         -s "sent selected_identity: 1" \
         -s "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -S "key exchange mode: ephemeral$" \
         -s "ticket is not authentic"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3: G->m: ephemeral_all/psk, fail, no common kex mode" \
            "$P_SRV tls13_kex_modes=psk debug_level=5 $(get_srv_psk_list)" \
            "$G_NEXT_CLI -d 10 --priority NORMAL:-VERS-ALL:-KX-ALL:+ECDHE-PSK:+DHE-PSK:-PSK:+VERS-TLS1.3 \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70 \
                         localhost" \
            1 \
            -s "found psk key exchange modes extension" \
            -s "found pre_shared_key extension" \
            -s "Found PSK_EPHEMERAL KEX MODE" \
            -S "Found PSK KEX MODE" \
            -S "key exchange mode: psk$"  \
            -S "key exchange mode: psk_ephemeral"  \
            -S "key exchange mode: ephemeral"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_disabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_config_disabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: G->m: PSK: configured psk only, good." \
            "$P_SRV tls13_kex_modes=all debug_level=5 $(get_srv_psk_list)" \
            "$G_NEXT_CLI -d 10 --priority NORMAL:-VERS-ALL:-KX-ALL:+ECDHE-PSK:+DHE-PSK:+PSK:+VERS-TLS1.3:+GROUP-ALL \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70 \
                         localhost" \
            0 \
            -s "found psk key exchange modes extension" \
            -s "found pre_shared_key extension"         \
            -s "Found PSK_EPHEMERAL KEX MODE"           \
            -s "Found PSK KEX MODE"                     \
            -s "key exchange mode: psk$"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_config_disabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_disabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test    "TLS 1.3: G->m: PSK: configured psk_ephemeral only, good." \
            "$P_SRV tls13_kex_modes=all debug_level=5 $(get_srv_psk_list)" \
            "$G_NEXT_CLI -d 10 --priority NORMAL:-VERS-ALL:-KX-ALL:+ECDHE-PSK:+DHE-PSK:+PSK:+VERS-TLS1.3:+GROUP-ALL \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70 \
                         localhost" \
            0 \
            -s "found psk key exchange modes extension" \
            -s "found pre_shared_key extension"         \
            -s "Found PSK_EPHEMERAL KEX MODE"           \
            -s "Found PSK KEX MODE"                     \
            -s "key exchange mode: psk_ephemeral$"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_disabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_disabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test    "TLS 1.3: G->m: PSK: configured ephemeral only, good." \
            "$P_SRV tls13_kex_modes=all debug_level=5 $(get_srv_psk_list)" \
            "$G_NEXT_CLI -d 10 --priority NORMAL:-VERS-ALL:-KX-ALL:+ECDHE-PSK:+DHE-PSK:+PSK:+VERS-TLS1.3:+GROUP-ALL \
                         --pskusername Client_identity --pskkey=6162636465666768696a6b6c6d6e6f70 \
                         localhost" \
            0 \
            -s "key exchange mode: ephemeral$"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session... ok" \
         -c "HTTP/1.0 200 OK" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption with servername" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key \
            sni=localhost,../framework/data_files/server2.crt,../framework/data_files/server2.key,-,-,-,polarssl.example,../framework/data_files/server1-nospace.crt,../framework/data_files/server1.key,-,-,-" \
         "$P_CLI server_name=localhost reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session... ok" \
         -c "HTTP/1.0 200 OK" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption with ticket max lifetime (7d)" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key ticket_timeout=604800 tickets=1" \
         "$P_CLI reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session... ok" \
         -c "HTTP/1.0 200 OK" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_ciphersuite_enabled TLS1-3-AES-256-GCM-SHA384
run_test "TLS 1.3 m->m: resumption with AES-256-GCM-SHA384 only" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI force_ciphersuite=TLS1-3-AES-256-GCM-SHA384 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Ciphersuite is TLS1-3-AES-256-GCM-SHA384" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session... ok" \
         -c "HTTP/1.0 200 OK" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite: 1302 - TLS1-3-AES-256-GCM-SHA384"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption with early data" \
         "$P_SRV debug_level=4 early_data=1 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI debug_level=3 early_data=1 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "HTTP/1.0 200 OK" \
         -c "received max_early_data_size" \
         -c "NewSessionTicket: early_data(42) extension received." \
         -c "ClientHello: early_data(42) extension exists." \
         -c "EncryptedExtensions: early_data(42) extension received." \
         -c "bytes of early data written" \
         -C "0 bytes of early data written" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -s "Sent max_early_data_size" \
         -s "NewSessionTicket: early_data(42) extension exists." \
         -s "ClientHello: early_data(42) extension exists." \
         -s "EncryptedExtensions: early_data(42) extension exists." \
         -s "early data bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_ciphersuite_enabled TLS1-3-AES-256-GCM-SHA384
run_test "TLS 1.3 m->m: resumption with early data, AES-256-GCM-SHA384 only" \
         "$P_SRV debug_level=4 early_data=1 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI debug_level=3 force_ciphersuite=TLS1-3-AES-256-GCM-SHA384 early_data=1 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Ciphersuite is TLS1-3-AES-256-GCM-SHA384" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "HTTP/1.0 200 OK" \
         -c "received max_early_data_size" \
         -c "NewSessionTicket: early_data(42) extension received." \
         -c "ClientHello: early_data(42) extension exists." \
         -c "EncryptedExtensions: early_data(42) extension received." \
         -c "bytes of early data written" \
         -C "0 bytes of early data written" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite: 1302 - TLS1-3-AES-256-GCM-SHA384" \
         -s "Sent max_early_data_size" \
         -s "NewSessionTicket: early_data(42) extension exists." \
         -s "ClientHello: early_data(42) extension exists." \
         -s "EncryptedExtensions: early_data(42) extension exists." \
         -s "early data bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, early data cli-enabled/srv-default" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI debug_level=3 early_data=1 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "HTTP/1.0 200 OK" \
         -C "received max_early_data_size" \
         -C "NewSessionTicket: early_data(42) extension received." \
         -C "ClientHello: early_data(42) extension exists." \
         -C "EncryptedExtensions: early_data(42) extension received." \
         -c "0 bytes of early data written" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -S "Sent max_early_data_size" \
         -S "NewSessionTicket: early_data(42) extension exists." \
         -S "ClientHello: early_data(42) extension exists." \
         -S "EncryptedExtensions: early_data(42) extension exists." \
         -S "early data bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, early data cli-enabled/srv-disabled" \
         "$P_SRV debug_level=4 early_data=0 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI debug_level=3 early_data=1 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "HTTP/1.0 200 OK" \
         -C "received max_early_data_size" \
         -C "NewSessionTicket: early_data(42) extension received." \
         -C "ClientHello: early_data(42) extension exists." \
         -C "EncryptedExtensions: early_data(42) extension received." \
         -c "0 bytes of early data written" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -S "Sent max_early_data_size" \
         -S "NewSessionTicket: early_data(42) extension exists." \
         -S "ClientHello: early_data(42) extension exists." \
         -S "EncryptedExtensions: early_data(42) extension exists." \
         -S "early data bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, early data cli-default/srv-enabled" \
         "$P_SRV debug_level=4 early_data=1 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI debug_level=3 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "HTTP/1.0 200 OK" \
         -c "received max_early_data_size" \
         -c "NewSessionTicket: early_data(42) extension received." \
         -C "ClientHello: early_data(42) extension exists." \
         -C "EncryptedExtensions: early_data(42) extension received." \
         -C "bytes of early data written" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -s "Sent max_early_data_size" \
         -s "NewSessionTicket: early_data(42) extension exists." \
         -S "ClientHello: early_data(42) extension exists." \
         -S "EncryptedExtensions: early_data(42) extension exists." \
         -S "early data bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, early data cli-disabled/srv-enabled" \
         "$P_SRV debug_level=4 early_data=1 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key" \
         "$P_CLI debug_level=3 early_data=0 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "HTTP/1.0 200 OK" \
         -c "received max_early_data_size" \
         -c "NewSessionTicket: early_data(42) extension received." \
         -C "ClientHello: early_data(42) extension exists." \
         -C "EncryptedExtensions: early_data(42) extension received." \
         -C "bytes of early data written" \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -s "Sent max_early_data_size" \
         -s "NewSessionTicket: early_data(42) extension exists." \
         -S "ClientHello: early_data(42) extension exists." \
         -S "EncryptedExtensions: early_data(42) extension exists." \
         -S "early data bytes read"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, ticket lifetime too long (7d + 1s)" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key ticket_timeout=604801 tickets=1" \
         "$P_CLI reco_mode=1 reconnect=1" \
         1 \
         -c "Protocol is TLSv1.3" \
         -C "Saving session for reuse... ok" \
         -c "Reconnecting with saved session... failed" \
         -S "Protocol is TLSv1.3" \
         -S "key exchange mode: psk" \
         -S "Select PSK ciphersuite" \
         -s "Ticket lifetime (604801) is greater than 7 days."

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, ticket lifetime=0" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key ticket_timeout=0 tickets=1" \
         "$P_CLI debug_level=2 reco_mode=1 reconnect=1" \
         1 \
         -c "Protocol is TLSv1.3" \
         -C "Saving session for reuse... ok" \
         -c "Discard new session ticket" \
         -c "Reconnecting with saved session... failed" \
         -s "Protocol is TLSv1.3" \
         -S "key exchange mode: psk" \
         -S "Select PSK ciphersuite"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, servername check failed" \
         "$P_SRV debug_level=2 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key \
            sni=localhost,../framework/data_files/server2.crt,../framework/data_files/server2.key,-,-,-,polarssl.example,../framework/data_files/server1-nospace.crt,../framework/data_files/server1.key,-,-,-" \
         "$P_CLI debug_level=4 server_name=localhost reco_server_name=remote reco_mode=1 reconnect=1" \
         1 \
         -c "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -c "Hostname mismatch the session ticket, disable session resumption." \
         -s "Protocol is TLSv1.3" \
         -S "key exchange mode: psk" \
         -S "Select PSK ciphersuite"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, ticket auth failed." \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key tickets=8 dummy_ticket=1" \
         "$P_CLI reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -S "key exchange mode: psk" \
         -s "ticket is not authentic" \
         -S "ticket is expired" \
         -S "Invalid ticket creation time" \
         -S "Ticket age exceeds limitation" \
         -S "Ticket age outside tolerance window"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, ticket expired." \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key tickets=8 dummy_ticket=2" \
         "$P_CLI reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -S "key exchange mode: psk" \
         -S "ticket is not authentic" \
         -s "ticket is expired" \
         -S "Invalid ticket creation time" \
         -S "Ticket age exceeds limitation" \
         -S "Ticket age outside tolerance window"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, invalid creation time." \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key tickets=8 dummy_ticket=3" \
         "$P_CLI debug_level=4 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -S "key exchange mode: psk" \
         -S "ticket is not authentic" \
         -S "ticket is expired" \
         -s "Invalid ticket creation time" \
         -S "Ticket age exceeds limitation" \
         -S "Ticket age outside tolerance window"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, ticket expired, too old" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key tickets=8 dummy_ticket=4" \
         "$P_CLI debug_level=4 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -S "key exchange mode: psk" \
         -S "ticket is not authentic" \
         -S "ticket is expired" \
         -S "Invalid ticket creation time" \
         -s "Ticket age exceeds limitation" \
         -S "Ticket age outside tolerance window"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, age outside tolerance window, too young" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key tickets=8 dummy_ticket=5" \
         "$P_CLI debug_level=4 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -S "key exchange mode: psk" \
         -S "ticket is not authentic" \
         -S "ticket is expired" \
         -S "Invalid ticket creation time" \
         -S "Ticket age exceeds limitation" \
         -s "Ticket age outside tolerance window"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, age outside tolerance window, too old" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key tickets=8 dummy_ticket=6" \
         "$P_CLI debug_level=4 reco_mode=1 reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "Protocol is TLSv1.3" \
         -c "Saving session for reuse... ok" \
         -c "Reconnecting with saved session" \
         -S "key exchange mode: psk" \
         -S "ticket is not authentic" \
         -S "ticket is expired" \
         -S "Invalid ticket creation time" \
         -S "Ticket age exceeds limitation" \
         -s "Ticket age outside tolerance window"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test "TLS 1.3 m->m: resumption fails, cli/tkt kex modes psk/none" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=7" \
         "$P_CLI debug_level=4 tls13_kex_modes=psk_or_ephemeral reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -s "No suitable PSK key exchange mode" \
         -s "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test "TLS 1.3 m->m: ephemeral over psk resumption, cli/tkt kex modes psk/psk" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=8" \
         "$P_CLI debug_level=4 tls13_kex_modes=psk_or_ephemeral reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test "TLS 1.3 m->m: resumption fails, cli/tkt kex modes psk/psk_ephemeral" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=9" \
         "$P_CLI debug_level=4 tls13_kex_modes=psk_or_ephemeral reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -s "No suitable PSK key exchange mode" \
         -s "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test "TLS 1.3 m->m: ephemeral over psk resumption, cli/tkt kex modes psk/psk_all" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=10" \
         "$P_CLI debug_level=4 tls13_kex_modes=psk_or_ephemeral reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, cli/tkt kex modes psk_ephemeral/none" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=7" \
         "$P_CLI debug_level=4 tls13_kex_modes=ephemeral_all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -s "No suitable PSK key exchange mode" \
         -s "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, cli/tkt kex modes psk_ephemeral/psk" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=8" \
         "$P_CLI debug_level=4 tls13_kex_modes=ephemeral_all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -s "No suitable PSK key exchange mode" \
         -s "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, cli/tkt kex modes psk_ephemeral/psk_ephemeral" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=9" \
         "$P_CLI debug_level=4 tls13_kex_modes=ephemeral_all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, cli/tkt kex modes psk_ephemeral/psk_all" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=10" \
         "$P_CLI debug_level=4 tls13_kex_modes=ephemeral_all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption fails, cli/tkt kex modes psk_all/none" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=7" \
         "$P_CLI debug_level=4 tls13_kex_modes=all reconnect=1" \
         0 \
         -c "Pre-configured PSK number = 1" \
         -S "sent selected_identity:" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "No suitable PSK key exchange mode" \
         -s "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: ephemeral over psk resumption, cli/tkt kex modes psk_all/psk" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=8" \
         "$P_CLI debug_level=4 tls13_kex_modes=all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, cli/tkt kex modes psk_all/psk_ephemeral" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=9" \
         "$P_CLI debug_level=4 tls13_kex_modes=all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_config_enabled MBEDTLS_SSL_PROTO_TLS1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: resumption, cli/tkt kex modes psk_all/psk_all" \
         "$P_SRV debug_level=4 crt_file=../framework/data_files/server5.crt key_file=../framework/data_files/server5.key dummy_ticket=10" \
         "$P_CLI debug_level=4 tls13_kex_modes=all reconnect=1" \
         0 \
         -c "Protocol is TLSv1.3" \
         -s "key exchange mode: ephemeral" \
         -s "key exchange mode: psk_ephemeral" \
         -S "key exchange mode: psk$" \
         -s "found matched identity" \
         -S "No suitable PSK key exchange mode" \
         -S "No usable PSK or ticket"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->O: resumption" \
            "$O_NEXT_SRV -msg -tls1_3 -no_resume_ephemeral -no_cache --num_tickets 1" \
            "$P_CLI reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session... ok" \
            -c "HTTP/1.0 200 ok"

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_disabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "TLS 1.3 m->O: resumption fails, no ticket support" \
            "$O_NEXT_SRV -msg -tls1_3 -no_resume_ephemeral -no_cache --num_tickets 1" \
            "$P_CLI debug_level=3 reco_mode=1 reconnect=1" \
            1 \
            -c "Protocol is TLSv1.3" \
            -C "Saving session for reuse... ok" \
            -C "Reconnecting with saved session... ok" \
            -c "Ignore NewSessionTicket, not supported."

# No early data m->O tests for the time being. The option -early_data is needed
# to enable early data on OpenSSL server and it is not compatible with the
# -www option we usually use for testing with OpenSSL server (see
# O_NEXT_SRV_EARLY_DATA definition). In this configuration when running the
# ephemeral then ticket based scenario we use for early data testing the first
# handshake fails. The following skipped test is here to illustrate the kind
# of testing we would like to do.
# https://github.com/Mbed-TLS/mbedtls/issues/9582
skip_next_test
requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->O: resumption with early data" \
            "$O_NEXT_SRV_EARLY_DATA -msg -tls1_3 -no_resume_ephemeral -no_cache --num_tickets 1" \
            "$P_CLI debug_level=3 early_data=1 reco_mode=1 reconnect=1" \
             0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session" \
            -c "HTTP/1.0 200 OK" \
            -c "received max_early_data_size: 16384" \
            -c "NewSessionTicket: early_data(42) extension received." \
            -c "ClientHello: early_data(42) extension exists." \
            -c "EncryptedExtensions: early_data(42) extension received." \
            -c "bytes of early data written" \
            -s "decrypted early data with length:"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->G: resumption" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert" \
            "$P_CLI reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session... ok" \
            -c "HTTP/1.0 200 OK"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_config_disabled MBEDTLS_SSL_SESSION_TICKETS
run_test    "TLS 1.3 m->G: resumption fails, no ticket support" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert" \
            "$P_CLI debug_level=3 reco_mode=1 reconnect=1" \
            1 \
            -c "Protocol is TLSv1.3" \
            -C "Saving session for reuse... ok" \
            -C "Reconnecting with saved session... ok" \
            -c "Ignore NewSessionTicket, not supported."

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_ciphersuite_enabled TLS1-3-AES-256-GCM-SHA384
run_test    "TLS 1.3 m->G: resumption with AES-256-GCM-SHA384 only" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert" \
            "$P_CLI force_ciphersuite=TLS1-3-AES-256-GCM-SHA384 reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ciphersuite is TLS1-3-AES-256-GCM-SHA384" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session... ok" \
            -c "HTTP/1.0 200 OK"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->G: resumption with early data" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert \
                         --earlydata --maxearlydata 16384" \
            "$P_CLI debug_level=3 early_data=1 reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session" \
            -c "HTTP/1.0 200 OK" \
            -c "received max_early_data_size: 16384" \
            -c "NewSessionTicket: early_data(42) extension received." \
            -c "ClientHello: early_data(42) extension exists." \
            -c "EncryptedExtensions: early_data(42) extension received." \
            -c "bytes of early data written" \
            -s "decrypted early data with length:"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_ciphersuite_enabled TLS1-3-AES-256-GCM-SHA384
run_test    "TLS 1.3 m->G: resumption with early data, AES-256-GCM-SHA384 only" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert \
                         --earlydata --maxearlydata 16384" \
            "$P_CLI debug_level=3 force_ciphersuite=TLS1-3-AES-256-GCM-SHA384 early_data=1 reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Ciphersuite is TLS1-3-AES-256-GCM-SHA384" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session" \
            -c "HTTP/1.0 200 OK" \
            -c "received max_early_data_size: 16384" \
            -c "NewSessionTicket: early_data(42) extension received." \
            -c "ClientHello: early_data(42) extension exists." \
            -c "EncryptedExtensions: early_data(42) extension received." \
            -c "bytes of early data written" \
            -s "decrypted early data with length:"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->G: resumption, early data cli-enabled/srv-disabled" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:+CIPHER-ALL:+ECDHE-PSK:+PSK --disable-client-cert" \
            "$P_CLI debug_level=3 early_data=1 reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session" \
            -c "HTTP/1.0 200 OK" \
            -C "received max_early_data_size: 16384" \
            -C "NewSessionTicket: early_data(42) extension received." \

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->G: resumption, early data cli-default/srv-enabled" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert \
                         --earlydata --maxearlydata 16384" \
            "$P_CLI debug_level=3 reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session" \
            -c "HTTP/1.0 200 OK" \
            -c "received max_early_data_size: 16384" \
            -c "NewSessionTicket: early_data(42) extension received." \
            -C "ClientHello: early_data(42) extension exists." \

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
run_test    "TLS 1.3 m->G: resumption, early data cli-disabled/srv-enabled" \
            "$G_NEXT_SRV -d 5 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 --disable-client-cert \
                         --earlydata --maxearlydata 16384" \
            "$P_CLI debug_level=3 early_data=0 reco_mode=1 reconnect=1" \
            0 \
            -c "Protocol is TLSv1.3" \
            -c "Saving session for reuse... ok" \
            -c "Reconnecting with saved session" \
            -c "HTTP/1.0 200 OK" \
            -c "received max_early_data_size: 16384" \
            -c "NewSessionTicket: early_data(42) extension received." \
            -C "ClientHello: early_data(42) extension exists." \

requires_openssl_tls1_3_with_compatible_ephemeral
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
# https://github.com/openssl/openssl/issues/10714
# Until now, OpenSSL client does not support reconnect.
skip_next_test
run_test    "TLS 1.3 O->m: resumption" \
            "$P_SRV debug_level=2 tickets=1" \
            "$O_NEXT_CLI -msg -debug -tls1_3 -reconnect" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "key exchange mode: psk" \
            -s "Select PSK ciphersuite"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test    "TLS 1.3 G->m: resumption" \
            "$P_SRV debug_level=2 tickets=1" \
            "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -r" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "key exchange mode: psk" \
            -s "Select PSK ciphersuite"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_ciphersuite_enabled TLS1-3-AES-256-GCM-SHA384
# Test the session resumption when the cipher suite for the original session is
# TLS1-3-AES-256-GCM-SHA384. In that case, the PSK is 384 bits long and not
# 256 bits long as with all the other TLS 1.3 cipher suites.
run_test    "TLS 1.3 G->m: resumption with AES-256-GCM-SHA384 only" \
            "$P_SRV debug_level=2 tickets=1" \
            "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-256-GCM -V -r" \
            0 \
            -s "Protocol is TLSv1.3" \
            -s "key exchange mode: psk" \
            -s "Select PSK ciphersuite: 1302 - TLS1-3-AES-256-GCM-SHA384"

EARLY_DATA_INPUT_LEN_BLOCKS=$(( ( $( cat $EARLY_DATA_INPUT | wc -c ) + 31 ) / 32 ))
EARLY_DATA_INPUT_LEN=$(( $EARLY_DATA_INPUT_LEN_BLOCKS * 32 ))

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 G->m: resumption with early data" \
         "$P_SRV debug_level=4 tickets=1 early_data=1 max_early_data_size=$EARLY_DATA_INPUT_LEN" \
         "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -r \
                      --earlydata $EARLY_DATA_INPUT" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -s "Sent max_early_data_size=$EARLY_DATA_INPUT_LEN"        \
         -s "NewSessionTicket: early_data(42) extension exists."    \
         -s "ClientHello: early_data(42) extension exists."         \
         -s "EncryptedExtensions: early_data(42) extension exists." \
         -s "$( head -1 $EARLY_DATA_INPUT )"                        \
         -s "$( tail -1 $EARLY_DATA_INPUT )"                        \
         -s "200 early data bytes read"                             \
         -s "106 early data bytes read"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
requires_ciphersuite_enabled TLS1-3-AES-256-GCM-SHA384
run_test "TLS 1.3 G->m: resumption with early data, AES-256-GCM-SHA384 only" \
         "$P_SRV debug_level=4 tickets=1 early_data=1 max_early_data_size=$EARLY_DATA_INPUT_LEN" \
         "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-256-GCM -V -r \
                      --earlydata $EARLY_DATA_INPUT" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite: 1302 - TLS1-3-AES-256-GCM-SHA384" \
         -s "Sent max_early_data_size=$EARLY_DATA_INPUT_LEN"        \
         -s "NewSessionTicket: early_data(42) extension exists."    \
         -s "ClientHello: early_data(42) extension exists."         \
         -s "EncryptedExtensions: early_data(42) extension exists." \
         -s "$( head -1 $EARLY_DATA_INPUT )"                        \
         -s "$( tail -1 $EARLY_DATA_INPUT )"                        \
         -s "200 early data bytes read"                             \
         -s "106 early data bytes read"

# The Mbed TLS server does not allow early data for the ticket it sends but
# the GnuTLS indicates early data anyway when resuming with the ticket and
# sends early data. The Mbed TLS server does not expect early data in
# association with the ticket thus it eventually fails the resumption
# handshake. The GnuTLS client behavior is not compliant here with the TLS 1.3
# specification and thus its behavior may change in following versions.
requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 G->m: resumption, early data cli-enabled/srv-default" \
         "$P_SRV debug_level=4 tickets=1" \
         "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -r \
                      --earlydata $EARLY_DATA_INPUT" \
         1 \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -S "Sent max_early_data_size" \
         -S "NewSessionTicket: early_data(42) extension exists." \
         -s "ClientHello: early_data(42) extension exists." \
         -s "EarlyData: rejected, feature disabled in server configuration." \
         -S "EncryptedExtensions: early_data(42) extension exists." \
         -s "EarlyData: deprotect and discard app data records" \
         -s "EarlyData: Too much early data received"

# The Mbed TLS server does not allow early data for the ticket it sends but
# the GnuTLS indicates early data anyway when resuming with the ticket and
# sends early data. The Mbed TLS server does not expect early data in
# association with the ticket thus it eventually fails the resumption
# handshake. The GnuTLS client behavior is not compliant here with the TLS 1.3
# specification and thus its behavior may change in following versions.
requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 G->m: resumption, early data cli-enabled/srv-disabled" \
         "$P_SRV debug_level=4 tickets=1 early_data=0" \
         "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -r \
                      --earlydata $EARLY_DATA_INPUT" \
         1 \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -S "Sent max_early_data_size" \
         -S "NewSessionTicket: early_data(42) extension exists." \
         -s "ClientHello: early_data(42) extension exists." \
         -s "EarlyData: rejected, feature disabled in server configuration." \
         -S "EncryptedExtensions: early_data(42) extension exists." \
         -s "EarlyData: deprotect and discard app data records" \
         -s "EarlyData: Too much early data received"

requires_gnutls_tls1_3
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
requires_any_configs_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
                             MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
run_test "TLS 1.3 G->m: resumption, early data cli-disabled/srv-enabled" \
         "$P_SRV debug_level=4 tickets=1 early_data=1" \
         "$G_NEXT_CLI localhost -d 4 --priority=NORMAL:-VERS-ALL:+VERS-TLS1.3 -V -r" \
         0 \
         -s "Protocol is TLSv1.3" \
         -s "key exchange mode: psk" \
         -s "Select PSK ciphersuite" \
         -s "Sent max_early_data_size" \
         -s "NewSessionTicket: early_data(42) extension exists." \
         -S "ClientHello: early_data(42) extension exists." \
         -S "EncryptedExtensions: early_data(42) extension exists."

requires_config_enabled MBEDTLS_SSL_EARLY_DATA
requires_config_enabled MBEDTLS_SSL_SESSION_TICKETS
requires_config_enabled MBEDTLS_SSL_CLI_C
requires_config_enabled MBEDTLS_SSL_SRV_C
requires_config_enabled MBEDTLS_DEBUG_C
requires_config_enabled MBEDTLS_HAVE_TIME
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
requires_config_enabled MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
run_test "TLS 1.3 m->m: Ephemeral over PSK kex with early data enabled" \
         "$P_SRV force_version=tls13 debug_level=4 early_data=1 max_early_data_size=1024" \
         "$P_CLI debug_level=4 early_data=1 tls13_kex_modes=psk_or_ephemeral reco_mode=1 reconnect=1" \
         0 \
         -s "key exchange mode: ephemeral" \
         -S "key exchange mode: psk" \
         -s "found matched identity" \
         -s "EarlyData: rejected, not a session resumption" \
         -C "EncryptedExtensions: early_data(42) extension exists."
