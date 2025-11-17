#ifndef COSMOPOLITAN_NET_HTTP_SSH_H_
#define COSMOPOLITAN_NET_HTTP_SSH_H_
COSMOPOLITAN_C_START_

#define SSH_ERR_OUT_OF_MEMORY         1
#define SSH_ERR_NOT_OPENSSH_KEY_V1    2
#define SSH_ERR_KEY_IS_ENCRYPTED      3
#define SSH_ERR_KEY_ISNT_ED25519      4
#define SSH_ERR_KEY_CORRUPTED         5
#define SSH_ERR_ALGORITHM_UNSUPPORTED 6

struct OpensshPublicKey {
  char *algo;
  uint32_t size;
  uint8_t *data;
  char *comment;
};

struct OpensshPrivateKey {
  uint32_t rng1;
  uint32_t rng2;
  struct OpensshPublicKey pub;
  size_t size;
  uint8_t *data;
  char *comment;
};

struct OpensshKey {
  struct OpensshPublicKey pub;
  struct OpensshPrivateKey key;
};

struct OpensshKeyV1 {
  char *cipher;
  char *kdfname;
  size_t kdfsize;
  char *kdfparams;
  size_t keycount;
  struct OpensshKey *keys;
};

struct OpensshKnownHost {
  char *host;
  uint8_t salt[20];
  uint8_t hash[20];
  struct OpensshPublicKey pub;
};

struct OpensshKnownHosts {
  size_t n;
  size_t c;
  struct OpensshKnownHost *p;
};

int ParseEd25519PrivateKey(const char *, size_t, uint8_t[32], char **);
int ExtractEd25519PrivateKey(const struct OpensshKeyV1 *, uint8_t[32], char **);

int ParseOpensshKeyV1(const char *, size_t, struct OpensshKeyV1 *);
int CheckOpensshKeyV1(const struct OpensshKeyV1 *);
void DestroyOpensshKeyV1(struct OpensshKeyV1 *);

int ParseOpensshPublicKey(const char *, size_t, struct OpensshPublicKey *);
void DestroyOpensshPublicKey(struct OpensshPublicKey *);

int ParseOpensshKnownHost(const char *, size_t, struct OpensshKnownHost *);
void DestroyOpensshKnownHost(struct OpensshKnownHost *);

int ParseOpensshKnownHosts(const char *, size_t, struct OpensshKnownHosts *);
void DestroyOpensshKnownHosts(struct OpensshKnownHosts *);

void DescribeOpensshParseError(char *, size_t, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTP_SSH_H_ */
