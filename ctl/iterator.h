// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_OUTPUT_ITERATOR_TAG_H_
#define CTL_OUTPUT_ITERATOR_TAG_H_

namespace ctl {

struct input_iterator_tag
{};

struct output_iterator_tag
{};

struct forward_iterator_tag : public input_iterator_tag
{};

struct bidirectional_iterator_tag : public forward_iterator_tag
{};

struct random_access_iterator_tag : public bidirectional_iterator_tag
{};

struct contiguous_iterator_tag : public random_access_iterator_tag
{};

} // namespace ctl

#endif // CTL_OUTPUT_ITERATOR_TAG_H_
