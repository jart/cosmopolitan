/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  wepoll                                                                      │
│  https://github.com/piscisaureus/wepoll                                      │
│                                                                              │
│  Copyright 2012-2020, Bert Belder <bertbelder@gmail.com>                     │
│  All rights reserved.                                                        │
│                                                                              │
│  Redistribution and use in source and binary forms, with or without          │
│  modification, are permitted provided that the following conditions are      │
│  met:                                                                        │
│                                                                              │
│    * Redistributions of source code must retain the above copyright          │
│      notice, this list of conditions and the following disclaimer.           │
│                                                                              │
│    * Redistributions in binary form must reproduce the above copyright       │
│      notice, this list of conditions and the following disclaimer in the     │
│      documentation and/or other materials provided with the distribution.    │
│                                                                              │
│  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS         │
│  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT           │
│  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR       │
│  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT        │
│  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       │
│  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT            │
│  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,       │
│  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY       │
│  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         │
│  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE       │
│  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.        │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/sock/epoll.h"
#include "libc/assert.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/afd.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/ioctl.h"
#include "libc/nt/enum/keyedevent.h"
#include "libc/nt/enum/sio.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/iocp.h"
#include "libc/nt/nt/file.h"
#include "libc/nt/nt/key.h"
#include "libc/nt/ntdll.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/afd.h"
#include "libc/nt/struct/criticalsection.h"
#include "libc/nt/struct/objectattributes.h"
#include "libc/nt/struct/overlappedentry.h"
#include "libc/nt/struct/unicodestring.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/epoll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * @fileoverview epoll
 *
 * This is an alternative to poll() that's popular for event driven
 * network servers that want >10,000 sockets per machine and don't do
 * cpu bound computations that would otherwise block the event loop.
 *
 * This works on Linux and is polyfilled on Windows. It's worth noting
 * that these polyfills depend on Microsoft's internal APIs. However
 * these particular NTDLL APIs are also used by libuv, nodejs, etc. so
 * we're reasonably certain Microsoft has compatibility policies in
 * place where they've promised not to break them.
 *
 * TODO(jart): Polyfill kqueue for XNU/FreeBSD/OpenBSD.
 */

asm(".ident\t\"\\n\\n\
wepoll (BSD-2)\\n\
Copyright 2012-2020 Bert Belder\\n\
https://github.com/piscisaureus/wepoll\"");
asm(".include \"libc/disclaimer.inc\"");

#define MAX_GROUP_SIZE 32

#define REFLOCK__REF          0x00000001
#define REFLOCK__REF_MASK     0x0fffffff
#define REFLOCK__DESTROY      0x10000000
#define REFLOCK__DESTROY_MASK 0xf0000000
#define REFLOCK__POISON       0x300dead0

#define KNOWN_EVENTS                                                   \
  (EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM | \
   EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG | EPOLLRDHUP)

#define RTL_CONSTANT_STRING(s) \
  { sizeof(s) - sizeof((s)[0]), sizeof(s), s }

#define RTL_CONSTANT_OBJECT_ATTRIBUTES(ObjectName, Attributes) \
  { sizeof(struct NtObjectAttributes), 0, ObjectName, Attributes, NULL, NULL }

#define RETURN_MAP_ERROR(value) \
  do {                          \
    err_map_win_error();        \
    return value;               \
  } while (0)

#define RETURN_SET_ERROR(value, error) \
  do {                                 \
    err_set_win_error(error);          \
    return value;                      \
  } while (0)

#define CONTAINOF(ptr, type, member) \
  ((type *)((uintptr_t)(ptr)-offsetof(type, member)))

#define TREE__ROTATE(cis, trans)       \
  struct TreeNode *p = node;           \
  struct TreeNode *q = node->trans;    \
  struct TreeNode *parent = p->parent; \
  if (parent) {                        \
    if (parent->left == p)             \
      parent->left = q;                \
    else                               \
      parent->right = q;               \
  } else {                             \
    tree->root = q;                    \
  }                                    \
  q->parent = parent;                  \
  p->parent = q;                       \
  p->trans = q->cis;                   \
  if (p->trans) p->trans->parent = p;  \
  q->cis = p;

#define TREE__INSERT_OR_DESCEND(side) \
  if (parent->side) {                 \
    parent = parent->side;            \
  } else {                            \
    parent->side = node;              \
    break;                            \
  }

#define TREE__REBALANCE_AFTER_INSERT(cis, trans) \
  struct TreeNode *grandparent = parent->parent; \
  struct TreeNode *uncle = grandparent->trans;   \
  if (uncle && uncle->red) {                     \
    parent->red = uncle->red = false;            \
    grandparent->red = true;                     \
    node = grandparent;                          \
  } else {                                       \
    if (node == parent->trans) {                 \
      tree__rotate_##cis(tree, parent);          \
      node = parent;                             \
      parent = node->parent;                     \
    }                                            \
    parent->red = false;                         \
    grandparent->red = true;                     \
    tree__rotate_##trans(tree, grandparent);     \
  }

#define TREE__REBALANCE_AFTER_REMOVE(cis, trans)   \
  struct TreeNode *sibling = parent->trans;        \
  if (sibling->red) {                              \
    sibling->red = false;                          \
    parent->red = true;                            \
    tree__rotate_##cis(tree, parent);              \
    sibling = parent->trans;                       \
  }                                                \
  if ((sibling->left && sibling->left->red) ||     \
      (sibling->right && sibling->right->red)) {   \
    if (!sibling->trans || !sibling->trans->red) { \
      sibling->cis->red = false;                   \
      sibling->red = true;                         \
      tree__rotate_##trans(tree, sibling);         \
      sibling = parent->trans;                     \
    }                                              \
    sibling->red = parent->red;                    \
    parent->red = sibling->trans->red = false;     \
    tree__rotate_##cis(tree, parent);              \
    node = tree->root;                             \
    break;                                         \
  }                                                \
  sibling->red = true;

#define tree_root(t)                         (t)->root
#define port_state_to_handle_tree_node(p)    (&(p)->handle_tree_node)
#define sock_state_from_queue_node(q)        CONTAINOF(q, struct SockState, queue_node)
#define sock_state_to_queue_node(s)          (&(s)->queue_node)
#define sock_state_from_tree_node(t)         CONTAINOF(t, struct SockState, tree_node)
#define sock_state_to_tree_node(s)           (&(s)->tree_node)
#define poll_group_from_queue_node(q)        CONTAINOF(q, struct PollGroup, queue_node)
#define poll_group_get_afd_device_handle(pg) (pg)->afd_device_handle

enum PollStatus {
  kPollIdle,
  kPollPending,
  kPollCancelled,
};

struct RefLock {
  int state;
};

struct TreeNode {
  struct TreeNode *left;
  struct TreeNode *right;
  struct TreeNode *parent;
  uintptr_t key;
  bool red;
};

struct Tree {
  struct TreeNode *root;
};

struct TsTree {
  struct Tree tree;
  intptr_t lock;
};

struct TsTreeNode {
  struct TreeNode tree_node;
  struct RefLock reflock;
};

struct QueueNode {
  struct QueueNode *prev;
  struct QueueNode *next;
};

struct Queue {
  struct QueueNode head;
};

struct PortState {
  int64_t iocp_handle;
  struct Tree sock_tree;
  struct Queue sock_update_queue;
  struct Queue sock_deleted_queue;
  struct Queue poll_group_queue;
  struct TsTreeNode handle_tree_node;
  struct NtCriticalSection lock;
  size_t active_poll_count;
};

struct PollGroup {
  struct PortState *port_state;
  struct QueueNode queue_node;
  int64_t afd_device_handle;
  size_t group_size;
};

struct SockState {
  struct NtIoStatusBlock io_status_block;
  struct NtAfdPollInfo poll_info;
  struct QueueNode queue_node;
  struct TreeNode tree_node;
  struct PollGroup *poll_group;
  int64_t base_socket;
  epoll_data_t user_data;
  uint32_t user_events;
  uint32_t pending_events;
  enum PollStatus poll_status;
  bool delete_pending;
};

static const struct NtUnicodeString afd__device_name =
    RTL_CONSTANT_STRING(u"\\Device\\Afd\\Wepoll");

static const struct NtObjectAttributes afd__device_attributes =
    RTL_CONSTANT_OBJECT_ATTRIBUTES(&afd__device_name, 0);

static int64_t reflock__keyed_event;
static struct TsTree epoll__handle_tree;

static textwindows void err_map_win_error(void) {
  errno = __dos2errno(GetLastError());
}

static textwindows void err_set_win_error(uint32_t error) {
  SetLastError(error);
  errno = __dos2errno(error);
}

static textwindows int err_check_handle(int64_t handle) {
  uint32_t flags;
  /* GetHandleInformation() succeeds when passed INVALID_HANDLE_VALUE,
     so check for this condition explicitly. */
  if (handle == kNtInvalidHandleValue) {
    RETURN_SET_ERROR(-1, kNtErrorInvalidHandle);
  }
  if (!GetHandleInformation(handle, &flags)) {
    RETURN_MAP_ERROR(-1);
  }
  return 0;
}

static textwindows void tree_init(struct Tree *tree) {
  bzero(tree, sizeof *tree);
}

static textwindows void ts_tree_init(struct TsTree *ts_tree) {
  tree_init(&ts_tree->tree);
  InitializeSRWLock(&ts_tree->lock);
}

static textwindows int reflock_global_init(void) {
  NtStatus status;
  if ((status = NtCreateKeyedEvent(&reflock__keyed_event,
                                   kNtKeyedeventAllAccess, NULL, 0)) !=
      kNtStatusSuccess) {
    RETURN_SET_ERROR(-1, RtlNtStatusToDosError(status));
  }
  return 0;
}

static textwindows int epoll_global_init(void) {
  ts_tree_init(&epoll__handle_tree);
  return 0;
}

static textwindows int wepoll_init(void) {
  static bool once;
  static bool result;
  if (!once) {
    if (reflock_global_init() < 0 || epoll_global_init() < 0) {
      result = false;
    } else {
      result = true;
    }
    once = true;
  }
  return result;
}

static textwindows int afd_create_device_handle(
    int64_t iocp_handle, int64_t *afd_device_handle_out) {
  NtStatus status;
  int64_t afd_device_handle;
  struct NtIoStatusBlock iosb;
  /* By opening \Device\Afd without specifying any extended attributes,
     we'll get a handle that lets us talk to the AFD driver, but that
     doesn't have an *associated endpoint (so it's not a socket). */
  status = NtCreateFile(&afd_device_handle, kNtSynchronize,
                        &afd__device_attributes, &iosb, NULL, 0,
                        kNtFileShareRead | kNtFileShareWrite, 1, 0, NULL, 0);
  if (status != kNtStatusSuccess) {
    RETURN_SET_ERROR(-1, RtlNtStatusToDosError(status));
  }
  if (!CreateIoCompletionPort(afd_device_handle, iocp_handle, 0, 0)) {
    goto error;
  }
  if (!SetFileCompletionNotificationModes(afd_device_handle,
                                          kNtFileSkipSetEventOnHandle)) {
    goto error;
  }
  *afd_device_handle_out = afd_device_handle;
  return 0;
error:
  CloseHandle(afd_device_handle);
  RETURN_MAP_ERROR(-1);
}

static textwindows int afd_poll(int64_t afd_device_handle,
                                struct NtAfdPollInfo *poll_info,
                                struct NtIoStatusBlock *io_status_block) {
  NtStatus status;
  /* Blocking operation is not supported.*/
  npassert(io_status_block);
  io_status_block->Status = kNtStatusPending;
  status =
      NtDeviceIoControlFile(afd_device_handle, 0, NULL, io_status_block,
                            io_status_block, kNtIoctlAfdPoll, poll_info,
                            sizeof(*poll_info), poll_info, sizeof(*poll_info));
  if (status == kNtStatusSuccess) {
    return 0;
  } else if (status == kNtStatusPending) {
    RETURN_SET_ERROR(-1, kNtErrorIoPending);
  } else {
    RETURN_SET_ERROR(-1, RtlNtStatusToDosError(status));
  }
}

static textwindows int afd_cancel_poll(
    int64_t afd_device_handle, struct NtIoStatusBlock *io_status_block) {
  NtStatus cancel_status;
  struct NtIoStatusBlock cancel_iosb;
  /* If the poll operation has already completed or has been cancelled
     earlier, there's nothing left for us to do. */
  if (io_status_block->Status != kNtStatusPending) return 0;
  cancel_status =
      NtCancelIoFileEx(afd_device_handle, io_status_block, &cancel_iosb);
  /* NtCancelIoFileEx() may return STATUS_NOT_FOUND if the operation completed
     just before calling NtCancelIoFileEx(). This is not an error. */
  if (cancel_status == kNtStatusSuccess || cancel_status == kNtStatusNotFound) {
    return 0;
  } else {
    RETURN_SET_ERROR(-1, RtlNtStatusToDosError(cancel_status));
  }
}

static textwindows void queue_node_init(struct QueueNode *node) {
  node->prev = node;
  node->next = node;
}

static textwindows void queue_init(struct Queue *queue) {
  queue_node_init(&queue->head);
}

static textwindows void queue__detach_node(struct QueueNode *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
}

forceinline bool queue_is_enqueued(const struct QueueNode *node) {
  return node->prev != node;
}

forceinline bool queue_is_empty(const struct Queue *queue) {
  return !queue_is_enqueued(&queue->head);
}

static textwindows struct QueueNode *queue_first(const struct Queue *queue) {
  return !queue_is_empty(queue) ? queue->head.next : NULL;
}

static textwindows struct QueueNode *queue_last(const struct Queue *queue) {
  return !queue_is_empty(queue) ? queue->head.prev : NULL;
}

static textwindows void queue_prepend(struct Queue *queue,
                                      struct QueueNode *node) {
  node->next = queue->head.next;
  node->prev = &queue->head;
  node->next->prev = node;
  queue->head.next = node;
}

static textwindows void queue_append(struct Queue *queue,
                                     struct QueueNode *node) {
  node->next = &queue->head;
  node->prev = queue->head.prev;
  node->prev->next = node;
  queue->head.prev = node;
}

static textwindows void queue_move_to_start(struct Queue *queue,
                                            struct QueueNode *node) {
  queue__detach_node(node);
  queue_prepend(queue, node);
}

static textwindows void queue_move_to_end(struct Queue *queue,
                                          struct QueueNode *node) {
  queue__detach_node(node);
  queue_append(queue, node);
}

static textwindows void queue_remove(struct QueueNode *node) {
  queue__detach_node(node);
  queue_node_init(node);
}

static textwindows struct PortState *port__alloc(void) {
  struct PortState *port_state = malloc(sizeof *port_state);
  if (!port_state) RETURN_SET_ERROR(NULL, kNtErrorNotEnoughMemory);
  return port_state;
}

static textwindows int64_t port__create_iocp(void) {
  int64_t iocp_handle = CreateIoCompletionPort(kNtInvalidHandleValue, 0, 0, 0);
  if (!iocp_handle) RETURN_MAP_ERROR(0);
  return iocp_handle;
}

static textwindows int port__close_iocp(struct PortState *port_state) {
  int64_t iocp_handle = port_state->iocp_handle;
  port_state->iocp_handle = 0;
  if (!CloseHandle(iocp_handle)) RETURN_MAP_ERROR(-1);
  return 0;
}

static textwindows void tree_node_init(struct TreeNode *node) {
  bzero(node, sizeof *node);
}

static textwindows void reflock_init(struct RefLock *reflock) {
  reflock->state = 0;
}

static textwindows void ts_tree_node_init(struct TsTreeNode *node) {
  tree_node_init(&node->tree_node);
  reflock_init(&node->reflock);
}

static textwindows void tree__rotate_left(struct Tree *tree,
                                          struct TreeNode *node) {
  TREE__ROTATE(left, right)
}

static textwindows void tree__rotate_right(struct Tree *tree,
                                           struct TreeNode *node) {
  TREE__ROTATE(right, left)
}

static textwindows int tree_add(struct Tree *tree, struct TreeNode *node,
                                uintptr_t key) {
  struct TreeNode *parent;
  parent = tree->root;
  if (parent) {
    for (;;) {
      if (key < parent->key) {
        TREE__INSERT_OR_DESCEND(left)
      } else if (key > parent->key) {
        TREE__INSERT_OR_DESCEND(right)
      } else {
        return -1;
      }
    }
  } else {
    tree->root = node;
  }
  node->key = key;
  node->left = node->right = NULL;
  node->parent = parent;
  node->red = true;
  for (; parent && parent->red; parent = node->parent) {
    if (parent == parent->parent->left) {
      TREE__REBALANCE_AFTER_INSERT(left, right)
    } else {
      TREE__REBALANCE_AFTER_INSERT(right, left)
    }
  }
  tree->root->red = false;
  return 0;
}

static textwindows int ts_tree_add(struct TsTree *ts_tree,
                                   struct TsTreeNode *node, uintptr_t key) {
  int r;
  AcquireSRWLockExclusive(&ts_tree->lock);
  r = tree_add(&ts_tree->tree, &node->tree_node, key);
  ReleaseSRWLockExclusive(&ts_tree->lock);
  return r;
}

static textwindows void port__free(struct PortState *port) {
  npassert(port);
  free(port);
}

static textwindows struct PortState *port_new(int64_t *iocp_handle_out) {
  struct PortState *port_state;
  int64_t iocp_handle;
  port_state = port__alloc();
  if (!port_state) goto err1;
  iocp_handle = port__create_iocp();
  if (!iocp_handle) goto err2;
  bzero(port_state, sizeof *port_state);
  port_state->iocp_handle = iocp_handle;
  tree_init(&port_state->sock_tree);
  queue_init(&port_state->sock_update_queue);
  queue_init(&port_state->sock_deleted_queue);
  queue_init(&port_state->poll_group_queue);
  ts_tree_node_init(&port_state->handle_tree_node);
  InitializeCriticalSection(&port_state->lock);
  *iocp_handle_out = iocp_handle;
  return port_state;
err2:
  port__free(port_state);
err1:
  return NULL;
}

static textwindows int sock__cancel_poll(struct SockState *sock_state) {
  npassert(sock_state->poll_status == kPollPending);
  if (afd_cancel_poll(poll_group_get_afd_device_handle(sock_state->poll_group),
                      &sock_state->io_status_block) < 0) {
    return -1;
  }
  sock_state->poll_status = kPollCancelled;
  sock_state->pending_events = 0;
  return 0;
}

static textwindows void port_cancel_socket_update(
    struct PortState *port_state, struct SockState *sock_state) {
  if (!queue_is_enqueued(sock_state_to_queue_node(sock_state))) return;
  queue_remove(sock_state_to_queue_node(sock_state));
}

static textwindows struct TreeNode *tree_find(const struct Tree *tree,
                                              uintptr_t key) {
  struct TreeNode *node = tree->root;
  while (node) {
    if (key < node->key) {
      node = node->left;
    } else if (key > node->key) {
      node = node->right;
    } else {
      return node;
    }
  }
  return NULL;
}

static textwindows struct TsTreeNode *ts_tree__find_node(struct TsTree *ts_tree,
                                                         uintptr_t key) {
  struct TreeNode *tree_node = tree_find(&ts_tree->tree, key);
  if (!tree_node) return NULL;
  return CONTAINOF(tree_node, struct TsTreeNode, tree_node);
}

static textwindows void tree_del(struct Tree *tree, struct TreeNode *node) {
  bool red;
  struct TreeNode *parent, *left, *right, *next;
  parent = node->parent;
  left = node->left;
  right = node->right;
  if (!left) {
    next = right;
  } else if (!right) {
    next = left;
  } else {
    next = right;
    while (next->left) next = next->left;
  }
  if (parent) {
    if (parent->left == node) {
      parent->left = next;
    } else {
      parent->right = next;
    }
  } else {
    tree->root = next;
  }
  if (left && right) {
    red = next->red;
    next->red = node->red;
    next->left = left;
    left->parent = next;
    if (next != right) {
      parent = next->parent;
      next->parent = node->parent;
      node = next->right;
      parent->left = node;
      next->right = right;
      right->parent = next;
    } else {
      next->parent = parent;
      parent = next;
      node = next->right;
    }
  } else {
    red = node->red;
    node = next;
  }
  if (node) node->parent = parent;
  if (red) return;
  if (node && node->red) {
    node->red = false;
    return;
  }
  do {
    if (node == tree->root) break;
    if (node == parent->left) {
      TREE__REBALANCE_AFTER_REMOVE(left, right)
    } else {
      TREE__REBALANCE_AFTER_REMOVE(right, left)
    }
    node = parent;
    parent = parent->parent;
  } while (!node->red);
  if (node) node->red = false;
}

static textwindows void reflock__signal_event(void *address) {
  NtStatus status =
      NtReleaseKeyedEvent(reflock__keyed_event, address, false, NULL);
  if (status != kNtStatusSuccess) abort();
}

static textwindows void reflock__await_event(void *address) {
  NtStatus status =
      NtWaitForKeyedEvent(reflock__keyed_event, address, false, NULL);
  if (status != kNtStatusSuccess) abort();
}

static textwindows void reflock_ref(struct RefLock *reflock) {
  long state = InterlockedAdd(&reflock->state, REFLOCK__REF);
  /* Verify that the counter didn 't overflow and the lock isn' t destroyed.*/
  npassert((state & REFLOCK__DESTROY_MASK) == 0);
}

static textwindows void reflock_unref(struct RefLock *reflock) {
  long state = InterlockedAdd(&reflock->state, -REFLOCK__REF);
  /* Verify that the lock was referenced and not already destroyed.*/
  npassert((state & REFLOCK__DESTROY_MASK & ~REFLOCK__DESTROY) == 0);
  if (state == REFLOCK__DESTROY) reflock__signal_event(reflock);
}

static textwindows struct TsTreeNode *ts_tree_del_and_ref(
    struct TsTree *ts_tree, uintptr_t key) {
  struct TsTreeNode *ts_tree_node;
  AcquireSRWLockExclusive(&ts_tree->lock);
  ts_tree_node = ts_tree__find_node(ts_tree, key);
  if (ts_tree_node != NULL) {
    tree_del(&ts_tree->tree, &ts_tree_node->tree_node);
    reflock_ref(&ts_tree_node->reflock);
  }
  ReleaseSRWLockExclusive(&ts_tree->lock);
  return ts_tree_node;
}

static textwindows struct TsTreeNode *ts_tree_find_and_ref(
    struct TsTree *ts_tree, uintptr_t key) {
  struct TsTreeNode *ts_tree_node;
  AcquireSRWLockShared(&ts_tree->lock);
  ts_tree_node = ts_tree__find_node(ts_tree, key);
  if (ts_tree_node != NULL) reflock_ref(&ts_tree_node->reflock);
  ReleaseSRWLockShared(&ts_tree->lock);
  return ts_tree_node;
}

static textwindows void ts_tree_node_unref(struct TsTreeNode *node) {
  reflock_unref(&node->reflock);
}

static textwindows void reflock_unref_and_destroy(struct RefLock *reflock) {
  long state, ref_count;
  state = InterlockedAdd(&reflock->state, REFLOCK__DESTROY - REFLOCK__REF);
  ref_count = state & REFLOCK__REF_MASK;
  /* Verify that the lock was referenced and not already destroyed. */
  npassert((state & REFLOCK__DESTROY_MASK) == REFLOCK__DESTROY);
  if (ref_count != 0) reflock__await_event(reflock);
  state = InterlockedExchange(&reflock->state, REFLOCK__POISON);
  npassert(state == REFLOCK__DESTROY);
}

static textwindows void ts_tree_node_unref_and_destroy(
    struct TsTreeNode *node) {
  reflock_unref_and_destroy(&node->reflock);
}

static textwindows void port_unregister_socket(struct PortState *port_state,
                                               struct SockState *sock_state) {
  tree_del(&port_state->sock_tree, sock_state_to_tree_node(sock_state));
}

static textwindows void port_remove_deleted_socket(
    struct PortState *port_state, struct SockState *sock_state) {
  if (!queue_is_enqueued(sock_state_to_queue_node(sock_state))) return;
  queue_remove(sock_state_to_queue_node(sock_state));
}

static textwindows struct Queue *port_get_poll_group_queue(
    struct PortState *port_state) {
  return &port_state->poll_group_queue;
}

static textwindows void poll_group_release(struct PollGroup *poll_group) {
  struct PortState *port_state = poll_group->port_state;
  struct Queue *poll_group_queue = port_get_poll_group_queue(port_state);
  poll_group->group_size--;
  npassert(poll_group->group_size < MAX_GROUP_SIZE);
  queue_move_to_end(poll_group_queue, &poll_group->queue_node);
  /* Poll groups are currently only freed when the epoll port is closed. */
}

static textwindows void sock__free(struct SockState *sock_state) {
  npassert(sock_state != NULL);
  free(sock_state);
}

static textwindows void port_add_deleted_socket(struct PortState *port_state,
                                                struct SockState *sock_state) {
  if (queue_is_enqueued(sock_state_to_queue_node(sock_state))) return;
  queue_append(&port_state->sock_deleted_queue,
               sock_state_to_queue_node(sock_state));
}

static textwindows int sock__delete(struct PortState *port_state,
                                    struct SockState *sock_state, bool force) {
  if (!sock_state->delete_pending) {
    if (sock_state->poll_status == kPollPending) {
      sock__cancel_poll(sock_state);
    }
    port_cancel_socket_update(port_state, sock_state);
    port_unregister_socket(port_state, sock_state);
    sock_state->delete_pending = true;
  }
  /* If the poll request still needs to complete, the sock_state object
     can't be free'd yet. `sock_feed_event()` or `port_close()` will
     take care of this later. */
  if (force || sock_state->poll_status == kPollIdle) {
    port_remove_deleted_socket(port_state, sock_state);
    poll_group_release(sock_state->poll_group);
    sock__free(sock_state);
  } else {
    /* Free the socket later.*/
    port_add_deleted_socket(port_state, sock_state);
  }
  return 0;
}

static textwindows void sock_delete(struct PortState *port_state,
                                    struct SockState *sock_state) {
  sock__delete(port_state, sock_state, false);
}

static textwindows void sock_force_delete(struct PortState *port_state,
                                          struct SockState *sock_state) {
  sock__delete(port_state, sock_state, true);
}

static textwindows void poll_group_delete(struct PollGroup *poll_group) {
  npassert(poll_group->group_size == 0);
  CloseHandle(poll_group->afd_device_handle);
  queue_remove(&poll_group->queue_node);
  free(poll_group);
}

static textwindows int port_delete(struct PortState *port_state) {
  struct TreeNode *tree_node;
  struct QueueNode *queue_node;
  struct SockState *sock_state;
  struct PollGroup *poll_group;
  /* At this point the IOCP port should have been closed.*/
  npassert(!port_state->iocp_handle);
  while ((tree_node = tree_root(&port_state->sock_tree)) != NULL) {
    sock_state = sock_state_from_tree_node(tree_node);
    sock_force_delete(port_state, sock_state);
  }
  while ((queue_node = queue_first(&port_state->sock_deleted_queue)) != NULL) {
    sock_state = sock_state_from_queue_node(queue_node);
    sock_force_delete(port_state, sock_state);
  }
  while ((queue_node = queue_first(&port_state->poll_group_queue)) != NULL) {
    poll_group = poll_group_from_queue_node(queue_node);
    poll_group_delete(poll_group);
  }
  npassert(queue_is_empty(&port_state->sock_update_queue));
  DeleteCriticalSection(&port_state->lock);
  port__free(port_state);
  return 0;
}

static textwindows int64_t port_get_iocp_handle(struct PortState *port_state) {
  npassert(port_state->iocp_handle);
  return port_state->iocp_handle;
}

static textwindows struct PollGroup *poll_group__new(
    struct PortState *port_state) {
  int64_t iocp_handle = port_get_iocp_handle(port_state);
  struct Queue *poll_group_queue = port_get_poll_group_queue(port_state);
  struct PollGroup *poll_group = malloc(sizeof *poll_group);
  if (!poll_group) RETURN_SET_ERROR(NULL, kNtErrorNotEnoughMemory);
  bzero(poll_group, sizeof *poll_group);
  queue_node_init(&poll_group->queue_node);
  poll_group->port_state = port_state;
  if (afd_create_device_handle(iocp_handle, &poll_group->afd_device_handle) <
      0) {
    free(poll_group);
    return NULL;
  }
  queue_append(poll_group_queue, &poll_group->queue_node);
  return poll_group;
}

static textwindows struct PollGroup *poll_group_acquire(
    struct PortState *port_state) {
  struct Queue *poll_group_queue = port_get_poll_group_queue(port_state);
  struct PollGroup *poll_group = !queue_is_empty(poll_group_queue)
                                     ? CONTAINOF(queue_last(poll_group_queue),
                                                 struct PollGroup, queue_node)
                                     : NULL;
  if (!poll_group || poll_group->group_size >= MAX_GROUP_SIZE)
    poll_group = poll_group__new(port_state);
  if (!poll_group) return NULL;
  if (++poll_group->group_size == MAX_GROUP_SIZE)
    queue_move_to_start(poll_group_queue, &poll_group->queue_node);
  return poll_group;
}

static textwindows int port_close(struct PortState *port_state) {
  int result;
  EnterCriticalSection(&port_state->lock);
  result = port__close_iocp(port_state);
  LeaveCriticalSection(&port_state->lock);
  return result;
}

static textwindows uint32_t sock__epoll_events_to_afd_events(uint32_t e) {
  /* Always monitor for kNtAfdPollLocalClose, which is triggered when
     the socket is closed with closesocket() or CloseHandle(). */
  uint32_t a = kNtAfdPollLocalClose;
  if (e & (EPOLLIN | EPOLLRDNORM)) a |= kNtAfdPollReceive | kNtAfdPollAccept;
  if (e & (EPOLLPRI | EPOLLRDBAND)) a |= kNtAfdPollReceiveExpedited;
  if (e & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND)) a |= kNtAfdPollSend;
  if (e & (EPOLLIN | EPOLLRDNORM | EPOLLRDHUP)) a |= kNtAfdPollDisconnect;
  if (e & EPOLLHUP) a |= kNtAfdPollAbort;
  if (e & EPOLLERR) a |= kNtAfdPollConnectFail;
  return a;
}

static textwindows uint32_t sock__afd_events_to_epoll_events(uint32_t a) {
  uint32_t e = 0;
  if (a & (kNtAfdPollReceive | kNtAfdPollAccept)) e |= EPOLLIN | EPOLLRDNORM;
  if (a & kNtAfdPollReceiveExpedited) e |= EPOLLPRI | EPOLLRDBAND;
  if (a & kNtAfdPollSend) e |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if (a & kNtAfdPollDisconnect) e |= EPOLLIN | EPOLLRDNORM | EPOLLRDHUP;
  if (a & kNtAfdPollAbort) e |= EPOLLHUP;
  if (a & kNtAfdPollConnectFail) {
    /* Linux reports all these events after connect() has failed. */
    e |= EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDNORM | EPOLLWRNORM | EPOLLRDHUP;
  }
  return e;
}

static textwindows int sock_update(struct PortState *port_state,
                                   struct SockState *sock_state) {
  npassert(!sock_state->delete_pending);
  if ((sock_state->poll_status == kPollPending) &&
      !(sock_state->user_events & KNOWN_EVENTS & ~sock_state->pending_events)) {
    /* All the events the user is interested in are already being
       monitored by the pending poll operation. It might spuriously
       complete because of an event that we're no longer interested in;
       when that happens we'll submit a new poll operation with the
       updated event mask. */
  } else if (sock_state->poll_status == kPollPending) {
    /* A poll operation is already pending, but it's not monitoring for
       all the *events that the user is interested in. Therefore, cancel
       the pending *poll operation; when we receive it's completion
       package, a new poll *operation will be submitted with the correct
       event mask. */
    if (sock__cancel_poll(sock_state) < 0) return -1;
  } else if (sock_state->poll_status == kPollCancelled) {
    /* The poll operation has already been cancelled, we're still waiting for
       it to return.For now, there' s nothing that needs to be done. */
  } else if (sock_state->poll_status == kPollIdle) {
    /* No poll operation is pending; start one. */
    sock_state->poll_info.Exclusive = false;
    sock_state->poll_info.NumberOfHandles = 1;
    sock_state->poll_info.Timeout = INT64_MAX;
    sock_state->poll_info.Handles[0].Handle = (int64_t)sock_state->base_socket;
    sock_state->poll_info.Handles[0].Status = 0;
    sock_state->poll_info.Handles[0].Events =
        sock__epoll_events_to_afd_events(sock_state->user_events);
    if (afd_poll(poll_group_get_afd_device_handle(sock_state->poll_group),
                 &sock_state->poll_info, &sock_state->io_status_block) < 0) {
      switch (GetLastError()) {
        case kNtErrorIoPending:
          /* Overlapped poll operation in progress; this is expected. */
          break;
        case kNtErrorInvalidHandle:
          /* Socket closed; it'll be dropped from the epoll set. */
          return sock__delete(port_state, sock_state, false);
        default:
          /* Other errors are propagated to the caller. */
          RETURN_MAP_ERROR(-1);
      }
    }
    /* The poll request was successfully submitted.*/
    sock_state->poll_status = kPollPending;
    sock_state->pending_events = sock_state->user_events;
  } else {
    __builtin_unreachable();
  }
  port_cancel_socket_update(port_state, sock_state);
  return 0;
}

static textwindows int port__update_events(struct PortState *port_state) {
  struct QueueNode *queue_node;
  struct SockState *sock_state;
  struct Queue *sock_update_queue = &port_state->sock_update_queue;
  /* Walk queue, submitting new poll requests for sockets needing it */
  while (!queue_is_empty(sock_update_queue)) {
    queue_node = queue_first(sock_update_queue);
    sock_state = sock_state_from_queue_node(queue_node);
    if (sock_update(port_state, sock_state) < 0) return -1;
    /* sock_update() removes the socket from the update queue.*/
  }
  return 0;
}

static textwindows void port__update_events_if_polling(
    struct PortState *port_state) {
  if (port_state->active_poll_count > 0) port__update_events(port_state);
}

static textwindows void port_request_socket_update(
    struct PortState *port_state, struct SockState *sock_state) {
  if (queue_is_enqueued(sock_state_to_queue_node(sock_state))) return;
  queue_append(&port_state->sock_update_queue,
               sock_state_to_queue_node(sock_state));
}

static textwindows int sock_feed_event(struct PortState *port_state,
                                       struct NtIoStatusBlock *io_status_block,
                                       struct epoll_event *ev) {
  uint32_t epoll_events;
  struct SockState *sock_state;
  struct NtAfdPollInfo *poll_info;
  epoll_events = 0;
  sock_state = CONTAINOF(io_status_block, struct SockState, io_status_block);
  poll_info = &sock_state->poll_info;
  sock_state->poll_status = kPollIdle;
  sock_state->pending_events = 0;
  if (sock_state->delete_pending) {
    /* Socket has been deleted earlier and can now be freed.*/
    return sock__delete(port_state, sock_state, false);
  } else if (io_status_block->Status == kNtStatusCancelled) {
    /* The poll request was cancelled by CancelIoEx.*/
  } else if (!NtSuccess(io_status_block->Status)) {
    /* The overlapped request itself failed in an unexpected way.*/
    epoll_events = EPOLLERR;
  } else if (poll_info->NumberOfHandles < 1) {
    /* This poll operation succeeded but didn't report any socket events. */
  } else if (poll_info->Handles[0].Events & kNtAfdPollLocalClose) {
    /* The poll operation reported that the socket was closed.*/
    return sock__delete(port_state, sock_state, false);
  } else {
    /* Events related to our socket were reported.*/
    epoll_events =
        sock__afd_events_to_epoll_events(poll_info->Handles[0].Events);
  }
  /* Requeue the socket so a new poll request will be submitted.*/
  port_request_socket_update(port_state, sock_state);
  /* Filter out events that the user didn't ask for. */
  epoll_events &= sock_state->user_events;
  /* Return if there are no epoll events to report.*/
  if (epoll_events == 0) return 0;
  /* If the the socket has the EPOLLONESHOT flag set, unmonitor all
     events, even EPOLLERR and EPOLLHUP. But always keep looking for
     closed sockets. */
  if (sock_state->user_events & EPOLLONESHOT) {
    sock_state->user_events = 0;
  }
  ev->data = sock_state->user_data;
  ev->events = epoll_events;
  return 1;
}

static textwindows int port__feed_events(struct PortState *port_state,
                                         struct epoll_event *epoll_events,
                                         struct NtOverlappedEntry *iocp_events,
                                         uint32_t iocp_event_count) {
  uint32_t i;
  int epoll_event_count;
  struct epoll_event *ev;
  struct NtIoStatusBlock *io_status_block;
  epoll_event_count = 0;
  for (i = 0; i < iocp_event_count; i++) {
    io_status_block = (struct NtIoStatusBlock *)iocp_events[i].lpOverlapped;
    ev = &epoll_events[epoll_event_count];
    epoll_event_count += sock_feed_event(port_state, io_status_block, ev);
  }
  return epoll_event_count;
}

static textwindows int port__poll(struct PortState *port_state,
                                  struct epoll_event *epoll_events,
                                  struct NtOverlappedEntry *iocp_events,
                                  uint32_t maxevents, uint32_t timeout) {
  bool32 r;
  uint32_t completion_count;
  if (port__update_events(port_state) < 0) return -1;
  port_state->active_poll_count++;
  LeaveCriticalSection(&port_state->lock);
  r = GetQueuedCompletionStatusEx(port_state->iocp_handle, iocp_events,
                                  maxevents, &completion_count, timeout, false);
  EnterCriticalSection(&port_state->lock);
  port_state->active_poll_count--;
  if (!r) RETURN_MAP_ERROR(-1);
  return port__feed_events(port_state, epoll_events, iocp_events,
                           completion_count);
}

static textwindows int port_wait(struct PortState *port_state,
                                 struct epoll_event *events, int maxevents,
                                 int timeout) {
  int result;
  uint64_t now, due = 0;
  uint32_t gqcs_timeout;
  struct NtOverlappedEntry *iocp_events;
  struct NtOverlappedEntry stack_iocp_events[64];
  /* Check whether `maxevents` is in range.*/
  if (maxevents <= 0) RETURN_SET_ERROR(-1, kNtErrorInvalidParameter);
  /* Decide whether the IOCP completion list can live on the stack, or
     allocate memory for it on the heap. */
  if ((size_t)maxevents <= ARRAYLEN(stack_iocp_events)) {
    iocp_events = stack_iocp_events;
  } else if ((iocp_events = malloc((size_t)maxevents * sizeof(*iocp_events))) ==
             NULL) {
    iocp_events = stack_iocp_events;
    maxevents = ARRAYLEN(stack_iocp_events);
  }
  /* Compute the timeout for GetQueuedCompletionStatus, and the wait end
     time, if the user specified a timeout other than zero or infinite. */
  if (timeout > 0) {
    due = GetTickCount64() + (uint64_t)timeout;
    gqcs_timeout = (uint32_t)timeout;
  } else if (timeout == 0) {
    gqcs_timeout = 0;
  } else {
    gqcs_timeout = -1;
  }
  EnterCriticalSection(&port_state->lock);
  /* Dequeue completion packets until either at least one interesting
     event has been discovered, or the timeout is reached. */
  for (;;) {
    result = port__poll(port_state, events, iocp_events, (uint32_t)maxevents,
                        gqcs_timeout);
    if (result < 0 || result > 0) break;
    /* Result, error, or time - out. */
    if (timeout < 0) continue;
    /* When timeout is negative, never time out. */
    /* Update time. */
    now = GetTickCount64();
    /* Do not allow the due time to be in the past. */
    if (now >= due) {
      SetLastError(kNtWaitTimeout);
      break;
    }
    /* Recompute time-out argument for GetQueuedCompletionStatus. */
    gqcs_timeout = (uint32_t)(due - now);
  }
  port__update_events_if_polling(port_state);
  LeaveCriticalSection(&port_state->lock);
  if (iocp_events != stack_iocp_events) {
    free(iocp_events);
  }
  if (result >= 0) {
    return result;
  } else if (GetLastError() == kNtWaitTimeout) {
    return 0;
  } else {
    return -1;
  }
}

static textwindows int64_t ws__ioctl_get_bsp_socket(int64_t socket,
                                                    uint32_t ioctl) {
  uint32_t bytes;
  int64_t bsp_socket;
  if (WSAIoctl(socket, ioctl, NULL, 0, &bsp_socket, sizeof(bsp_socket), &bytes,
               NULL, NULL) != -1) {
    return bsp_socket;
  } else {
    return -1;
  }
}

static textwindows int64_t ws_get_base_socket(int64_t socket) {
  uint32_t error;
  int64_t base_socket;
  for (;;) {
    base_socket = ws__ioctl_get_bsp_socket(socket, kNtSioBaseHandle);
    if (base_socket != -1) {
      return base_socket;
    }
    error = GetLastError();
    if (error == WSAENOTSOCK) {
      RETURN_SET_ERROR(-1, error);
    }
    /*
     * Even though Microsoft documentation clearly states that Layered
     * Spyware Providers must never ever intercept the SIO_BASE_HANDLE
     * ioctl, Komodia LSPs (that Lenovo got sued for preinstalling) do
     * so anyway in order to redirect decrypted https requests through
     * some foreign proxy and inject ads which breaks high-performance
     * network event io. However it doesn't handle SIO_BSP_HANDLE_POLL
     * which will at least let us obtain the socket associated with the
     * next winsock protocol chain entry. If this succeeds, loop around
     * and call SIO_BASE_HANDLE again with the returned BSP socket, to
     * make sure we unwrap all layers and retrieve the real base socket.
     */
    base_socket = ws__ioctl_get_bsp_socket(socket, kNtSioBspHandlePoll);
    if (base_socket != -1 && base_socket != socket) {
      socket = base_socket;
    } else {
      RETURN_SET_ERROR(-1, error);
    }
  }
}

static textwindows struct SockState *sock__alloc(void) {
  struct SockState *sock_state = malloc(sizeof *sock_state);
  if (!sock_state) RETURN_SET_ERROR(NULL, kNtErrorNotEnoughMemory);
  return sock_state;
}

static textwindows int port_register_socket(struct PortState *port_state,
                                            struct SockState *sock_state,
                                            int64_t socket) {
  if (tree_add(&port_state->sock_tree, sock_state_to_tree_node(sock_state),
               socket) < 0) {
    RETURN_SET_ERROR(-1, kNtErrorAlreadyExists);
  }
  return 0;
}

static textwindows struct SockState *sock_new(struct PortState *port_state,
                                              int64_t socket) {
  int64_t base_socket;
  struct PollGroup *poll_group;
  struct SockState *sock_state;
  if (socket == 0 || socket == -1) RETURN_SET_ERROR(0, kNtErrorInvalidHandle);
  base_socket = ws_get_base_socket(socket);
  if (base_socket == -1) return NULL;
  poll_group = poll_group_acquire(port_state);
  if (!poll_group) return NULL;
  sock_state = sock__alloc();
  if (!sock_state) goto err1;
  bzero(sock_state, sizeof *sock_state);
  sock_state->base_socket = base_socket;
  sock_state->poll_group = poll_group;
  tree_node_init(&sock_state->tree_node);
  queue_node_init(&sock_state->queue_node);
  if (port_register_socket(port_state, sock_state, socket) < 0) goto err2;
  return sock_state;
err2:
  sock__free(sock_state);
err1:
  poll_group_release(poll_group);
  return NULL;
}

static textwindows int sock_set_event(struct PortState *port_state,
                                      struct SockState *sock_state,
                                      const struct epoll_event *ev) {
  /* EPOLLERR and EPOLLHUP are always reported, even when not requested
     by the caller. However they are disabled after a event has been
     reported for a socket for which the EPOLLONESHOT flag was set. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;
  sock_state->user_events = events;
  sock_state->user_data = ev->data;
  if ((events & KNOWN_EVENTS & ~sock_state->pending_events) != 0) {
    port_request_socket_update(port_state, sock_state);
  }
  return 0;
}

static textwindows int port__ctl_add(struct PortState *port_state, int64_t sock,
                                     struct epoll_event *ev) {
  struct SockState *sock_state = sock_new(port_state, sock);
  if (!sock_state) return -1;
  if (sock_set_event(port_state, sock_state, ev) < 0) {
    sock_delete(port_state, sock_state);
    return -1;
  }
  port__update_events_if_polling(port_state);
  return 0;
}

static textwindows struct SockState *port_find_socket(
    struct PortState *port_state, int64_t socket) {
  struct TreeNode *tree_node = tree_find(&port_state->sock_tree, socket);
  if (!tree_node) RETURN_SET_ERROR(NULL, kNtErrorNotFound);
  return sock_state_from_tree_node(tree_node);
}

static textwindows int port__ctl_mod(struct PortState *port_state, int64_t sock,
                                     struct epoll_event *ev) {
  struct SockState *sock_state = port_find_socket(port_state, sock);
  if (!sock_state) return -1;
  if (sock_set_event(port_state, sock_state, ev) < 0) return -1;
  port__update_events_if_polling(port_state);
  return 0;
}

static textwindows int port__ctl_del(struct PortState *port_state,
                                     int64_t sock) {
  struct SockState *sock_state = port_find_socket(port_state, sock);
  if (!sock_state) return -1;
  sock_delete(port_state, sock_state);
  return 0;
}

static textwindows int port__ctl_op(struct PortState *port_state, int op,
                                    int64_t sock, struct epoll_event *ev) {
  switch (op) {
    case EPOLL_CTL_ADD:
      return port__ctl_add(port_state, sock, ev);
    case EPOLL_CTL_MOD:
      return port__ctl_mod(port_state, sock, ev);
    case EPOLL_CTL_DEL:
      return port__ctl_del(port_state, sock);
    default:
      RETURN_SET_ERROR(-1, kNtErrorInvalidParameter);
  }
}

static textwindows int port_ctl(struct PortState *port_state, int op,
                                int64_t sock, struct epoll_event *ev) {
  int result;
  EnterCriticalSection(&port_state->lock);
  result = port__ctl_op(port_state, op, sock, ev);
  LeaveCriticalSection(&port_state->lock);
  return result;
}

static textwindows struct PortState *port_state_from_handle_tree_node(
    struct TsTreeNode *tree_node) {
  return CONTAINOF(tree_node, struct PortState, handle_tree_node);
}

static textwindows dontinline int sys_epoll_create1_nt(uint32_t flags) {
  int fd;
  int64_t ephnd;
  struct PortState *port_state;
  struct TsTreeNode *tree_node;
  if (wepoll_init() < 0) return -1;
  fd = __reservefd(-1);
  if (fd == -1) return -1;
  port_state = port_new(&ephnd);
  if (!port_state) {
    __releasefd(fd);
    return -1;
  }
  tree_node = port_state_to_handle_tree_node(port_state);
  if (ts_tree_add(&epoll__handle_tree, tree_node, (uintptr_t)ephnd) < 0) {
    /* This should never happen. */
    port_delete(port_state);
    err_set_win_error(kNtErrorAlreadyExists);
    __releasefd(fd);
    return -1;
  }
  __fds_lock();
  g_fds.p[fd].kind = kFdEpoll;
  g_fds.p[fd].handle = ephnd;
  g_fds.p[fd].flags = flags;
  g_fds.p[fd].mode = 0140666;
  __fds_unlock();
  return fd;
}

static textwindows dontinline int sys_epoll_ctl_nt(int epfd, int op, int fd,
                                                   struct epoll_event *ev) {
  int r;
  struct PortState *port_state;
  struct TsTreeNode *tree_node;
  if (!IsWindows()) {
    return sys_epoll_ctl(epfd, op, fd, ev);
  } else {
    if (wepoll_init() < 0) return -1;
    if (!__isfdopen(fd)) return ebadf();
    if (!__isfdkind(epfd, kFdEpoll)) return ebadf();
    tree_node = ts_tree_find_and_ref(&epoll__handle_tree, g_fds.p[epfd].handle);
    if (!tree_node) {
      err_set_win_error(kNtErrorInvalidParameter);
      goto err;
    }
    port_state = port_state_from_handle_tree_node(tree_node);
    r = port_ctl(port_state, op, g_fds.p[fd].handle, ev);
    ts_tree_node_unref(tree_node);
    if (r < 0) goto err;
    return 0;
  err:
    /* On Linux, in the case of epoll_ctl(), EBADF takes priority over
       other *errors. Wepoll mimics this behavior. */
    err_check_handle(g_fds.p[epfd].handle);
    err_check_handle(g_fds.p[fd].handle);
    return -1;
  }
}

static textwindows dontinline int sys_epoll_wait_nt(int epfd,
                                                    struct epoll_event *events,
                                                    int maxevents,
                                                    int timeoutms) {
  int num_events;
  struct PortState *port_state;
  struct TsTreeNode *tree_node;
  if (!__isfdkind(epfd, kFdEpoll)) return ebadf();
  if (maxevents <= 0) return einval();
  if (wepoll_init() < 0) return -1;
  tree_node = ts_tree_find_and_ref(&epoll__handle_tree, g_fds.p[epfd].handle);
  if (!tree_node) {
    err_set_win_error(kNtErrorInvalidParameter);
    goto err;
  }
  port_state = port_state_from_handle_tree_node(tree_node);
  num_events = port_wait(port_state, events, maxevents, timeoutms);
  ts_tree_node_unref(tree_node);
  if (num_events < 0) goto err;
  return num_events;
err:
  err_check_handle(g_fds.p[epfd].handle);
  return -1;
}

#if SupportsWindows()
textwindows int sys_close_epoll_nt(int fd) {
  struct PortState *port_state;
  struct TsTreeNode *tree_node;
  if (wepoll_init() < 0) return -1;
  tree_node = ts_tree_del_and_ref(&epoll__handle_tree, g_fds.p[fd].handle);
  if (!tree_node) {
    err_set_win_error(kNtErrorInvalidParameter);
    goto err;
  }
  port_state = port_state_from_handle_tree_node(tree_node);
  port_close(port_state);
  ts_tree_node_unref_and_destroy(tree_node);
  return port_delete(port_state);
err:
  err_check_handle(g_fds.p[fd].handle);
  return -1;
}
#endif

/**
 * Creates new epoll instance.
 *
 * @param size is ignored but must be greater than zero
 * @param flags must be zero as there are no supported flags
 * @return epoll file descriptor, or -1 on failure
 */
int epoll_create(int size) {
  int rc;
  if (size <= 0) {
    rc = einval();
  } else {
    BLOCK_SIGNALS;
    rc = epoll_create1(0);
    ALLOW_SIGNALS;
  }
  STRACE("epoll_create(%d) → %d% m", size, rc);
  return rc;
}

/**
 * Creates new epoll instance.
 *
 * @param size is ignored but must be greater than zero
 * @param flags must be zero or can have O_CLOEXEC
 * @return epoll file descriptor, or -1 on failure
 */
int epoll_create1(int flags) {
  int rc;
  if (flags & ~O_CLOEXEC) {
    rc = einval();
  } else if (!IsWindows()) {
    rc = __fixupnewfd(sys_epoll_create(1337), flags);
  } else {
    BLOCK_SIGNALS;
    rc = sys_epoll_create1_nt(flags);
    ALLOW_SIGNALS;
  }
  STRACE("epoll_create1(%#x) → %d% m", flags, rc);
  return rc;
}

/**
 * Controls which socket events are monitored.
 *
 * It is recommended to always explicitly remove a socket from its epoll
 * set using EPOLL_CTL_DEL before closing it. As on Linux, your closed
 * sockets are automatically removed from the epoll set, but wepoll may
 * not be able to detect that a socket was closed until the next call to
 * epoll_wait().
 *
 * @param epfd is file descriptor created by epoll_create()
 * @param op can be EPOLL_CTL_{ADD,MOD,DEL}
 * @param fd is file descriptor to monitor
 * @param ev is ignored if op is EPOLL_CTL_DEL
 * @param ev->events can have these flags:
 *     - `EPOLLIN`: trigger on fd readable
 *     - `EPOLLOUT`: trigger on fd writeable
 *     - `EPOLLERR`: trigger on fd error (superfluous: always reported)
 *     - `EPOLLHUP`: trigger on fd remote hangup (superfluous: always reported)
 *     - `EPOLLPRI`: trigger on fd exceptional conditions, e.g. oob
 *     - `EPOLLONESHOT`: report event(s) only once
 *     - `EPOLLEXCLUSIVE`: not supported on windows
 *     - `EPOLLWAKEUP`: not supported on windows
 *     - `EPOLLET`: edge triggered mode (not supported on windows)
 *     - `EPOLLRDNORM`
 *     - `EPOLLRDBAND`
 *     - `EPOLLWRNORM`
 *     - `EPOLLWRBAND`
 *     - `EPOLLRDHUP`
 *     - `EPOLLMSG`
 * @error ENOTSOCK on Windows if fd isn't a socket :(
 * @return 0 on success, or -1 w/ errno
 */
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *ev) {
  int rc;
  if (!IsWindows()) {
    rc = sys_epoll_ctl(epfd, op, fd, ev);
  } else {
    BLOCK_SIGNALS;
    rc = sys_epoll_ctl_nt(epfd, op, fd, ev);
    ALLOW_SIGNALS;
  }
  STRACE("epoll_ctl(%d, %d, %d, %p) → %d% m", epfd, op, fd, ev, rc);
  return rc;
}

/**
 * Receives socket events.
 *
 * @param events will receive information about what happened
 * @param maxevents is array length of events
 * @param timeoutms is milliseconds, 0 to not block, or -1 for forever
 * @return number of events stored, 0 on timeout, or -1 w/ errno
 * @cancelationpoint
 * @norestart
 */
int epoll_wait(int epfd, struct epoll_event *events, int maxevents,
               int timeoutms) {
  int e, rc;
  BEGIN_CANCELATION_POINT;
  if (!IsWindows()) {
    e = errno;
    rc = sys_epoll_wait(epfd, events, maxevents, timeoutms);
    if (rc == -1 && errno == ENOSYS) {
      errno = e;
      rc = sys_epoll_pwait(epfd, events, maxevents, timeoutms, 0, 0);
    }
  } else {
    BLOCK_SIGNALS;
    // eintr/ecanceled not implemented for epoll() on win32 yet
    rc = sys_epoll_wait_nt(epfd, events, maxevents, timeoutms);
    ALLOW_SIGNALS;
  }
  END_CANCELATION_POINT;
  STRACE("epoll_wait(%d, %p, %d, %d) → %d% m", epfd, events, maxevents,
         timeoutms, rc);
  return rc;
}

/**
 * Receives socket events.
 *
 * @param events will receive information about what happened
 * @param maxevents is array length of events
 * @param timeoutms is milliseconds, 0 to not block, or -1 for forever
 * @param sigmask is an optional sigprocmask() to use during call
 * @return number of events stored, 0 on timeout, or -1 w/ errno
 * @cancelationpoint
 * @norestart
 */
int epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
                int timeoutms, const sigset_t *sigmask) {
  int e, rc;
  sigset_t oldmask;
  BEGIN_CANCELATION_POINT;
  if (!IsWindows()) {
    e = errno;
    rc = sys_epoll_pwait(epfd, events, maxevents, timeoutms, sigmask,
                         sizeof(*sigmask));
    if (rc == -1 && errno == ENOSYS) {
      errno = e;
      if (sigmask) sys_sigprocmask(SIG_SETMASK, sigmask, &oldmask);
      rc = sys_epoll_wait(epfd, events, maxevents, timeoutms);
      if (sigmask) sys_sigprocmask(SIG_SETMASK, &oldmask, 0);
    }
  } else {
    BLOCK_SIGNALS;
    // eintr/ecanceled not implemented for epoll() on win32 yet
    rc = sys_epoll_wait_nt(epfd, events, maxevents, timeoutms);
    ALLOW_SIGNALS;
  }
  END_CANCELATION_POINT;
  STRACE("epoll_pwait(%d, %p, %d, %d) → %d% m", epfd, events, maxevents,
         timeoutms, DescribeSigset(0, sigmask), rc);
  return rc;
}
