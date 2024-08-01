// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_REQUIRE_INPUT_ITERATOR_H_
#define CTL_REQUIRE_INPUT_ITERATOR_H_
#include "enable_if.h"
#include "is_convertible.h"
#include "iterator.h"
#include "iterator_traits.h"

namespace ctl {

template<typename InputIt>
using require_input_iterator = typename ctl::enable_if<
  ctl::is_convertible<typename ctl::iterator_traits<InputIt>::iterator_category,
                      ctl::input_iterator_tag>::value>::type;

} // namespace ctl

#endif /* CTL_REQUIRE_INPUT_ITERATOR_H_ */
