// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IOS_BASE_H_
#define CTL_IOS_BASE_H_

namespace ctl {

class ios_base
{
  public:
    typedef size_t streamsize;

    enum iostate
    {
        goodbit = 0,
        badbit = 1,
        failbit = 2,
        eofbit = 4
    };

    enum fmtflags
    {
        boolalpha = 1 << 0,
        dec = 1 << 1,
        fixed = 1 << 2,
        hex = 1 << 3,
        internal = 1 << 4,
        left = 1 << 5,
        oct = 1 << 6,
        right = 1 << 7,
        scientific = 1 << 8,
        showbase = 1 << 9,
        showpoint = 1 << 10,
        showpos = 1 << 11,
        skipws = 1 << 12,
        unitbuf = 1 << 13,
        uppercase = 1 << 14,
        adjustfield = left | right | internal,
        basefield = dec | oct | hex,
        floatfield = scientific | fixed
    };

    enum openmode
    {
        app = 1 << 0,
        binary = 1 << 1,
        in = 1 << 2,
        out = 1 << 3,
        trunc = 1 << 4,
        ate = 1 << 5
    };

  protected:
    ios_base();
    virtual ~ios_base();

    int state_;
    int flags_;

  public:
    fmtflags flags() const;
    fmtflags flags(fmtflags);
    fmtflags setf(fmtflags);
    fmtflags setf(fmtflags, fmtflags);
    void unsetf(fmtflags);

    iostate rdstate() const;
    void clear(int = goodbit);
    void setstate(int);

    bool good() const;
    bool eof() const;
    bool fail() const;
    bool bad() const;

    bool operator!() const;
    explicit operator bool() const;

  private:
    ios_base(const ios_base&) = delete;
    ios_base& operator=(const ios_base&) = delete;
};

} // namespace ctl

#endif // CTL_IOS_BASE_H_
