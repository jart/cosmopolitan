// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_EXCEPTION_H_
#define CTL_EXCEPTION_H_

namespace ctl {

class exception
{
  public:
    exception() noexcept = default;
    virtual ~exception() = default;

    virtual const char* what() const noexcept
    {
        return "ctl::exception";
    }
};

} // namespace ctl

#endif // CTL_EXCEPTION_H_
