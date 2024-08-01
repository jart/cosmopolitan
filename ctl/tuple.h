// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_TUPLE_H_
#define CTL_TUPLE_H_
#include "decay.h"
#include "enable_if.h"
#include "is_same.h"
#include "utility.h"

namespace ctl {

// Forward declaration
template<typename... Types>
class tuple;

// Base case for tuple
template<>
class tuple<>
{};

// Recursive case for tuple
template<typename Head, typename... Tail>
class tuple<Head, Tail...> : public tuple<Tail...>
{
    using Base = tuple<Tail...>;

  public:
    Head head;

    constexpr tuple() : Base(), head()
    {
    }

    template<typename H,
             typename... T,
             typename = typename ctl::enable_if<
               !ctl::is_same<typename ctl::decay<H>::type, tuple>::value>::type>
    constexpr tuple(H&& h, T&&... t)
      : Base(ctl::forward<T>(t)...), head(ctl::forward<H>(h))
    {
    }

    template<typename... UTypes>
    constexpr tuple(const tuple<UTypes...>& other)
      : Base(static_cast<const tuple<UTypes...>&>(other)), head(other.head)
    {
    }

    template<typename... UTypes>
    constexpr tuple(tuple<UTypes...>&& other)
      : Base(static_cast<tuple<UTypes...>&&>(other))
      , head(ctl::forward<Head>(other.head))
    {
    }

    tuple(const tuple&) = default;
    tuple(tuple&&) = default;
    tuple& operator=(const tuple&) = default;
    tuple& operator=(tuple&&) = default;
};

// Helper struct for getting element type
template<size_t I, typename T>
struct tuple_element;

template<size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>>
  : tuple_element<I - 1, tuple<Tail...>>
{};

template<typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>>
{
    using type = Head;
};

// Helper function to get element
template<size_t I, typename Head, typename... Tail>
constexpr typename tuple_element<I, tuple<Head, Tail...>>::type&
get(tuple<Head, Tail...>& t)
{
    if constexpr (I == 0) {
        return t.head;
    } else {
        return get<I - 1>(static_cast<tuple<Tail...>&>(t));
    }
}

template<size_t I, typename Head, typename... Tail>
constexpr const typename tuple_element<I, tuple<Head, Tail...>>::type&
get(const tuple<Head, Tail...>& t)
{
    if constexpr (I == 0) {
        return t.head;
    } else {
        return get<I - 1>(static_cast<const tuple<Tail...>&>(t));
    }
}

// Helper function to create a tuple
template<typename... Types>
constexpr tuple<typename ctl::decay<Types>::type...>
make_tuple(Types&&... args)
{
    return tuple<typename ctl::decay<Types>::type...>(
      ctl::forward<Types>(args)...);
}

// Helper function for tuple comparison
template<size_t I = 0, typename... TTypes, typename... UTypes>
constexpr bool
tuple_equals(const tuple<TTypes...>& t, const tuple<UTypes...>& u)
{
    if constexpr (I == sizeof...(TTypes)) {
        return true;
    } else {
        return get<I>(t) == get<I>(u) && tuple_equals<I + 1>(t, u);
    }
}

// Equality comparison
template<typename... TTypes, typename... UTypes>
constexpr bool
operator==(const tuple<TTypes...>& t, const tuple<UTypes...>& u)
{
    return tuple_equals(t, u);
}

template<typename... TTypes, typename... UTypes>
constexpr bool
operator!=(const tuple<TTypes...>& t, const tuple<UTypes...>& u)
{
    return !(t == u);
}

} // namespace ctl

#endif // CTL_TUPLE_H_
