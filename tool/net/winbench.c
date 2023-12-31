#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/wsaid.h"
#include "libc/nt/errors.h"
#include "libc/nt/iocp.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/wsaid.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "third_party/getopt/getopt.internal.h"

__msabi extern typeof(__sys_bind_nt) *const __imp_bind;
__msabi extern typeof(__sys_closesocket_nt) *const __imp_closesocket;

enum State { SENDING, RECEIVING };

struct Client {
  struct NtOverlapped overlap;
  int64_t handle;
  enum State state;
  char buf[1500];
  struct NtIovec iov;
};

static int64_t iocp;
static char msg[128];
static uint32_t msglen;
static const char *prog;
static atomic_int a_termsig;
static atomic_long a_requests;
static atomic_bool a_finished;

static wontreturn void PrintUsage(int fd, int rc) {
  tinyprint(fd, "Usage: ", prog, " -H HOST -P PORT\n", NULL);
  exit(rc);
}

static bool32 (*__msabi ConnectEx)(int64_t hSocket, const void *name,
                                   int namelen, const void *opt_lpSendBuffer,
                                   uint32_t dwSendDataLength,
                                   uint32_t *opt_out_lpdwBytesSent,
                                   struct NtOverlapped *lpOverlapped);

static int64_t CreateNewCompletionPort(uint32_t dwNumberOfConcurrentThreads) {
  return CreateIoCompletionPort(-1, 0, 0, dwNumberOfConcurrentThreads);
}

static bool AssociateDeviceWithCompletionPort(int64_t hCompletionPort,
                                              int64_t hDevice,
                                              uint64_t dwCompletionKey) {
  int64_t h =
      CreateIoCompletionPort(hDevice, hCompletionPort, dwCompletionKey, 0);
  return h == hCompletionPort;
}

static void NewClient(struct Client *client, const struct sockaddr_in *addr) {

  client->handle = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0,
                             kNtWsaFlagOverlapped);
  if (client->handle == -1) {
    fprintf(stderr, "WSASocket() failed w/ %d\n", WSAGetLastError());
    exit(1);
  }
  struct sockaddr_in bindaddr = {AF_INET};
  if (__imp_bind(client->handle, &bindaddr, sizeof(bindaddr))) {
    fprintf(stderr, "bind() failed w/ %d\n", WSAGetLastError());
    exit(1);
  }

  if (!AssociateDeviceWithCompletionPort(iocp, client->handle,
                                         (intptr_t)client)) {
    fprintf(stderr, "AssociateDeviceWithCompletionPort() failed w/ %d\n",
            GetLastError());
    exit(1);
  }
  if (!SetFileCompletionNotificationModes(client->handle,
                                          kNtFileSkipSetEventOnHandle)) {
    fprintf(stderr, "SetFileCompletionNotificationModes() failed w/ %d\n",
            GetLastError());
    exit(1);
  }

  struct NtOverlapped overlap = {0};
  if (!ConnectEx(client->handle, addr, sizeof(*addr), msg, msglen, 0,
                 &overlap) &&
      WSAGetLastError() != kNtErrorIoPending) {
    fprintf(stderr, "ConnectEx() failed w/ %d\n", WSAGetLastError());
    exit(1);
  }
}

static void *Worker(void *arg) {
  while (!a_finished) {
    uint32_t i;
    uint32_t dwFlags;
    uint32_t dwRecordCount;
    struct NtOverlappedEntry records[8];
    if (!GetQueuedCompletionStatusEx(iocp, records, ARRAYLEN(records),
                                     &dwRecordCount, -1u, false)) {
      fprintf(stderr, "GetQueuedCompletionStatus() failed w/ %d\n",
              GetLastError());
      exit(1);
    }
    for (i = 0; i < dwRecordCount; ++i) {
      uint32_t dwBytes = records[i].dwNumberOfBytesTransferred;
      struct Client *client = (struct Client *)records[i].lpCompletionKey;
      switch (client->state) {
        case SENDING:
          dwFlags = 0;
          client->state = RECEIVING;
          client->iov.buf = client->buf;
          client->iov.len = sizeof(client->buf) - 1;
          bzero(&client->overlap, sizeof(client->overlap));
          if (WSARecv(client->handle, &client->iov, 1, 0, &dwFlags,
                      &client->overlap, 0) &&
              WSAGetLastError() != kNtErrorIoPending) {
            fprintf(stderr, "WSARecv() failed w/ %d\n", WSAGetLastError());
            exit(1);
          }
          break;
        case RECEIVING:
          if (!dwBytes) {
            fprintf(stderr, "got disconnect\n");
            break;
          }
          client->buf[dwBytes] = 0;
          if (!startswith(client->buf, "HTTP/1.1 200")) {
            fprintf(stderr, "request failed: %d bytes %`'s\n", dwBytes,
                    client->buf);
            exit(1);
          }
          ++a_requests;
          client->state = SENDING;
          client->iov.buf = msg;
          client->iov.len = msglen;
          bzero(&client->overlap, sizeof(client->overlap));
          if (WSASend(client->handle, &client->iov, 1, 0, 0, &client->overlap,
                      0) &&
              WSAGetLastError() != kNtErrorIoPending) {
            fprintf(stderr, "WSASend() failed w/ %d\n", WSAGetLastError());
            exit(1);
          }
          break;
        default:
          __builtin_unreachable();
      }
    }
  }
  return 0;
}

static void OnTerm(int sig) {
  a_termsig = sig;
}

int main(int argc, char *argv[]) {

  if (!IsWindows()) {
    tinyprint(2, "error: this program is intended for windows\n", NULL);
    return 1;
  }

  prog = argv[0];
  if (!prog) {
    prog = "winbench";
  }

  int opt;
  int nclients = 1000;
  int nthreads = GetMaximumProcessorCount(0xffff);
  struct sockaddr_in destaddr = {AF_INET, htons(8080), {htonl(0x7f000001)}};
  while ((opt = getopt(argc, argv, "hH:P:")) != -1) {
    switch (opt) {
      case 'H':
        destaddr.sin_addr.s_addr = inet_addr(optarg);
        break;
      case 'P':
        destaddr.sin_port = htons(atoi(optarg));
        break;
      case 'h':
        PrintUsage(1, 0);
      default:
        PrintUsage(2, 1);
    }
  }

  uint32_t ip = ntohl(destaddr.sin_addr.s_addr);
  uint16_t port = ntohs(destaddr.sin_port);
  msglen = snprintf(msg, sizeof(msg),
                    "GET / HTTP/1.1\r\n"
                    "Host: %hhu.%hhu.%hhu.%hhu:%hu\r\n"
                    "\r\n",
                    ip >> 24, ip >> 16, ip >> 8, ip, port);

  struct NtWsaData kNtWsaData = {0};
  WSAStartup(0x0202, &kNtWsaData);

  static struct NtGuid ConnectExGuid = WSAID_CONNECTEX;
  ConnectEx = __get_wsaid(&ConnectExGuid);

  iocp = CreateNewCompletionPort(0);
  if (!iocp) {
    fprintf(stderr, "CreateNewCompletionPort() failed w/ %d\n", GetLastError());
    exit(1);
  }

  struct sigaction sa = {.sa_handler = OnTerm};
  unassert(!sigaction(SIGINT, &sa, 0));
  unassert(!sigaction(SIGHUP, &sa, 0));
  unassert(!sigaction(SIGTERM, &sa, 0));

  sigset_t block;
  sigfillset(&block);
  pthread_attr_t attr;
  int pagesz = getauxval(AT_PAGESZ);
  pthread_t *threads = calloc(nthreads, sizeof(pthread_t));
  unassert(!pthread_attr_init(&attr));
  unassert(!pthread_attr_setstacksize(&attr, 65536));
  unassert(!pthread_attr_setguardsize(&attr, pagesz));
  unassert(!pthread_attr_setsigmask_np(&attr, &block));
  for (int i = 0; i < nthreads; ++i) {
    if ((errno = pthread_create(threads + i, &attr, Worker, 0))) {
      perror("pthread_create");
      exit(1);
    }
  }
  unassert(!pthread_attr_destroy(&attr));

  struct timespec start_time = timespec_real();

  struct Client *clients = calloc(nclients, sizeof(struct Client));
  for (int i = 0; i < nclients; ++i) {
    NewClient(clients + i, &destaddr);
  }

  sleep(10);

  a_finished = true;
  long request_count = a_requests;
  struct timespec end_time = timespec_real();

  for (int i = 0; i < nthreads; ++i) {
    if ((errno = pthread_join(threads[i], 0))) {
      perror("pthread_join");
      exit(1);
    }
  }

  double elapsed_seconds =
      timespec_tonanos(timespec_sub(end_time, start_time)) * 1e-9;
  double requests_per_second = request_count / elapsed_seconds;
  fprintf(stderr, "qps = %g\n", requests_per_second);
}
