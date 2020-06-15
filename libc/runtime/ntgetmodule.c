/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nt/ntdll.h"
#include "libc/nt/struct/ldr.h"
#include "libc/nt/struct/ldrdatatableentry.h"
#include "libc/nt/struct/linkedlist.h"
#include "libc/nt/struct/teb.h"
#include "libc/str/str.h"

textwindows const struct NtLdrDataTableEntry *NtGetModule(
    const char *basename) {
  struct NtLinkedList *head = &NtGetPeb()->Ldr->InLoadOrderModuleList;
  struct NtLinkedList *ldr = head->Next;
  do {
    const struct NtLdrDataTableEntry *dll =
        (const struct NtLdrDataTableEntry *)ldr;
    if (strcasecmp8to16(basename, dll->BaseDllName.Data) == 0) return dll;
  } while ((ldr = ldr->Next) && ldr != head);
  return NULL;
}
