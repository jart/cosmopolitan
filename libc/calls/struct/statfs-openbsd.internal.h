#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_OPENBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_OPENBSD_H_
#include "libc/calls/struct/fsid.h"
#include "libc/sock/struct/sockaddr.h"
COSMOPOLITAN_C_START_

struct xucred_openbsd {
  uint32_t cr_uid;        /* user id */
  uint32_t cr_gid;        /* group id */
  int16_t cr_ngroups;     /* number of groups */
  uint32_t cr_groups[16]; /* groups */
};

struct export_args_openbsd {
  int32_t ex_flags;              /* export related flags */
  uint32_t ex_root;              /* mapping for root uid */
  struct xucred_openbsd ex_anon; /* mapping for anonymous user */
  struct sockaddr *ex_addr;      /* net address to which exported */
  int32_t ex_addrlen;            /* and the net address length */
  struct sockaddr *ex_mask;      /* mask of valid bits in saddr */
  int32_t ex_masklen;            /* and the smask length */
};

struct ufs_args_openbsd {
  char *fspec;                            /* block special device to mount */
  struct export_args_openbsd export_info; /* network export information */
};

struct mfs_args_openbsd {
  char *fspec;                            /* name to export for statfs */
  struct export_args_openbsd export_info; /* if exported MFSes are supported */
  char *base;                             /* base of file system in memory */
  uint64_t size;                          /* size of file system */
};

struct iso_args_openbsd {
  char *fspec;                            /* block special device to mount */
  struct export_args_openbsd export_info; /* network export info */
  int32_t flags;                          /* mounting flags, see below */
  int32_t sess;                           /* start sector of session */
};

struct nfs_args_openbsd {
  int32_t version;       /* args structure version number */
  struct sockaddr *addr; /* file server address */
  int32_t addrlen;       /* length of address */
  int32_t sotype;        /* Socket type */
  int32_t proto;         /* and Protocol */
  unsigned char *fh;     /* File handle to be mounted */
  int32_t fhsize;        /* Size, in bytes, of fh */
  int32_t flags;         /* flags */
  int32_t wsize;         /* write size in bytes */
  int32_t rsize;         /* read size in bytes */
  int32_t readdirsize;   /* readdir size in bytes */
  int32_t timeo;         /* initial timeout in .1 secs */
  int32_t retrans;       /* times to retry send */
  int32_t maxgrouplist;  /* Max. size of group list */
  int32_t readahead;     /* # of blocks to readahead */
  int32_t leaseterm;     /* Term (sec) of lease */
  int32_t deadthresh;    /* Retrans threshold */
  char *hostname;        /* server's name */
  int32_t acregmin;      /* Attr cache file recently modified */
  int32_t acregmax;      /* ac file not recently modified */
  int32_t acdirmin;      /* ac for dir recently modified */
  int32_t acdirmax;      /* ac for dir not recently modified */
};

struct msdosfs_args_openbsd {
  char *fspec; /* blocks special holding the fs to mount */
  struct export_args_openbsd export_info; /* network export information */
  uint32_t uid;                           /* uid that owns msdosfs files */
  uint32_t gid;                           /* gid that owns msdosfs files */
  uint32_t mask; /* mask to be applied for msdosfs perms */
  int32_t flags; /* see below */
};

struct ntfs_args_openbsd {
  char *fspec;                            /* block special device to mount */
  struct export_args_openbsd export_info; /* network export information */
  uint32_t uid;                           /* uid that owns ntfs files */
  uint32_t gid;                           /* gid that owns ntfs files */
  uint32_t mode; /* mask to be applied for ntfs perms */
  uint64_t flag; /* additional flags */
};

struct tmpfs_args_openbsd {
  int32_t ta_version;
  /* Size counters. */
  uint64_t ta_nodes_max;
  int64_t ta_size_max;
  /* Root node attributes. */
  uint32_t ta_root_uid;
  uint32_t ta_root_gid;
  uint32_t ta_root_mode;
};

union mount_info_openbsd {
  struct ufs_args_openbsd ufs_args;
  struct mfs_args_openbsd mfs_args;
  struct nfs_args_openbsd nfs_args;
  struct iso_args_openbsd iso_args;
  struct msdosfs_args_openbsd msdosfs_args;
  struct ntfs_args_openbsd ntfs_args;
  struct tmpfs_args_openbsd tmpfs_args;
  char __align[160]; /* 64-bit alignment and room to grow */
};

struct statfs_openbsd {
  uint32_t f_flags;                    /* copy of mount flags */
  uint32_t f_bsize;                    /* file system block size */
  uint32_t f_iosize;                   /* optimal transfer block size */
  uint64_t f_blocks;                   /* total data blocks in file system */
  uint64_t f_bfree;                    /* free blocks in fs */
  int64_t f_bavail;                    /* free blocks avail to non-superuser */
  uint64_t f_files;                    /* total file nodes in file system */
  uint64_t f_ffree;                    /* free file nodes in fs */
  int64_t f_favail;                    /* free file nodes avail to non-root */
  uint64_t f_syncwrites;               /* count of sync writes since mount */
  uint64_t f_syncreads;                /* count of sync reads since mount */
  uint64_t f_asyncwrites;              /* count of async writes since mount */
  uint64_t f_asyncreads;               /* count of async reads since mount */
  fsid_t f_fsid;                       /* file system id */
  uint32_t f_namemax;                  /* maximum filename length */
  uint32_t f_owner;                    /* user that mounted the file system */
  uint64_t f_ctime;                    /* last mount [-u] time */
  char f_fstypename[16];               /* fs type name */
  char f_mntonname[90];                /* directory on which mounted */
  char f_mntfromname[90];              /* mounted file system */
  char f_mntfromspec[90];              /* special for mount request */
  union mount_info_openbsd mount_info; /* per-filesystem mount options */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_OPENBSD_H_ */
