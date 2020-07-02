/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                              :vi│
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
#ifndef COSMOPOLITAN_LIBC_OHMYPLUS_VECTOR_H_
#define COSMOPOLITAN_LIBC_OHMYPLUS_VECTOR_H_
extern "C" {
void __vector_reserve(size_t, size_t, intptr_t **, size_t *);
} /* extern c */
namespace std {

template <class T>
class vector {
 public:
  vector() : data_(NULL), size_(0), toto_(0) {}
  vector(size_t n) : data_(NULL), size_(n), toto_(0) { VectorReserve(n); }
  size_t size() const { return size_; }
  size_t capacity() const { return toto_; }
  T &front() { return data_[0]; }
  T &back() { return data_[size_ - 1]; }
  void clear() { size_ = 0; }
  void reserve(size_t n) { VectorReserve(n); }
  void resize(size_t n) { reserve((size_ = n)); }
  bool empty() const { return !size_; }
  T &operator[](size_t i) { return data_[i]; }

 private:
  T *data_;
  size_t size_;
  size_t toto_;
  void VectorReserve(size_t n) {
    __vector_reserve(n, sizeof(T), (intptr_t **)&data_, &toto_);
  }
};

};     /* namespace std */
#endif /* COSMOPOLITAN_LIBC_OHMYPLUS_VECTOR_H_ */
