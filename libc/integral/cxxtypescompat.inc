#define NAME __cxx_types_compatible
#define QUALIFIED(Q1, Q2)       \
  template <class _T, class _U> \
  struct NAME<_T Q1, _U Q2> : NAME<_T, _U> {}

template <class, class>
struct NAME {
  enum { _value = 0 };
};

template <class _T>
struct NAME<_T, _T> {
  enum { _value = 1 };
};

template <class _T, size_t N>
struct NAME<_T[], _T[N]> {
  enum { _value = 1 };
};

template <class _T, size_t N>
struct NAME<_T[N], _T[]> {
  enum { _value = 1 };
};

QUALIFIED(const volatile, );
QUALIFIED(const volatile, const);
QUALIFIED(const, const volatile);
QUALIFIED(volatile, const volatile);
QUALIFIED(const volatile, volatile);
QUALIFIED(const, volatile);
QUALIFIED(volatile, const);
QUALIFIED(, const);
QUALIFIED(const, );
QUALIFIED(, volatile);
QUALIFIED(volatile, );

#undef QUALIFIED
#undef NAME
