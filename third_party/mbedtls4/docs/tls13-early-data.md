
Writing early data
------------------

An application function to write and send a buffer of data to a server through
TLS may plausibly look like:

```
int write_data(mbedtls_ssl_context *ssl,
               const unsigned char *data_to_write,
               size_t data_to_write_len,
               size_t *data_written)
{
    int ret;
    *data_written = 0;

    while (*data_written < data_to_write_len) {
        ret = mbedtls_ssl_write(ssl, data_to_write + *data_written,
                                data_to_write_len - *data_written);

        if (ret < 0 &&
            ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            return ret;
        }

        *data_written += ret;
    }

    return 0;
}
```
where ssl is the SSL context to use, data_to_write the address of the data
buffer and data_to_write_len the number of data bytes. The handshake may
not be completed, not even started for the SSL context ssl when the function is
called and in that case the mbedtls_ssl_write() API takes care transparently of
completing the handshake before to write and send data to the server. The
mbedtls_ssl_write() may not be able to write and send all data in one go thus
the need for a loop calling it as long as there are still data to write and
send.

An application function to write and send early data and only early data,
data sent during the first flight of client messages while the handshake is in
its initial phase, would look completely similar but the call to
mbedtls_ssl_write_early_data() instead of mbedtls_ssl_write().
```
int write_early_data(mbedtls_ssl_context *ssl,
                     const unsigned char *data_to_write,
                     size_t data_to_write_len,
                     size_t *data_written)
{
    int ret;
    *data_written = 0;

    while (*data_written < data_to_write_len) {
        ret = mbedtls_ssl_write_early_data(ssl, data_to_write + *data_written,
                                           data_to_write_len - *data_written);

        if (ret < 0 &&
            ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            return ret;
        }

        *data_written += ret;
    }

    return 0;
}
```
Note that compared to write_data(), write_early_data() can also return
MBEDTLS_ERR_SSL_CANNOT_WRITE_EARLY_DATA and that should be handled
specifically by the user of write_early_data(). A fresh SSL context (typically
just after a call to mbedtls_ssl_setup() or mbedtls_ssl_session_reset()) would
be expected when calling `write_early_data`.

All together, code to write and send a buffer of data as long as possible as
early data and then as standard post-handshake application data could
plausibly look like:

```
ret = write_early_data(ssl,
                       data_to_write,
                       data_to_write_len,
                       &early_data_written);
if (ret < 0 &&
    ret != MBEDTLS_ERR_SSL_CANNOT_WRITE_EARLY_DATA) {
    goto error;
}

ret = write_data(ssl,
                 data_to_write + early_data_written,
                 data_to_write_len - early_data_written,
                 &data_written);
if (ret < 0) {
    goto error;
}

data_written += early_data_written;
```

Finally, taking into account that the server may reject early data, application
code to write and send a buffer of data could plausibly look like:
```
ret = write_early_data(ssl,
                       data_to_write,
                       data_to_write_len,
                       &early_data_written);
if (ret < 0 &&
    ret != MBEDTLS_ERR_SSL_CANNOT_WRITE_EARLY_DATA) {
    goto error;
}

/*
 * Make sure the handshake is completed as it is a requisite of
 * mbedtls_ssl_get_early_data_status().
 */
while (!mbedtls_ssl_is_handshake_over(ssl)) {
    ret = mbedtls_ssl_handshake(ssl);
    if (ret < 0 &&
        ret != MBEDTLS_ERR_SSL_WANT_READ &&
        ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        goto error;
    }
}

ret = mbedtls_ssl_get_early_data_status(ssl);
if (ret < 0) {
    goto error;
}

if (ret == MBEDTLS_SSL_EARLY_DATA_STATUS_REJECTED) {
   early_data_written = 0;
}

ret = write_data(ssl,
                 data_to_write + early_data_written,
                 data_to_write_len - early_data_written,
                 &data_written);
if (ret < 0) {
    goto error;
}

data_written += early_data_written;
```

Reading early data
------------------
Mbed TLS provides the mbedtls_ssl_read_early_data() API to read the early data
that a TLS 1.3 server might receive during the TLS 1.3 handshake.

While establishing a TLS 1.3 connection with a client using a combination
of the mbedtls_ssl_handshake(), mbedtls_ssl_read() and mbedtls_ssl_write() APIs,
the reception of early data is signaled by an API returning the
MBEDTLS_ERR_SSL_RECEIVED_EARLY_DATA error code. Early data can then be read
with the mbedtls_ssl_read_early_data() API.

For example, a typical code to establish a TLS connection, where ssl is the SSL
context to use:
```
while ((int ret = mbedtls_ssl_handshake(&ssl)) != 0) {

    if (ret < 0 &&
        ret != MBEDTLS_ERR_SSL_WANT_READ &&
        ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        break;
    }
}
```
could be adapted to handle early data in the following way:
```
size_t data_read_len = 0;
while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {

    if (ret == MBEDTLS_ERR_SSL_RECEIVED_EARLY_DATA) {
        ret = mbedtls_ssl_read_early_data(&ssl,
                                          buffer + data_read_len,
                                          sizeof(buffer) - data_read_len);
        if (ret < 0) {
            break;
        }
        data_read_len += ret;
        continue;
    }

    if (ret < 0 &&
        ret != MBEDTLS_ERR_SSL_WANT_READ &&
        ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        break;
    }
}
```
