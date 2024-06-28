#ifndef CTL_ENABLE_IF_H_
#define CTL_ENABLE_IF_H_

namespace ctl {

template<bool B, class T = void>
struct enable_if
{};

template<class T>
struct enable_if<true, T>
{
    typedef T type;
};

template<bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

} // namespace ctl

#endif // CTL_ENABLE_IF_H_
