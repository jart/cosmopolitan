#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "libc/sysv/errfuns.h"
#include "net/http/escape.h"
#include "third_party/haclstar/haclstar.h"

// Server's long-term Curve25519 keypair (hardcoded for demo)
static const uint8_t server_private_key[32] = {
    0xa0, 0x76, 0x73, 0xf4, 0x5a, 0x73, 0x8d, 0xda,  //
    0xa5, 0x26, 0xae, 0x98, 0x44, 0x55, 0xdf, 0x00,  //
    0x40, 0xc5, 0x53, 0x9d, 0xf4, 0x9e, 0x7e, 0xb9,  //
    0xf3, 0xb4, 0xdf, 0x04, 0xf7, 0x4d, 0x21, 0x7c,  //
};

static const uint8_t server_public_key[32] = {
    0x20, 0xdc, 0x38, 0x99, 0x7d, 0x22, 0x62, 0xde,  //
    0xc8, 0x77, 0x59, 0xe3, 0x75, 0x7a, 0x3b, 0xca,  //
    0xd7, 0x36, 0x88, 0x67, 0xec, 0x65, 0x48, 0x97,  //
    0x17, 0x7a, 0x1d, 0xf9, 0xe0, 0x52, 0xfa, 0x49,  //
};

static Hacl_Impl_HPKE_context_s hpke_ctx;

static bool Tune(int fd, int a, int b, int x) {
  if (!b)
    return false;
  return setsockopt(fd, a, b, &x, sizeof(x)) != -1;
}

static int Socket(int family, int type, int protocol) {
  int fd;
  if ((fd = socket(family, type, protocol)) != -1) {
    Tune(fd, SOL_SOCKET, SO_REUSEADDR, 1);
    Tune(fd, SOL_TCP, TCP_FASTOPEN, 1);
    Tune(fd, SOL_TCP, TCP_QUICKACK, 1);
    Tune(fd, SOL_TCP, TCP_NODELAY, 1);
  }
  return fd;
}

static ssize_t encrypt_message(const uint8_t* plaintext, size_t plaintext_len,
                               uint8_t* ciphertext) {
  uint8_t nonce[12];
  if (Hacl_HPKE_getNonce(&hpke_ctx, nonce))
    return -1;
  uint8_t* cipher = ciphertext;
  uint8_t* tag = ciphertext + plaintext_len;
  Hacl_AEAD_Chacha20Poly1305_encrypt(cipher, tag, plaintext, plaintext_len,
                                     NULL, 0, hpke_ctx.ctx_key, nonce);
  return plaintext_len + 16;  // ciphertext + tag
}

static ssize_t decrypt_message(const uint8_t* ciphertext, size_t ciphertext_len,
                               uint8_t* plaintext) {
  if (ciphertext_len < 16)
    return -1;
  uint8_t nonce[12];
  if (Hacl_HPKE_getNonce(&hpke_ctx, nonce))
    return -1;
  const uint8_t* cipher = ciphertext;
  const uint8_t* tag = ciphertext + ciphertext_len - 16;
  if (Hacl_AEAD_Chacha20Poly1305_decrypt(plaintext, cipher, ciphertext_len - 16,
                                         NULL, 0, hpke_ctx.ctx_key, nonce, tag))
    return -1;
  return ciphertext_len - 16;  // plaintext length
}

int main(int argc, char* argv[]) {

  // get args
  if (argc != 3) {
    fprintf(stderr, "usage: %s HOST PORT\n", argv[0]);
    exit(1);
  }
  const char* host = argv[1];
  const char* port = argv[2];

  // perform dns lookup
  int err;
  struct addrinfo* addr;
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  if ((err = getaddrinfo(host, port, &hints, &addr))) {
    fprintf(stderr, "%s:%s: dns lookup failed: %s\n", host, port,
            gai_strerror(err));
    exit(1);
  }

  // listen for client
  int sockfd;
  if ((sockfd = Socket(addr->ai_family, addr->ai_socktype,
                       addr->ai_protocol)) == -1) {
    perror("socket");
    exit(1);
  }
  if (bind(sockfd, addr->ai_addr, addr->ai_addrlen)) {
    fprintf(stderr, "%s:%s: bind failed: %s\n", host, port, strerror(errno));
    exit(1);
  }
  freeaddrinfo(addr);
  if (listen(sockfd, SOMAXCONN)) {
    perror("listen");
    exit(1);
  }

  // wait for client
  int clientfd;
  if ((clientfd = accept(sockfd, 0, 0)) == -1) {
    perror("listen");
    exit(1);
  }

  // Perform HPKE handshake - receive client's ephemeral public key
  uint8_t client_ephemeral_pubkey[32];
  ssize_t got = read(clientfd, client_ephemeral_pubkey, 32);
  if (got != 32) {
    fprintf(stderr, "Failed to receive client ephemeral public key\n");
    exit(1);
  }

  // Setup HPKE receiver context
  const uint8_t info[] = "chat-server-v1";
  if (Hacl_HPKE_Curve25519_CP128_SHA256_setupBaseR(
          &hpke_ctx, client_ephemeral_pubkey, server_private_key,
          sizeof(info) - 1, info) == -1) {
    fprintf(stderr, "HPKE setup failed\n");
    exit(1);
  }

  // Print HPKE context
  printf("HPKE ctx_key: %s\n", EncodeBase64(hpke_ctx.ctx_key, 32, 0));
  printf("HPKE ctx_nonce: %s\n", EncodeBase64(hpke_ctx.ctx_nonce, 12, 0));
  printf("HPKE ctx_exporter: %s\n", EncodeBase64(hpke_ctx.ctx_exporter, 32, 0));

  // chat loop
  for (;;) {
    struct pollfd fds[2] = {
        {0, POLLIN},
        {clientfd, POLLIN},
    };
    if (poll(fds, 2, -1) == -1) {
      perror("poll");
      exit(1);
    }

    // handle activity in terminal
    if (fds[0].revents) {
      char buf[512];
      ssize_t got = read(0, buf, sizeof(buf));
      if (got == -1) {
        perror("read");
        exit(1);
      }
      if (!got)
        break;

      // Encrypt message before sending
      uint8_t encrypted[512 + 16];  // plaintext + tag
      ssize_t encrypted_len = encrypt_message((uint8_t*)buf, got, encrypted);
      if (encrypted_len == -1) {
        fprintf(stderr, "Encryption failed\n");
        exit(1);
      }

      ssize_t wrote = write(clientfd, encrypted, encrypted_len);
      if (wrote == -1) {
        fprintf(stderr, "%s:%s: send failed: %s\n", host, port,
                strerror(errno));
        exit(1);
      }
    }

    // handle activity on socket
    if (fds[1].revents) {
      uint8_t encrypted_buf[512 + 16];
      ssize_t got = read(clientfd, encrypted_buf, sizeof(encrypted_buf));
      if (got == -1) {
        fprintf(stderr, "%s:%s: recv failed: %s\n", host, port,
                strerror(errno));
        exit(1);
      }
      if (!got)
        break;

      // Decrypt message before displaying
      char plaintext[512];
      ssize_t plaintext_len =
          decrypt_message(encrypted_buf, got, (uint8_t*)plaintext);
      if (plaintext_len == -1) {
        fprintf(stderr, "Decryption failed\n");
        exit(1);
      }

      ssize_t wrote = write(1, plaintext, plaintext_len);
      if (wrote == -1) {
        perror("write");
        exit(1);
      }
    }
  }

  // cleanup
  if (close(clientfd)) {
    fprintf(stderr, "%s:%s: close failed: %s\n", host, port, strerror(errno));
    exit(1);
  }
  if (close(sockfd)) {
    fprintf(stderr, "%s:%s: close failed: %s\n", host, port, strerror(errno));
    exit(1);
  }
}
