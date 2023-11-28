#ifndef COSMOPOLITAN_LIBC_CALLS_LANDLOCK_H_
#define COSMOPOLITAN_LIBC_CALLS_LANDLOCK_H_

#define LANDLOCK_CREATE_RULESET_VERSION 0x0001ul

#define LANDLOCK_ACCESS_FS_EXECUTE     0x0001ul
#define LANDLOCK_ACCESS_FS_WRITE_FILE  0x0002ul
#define LANDLOCK_ACCESS_FS_READ_FILE   0x0004ul
#define LANDLOCK_ACCESS_FS_READ_DIR    0x0008ul
#define LANDLOCK_ACCESS_FS_REMOVE_DIR  0x0010ul
#define LANDLOCK_ACCESS_FS_REMOVE_FILE 0x0020ul
#define LANDLOCK_ACCESS_FS_MAKE_CHAR   0x0040ul
#define LANDLOCK_ACCESS_FS_MAKE_DIR    0x0080ul
#define LANDLOCK_ACCESS_FS_MAKE_REG    0x0100ul
#define LANDLOCK_ACCESS_FS_MAKE_SOCK   0x0200ul
#define LANDLOCK_ACCESS_FS_MAKE_FIFO   0x0400ul
#define LANDLOCK_ACCESS_FS_MAKE_BLOCK  0x0800ul
#define LANDLOCK_ACCESS_FS_MAKE_SYM    0x1000ul

/**
 * Allow renaming or linking file to a different directory.
 *
 * @see https://lore.kernel.org/r/20220329125117.1393824-8-mic@digikod.net
 * @see https://docs.kernel.org/userspace-api/landlock.html
 * @note ABI 2+
 */
#define LANDLOCK_ACCESS_FS_REFER 0x2000ul

/**
 * Control file truncation.
 *
 * @see
 * https://lore.kernel.org/all/20221018182216.301684-1-gnoack3000@gmail.com/
 * @see https://docs.kernel.org/userspace-api/landlock.html
 * @note ABI 3+
 */
#define LANDLOCK_ACCESS_FS_TRUNCATE 0x4000ul

COSMOPOLITAN_C_START_

enum landlock_rule_type {
  LANDLOCK_RULE_PATH_BENEATH = 1,
};

struct landlock_ruleset_attr {
  uint64_t handled_access_fs;
};

struct thatispacked landlock_path_beneath_attr {
  uint64_t allowed_access;
  int32_t parent_fd;
};

int landlock_restrict_self(int, uint32_t);
int landlock_add_rule(int, enum landlock_rule_type, const void *, uint32_t);
int landlock_create_ruleset(const struct landlock_ruleset_attr *, size_t,
                            uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_LANDLOCK_H_ */
