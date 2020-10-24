#ifndef RPNX_META_HPP
#define RPNX_META_HPP
#include <tuple>

namespace rpnx
{
    template <typename T, typename Tuple>
    struct tuple_type_index;

    template <typename T, typename T2, typename ... Ts>
    struct tuple_type_index<T, std::tuple<T2, Ts...> >
    {
        static const int value = (std::is_same<T, T2>::value ? 0 : tuple_type_index<T, std::tuple<Ts...>>::value +1 );
    };

    template <typename T, typename T2>
    struct tuple_type_index<T, std::tuple<T2> >
    {
        static const int value = (std::is_same<T, T2>::value ? 0 : -1 );
    };

    static_assert(tuple_type_index<int, std::tuple<int, char>>::value == 0);
    static_assert(tuple_type_index<char, std::tuple<int, char>>::value == 1);
}

#endif