// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_OSTREAM_H_
#define CTL_OSTREAM_H_
#include "ios.h"

namespace ctl {

struct string_view;

class ostream : public ios
{
  public:
    ostream() = delete;
    explicit ostream(FILE*);
    virtual ~ostream();

    ostream& operator<<(const char*);
    ostream& operator<<(bool);
    ostream& operator<<(char);
    ostream& operator<<(int);
    ostream& operator<<(unsigned);
    ostream& operator<<(long);
    ostream& operator<<(unsigned long);
    ostream& operator<<(float);
    ostream& operator<<(double);
    ostream& operator<<(const ctl::string_view&);
    ostream& operator<<(ostream& (*)(ostream&));

    ostream& put(char);
    ostream& write(const char*, streamsize);
    ostream& flush();

    ostream(ostream&&) noexcept;
    ostream& operator=(ostream&&) noexcept;

  private:
    ostream(const ostream&) = delete;
    ostream& operator=(const ostream&) = delete;
};

extern ostream cout;
extern ostream cerr;

ostream&
endl(ostream&);

ostream&
ends(ostream&);

ostream&
flush(ostream&);

} // namespace ctl

#endif // CTL_OSTREAM_H_
