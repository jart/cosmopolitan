#ifndef COSMOPOLITAN_THIRD_PARTY_MAXMIND_MAXMINDDB_H_
#define COSMOPOLITAN_THIRD_PARTY_MAXMIND_MAXMINDDB_H_
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"

#define MMDB_MODE_MMAP 1
#define MMDB_MODE_MASK 7

#define MMDB_DATA_TYPE_EXTENDED    0
#define MMDB_DATA_TYPE_POINTER     1
#define MMDB_DATA_TYPE_UTF8_STRING 2
#define MMDB_DATA_TYPE_DOUBLE      3
#define MMDB_DATA_TYPE_BYTES       4
#define MMDB_DATA_TYPE_UINT16      5
#define MMDB_DATA_TYPE_UINT32      6
#define MMDB_DATA_TYPE_MAP         7
#define MMDB_DATA_TYPE_INT32       8
#define MMDB_DATA_TYPE_UINT64      9
#define MMDB_DATA_TYPE_UINT128     10
#define MMDB_DATA_TYPE_ARRAY       11
#define MMDB_DATA_TYPE_CONTAINER   12
#define MMDB_DATA_TYPE_END_MARKER  13
#define MMDB_DATA_TYPE_BOOLEAN     14
#define MMDB_DATA_TYPE_FLOAT       15

#define MMDB_RECORD_TYPE_SEARCH_NODE 0
#define MMDB_RECORD_TYPE_EMPTY       1
#define MMDB_RECORD_TYPE_DATA        2
#define MMDB_RECORD_TYPE_INVALID     3

#define MMDB_SUCCESS                               0
#define MMDB_FILE_OPEN_ERROR                       1
#define MMDB_CORRUPT_SEARCH_TREE_ERROR             2
#define MMDB_INVALID_METADATA_ERROR                3
#define MMDB_IO_ERROR                              4
#define MMDB_OUT_OF_MEMORY_ERROR                   5
#define MMDB_UNKNOWN_DATABASE_FORMAT_ERROR         6
#define MMDB_INVALID_DATA_ERROR                    7
#define MMDB_INVALID_LOOKUP_PATH_ERROR             8
#define MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR 9
#define MMDB_INVALID_NODE_NUMBER_ERROR             10
#define MMDB_IPV6_LOOKUP_IN_IPV4_DATABASE_ERROR    11

COSMOPOLITAN_C_START_

/* This is a pointer into the data section for a given IP address lookup */
typedef struct MMDB_entry_s {
  const struct MMDB_s *mmdb;
  uint32_t offset;
} MMDB_entry_s;

typedef struct MMDB_lookup_result_s {
  bool found_entry;
  MMDB_entry_s entry;
  uint16_t netmask;
} MMDB_lookup_result_s;

typedef struct MMDB_entry_data_s {
  bool has_data;
  union {
    uint32_t pointer;
    const char *utf8_string;
    double double_value;
    const uint8_t *bytes;
    uint16_t uint16;
    uint32_t uint32;
    int32_t int32;
    uint64_t uint64;
    uint128_t uint128;
    bool boolean;
    float float_value;
  };
  /* This is a 0 if a given entry cannot be found. This can only happen
   * when a call to MMDB_(v)get_value() asks for hash keys or array
   * indices that don't exist. */
  uint32_t offset;
  /* This is the next entry in the data section, but it's really only
   * relevant for entries that part of a larger map or array
   * struct. There's no good reason for an end user to look at this
   * directly. */
  uint32_t offset_to_next;
  /* This is only valid for strings, utf8_strings or binary data */
  uint32_t data_size;
  /* This is an MMDB_DATA_TYPE_* constant */
  uint32_t type;
} MMDB_entry_data_s;

/* This is the return type when someone asks for all the entry data in a map or
 * array */
typedef struct MMDB_entry_data_list_s {
  MMDB_entry_data_s entry_data;
  struct MMDB_entry_data_list_s *next;
  void *pool;
} MMDB_entry_data_list_s;

typedef struct MMDB_description_s {
  const char *language;
  const char *description;
} MMDB_description_s;

/* WARNING: do not add new fields to this struct without bumping the SONAME.
 * The struct is allocated by the users of this library and increasing the
 * size will cause existing users to allocate too little space when the shared
 * library is upgraded */
typedef struct MMDB_metadata_s {
  uint32_t node_count;
  uint16_t record_size;
  uint16_t ip_version;
  const char *database_type;
  struct {
    size_t count;
    const char **names;
  } languages;
  uint16_t binary_format_major_version;
  uint16_t binary_format_minor_version;
  uint64_t build_epoch;
  struct {
    size_t count;
    MMDB_description_s **descriptions;
  } description;
  /* See above warning before adding fields */
} MMDB_metadata_s;

/* WARNING: do not add new fields to this struct without bumping the SONAME.
 * The struct is allocated by the users of this library and increasing the
 * size will cause existing users to allocate too little space when the shared
 * library is upgraded */
typedef struct MMDB_ipv4_start_node_s {
  uint16_t netmask;
  uint32_t node_value;
  /* See above warning before adding fields */
} MMDB_ipv4_start_node_s;

/* WARNING: do not add new fields to this struct without bumping the SONAME.
 * The struct is allocated by the users of this library and increasing the
 * size will cause existing users to allocate too little space when the shared
 * library is upgraded */
typedef struct MMDB_s {
  uint32_t flags;
  const char *filename;
  ssize_t file_size;
  const uint8_t *file_content;
  const uint8_t *data_section;
  uint32_t data_section_size;
  const uint8_t *metadata_section;
  uint32_t metadata_section_size;
  uint16_t full_record_byte_size;
  uint16_t depth;
  MMDB_ipv4_start_node_s ipv4_start_node;
  MMDB_metadata_s metadata;
  /* See above warning before adding fields */
} MMDB_s;

typedef struct MMDB_search_node_s {
  uint64_t left_record;
  uint64_t right_record;
  uint8_t left_record_type;
  uint8_t right_record_type;
  MMDB_entry_s left_record_entry;
  MMDB_entry_s right_record_entry;
} MMDB_search_node_s;

void MMDB_close(MMDB_s *);
int MMDB_open(const char *, uint32_t, MMDB_s *);
MMDB_lookup_result_s MMDB_lookup(const MMDB_s *, uint32_t, int *);
int MMDB_read_node(const MMDB_s *, uint32_t, MMDB_search_node_s *);
int MMDB_get_value(MMDB_entry_s *, MMDB_entry_data_s *, ...);
int MMDB_vget_value(MMDB_entry_s *, MMDB_entry_data_s *, va_list);
int MMDB_aget_value(MMDB_entry_s *, MMDB_entry_data_s *, const char *const *);
int MMDB_get_metadata_as_entry_data_list(const MMDB_s *,
                                         MMDB_entry_data_list_s **);
int MMDB_get_entry_data_list(MMDB_entry_s *, MMDB_entry_data_list_s **);
void MMDB_free_entry_data_list(MMDB_entry_data_list_s *);
int MMDB_dump_entry_data_list(FILE *, MMDB_entry_data_list_s *, int);
const char *MMDB_lib_version(void);
const char *MMDB_strerror(int);
const char *GetMetroName(int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MAXMIND_MAXMINDDB_H_ */
