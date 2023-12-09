/*
 * Copyright (c) 2013 by Wil Tan <wil@cloudregistry.net>
 *
 * Based on dump_dns.c from the dnscap <https://www.dns-oarc.net/tools/dnscap>
 * originally written by Paul Vixie.
 *
 * Copyright (c) 2007 by Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "libc/calls/typedef/u.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "third_party/musl/nameser.h"
#include "third_party/musl/resolv.h"

const char *_res_opcodes[] = {
    "QUERY",     //
    "IQUERY",    //
    "CQUERYM",   //
    "CQUERYU",   //
    "NOTIFY",    //
    "UPDATE",    //
    "6",         //
    "7",         //
    "8",         //
    "9",         //
    "10",        //
    "11",        //
    "12",        //
    "13",        //
    "ZONEINIT",  //
    "ZONEREF",   //
};

#define MY_GET16(s, cp)                                     \
  do {                                                      \
    register const u_char *t_cp = (const u_char *)(cp);     \
    (s) = ((u_int16_t)t_cp[0] << 8) | ((u_int16_t)t_cp[1]); \
    (cp) += NS_INT16SZ;                                     \
  } while (0)

#define MY_GET32(l, cp)                                             \
  do {                                                              \
    register const u_char *t_cp = (const u_char *)(cp);             \
    (l) = ((u_int32_t)t_cp[0] << 24) | ((u_int32_t)t_cp[1] << 16) | \
          ((u_int32_t)t_cp[2] << 8) | ((u_int32_t)t_cp[3]);         \
    (cp) += NS_INT32SZ;                                             \
  } while (0)

static void dump_dns_rr(ns_msg *msg, ns_rr *rr, ns_sect sect, FILE *trace) {
  char buf[NS_MAXDNAME];
  u_int class, type;
  const u_char *rd;
  u_int32_t soa[5];
  u_int16_t mx;
  int n;

  class = ns_rr_class(*rr);
  type = ns_rr_type(*rr);
  fprintf(trace, "%s,%d,%d", ns_rr_name(*rr), class, type);
  if (sect == ns_s_qd) return;
  fprintf(trace, ",%lu", (u_long)ns_rr_ttl(*rr));
  rd = ns_rr_rdata(*rr);
  switch (type) {
    case ns_t_soa:
      n = ns_name_uncompress(ns_msg_base(*msg), ns_msg_end(*msg), rd, buf,
                             sizeof buf);
      if (n < 0) goto error;
      putc(',', trace);
      fputs(buf, trace);
      rd += n;
      n = ns_name_uncompress(ns_msg_base(*msg), ns_msg_end(*msg), rd, buf,
                             sizeof buf);
      if (n < 0) goto error;
      putc(',', trace);
      fputs(buf, trace);
      rd += n;
      if (ns_msg_end(*msg) - rd < 5 * NS_INT32SZ) goto error;
      for (n = 0; n < 5; n++) MY_GET32(soa[n], rd);
      sprintf(buf, "%u,%u,%u,%u,%u", soa[0], soa[1], soa[2], soa[3], soa[4]);
      break;
    case ns_t_a:
      inet_ntop(AF_INET, rd, buf, sizeof buf);
      break;
    case ns_t_aaaa:
      inet_ntop(AF_INET6, rd, buf, sizeof buf);
      break;
    case ns_t_mx:
      MY_GET16(mx, rd);
      fprintf(trace, ",%u", mx);
      /* FALLTHROUGH */
    case ns_t_ns:
    case ns_t_ptr:
    case ns_t_cname:
      n = ns_name_uncompress(ns_msg_base(*msg), ns_msg_end(*msg), rd, buf,
                             sizeof buf);
      if (n < 0) goto error;
      break;
    case ns_t_txt:
      snprintf(buf, (size_t)rd[0] + 1, "%s", rd + 1);
      break;
    default:
    error:
      sprintf(buf, "[%u]", ns_rr_rdlen(*rr));
  }
  if (buf[0] != '\0') {
    putc(',', trace);
    fputs(buf, trace);
  }
}

static void dump_dns_sect(ns_msg *msg, ns_sect sect, FILE *trace,
                          const char *endline) {
  int rrnum, rrmax;
  const char *sep;
  ns_rr rr;

  rrmax = ns_msg_count(*msg, sect);
  if (rrmax == 0) {
    fputs(" 0", trace);
    return;
  }
  fprintf(trace, " %s%d", endline, rrmax);
  sep = "";
  for (rrnum = 0; rrnum < rrmax; rrnum++) {
    if (ns_parserr(msg, sect, rrnum, &rr)) {
      fputs(strerror(errno), trace);
      return;
    }
    fprintf(trace, " %s", sep);
    dump_dns_rr(msg, &rr, sect, trace);
    sep = endline;
  }
}

void dump_dns(const u_char *payload, size_t paylen, FILE *trace,
              const char *endline) {
  u_int opcode, rcode, id;
  const char *sep;
  ns_msg msg;

  fprintf(trace, " %sdns ", endline);
  if (ns_initparse(payload, paylen, &msg) < 0) {
    fputs(strerror(errno), trace);
    return;
  }
  opcode = ns_msg_getflag(msg, ns_f_opcode);
  rcode = ns_msg_getflag(msg, ns_f_rcode);
  id = ns_msg_id(msg);
  fprintf(trace, "%s,%d,%u", _res_opcodes[opcode], rcode, id);
  sep = ",";
#define FLAG(t, f)                  \
  if (ns_msg_getflag(msg, f)) {     \
    fprintf(trace, "%s%s", sep, t); \
    sep = "|";                      \
  }
  FLAG("qr", ns_f_qr);
  FLAG("aa", ns_f_aa);
  FLAG("tc", ns_f_tc);
  FLAG("rd", ns_f_rd);
  FLAG("ra", ns_f_ra);
  FLAG("z", ns_f_z);
  FLAG("ad", ns_f_ad);
  FLAG("cd", ns_f_cd);
#undef FLAG

  dump_dns_sect(&msg, ns_s_an, trace, endline);
}

int main() {
  ShowCrashReports();
  u_char answer[1024] = "";
  res_init();
  int rv = res_query("google.com", ns_c_in, ns_t_txt, answer, sizeof(answer));
  // printf("rv=%d\n", rv);
  dump_dns(answer, rv, stdout, "\n");
  printf("\n");
}
