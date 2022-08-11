#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/intrin/bits.h"
#include "libc/stdio/stdio.h"
// clang-format off

static char ip[] = "\
\x45\x00\x00\x3c\xc8\xbc\x40\x00\x40\x06\x48\xf6\x7f\x0a\x0a\x7a\
\x7f\x0a\x0a\x7c\xe2\x24\x1b\x58\xf6\xe9\xf2\x85\x00\x00\x00\x00\
\xa0\x02\xfa\xf0\x5f\xac\x00\x00\x02\x04\x05\xb4\x04\x02\x08\x0a\
\x4a\x43\x93\x29\x00\x00\x00\x00\x01\x03\x03\x07";

static const char *const kTcpOptionNames[] = {
    [0] = "End of Option List",
    [1] = "No-Operation",
    [2] = "Maximum Segment Size",
    [3] = "Window Scale",
    [4] = "SACK Permitted",
    [5] = "SACK",
    [6] = "Echo (obsoleted by option 8)",
    [7] = "Echo Reply (obsoleted by option 8)",
    [8] = "Timestamps",
    [9] = "Partial Order Connection Permitted (obsolete)",
    [10] = "Partial Order Service Profile (obsolete)",
    [11] = "CC (obsolete)",
    [12] = "CC.NEW (obsolete)",
    [13] = "CC.ECHO (obsolete)",
    [14] = "TCP Alternate Checksum Request (obsolete)",
    [15] = "TCP Alternate Checksum Data (obsolete)",
    [16] = "Skeeter",
    [17] = "Bubba",
    [18] = "Trailer Checksum Option",
    [19] = "MD5 Signature Option (obsoleted by option 29)",
    [20] = "SCPS Capabilities",
    [21] = "Selective Negative Acknowledgements",
    [22] = "Record Boundaries",
    [23] = "Corruption experienced",
    [24] = "SNAP",
    [25] = "Unassigned (released 2000-12-18)",
    [26] = "TCP Compression Filter",
    [27] = "Quick-Start Response",
    [28] = "User Timeout Option",
    [29] = "TCP Authentication Option (TCP-AO)",
    [30] = "Multipath TCP (MPTCP)",
    [31] = "Reserved (known unauthorized use without proper IANA assignm",
    [32] = "Reserved (known unauthorized use without proper IANA assignm",
    [33] = "Reserved (known unauthorized use without proper IANA assignm",
    [34] = "variable TCP Fast Open Cookie",
    [35] = "Reserved",
    [69] = "Encryption Negotiation",
    [253] = "RFC3692-1",
    [254] = "RFC3692-2",
};

int main(int argc, char *argv[]) {
  int version = (ip[0] & 0b11110000) >> 4;
  int ihl = (ip[0] & 0b00001111) >> 0;
  int dscp = (ip[1] & 0b11111100) >> 2;
  int ecn = (ip[1] & 0b00000011) >> 0;
  int lengthtotal = READ16BE(ip + 2);
  int identification = READ16BE(ip + 4);
  int flags = (ip[6] & 0b11100000) >> 5;
  int fragmentoffset = (ip[6] & 0b00011111) << 8 | (ip[7] & 255);
  int ttl = ip[8] & 255;
  int protocol = ip[9] & 255;
  int ipchecksum = (ip[10] & 255) << 8 | (ip[11] & 255);
  int srcip = READ32BE(ip + 12);
  int dstip = READ32BE(ip + 16);

  printf("\n");
  printf("// version        = %u\n", version);
  printf("// ihl            = %u\n", ihl * 4);
  printf("// dscp           = %u\n", dscp);
  printf("// ecn            = %u\n", ecn);
  printf("// lengthtotal    = %u\n", lengthtotal);
  printf("// identification = %u\n", identification);
  printf("// flags          = %u\n", flags);
  printf("// fragmentoffset = %u\n", fragmentoffset);
  printf("// ttl            = %u\n", ttl);
  printf("// protocol       = %u\n", protocol);
  printf("// ipchecksum     = %u\n", ipchecksum);
  printf("// srcip          = %hhu.%hhu.%hhu.%hhu\n", srcip >> 24, srcip >> 16, srcip >> 8, srcip);
  printf("// dstip          = %hhu.%hhu.%hhu.%hhu\n", dstip >> 24, dstip >> 16, dstip >> 8, dstip);
  printf("// \n");

  char *tcp = ip + ihl * 4;
  int srcport = READ16BE(tcp + 0);
  int dstport = READ16BE(tcp + 2);
  int sequence = READ32BE(tcp + 4);
  int acknumber = READ32BE(tcp + 8);
  int dataoffset = (tcp[12] & 0b11110000) >> 4;
  bool ns = !!(tcp[12] & 0b00000001);
  bool cwr = !!(tcp[13] & 0b10000000);
  bool ece = !!(tcp[13] & 0b01000000);
  bool urg = !!(tcp[13] & 0b00100000);
  bool ack = !!(tcp[13] & 0b00010000);
  bool psh = !!(tcp[13] & 0b00001000);
  bool rst = !!(tcp[13] & 0b00000100);
  bool syn = !!(tcp[13] & 0b00000010);
  bool fin = !!(tcp[13] & 0b00000001);
  int wsize = READ16BE(tcp + 14);
  int tcpchecksum = READ16BE(tcp + 16);
  int urgpointer = READ16BE(tcp + 18);

  printf("// srcport     = %u\n", srcport);
  printf("// dstport     = %u\n", dstport);
  printf("// sequence    = %u\n", sequence);
  printf("// acknumber   = %u\n", acknumber);
  printf("// dataoffset  = %u\n", dataoffset);
  printf("// ns          = %u\n", ns);
  printf("// cwr         = %u\n", cwr);
  printf("// ece         = %u\n", ece);
  printf("// urg         = %u\n", urg);
  printf("// ack         = %u\n", ack);
  printf("// psh         = %u\n", psh);
  printf("// rst         = %u\n", rst);
  printf("// syn         = %u\n", syn);
  printf("// fin         = %u\n", fin);
  printf("// wsize       = %u\n", wsize);
  printf("// tcpchecksum = %u\n", tcpchecksum);
  printf("// urgpointer  = %u\n", urgpointer);
  printf("// \n");

  int c, i, j, n;
  for (i = 20; i + 1 < dataoffset * 4;) {
    printf("// option");
    switch ((c = tcp[i] & 255)) {
      case 0:
      case 1:
        printf(" %u", c);
        ++i;
        break;
      default:
        n = tcp[i + 1] & 255;
        printf(" %u %u", c, n);
        for (j = 2; j < n; ++j) {
          printf(" %u", tcp[i + j] & 255);
        }
        i += n;
        break;
    }
    if (kTcpOptionNames[c]) {
      printf(" (%s)", kTcpOptionNames[c]);
    }
    printf("\n");
  }

  return 0;
}
