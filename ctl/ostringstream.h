// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_OSTRINGSTREAM_H_
#define CTL_OSTRINGSTREAM_H_
#include "ios_base.h"
#include "string.h"

namespace ctl {

class ostringstream : public ios_base
{
  public:
    ostringstream();
    explicit ostringstream(const ctl::string_view&);

    ctl::string str() const;
    void str(const ctl::string& s);
    void clear();

    ostringstream& operator<<(char);
    ostringstream& operator<<(const ctl::string_view&);
    ostringstream& operator<<(int);
    ostringstream& operator<<(unsigned int);
    ostringstream& operator<<(long);
    ostringstream& operator<<(unsigned long);
    ostringstream& operator<<(float);
    ostringstream& operator<<(double);

  private:
    ctl::string buffer_;
    size_t write_pos_;
};

} // namespace ctl

#endif // CTL_OSTRINGSTREAM_H_
