// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_DECAY_H_
#define CTL_DECAY_H_
#include "add_pointer.h"
#include "conditional.h"
#include "is_array.h"
#include "is_function.h"
#include "remove_cv.h"
#include "remove_extent.h"
#include "remove_reference.h"

namespace ctl {

template<typename T>
struct decay
{
  private:
    typedef typename ctl::remove_reference<T>::type U;

  public:
    typedef typename ctl::conditional<
      ctl::is_array<U>::value,
      typename ctl::remove_extent<U>::type*,
      typename ctl::conditional<ctl::is_function<U>::value,
                                typename ctl::add_pointer<U>::type,
                                typename ctl::remove_cv<U>::type>::type>::type
      type;
};

template<typename T>
using decay_t = typename decay<T>::type;

} // namespace ctl

#endif // CTL_DECAY_H_
