// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IOS_H_
#define CTL_IOS_H_
#include "ios_base.h"

struct FILE;

namespace ctl {

class ios : public ios_base
{
  public:
    explicit ios(FILE*);
    virtual ~ios();

  protected:
    ios& operator=(ios&&) noexcept;
    FILE* file_;

  private:
    ios(const ios&) = delete;
};

} // namespace ctl

#endif // CTL_IOS_H_
