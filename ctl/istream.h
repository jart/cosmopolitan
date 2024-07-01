// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ISTREAM_H_
#define CTL_ISTREAM_H_
#include "ios.h"

namespace ctl {

struct string;

class istream : public ios
{
  public:
    istream() = delete;
    explicit istream(FILE*);
    virtual ~istream();

    istream& operator>>(char&);
    istream& operator>>(int&);
    istream& operator>>(long&);
    istream& operator>>(double&);
    istream& operator>>(bool&);
    istream& operator>>(ctl::string&);
    istream& operator>>(istream& (*)(istream&));

    int get();
    istream& get(char&);
    istream& getline(char*, streamsize);
    istream& getline(char*, streamsize, char);
    istream& read(char*, streamsize);
    streamsize gcount() const;

    int peek();
    istream& ignore(streamsize = 1, int = -1);

    istream(istream&&) noexcept;
    istream& operator=(istream&&) noexcept;

  private:
    streamsize gcount_;

    istream(const istream&) = delete;
    istream& operator=(const istream&) = delete;
};

extern istream cin;

istream&
ws(istream& is);

} // namespace ctl

#endif // CTL_ISTREAM_H_
