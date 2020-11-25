#ifndef COSMOPOLITAN_LIBC_OHMYPLUS_VECTOR_H_
#define COSMOPOLITAN_LIBC_OHMYPLUS_VECTOR_H_
#ifdef __cplusplus
extern "C" {
void __vector_reserve(size_t, size_t, intptr_t **, size_t *);
} /* extern c */
namespace std {

template <class T>
class vector {
 public:
  vector() : data_(NULL), size_(0), toto_(0) {
  }
  vector(size_t n) : data_(NULL), size_(n), toto_(0) {
    VectorReserve(n);
  }
  size_t size() const {
    return size_;
  }
  size_t capacity() const {
    return toto_;
  }
  T &front() {
    return data_[0];
  }
  T &back() {
    return data_[size_ - 1];
  }
  void clear() {
    size_ = 0;
  }
  void reserve(size_t n) {
    VectorReserve(n);
  }
  void resize(size_t n) {
    reserve((size_ = n));
  }
  bool empty() const {
    return !size_;
  }
  T &operator[](size_t i) {
    return data_[i];
  }

 private:
  T *data_;
  size_t size_;
  size_t toto_;
  void VectorReserve(size_t n) {
    __vector_reserve(n, sizeof(T), (intptr_t **)&data_, &toto_);
  }
};

};     /* namespace std */
#endif /* __cplusplus */
#endif /* COSMOPOLITAN_LIBC_OHMYPLUS_VECTOR_H_ */
