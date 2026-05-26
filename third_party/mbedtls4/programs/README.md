Mbed TLS sample programs
========================

This subdirectory mostly contains sample programs that illustrate specific features of the library, as well as a few test and support programs.

We try to ensure that the sample programs are good examples of how to use Mbed TLS but we make no hard guarantees about their security. They should not be used in production unless they have been separately tested and thoroughly audited for security. Note that this means vulnerabilities in the sample programs are out of the scope of our usual security process and will be fixed in public.

### SSL/TLS sample applications

* [`ssl/dtls_client.c`](ssl/dtls_client.c): a simple DTLS client program, which sends one datagram to the server and reads one datagram in response.

* [`ssl/dtls_server.c`](ssl/dtls_server.c): a simple DTLS server program, which expects one datagram from the client and writes one datagram in response. This program supports DTLS cookies for hello verification.

* [`ssl/mini_client.c`](ssl/mini_client.c): a minimalistic SSL client, which sends a short string and disconnects. This is primarily intended as a benchmark; for a better example of a typical TLS client, see `ssl/ssl_client1.c`.

* [`ssl/ssl_client1.c`](ssl/ssl_client1.c): a simple HTTPS client that sends a fixed request and displays the response.

* [`ssl/ssl_fork_server.c`](ssl/ssl_fork_server.c): a simple HTTPS server using one process per client to send a fixed response. This program requires a Unix/POSIX environment implementing the `fork` system call.

* [`ssl/ssl_mail_client.c`](ssl/ssl_mail_client.c): a simple SMTP-over-TLS or SMTP-STARTTLS client. This client sends an email with fixed content.

* [`ssl/ssl_pthread_server.c`](ssl/ssl_pthread_server.c): a simple HTTPS server using one thread per client to send a fixed response. This program requires the pthread library.

* [`ssl/ssl_server.c`](ssl/ssl_server.c): a simple HTTPS server that sends a fixed response. It serves a single client at a time.

### SSL/TLS feature demonstrators

Note: unlike most of the other programs under the `programs/` directory, these two programs are not intended as a basis for writing an application. They combine most of the features supported by the library, and most applications require only a few features. To write a new application, we recommended that you start with `ssl_client1.c` or `ssl_server.c`, and then look inside `ssl/ssl_client2.c` or `ssl/ssl_server2.c` to see how to use the specific features that your application needs.

* [`ssl/ssl_client2.c`](ssl/ssl_client2.c): an HTTPS client that sends a fixed request and displays the response, with options to select TLS protocol features and Mbed TLS library features.

* [`ssl/ssl_server2.c`](ssl/ssl_server2.c): an HTTPS server that sends a fixed response, with options to select TLS protocol features and Mbed TLS library features.

In addition to providing options for testing client-side features, the `ssl_client2` program has options that allow you to trigger certain behaviors in the server. For example, there are options to select ciphersuites, or to force a renegotiation. These options are useful for testing the corresponding features in a TLS server. Likewise, `ssl_server2` has options to activate certain behaviors that are useful for testing a TLS client.

## Test utilities

* [`test/selftest.c`](test/selftest.c): runs the self-test function in each library module.

* [`test/udp_proxy.c`](test/udp_proxy.c): a UDP proxy that can inject certain failures (delay, duplicate, drop). Useful for testing DTLS.

* [`test/zeroize.c`](../framework/tests/programs/zeroize.c): a test program for `mbedtls_platform_zeroize`, used by [`test_zeroize.gdb`](../framework/tests/programs/test_zeroize.gdb).

## Development utilities

* [`util/pem2der.c`](util/pem2der.c): a PEM to DER converter. Mbed TLS can read PEM files directly, but this utility can be useful for interacting with other tools or with minimal Mbed TLS builds that lack PEM support.

* [`util/strerror.c`](util/strerror.c): prints the error description corresponding to an integer status returned by an Mbed TLS function.

## X.509 certificate examples

* [`x509/cert_app.c`](x509/cert_app.c): connects to a TLS server and verifies its certificate chain.

* [`x509/cert_req.c`](x509/cert_req.c): generates a certificate signing request (CSR) for a private key.

* [`x509/cert_write.c`](x509/cert_write.c): signs a certificate signing request, or self-signs a certificate.

* [`x509/crl_app.c`](x509/crl_app.c): loads and dumps a certificate revocation list (CRL).

* [`x509/req_app.c`](x509/req_app.c): loads and dumps a certificate signing request (CSR).
