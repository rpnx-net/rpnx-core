// Copyright 2020 Ryan P. Nicholl <rnicholl@protonmail.com>, http://rpnx.net/
// All rights reserved
// See rpnx-core/LICENSE.txt

#ifndef RPNX_META_HPP
#define RPNX_META_HPP
#include <tuple>

namespace rpnx
{
    template <typename ... Ts>
    struct typelist;

    template <typename T, typename Tuple>
    struct tuple_type_index;

    template <typename T, typename T2, typename ... Ts>
    struct tuple_type_index<T, std::tuple<T2, Ts...> >
    {
        static const int value = (std::is_same<T, T2>::value ? 0 : (tuple_type_index<T, std::tuple<Ts...>>::value == -1? -1 : tuple_type_index<T, std::tuple<Ts...>>::value +1 ));
    };

    template <typename T, typename T2>
    struct tuple_type_index<T, std::tuple<T2> >
    {
        static const int value = (std::is_same<T, T2>::value ? 0 : -1 );
    };

    static_assert(tuple_type_index<int, std::tuple<int, char>>::value == 0);
    static_assert(tuple_type_index<char, std::tuple<int, char>>::value == 1);
    static_assert(tuple_type_index<long long, std::tuple<int, char>>::value == -1);

    template <template <typename> typename TT, typename ... Ts>
    struct is_true_for_all;
    
    template < template < typename > typename TT, typename T, typename... Ts >
    struct is_true_for_all< TT, T, Ts... >
    {
        static constexpr bool const value = TT< T >::value && is_true_for_all< TT, Ts... >::value;
    };

    template < template < typename > typename TT, typename T>
    struct is_true_for_all< TT, T >
    {
        static constexpr bool const value = TT< T >::value;
    };

    template <typename I, template <typename > typename TT, typename ... Ts>
    struct sum_for_all;

    template < typename I, template < typename > typename TT, typename T, typename... Ts >
    struct sum_for_all< I, TT, T, Ts... >
    {
        static constexpr I const value = TT<  T >::value + sum_for_all< I, TT, Ts... >::value;
    };

    template < typename I, template < typename > typename TT, typename T >
    struct sum_for_all< I, TT, T >
    {
        static constexpr I const value = TT< T >::value ;
    };


}

#endif