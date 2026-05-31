/*
 *  Certificate generation and signing
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

#if !defined(MBEDTLS_X509_CRT_WRITE_C) || \
    !defined(MBEDTLS_X509_CRT_PARSE_C) || !defined(MBEDTLS_FS_IO) || \
    !defined(MBEDTLS_ERROR_C) || !defined(PSA_WANT_ALG_SHA_256) || \
    !defined(MBEDTLS_PEM_WRITE_C) || !defined(MBEDTLS_MD_C)
int main(void)
{
    mbedtls_printf("MBEDTLS_X509_CRT_WRITE_C and/or MBEDTLS_X509_CRT_PARSE_C and/or "
                   "MBEDTLS_FS_IO and/or PSA_WANT_ALG_SHA_256 and/or "
                   "MBEDTLS_ERROR_C not defined.\n");
    mbedtls_exit(0);
}
#else

#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/oid.h"
#include "mbedtls/error.h"
#include "test/helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SET_OID(x, oid) \
    do { x.len = MBEDTLS_OID_SIZE(oid); x.p = (unsigned char *) oid; } while (0)

#if defined(MBEDTLS_X509_CSR_PARSE_C)
#define USAGE_CSR                                                           \
    "    request_file=%%s         default: (empty)\n"                           \
    "                            If request_file is specified, subject_key,\n"  \
    "                            subject_pwd and subject_name are ignored!\n"
#else
#define USAGE_CSR ""
#endif /* MBEDTLS_X509_CSR_PARSE_C */

#define FORMAT_PEM              0
#define FORMAT_DER              1

#define DFL_ISSUER_CRT          ""
#define DFL_REQUEST_FILE        ""
#define DFL_SUBJECT_KEY         "subject.key"
#define DFL_ISSUER_KEY          "ca.key"
#define DFL_SUBJECT_PWD         ""
#define DFL_ISSUER_PWD          ""
#define DFL_OUTPUT_FILENAME     "cert.crt"
#define DFL_SUBJECT_NAME        "CN=Cert,O=mbed TLS,C=UK"
#define DFL_ISSUER_NAME         "CN=CA,O=mbed TLS,C=UK"
#define DFL_NOT_BEFORE          "20010101000000"
#define DFL_NOT_AFTER           "20301231235959"
#define DFL_SERIAL              "1"
#define DFL_SERIAL_HEX          "1"
#define DFL_EXT_SUBJECTALTNAME  ""
#define DFL_SELFSIGN            0
#define DFL_IS_CA               0
#define DFL_MAX_PATHLEN         -1
#define DFL_SIG_ALG             MBEDTLS_MD_SHA256
#define DFL_KEY_USAGE           0
#define DFL_EXT_KEY_USAGE       NULL
#define DFL_NS_CERT_TYPE        0
#define DFL_VERSION             3
#define DFL_AUTH_IDENT          1
#define DFL_SUBJ_IDENT          1
#define DFL_CONSTRAINTS         1
#define DFL_DIGEST              MBEDTLS_MD_SHA256
#define DFL_FORMAT              FORMAT_PEM

#define USAGE \
    "\n usage: cert_write param=<>...\n"                \
    "\n acceptable parameters:\n"                       \
    USAGE_CSR                                           \
    "    subject_key=%%s          default: subject.key\n"   \
    "    subject_pwd=%%s          default: (empty)\n"       \
    "    subject_name=%%s         default: CN=Cert,O=mbed TLS,C=UK\n"   \
    "\n"                                                \
    "    issuer_crt=%%s           default: (empty)\n"       \
    "                            If issuer_crt is specified, issuer_name is\n"  \
    "                            ignored!\n"                \
    "    issuer_name=%%s          default: CN=CA,O=mbed TLS,C=UK\n"     \
    "\n"                                                \
    "    selfsign=%%d             default: 0 (false)\n"     \
    "                            If selfsign is enabled, issuer_name and\n" \
    "                            issuer_key are required (issuer_crt and\n" \
    "                            subject_* are ignored\n"   \
    "    issuer_key=%%s           default: ca.key\n"        \
    "    issuer_pwd=%%s           default: (empty)\n"       \
    "    output_file=%%s          default: cert.crt\n"      \
    "    serial=%%s               default: 1\n"             \
    "                            In decimal format; it can be used as\n"     \
    "                            alternative to serial_hex, but it's\n"      \
    "                            limited in max length to\n"                 \
    "                            unsigned long long int\n"                   \
    "    serial_hex=%%s           default: 1\n"             \
    "                            In hex format; it can be used as\n"         \
    "                            alternative to serial\n"                    \
    "    not_before=%%s           default: 20010101000000\n" \
    "    not_after=%%s            default: 20301231235959\n" \
    "    is_ca=%%d                default: 0 (disabled)\n"  \
    "    max_pathlen=%%d          default: -1 (none)\n"     \
    "    md=%%s                   default: SHA256\n"        \
    "                            Supported values (if enabled):\n"      \
    "                            MD5, RIPEMD160, SHA1,\n" \
    "                            SHA224, SHA256, SHA384, SHA512\n" \
    "    version=%%d              default: 3\n"            \
    "                            Possible values: 1, 2, 3\n" \
    "    subject_identifier=%%s   default: 1\n"             \
    "                            Possible values: 0, 1\n"   \
    "                            (Considered for v3 only)\n" \
    "    san=%%s                   default: (none)\n"       \
    "                            Semicolon-separated-list of values:\n" \
    "                             DNS:value\n"            \
    "                             URI:value\n"            \
    "                             RFC822:value\n"         \
    "                             IP:value (Only IPv4 is supported)\n" \
    "                             DN:list of comma separated key=value pairs\n" \
    "    authority_identifier=%%s default: 1\n"             \
    "                            Possible values: 0, 1\n"   \
    "                            (Considered for v3 only)\n" \
    "    basic_constraints=%%d    default: 1\n"             \
    "                            Possible values: 0, 1\n"   \
    "                            (Considered for v3 only)\n" \
    "    key_usage=%%s            default: (empty)\n"       \
    "                            Comma-separated-list of values:\n"     \
    "                            digital_signature\n"     \
    "                            non_repudiation\n"       \
    "                            key_encipherment\n"      \
    "                            data_encipherment\n"     \
    "                            key_agreement\n"         \
    "                            key_cert_sign\n"  \
    "                            crl_sign\n"              \
    "                            (Considered for v3 only)\n" \
    "    ext_key_usage=%%s        default: (empty)\n"      \
    "                            Comma-separated-list of values:\n"     \
    "                            serverAuth\n"             \
    "                            clientAuth\n"             \
    "                            codeSigning\n"            \
    "                            emailProtection\n"        \
    "                            timeStamping\n"           \
    "                            OCSPSigning\n"            \
    "    ns_cert_type=%%s         default: (empty)\n"       \
    "                            Comma-separated-list of values:\n"     \
    "                            ssl_client\n"            \
    "                            ssl_server\n"            \
    "                            email\n"                 \
    "                            object_signing\n"        \
    "                            ssl_ca\n"                \
    "                            email_ca\n"              \
    "                            object_signing_ca\n"     \
    "   format=pem|der           default: pem\n"         \
    "\n"

typedef enum {
    SERIAL_FRMT_UNSPEC,
    SERIAL_FRMT_DEC,
    SERIAL_FRMT_HEX
} serial_format_t;

/*
 * global options
 */
struct options {
    const char *issuer_crt;     /* filename of the issuer certificate   */
    const char *request_file;   /* filename of the certificate request  */
    const char *subject_key;    /* filename of the subject key file     */
    const char *issuer_key;     /* filename of the issuer key file      */
    const char *subject_pwd;    /* password for the subject key file    */
    const char *issuer_pwd;     /* password for the issuer key file     */
    const char *output_file;    /* where to store the constructed CRT   */
    const char *subject_name;   /* subject name for certificate         */
    mbedtls_x509_san_list *san_list; /* subjectAltName for certificate  */
    const char *issuer_name;    /* issuer name for certificate          */
    const char *not_before;     /* validity period not before           */
    const char *not_after;      /* validity period not after            */
    const char *serial;         /* serial number string (decimal)       */
    const char *serial_hex;     /* serial number string (hex)           */
    int selfsign;               /* selfsign the certificate             */
    int is_ca;                  /* is a CA certificate                  */
    int max_pathlen;            /* maximum CA path length               */
    int authority_identifier;   /* add authority identifier to CRT      */
    int subject_identifier;     /* add subject identifier to CRT        */
    int basic_constraints;      /* add basic constraints ext to CRT     */
    int version;                /* CRT version                          */
    mbedtls_md_type_t md;       /* Hash used for signing                */
    unsigned char key_usage;    /* key usage flags                      */
    mbedtls_asn1_sequence *ext_key_usage; /* extended key usages        */
    unsigned char ns_cert_type; /* NS cert type                         */
    int format;                 /* format                               */
} opt;

static int write_certificate(mbedtls_x509write_cert *crt, const char *output_file)
{
    int ret;
    FILE *f;
    unsigned char output_buf[4096];
    unsigned char *output_start;
    size_t len = 0;

    memset(output_buf, 0, 4096);
    if (opt.format == FORMAT_DER) {
        ret = mbedtls_x509write_crt_der(crt, output_buf, 4096);
        if (ret < 0) {
            return ret;
        }

        len = ret;
        output_start = output_buf + 4096 - len;
    } else {
        ret = mbedtls_x509write_crt_pem(crt, output_buf, 4096);
        if (ret < 0) {
            return ret;
        }

        len = strlen((char *) output_buf);
        output_start = output_buf;
    }

    if ((f = fopen(output_file, "w")) == NULL) {
        return -1;
    }

    if (fwrite(output_start, 1, len, f) != len) {
        fclose(f);
        return -1;
    }

    fclose(f);

    return 0;
}

static int parse_serial_decimal_format(unsigned char *obuf, size_t obufmax,
                                       const char *ibuf, size_t *len)
{
    unsigned long long int dec;
    unsigned int remaining_bytes = sizeof(dec);
    unsigned char *p = obuf;
    unsigned char val;
    char *end_ptr = NULL;

    errno = 0;
    dec = strtoull(ibuf, &end_ptr, 10);

    if ((errno != 0) || (end_ptr == ibuf)) {
        return -1;
    }

    *len = 0;

    while (remaining_bytes > 0) {
        if (obufmax < (*len + 1)) {
            return -1;
        }

        val = (dec >> ((remaining_bytes - 1) * 8)) & 0xFF;

        /* Skip leading zeros */
        if ((val != 0) || (*len != 0)) {
            *p = val;
            (*len)++;
            p++;
        }

        remaining_bytes--;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_x509_crt issuer_crt;
    mbedtls_pk_context loaded_issuer_key, loaded_subject_key;
    mbedtls_pk_context *issuer_key = &loaded_issuer_key,
                       *subject_key = &loaded_subject_key;
    char buf[1024];
    char issuer_name[256];
    int i;
    char *p, *q, *r;
#if defined(MBEDTLS_X509_CSR_PARSE_C)
    char subject_name[256];
    mbedtls_x509_csr csr;
#endif
    mbedtls_x509write_cert crt;
    serial_format_t serial_frmt = SERIAL_FRMT_UNSPEC;
    unsigned char serial[MBEDTLS_X509_RFC5280_MAX_SERIAL_LEN];
    size_t serial_len;
    mbedtls_asn1_sequence *ext_key_usage;
    mbedtls_x509_san_list *cur, *prev;
    uint8_t ip[4] = { 0 };
    /*
     * Set to sane values
     */
    mbedtls_x509write_crt_init(&crt);
    mbedtls_pk_init(&loaded_issuer_key);
    mbedtls_pk_init(&loaded_subject_key);
#if defined(MBEDTLS_X509_CSR_PARSE_C)
    mbedtls_x509_csr_init(&csr);
#endif
    mbedtls_x509_crt_init(&issuer_crt);
    memset(buf, 0, sizeof(buf));
    memset(serial, 0, sizeof(serial));

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

    opt.issuer_crt          = DFL_ISSUER_CRT;
    opt.request_file        = DFL_REQUEST_FILE;
    opt.subject_key         = DFL_SUBJECT_KEY;
    opt.issuer_key          = DFL_ISSUER_KEY;
    opt.subject_pwd         = DFL_SUBJECT_PWD;
    opt.issuer_pwd          = DFL_ISSUER_PWD;
    opt.output_file         = DFL_OUTPUT_FILENAME;
    opt.subject_name        = DFL_SUBJECT_NAME;
    opt.issuer_name         = DFL_ISSUER_NAME;
    opt.not_before          = DFL_NOT_BEFORE;
    opt.not_after           = DFL_NOT_AFTER;
    opt.serial              = DFL_SERIAL;
    opt.serial_hex          = DFL_SERIAL_HEX;
    opt.selfsign            = DFL_SELFSIGN;
    opt.is_ca               = DFL_IS_CA;
    opt.max_pathlen         = DFL_MAX_PATHLEN;
    opt.key_usage           = DFL_KEY_USAGE;
    opt.ext_key_usage       = DFL_EXT_KEY_USAGE;
    opt.ns_cert_type        = DFL_NS_CERT_TYPE;
    opt.version             = DFL_VERSION - 1;
    opt.md                  = DFL_DIGEST;
    opt.subject_identifier   = DFL_SUBJ_IDENT;
    opt.authority_identifier = DFL_AUTH_IDENT;
    opt.basic_constraints    = DFL_CONSTRAINTS;
    opt.format              = DFL_FORMAT;
    opt.san_list            = NULL;

    for (i = 1; i < argc; i++) {

        p = argv[i];
        if ((q = strchr(p, '=')) == NULL) {
            goto usage;
        }
        *q++ = '\0';

        if (strcmp(p, "request_file") == 0) {
            opt.request_file = q;
        } else if (strcmp(p, "subject_key") == 0) {
            opt.subject_key = q;
        } else if (strcmp(p, "issuer_key") == 0) {
            opt.issuer_key = q;
        } else if (strcmp(p, "subject_pwd") == 0) {
            opt.subject_pwd = q;
        } else if (strcmp(p, "issuer_pwd") == 0) {
            opt.issuer_pwd = q;
        } else if (strcmp(p, "issuer_crt") == 0) {
            opt.issuer_crt = q;
        } else if (strcmp(p, "output_file") == 0) {
            opt.output_file = q;
        } else if (strcmp(p, "subject_name") == 0) {
            opt.subject_name = q;
        } else if (strcmp(p, "issuer_name") == 0) {
            opt.issuer_name = q;
        } else if (strcmp(p, "not_before") == 0) {
            opt.not_before = q;
        } else if (strcmp(p, "not_after") == 0) {
            opt.not_after = q;
        } else if (strcmp(p, "serial") == 0) {
            if (serial_frmt != SERIAL_FRMT_UNSPEC) {
                mbedtls_printf("Invalid attempt to set the serial more than once\n");
                goto usage;
            }
            serial_frmt = SERIAL_FRMT_DEC;
            opt.serial = q;
        } else if (strcmp(p, "serial_hex") == 0) {
            if (serial_frmt != SERIAL_FRMT_UNSPEC) {
                mbedtls_printf("Invalid attempt to set the serial more than once\n");
                goto usage;
            }
            serial_frmt = SERIAL_FRMT_HEX;
            opt.serial_hex = q;
        } else if (strcmp(p, "authority_identifier") == 0) {
            opt.authority_identifier = atoi(q);
            if (opt.authority_identifier != 0 &&
                opt.authority_identifier != 1) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
        } else if (strcmp(p, "subject_identifier") == 0) {
            opt.subject_identifier = atoi(q);
            if (opt.subject_identifier != 0 &&
                opt.subject_identifier != 1) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
        } else if (strcmp(p, "basic_constraints") == 0) {
            opt.basic_constraints = atoi(q);
            if (opt.basic_constraints != 0 &&
                opt.basic_constraints != 1) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
        } else if (strcmp(p, "md") == 0) {
            const mbedtls_md_info_t *md_info =
                mbedtls_md_info_from_string(q);
            if (md_info == NULL) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
            opt.md = mbedtls_md_get_type(md_info);
        } else if (strcmp(p, "version") == 0) {
            opt.version = atoi(q);
            if (opt.version < 1 || opt.version > 3) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
            opt.version--;
        } else if (strcmp(p, "selfsign") == 0) {
            opt.selfsign = atoi(q);
            if (opt.selfsign < 0 || opt.selfsign > 1) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
        } else if (strcmp(p, "is_ca") == 0) {
            opt.is_ca = atoi(q);
            if (opt.is_ca < 0 || opt.is_ca > 1) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
        } else if (strcmp(p, "max_pathlen") == 0) {
            opt.max_pathlen = atoi(q);
            if (opt.max_pathlen < -1 || opt.max_pathlen > 127) {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
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
                    mbedtls_printf("Invalid argument for option %s\n", p);
                    goto usage;
                }

                q = r;
            }
        } else if (strcmp(p, "ext_key_usage") == 0) {
            mbedtls_asn1_sequence **tail = &opt.ext_key_usage;

            while (q != NULL) {
                if ((r = strchr(q, ',')) != NULL) {
                    *r++ = '\0';
                }

                ext_key_usage = mbedtls_calloc(1, sizeof(mbedtls_asn1_sequence));
                ext_key_usage->buf.tag = MBEDTLS_ASN1_OID;
                if (strcmp(q, "serverAuth") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_SERVER_AUTH);
                } else if (strcmp(q, "clientAuth") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_CLIENT_AUTH);
                } else if (strcmp(q, "codeSigning") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_CODE_SIGNING);
                } else if (strcmp(q, "emailProtection") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_EMAIL_PROTECTION);
                } else if (strcmp(q, "timeStamping") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_TIME_STAMPING);
                } else if (strcmp(q, "OCSPSigning") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_OCSP_SIGNING);
                } else if (strcmp(q, "any") == 0) {
                    SET_OID(ext_key_usage->buf, MBEDTLS_OID_ANY_EXTENDED_KEY_USAGE);
                } else {
                    mbedtls_printf("Invalid argument for option %s\n", p);
                    goto usage;
                }

                *tail = ext_key_usage;
                tail = &ext_key_usage->next;

                q = r;
            }
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
                    mbedtls_printf("Invalid argument for option %s\n", p);
                    goto usage;
                }

                q = r;
            }
        } else if (strcmp(p, "format") == 0) {
            if (strcmp(q, "der") == 0) {
                opt.format = FORMAT_DER;
            } else if (strcmp(q, "pem") == 0) {
                opt.format = FORMAT_PEM;
            } else {
                mbedtls_printf("Invalid argument for option %s\n", p);
                goto usage;
            }
        } else {
            goto usage;
        }
    }

    mbedtls_printf("\n");

    /*
     * 0. Seed the PRNG
     */
    mbedtls_printf("  . Seeding the random number generator...");
    fflush(stdout);

    mbedtls_printf(" ok\n");

    // Parse serial to MPI
    //
    mbedtls_printf("  . Reading serial number...");
    fflush(stdout);

    if (serial_frmt == SERIAL_FRMT_HEX) {
        ret = mbedtls_test_unhexify(serial, sizeof(serial),
                                    opt.serial_hex, &serial_len);
    } else { // SERIAL_FRMT_DEC || SERIAL_FRMT_UNSPEC
        ret = parse_serial_decimal_format(serial, sizeof(serial),
                                          opt.serial, &serial_len);
    }

    if (ret != 0) {
        mbedtls_printf(" failed\n  !  Unable to parse serial\n");
        goto exit;
    }

    mbedtls_printf(" ok\n");

    // Parse issuer certificate if present
    //
    if (!opt.selfsign && strlen(opt.issuer_crt)) {
        /*
         * 1.0.a. Load the certificates
         */
        mbedtls_printf("  . Loading the issuer certificate ...");
        fflush(stdout);

        if ((ret = mbedtls_x509_crt_parse_file(&issuer_crt, opt.issuer_crt)) != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        ret = mbedtls_x509_dn_gets(issuer_name, sizeof(issuer_name),
                                   &issuer_crt.subject);
        if (ret < 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509_dn_gets "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        opt.issuer_name = issuer_name;

        mbedtls_printf(" ok\n");
    }

#if defined(MBEDTLS_X509_CSR_PARSE_C)
    // Parse certificate request if present
    //
    if (!opt.selfsign && strlen(opt.request_file)) {
        /*
         * 1.0.b. Load the CSR
         */
        mbedtls_printf("  . Loading the certificate request ...");
        fflush(stdout);

        if ((ret = mbedtls_x509_csr_parse_file(&csr, opt.request_file)) != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509_csr_parse_file "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        ret = mbedtls_x509_dn_gets(subject_name, sizeof(subject_name),
                                   &csr.subject);
        if (ret < 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509_dn_gets "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        opt.subject_name = subject_name;
        subject_key = &csr.pk;

        mbedtls_printf(" ok\n");
    }
#endif /* MBEDTLS_X509_CSR_PARSE_C */

    /*
     * 1.1. Load the keys
     */
    if (!opt.selfsign && !strlen(opt.request_file)) {
        mbedtls_printf("  . Loading the subject key ...");
        fflush(stdout);

        ret = mbedtls_pk_parse_keyfile(&loaded_subject_key, opt.subject_key,
                                       opt.subject_pwd);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_pk_parse_keyfile "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }

    mbedtls_printf("  . Loading the issuer key ...");
    fflush(stdout);

    ret = mbedtls_pk_parse_keyfile(&loaded_issuer_key, opt.issuer_key,
                                   opt.issuer_pwd);
    if (ret != 0) {
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" failed\n  !  mbedtls_pk_parse_keyfile "
                       "returned -x%02x - %s\n\n", (unsigned int) -ret, buf);
        goto exit;
    }

    // Check if key and issuer certificate match
    //
    if (strlen(opt.issuer_crt)) {
        if (mbedtls_pk_check_pair(&issuer_crt.pk, issuer_key) != 0) {
            mbedtls_printf(" failed\n  !  issuer_key does not match "
                           "issuer certificate\n\n");
            goto exit;
        }
    }

    mbedtls_printf(" ok\n");

    if (opt.selfsign) {
        opt.subject_name = opt.issuer_name;
        subject_key = issuer_key;
    }

    mbedtls_x509write_crt_set_subject_key(&crt, subject_key);
    mbedtls_x509write_crt_set_issuer_key(&crt, issuer_key);

    /*
     * 1.0. Check the names for validity
     */
    if ((ret = mbedtls_x509write_crt_set_subject_name(&crt, opt.subject_name)) != 0) {
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_subject_name "
                       "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
        goto exit;
    }

    if ((ret = mbedtls_x509write_crt_set_issuer_name(&crt, opt.issuer_name)) != 0) {
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_issuer_name "
                       "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
        goto exit;
    }

    mbedtls_printf("  . Setting certificate values ...");
    fflush(stdout);

    mbedtls_x509write_crt_set_version(&crt, opt.version);
    mbedtls_x509write_crt_set_md_alg(&crt, opt.md);

    ret = mbedtls_x509write_crt_set_serial_raw(&crt, serial, serial_len);
    if (ret != 0) {
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_serial_raw "
                       "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
        goto exit;
    }

    ret = mbedtls_x509write_crt_set_validity(&crt, opt.not_before, opt.not_after);
    if (ret != 0) {
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_validity "
                       "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    if (opt.version == MBEDTLS_X509_CRT_VERSION_3 &&
        opt.basic_constraints != 0) {
        mbedtls_printf("  . Adding the Basic Constraints extension ...");
        fflush(stdout);

        ret = mbedtls_x509write_crt_set_basic_constraints(&crt, opt.is_ca,
                                                          opt.max_pathlen);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  x509write_crt_set_basic_constraints "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }

#if defined(PSA_WANT_ALG_SHA_1)
    if (opt.version == MBEDTLS_X509_CRT_VERSION_3 &&
        opt.subject_identifier != 0) {
        mbedtls_printf("  . Adding the Subject Key Identifier ...");
        fflush(stdout);

        ret = mbedtls_x509write_crt_set_subject_key_identifier(&crt);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_subject"
                           "_key_identifier returned -0x%04x - %s\n\n",
                           (unsigned int) -ret, buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }

    if (opt.version == MBEDTLS_X509_CRT_VERSION_3 &&
        opt.authority_identifier != 0) {
        mbedtls_printf("  . Adding the Authority Key Identifier ...");
        fflush(stdout);

        ret = mbedtls_x509write_crt_set_authority_key_identifier(&crt);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_authority_"
                           "key_identifier returned -0x%04x - %s\n\n",
                           (unsigned int) -ret, buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }
#endif /* PSA_WANT_ALG_SHA_1 */

    if (opt.version == MBEDTLS_X509_CRT_VERSION_3 &&
        opt.key_usage != 0) {
        mbedtls_printf("  . Adding the Key Usage extension ...");
        fflush(stdout);

        ret = mbedtls_x509write_crt_set_key_usage(&crt, opt.key_usage);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_key_usage "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }

    if (opt.san_list != NULL) {
        ret = mbedtls_x509write_crt_set_subject_alternative_name(&crt, opt.san_list);

        if (ret != 0) {
            mbedtls_printf(
                " failed\n  !  mbedtls_x509write_crt_set_subject_alternative_name returned %d",
                ret);
            goto exit;
        }
    }

    if (opt.ext_key_usage) {
        mbedtls_printf("  . Adding the Extended Key Usage extension ...");
        fflush(stdout);

        ret = mbedtls_x509write_crt_set_ext_key_usage(&crt, opt.ext_key_usage);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(
                " failed\n  !  mbedtls_x509write_crt_set_ext_key_usage returned -0x%02x - %s\n\n",
                (unsigned int) -ret,
                buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }

    if (opt.version == MBEDTLS_X509_CRT_VERSION_3 &&
        opt.ns_cert_type != 0) {
        mbedtls_printf("  . Adding the NS Cert Type extension ...");
        fflush(stdout);

        ret = mbedtls_x509write_crt_set_ns_cert_type(&crt, opt.ns_cert_type);
        if (ret != 0) {
            mbedtls_strerror(ret, buf, sizeof(buf));
            mbedtls_printf(" failed\n  !  mbedtls_x509write_crt_set_ns_cert_type "
                           "returned -0x%04x - %s\n\n", (unsigned int) -ret, buf);
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }

    /*
     * 1.2. Writing the certificate
     */
    mbedtls_printf("  . Writing the certificate...");
    fflush(stdout);

    if ((ret = write_certificate(&crt, opt.output_file)) != 0) {
        mbedtls_strerror(ret, buf, sizeof(buf));
        mbedtls_printf(" failed\n  !  write_certificate -0x%04x - %s\n\n",
                       (unsigned int) -ret, buf);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:
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

#if defined(MBEDTLS_X509_CSR_PARSE_C)
    mbedtls_x509_csr_free(&csr);
#endif /* MBEDTLS_X509_CSR_PARSE_C */
    mbedtls_x509_crt_free(&issuer_crt);
    mbedtls_x509write_crt_free(&crt);
    mbedtls_pk_free(&loaded_subject_key);
    mbedtls_pk_free(&loaded_issuer_key);
    mbedtls_psa_crypto_free();

    mbedtls_exit(exit_code);
}
#endif /* MBEDTLS_X509_CRT_WRITE_C && MBEDTLS_X509_CRT_PARSE_C &&
          MBEDTLS_FS_IO
          MBEDTLS_ERROR_C && MBEDTLS_PEM_WRITE_C */
