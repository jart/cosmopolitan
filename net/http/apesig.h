#ifndef COSMOPOLITAN_NET_HTTP_APESIG_H_
#define COSMOPOLITAN_NET_HTTP_APESIG_H_
COSMOPOLITAN_C_START_

#define APESIG_MAGIC   0x0000110011110001
#define APETRUST_MAGIC 0x0000100101000001

#define APESIG_ALGORITHM_ED25519 1

#define APESIG_ERR_OOM       1
#define APESIG_ERR_MAGIC     2
#define APESIG_ERR_ALGORITHM 3

#define APESIG_ZIP_NAME ".apesig"

#define APESIG_SIG_ELF  0
#define APESIG_SIG_PHDR 1

struct ApeBytes {
  size_t size;
  uint8_t *data;
};

struct ApeSigSig {
  uint8_t algorithm;
  struct ApeBytes sig;
};

void DestroyApeSigSig(struct ApeSigSig *);
int ParseApeSigSig(const uint8_t *, size_t, struct ApeSigSig *);
int SerializeApeSigSig(const struct ApeSigSig *, struct ApeBytes *);
int CheckApeSigSig(const struct ApeSigSig *);

struct ApeSigKey {
  uint8_t algorithm;
  struct ApeBytes pub;
};

void DestroyApeSigKey(struct ApeSigKey *);
int ParseApeSigKey(const uint8_t *, size_t, struct ApeSigKey *);
int SerializeApeSigKey(const struct ApeSigKey *, struct ApeBytes *);
int CheckApeSigKey(const struct ApeSigKey *);

struct ApeSigElfPhdr {
  uint32_t type;
  uint32_t flags;
  struct ApeSigSig sig;
};

void DestroyApeSigElfPhdr(struct ApeSigElfPhdr *);
int ParseApeSigElfPhdr(const uint8_t *, size_t, struct ApeSigElfPhdr *);
int SerializeApeSigElfPhdr(const struct ApeSigElfPhdr *, struct ApeBytes *);
int CheckApeSigElfPhdr(const struct ApeSigElfPhdr *);

struct ApeSigElf {
  uint16_t machine;
  uint8_t phdrs_count;
  struct ApeSigSig sig;
  struct ApeSigElfPhdr *phdrs;
};

void DestroyApeSigElf(struct ApeSigElf *);
int ParseApeSigElf(const uint8_t *, size_t, struct ApeSigElf *);
int SerializeApeSigElf(const struct ApeSigElf *, struct ApeBytes *);
int CheckApeSigElf(const struct ApeSigElf *);

struct ApeSig {
  const char *path;
  int64_t issued;
  int64_t expires;
  const char *identity;
  const char *github;
  struct ApeSigKey key;
  uint8_t elfs_count;
  struct ApeSigElf *elfs;
};

void DestroyApeSig(struct ApeSig *);
int ParseApeSig(const uint8_t *, size_t, struct ApeSig *);
int SerializeApeSig(const struct ApeSig *, struct ApeBytes *);
int CheckApeSig(const struct ApeSig *);

struct ApeSigs {
  uint64_t magic;
  uint16_t sigs_count;
  struct ApeSig *sigs;
};

void DestroyApeSigs(struct ApeSigs *);
int ParseApeSigs(const uint8_t *, size_t, struct ApeSigs *);
int SerializeApeSigs(const struct ApeSigs *, struct ApeBytes *);
int CheckApeSigs(const struct ApeSigs *);

struct ApeTrust {
  int64_t issued;
  int64_t expires;
  const char *comment;
  const char *github;
  struct ApeSigKey key;
};

void DestroyApeTrust(struct ApeTrust *);
int ParseApeTrust(const uint8_t *, size_t, struct ApeTrust *);
int SerializeApeTrust(const struct ApeTrust *, struct ApeBytes *);
int CheckApeTrust(const struct ApeTrust *);

struct ApeTrusts {
  uint64_t magic;
  uint32_t trusts_count;
  struct ApeTrust *trusts;
};

void DestroyApeTrusts(struct ApeTrusts *);
int ParseApeTrusts(const uint8_t *, size_t, struct ApeTrusts *);
int SerializeApeTrusts(const struct ApeTrusts *, struct ApeBytes *);
int CheckApeTrusts(const struct ApeTrusts *);

void DescribeApeSigError(char *, size_t, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTP_APESIG_H_ */
