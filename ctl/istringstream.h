// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ISTRINGSTREAM_H_
#define CTL_ISTRINGSTREAM_H_
#include "ios_base.h"
#include "string.h"

namespace ctl {

class istringstream : public ios_base
{
  public:
    istringstream();
    explicit istringstream(const ctl::string_view&);

    string str() const;
    void str(const string&);

    istringstream& operator>>(char&);
    istringstream& operator>>(char*);
    istringstream& operator>>(ctl::string&);
    istringstream& operator>>(short&);
    istringstream& operator>>(unsigned short&);
    istringstream& operator>>(int&);
    istringstream& operator>>(unsigned int&);
    istringstream& operator>>(long&);
    istringstream& operator>>(unsigned long&);
    istringstream& operator>>(float&);
    istringstream& operator>>(double&);

  private:
    ctl::string buffer_;
    size_t read_pos_;

    template<typename T>
    istringstream& read_numeric(T&);
};

} // namespace ctl

#endif // CTL_ISTRINGSTREAM_H_
