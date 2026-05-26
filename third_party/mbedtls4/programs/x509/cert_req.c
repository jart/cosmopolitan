/*
 *  Certificate request generation
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* Tell MSVC that we're ok with using classic C functions even
 * when an `_s` variant exist. For most functions, the improvements
 * of the `_s` variants are of limited usefulness and not worth
 * the portability headaches.
 */
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

#include "mbedtls/build_info.h"

#include "mbedtls/platform.h"
/* md.h is included this early since MD_CAN_XXX macros are defined there. */
#include "mbedtls/md.h"

#if !defined(MBEDTLS_X509_CSR_WRITE_C) || !defined(MBEDTLS_X509_CRT_PARSE_C) || \
    !defined(MBEDTLS_PK_PARSE_C) || !defined(PSA_WANT_ALG_SHA_256) || \
    !defined(MBEDTLS_PEM_WRITE_C) || !defined(MBEDTLS_FS_IO) || \
    !defined(MBEDTLS_MD_C)
int main(void)
{
    mbedtls_printf("MBEDTLS_X509_CSR_WRITE_C and/or MBEDTLS_FS_IO and/or "
                   "MBEDTLS_PK_PARSE_C and/or PSA_WANT_ALG_SHA_256 "
                   "not defined.\n");
    mbedtls_exit(0);
}
#else

#include "mbedtls/x509_csr.h"
#include "mbedtls/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DFL_FILENAME            "keyfile.key"
#define DFL_PASSWORD            NULL
#define DFL_DEBUG_LEVEL         0
#define DFL_OUTPUT_FILENAME     "cert.req"
#define DFL_SUBJECT_NAME        "CN=Cert,O=mbed TLS,C=UK"
#define DFL_KEY_USAGE           0
#define DFL_FORCE_KEY_USAGE     0
#define DFL_NS_CERT_TYPE        0
#define DFL_FORCE_NS_CERT_TYPE  0
#define DFL_MD_ALG              MBEDTLS_MD_SHA256

#define USAGE \
    "\n usage: cert_req param=<>...\n"                  \
    "\n acceptable parameters:\n"                       \
    "    filename=%%s         default: keyfile.key\n"   \
    "    password=%%s         default: NULL\n"          \
    "    debug_level=%%d      default: 0 (disabled)\n"  \
    "    output_file=%%s      default: cert.req\n"      \
    "    subject_name=%%s     default: CN=Cert,O=mbed TLS,C=UK\n"   \
    "    san=%%s              default: (none)\n"       \
    "                           Semicolon-separated-list of values:\n" \
    "                           DNS:value\n"            \
    "                           URI:value\n"            \
    "                           RFC822:value\n"         \
    "                           IP:value (Only IPv4 is supported)\n" \
    "                           DN:list of comma separated key=value pairs\n" \
    "    key_usage=%%s        default: (empty)\n"       \
    "                        Comma-separated-list of values:\n"     \
    "                          digital_signature\n"     \
    "                          non_repudiation\n"       \
    "                          key_encipherment\n"      \
    "                          data_encipherment\n"     \
    "                          key_agreement\n"         \
    "                          key_cert_sign\n"  \
    "                          crl_sign\n"              \
    "    force_key_usage=0/1  default: off\n"           \
    "                          Add KeyUsage even if it is empty\n"  \
    "    ns_cert_type=%%s     default: (empty)\n"       \
    "                        Comma-separated-list of values:\n"     \
    "                          ssl_client\n"            \
    "                          ssl_server\n"            \
    "                          email\n"                 \
    "                          object_signing\n"        \
    "                          ssl_ca\n"                \
    "                          email_ca\n"              \
    "                          object_signing_ca\n"     \
    "    force_ns_cert_type=0/1 default: off\n"         \
    "                          Add NsCertType even if it is empty\n"    \
    "    md=%%s               default: SHA256\n"       \
    "                          possible values:\n"     \
    "                          MD5, RIPEMD160, SHA1,\n" \
    "                          SHA224, SHA256, SHA384, SHA512\n" \
    "\n"


/*
 * global options
 */
struct options {
    const char *filename;             /* filename of the key file                 */
    const char *password;             /* password for the key file                */
    int debug_level;                  /* level of debugging                       */
    const char *output_file;          /* where to store the constructed key file  */
    const char *subject_name;         /* subject name for certificate request     */
    mbedtls_x509_san_list *san_list;  /* subjectAltName for certificate request   */
    unsigned char key_usage;          /* key usage flags                          */
    int force_key_usage;              /* Force adding the KeyUsage extension      */
    unsigned char ns_cert_type;       /* NS cert type                             */
    int force_ns_cert_type;           /* Force adding NsCertType extension        */
    mbedtls_md_type_t md_alg;         /* Hash algorithm used for signature.       */
} opt;

static int write_certificate_request(mbedtls_x509write_csr *req, const char *output_file)
{
    int ret;
    FILE *f;
    unsigned char output_buf[4096];
    size_t len = 0;

    memset(output_buf, 0, 4096);
    if ((ret = mbedtls_x509write_csr_pem(req, output_buf, 4096)) < 0) {
        return ret;
    }

    len = strlen((char *) output_buf);

    if ((f = fopen(output_file, "w")) == NULL) {
        return -1;
    }

    if (fwrite(output_buf, 1, len, f) != len) {
        fclose(f);
        return -1;
    }

    fclose(f);

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_pk_context key;
    char buf[1024];
    int i;
    char *p, *q, *r;
    mbedtls_x509write_csr req;
    mbedtls_x509_san_list *cur, *prev;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    uint8_t ip[4] = { 0 };
#endif
    /*
     * Set to sane values
     */
    mbedtls_x509write_csr_init(&req);
    mbedtls_pk_init(&key);
    memset(buf, 0, sizeof(buf));

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "Failed to initialize PSA Crypto implementation: %d\n",
                        (int) status);
        goto exit;
    }

    if (argc < 2) {
usage:
        mbedtls_printf(USAGE);
        goto exit;
    }

    opt.filename            = DFL_FILENAME;
    opt.password            = DFL_PASSWORD;
    opt.debug_level         = DFL_DEBUG_LEVEL;
    opt.output_file         = DFL_OUTPUT_FILENAME;
    opt.subject_name        = DFL_SUBJECT_NAME;
    opt.key_usage           = DFL_KEY_USAGE;
    opt.force_key_usage     = DFL_FORCE_KEY_USAGE;
    opt.ns_cert_type        = DFL_NS_CERT_TYPE;
    opt.force_ns_cert_type  = DFL_FORCE_NS_CERT_TYPE;
    opt.md_alg              = DFL_MD_ALG;
    opt.san_list            = NULL;

    for (i = 1; i < argc; i++) {
        p = argv[i];
        if ((q = strchr(p, '=')) == NULL) {
            goto usage;
        }
        *q++ = '\0';
        if (strcmp(p, "filename") == 0) {
            opt.filename = q;
        } else if (strcmp(p, "password") == 0) {
            opt.password = q;
        } else if (strcmp(p, "output_file") == 0) {
            opt.output_file = q;
        } else if (strcmp(p, "debug_level") == 0) {
            opt.debug_level = atoi(q);
            if (opt.debug_level < 0 || opt.debug_level > 65535) {
                goto usage;
            }
        } else if (strcmp(p, "subject_name") == 0) {
            opt.subject_name = q;
        } else if (strcmp(p, "san") == 0) {
            char *subtype_value;
            prev = NULL;

            while (q != NULL) {
                char *semicolon;
                r = q;

                /* Find the first non-escaped ; occurrence and remove escaped ones */
                do {
                    if ((semicolon = strchr(r, ';')) != NULL) {
                        if (*(semicolon-1) != '\\') {
                            r = semicolon;
                            break;
                        }
                        /* Remove the escape character */
                        size_t size_left = strlen(semicolon);
                        memmove(semicolon-1, semicolon, size_left);
                        *(semicolon + size_left - 1) = '\0';
                        /* r will now point at the character after the semicolon */
                        r = semicolon;
                    }

                } while (semicolon != NULL);

                if (semicolon != NULL) {
                    *r++ = '\0';
                } else {
                    r = NULL;
                }

                cur = mbedtls_calloc(1, sizeof(mbedtls_x509_san_list));
                if (cur == NULL) {
                    mbedtls_printf("Not enough memory for subjectAltName list\n");
                    goto usage;
                }

                cur->next = NULL;

                if ((subtype_value = strchr(q, ':')) != NULL) {
                    *subtype_value++ = '\0';
                } else {
                    mbedtls_printf(
                        "Invalid argument for option SAN: Entry must be of the form TYPE:value\n");
                    goto usage;
                }
                if (strcmp(q, "RFC822") == 0) {
                    cur->node.type = MBEDTLS_X509_SAN_RFC822_NAME;
                } else if (strcmp(q, "URI") == 0) {
                    cur->node.type = MBEDTLS_X509_SAN_UNIFORM_RESOURCE_IDENTIFIER;
                } else if (strcmp(q, "DNS") == 0) {
                    cur->node.type = MBEDTLS_X509_SAN_DNS_NAME;
                } else if (strcmp(q, "IP") == 0) {
                    size_t ip_addr_len = 0;
                    cur->node.type = MBEDTLS_X509_SAN_IP_ADDRESS;
                    ip_addr_len = mbedtls_x509_crt_parse_cn_inet_pton(subtype_value, ip);
                    if (ip_addr_len == 0) {
                        mbedtls_printf("mbedtls_x509_crt_parse_cn_inet_pton failed to parse %s\n",
                                       subtype_value);
                        goto exit;
                    }
                    cur->node.san.unstructured_name.p = (unsigned char *) ip;
                    cur->node.san.unstructured_name.len = sizeof(ip);
                } else if (strcmp(q, "DN") == 0) {
                    cur->node.type = MBEDTLS_X509_SAN_DIRECTORY_NAME;
                    /* Work around an API mismatch between string_to_names() and
                     * mbedtls_x509_subject_alternative_name, which holds an
                     * actual mbedtls_x509_name while a pointer to one would be
                     * more convenient here. (Note mbedtls_x509_name and
                     * mbedtls_asn1_named_data are synonymous, again
                     * string_to_names() uses one while
                     * cur->node.san.directory_name is nominally the other.) */
                    mbedtls_asn1_named_data *tmp_san_dirname = NULL;
                    if ((ret = mbedtls_x509_string_to_names(&tmp_san_dirname,
                                                            subtype_value)) != 0) {
                        mbedtls_strerror(ret, buf, sizeof(buf));
                        mbedtls_printf(
                            " failed\n  !  mbedtls_x509_string_to_names "
                            "returned -0x%04x - %s\n\n",
                            (unsigned int) -ret, buf);
                        goto exit;
                    }
                    cur->node.san.directory_name = *tmp_san_dirname;
                    mbedtls_free(tmp_san_dirname);
                    tmp_san_dirname = NULL;
                } else {
                    mbedtls_free(cur);
                    goto usage;
                }

                if (cur->node.type == MBEDTLS_X509_SAN_RFC822_NAME ||
                    cur->node.type == MBEDTLS_X509_SAN_UNIFORM_RESOURCE_IDENTIFIER ||
                    cur->node.type == MBEDTLS_X509_SAN_DNS_NAME) {
                    q = subtype_value;
                    cur->node.san.unstructured_name.p = (unsigned char *) q;
                    cur->node.san.unstructured_name.len = strlen(q);
                }

                if (prev == NULL) {
                    opt.san_list = cur;
                } else {
                    prev->next = cur;
                }

                prev = cur;
                q = r;
            }
        } else if (strcmp(p, "md") == 0) {
            const mbedtls_md_info_t *md_info =
                mbedtls_md_info_from_string(q);
            if (md_info == NULL) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
            opt.md_alg = mbedtls_md_get_type(md_info);
        } else if (strcmp(p, "key_usage") == 0) {
            while (q != NULL) {
                if ((r = strchr(q, ',')) != NULL) {
                    *r++ = '\0';
                }

                if (strcmp(q, "digital_signature") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_DIGITAL_SIGNATURE;
                } else if (strcmp(q, "non_repudiation") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_NON_REPUDIATION;
                } else if (strcmp(q, "key_encipherment") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_KEY_ENCIPHERMENT;
                } else if (strcmp(q, "data_encipherment") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_DATA_ENCIPHERMENT;
                } else if (strcmp(q, "key_agreement") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_KEY_AGREEMENT;
                } else if (strcmp(q, "key_cert_sign") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_KEY_CERT_SIGN;
                } else if (strcmp(q, "crl_sign") == 0) {
                    opt.key_usage |= MBEDTLS_X509_KU_CRL_SIGN;
                } else {
                    goto usage;
                }

                q = r;
            }
        } else if (strcmp(p, "force_key_usage") == 0) {
            switch (atoi(q)) {
                case 0: opt.force_key_usage = 0; break;
                case 1: opt.force_key_usage = 1; break;
                default: goto usage;
            }
        } else if (strcmp(p, "ns_cert_type") == 0) {
            while (q != NULL) {
                if ((r = strchr(q, ',')) != NULL) {
                    *r++ = '\0';
                }

                if (strcmp(q, "ssl_client") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_SSL_CLIENT;
                } else if (strcmp(q, "ssl_server") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER;
                } else if (strcmp(q, "email") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_EMAIL;
                } else if (strcmp(q, "object_signing") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING;
                } else if (strcmp(q, "ssl_ca") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_SSL_CA;
                } else if (strcmp(q, "email_ca") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_EMAIL_CA;
                } else if (strcmp(q, "object_signing_ca") == 0) {
                    opt.ns_cert_type |= MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING_CA;
                } else {
                    goto usage;
                }

                q = r;
            }
        } else if (strcmp(p, "force_ns_cert_type") == 0) {
            switch (atoi(q)) {
                case 0: opt.force_ns_cert_type = 0; break;
                case 1: opt.force_ns_cert_type = 1; break;
                default: goto usage;
            }
        } else {
            goto usage;
        }
    }

    /* Set the MD algorithm to use for the signature in the CSR */
    mbedtls_x509write_csr_set_md_alg(&req, opt.md_alg);

    /* Set the Key Usage Extension flags in the CSR */
    if (opt.key_usage || opt.force_key_usage == 1) {
        ret = mbedtls_x509write_csr_set_key_usage(&req, opt.key_usage);

        if (ret != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509write_csr_set_key_usage returned %d", ret);
            goto exit;
        }
    }

    /* Set the Cert Type flags in the CSR */
    if (opt.ns_cert_type || opt.force_ns_cert_type == 1) {
        ret = mbedtls_x509write_csr_set_ns_cert_type(&req, opt.ns_cert_type);

        if (ret != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509write_csr_set_ns_cert_type returned %d", ret);
            goto exit;
        }
    }

    /* Set the SubjectAltName in the CSR */
    if (opt.san_list != NULL) {
        ret = mbedtls_x509write_csr_set_subject_alternative_name(&req, opt.san_list);

        if (ret != 0) {
            mbedtls_printf(
                " failed\n  !  mbedtls_x509write_csr_set_subject_alternative_name returned %d",
                ret);
            goto exit;
        }
    }

    /*
     * 0. Seed the PRNG
     */
    mbedtls_printf("  . Seeding the random number generator...");
    fflush(stdout);

    mbedtls_printf(" ok\n");

    /*
     * 1.0. Check the subject name for validity
     */
    mbedtls_printf("  . Checking subject name...");
    fflush(stdout);

    if ((ret = mbedtls_x509write_csr_set_subject_name(&req, opt.subject_name)) != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_x509write_csr_set_subject_name returned %d", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /*
     * 1.1. Load the key
     */
    mbedtls_printf("  . Loading the private key ...");
    fflush(stdout);

    ret = mbedtls_pk_parse_keyfile(&key, opt.filename, opt.password);

    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_pk_parse_keyfile returned %d", ret);
        goto exit;
    }

    mbedtls_x509write_csr_set_key(&req, &key);

    mbedtls_printf(" ok\n");

    /*
     * 1.2. Writing the request
     */
    mbedtls_printf("  . Writing the certificate request ...");
    fflush(stdout);

    if ((ret = write_certificate_request(&req, opt.output_file)) != 0) {
        mbedtls_printf(" failed\n  !  write_certificate_request %d", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

    if (exit_code != MBEDTLS_EXIT_SUCCESS) {
#ifdef MBEDTLS_ERROR_C
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" - %s\n", buf);
#else
        mbedtls_printf("\n");
#endif
    }

    mbedtls_x509write_csr_free(&req);
    mbedtls_pk_free(&key);
    mbedtls_psa_crypto_free();

    cur = opt.san_list;
    while (cur != NULL) {
        mbedtls_x509_san_list *next = cur->next;
        /* Note: mbedtls_x509_free_subject_alt_name() is not what we want here.
         * It's the right thing for entries that were parsed from a certificate,
         * where pointers are to the raw certificate, but here all the
         * pointers were allocated while parsing from a user-provided string. */
        if (cur->node.type == MBEDTLS_X509_SAN_DIRECTORY_NAME) {
            mbedtls_x509_name *dn = &cur->node.san.directory_name;
            mbedtls_free(dn->oid.p);
            mbedtls_free(dn->val.p);
            mbedtls_asn1_free_named_data_list(&dn->next);
        }
        mbedtls_free(cur);
        cur = next;
    }

    mbedtls_exit(exit_code);
}
#endif /* MBEDTLS_X509_CSR_WRITE_C && MBEDTLS_PK_PARSE_C && MBEDTLS_FS_IO &&
          MBEDTLS_PEM_WRITE_C */
