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

    template < typename... Ts >
    auto make_knot(std::tuple< Ts... > const& input_tuple)
    {
        return std::apply(
            [&](auto &... ts) {
                return std::tie(std::forward< decltype(ts) >(ts)...);
            },
            input_tuple);
    }

    template < typename... Ts >
    auto untie(std::tuple< Ts... > const & input_tuple)
    {
        return std::apply(
            [&](auto&... ts) {
                return std::make_tuple(std::forward< decltype(ts) >(ts)...);
            },
            input_tuple);
    }

    template < typename... Ts >
    auto untie(std::tuple< Ts... > && input_tuple)
    {
        return std::apply(
            [&](auto&... ts) {
                return std::make_tuple(std::forward< decltype(ts) >(ts)...);
            },
            input_tuple);
    }

    template < typename... Ts >
    auto untie(std::tuple< Ts... > & input_tuple)
    {
        return std::apply(
            [&](auto&... ts) {
                return std::make_tuple(std::forward< decltype(ts) >(ts)...);
            },
            input_tuple);
    }

    template < typename... Ts >
    auto make_const_knot(std::tuple< Ts... > const& input_tuple)
    {
        return std::apply(
            [&](auto const&... ts) {
                return std::tie(std::forward< decltype(ts) >(ts)...);
            },
            input_tuple);
    }

    template <typename T>
    struct const_knotted;

    template <typename ... Ts>
    struct const_knotted<std::tuple<Ts...>>
    {
        using type = decltype(make_const_knot(*(std::tuple<Ts...>const*)nullptr));
    };

    template < typename... Ts >
    struct const_knotted< std::tuple< Ts... > const >
    {
        using type = decltype(make_const_knot(*(std::tuple< Ts... > const*)nullptr));
    };

    template < typename T >
    struct untied;

    template < typename... Ts >
    struct untied< std::tuple< Ts... > >
    {
        using type = decltype(untie(*(std::tuple< Ts... > *)nullptr));
    };

    template < typename... Ts >
    struct untied< std::tuple< Ts... > const >
    {
        using type = decltype(untie(*(std::tuple< Ts... > *)nullptr));
    };

    template <typename T>
    using untied_t = typename untied<T>::type;

    template < std::size_t I, std::size_t N, typename... Ts>
    void tuple_set_bool_sequence(std::tuple<Ts ...> & tuple, std::uint8_t bool_val)
    {
        static_assert(N >= 1 && N <= 8);

        std::get< I >(tuple) = bool_val & (1 << 0) ? true : false;
        if constexpr (N > 1) 
        {
            std::get< I + 1 >(tuple) = bool_val & (1 << 1) ? true : false;
        }
        if constexpr (N > 2)
        {
            std::get< I + 2 >(tuple) = bool_val & (1 << 2) ? true : false;
        }
        if constexpr (N > 3)
        {
            std::get< I + 3 >(tuple) = bool_val & (1 << 3) ? true : false;
        }
        if constexpr (N > 4)
        {
            std::get< I + 4 >(tuple) = bool_val & (1 << 4) ? true : false;
        }
        if constexpr (N > 5)
        {
            std::get< I + 5 >(tuple) = bool_val & (1 << 5) ? true : false;
        }
        if constexpr (N > 6)
        {
            std::get< I + 6 >(tuple) = bool_val & (1 << 6) ? true : false;
        }    
        if constexpr (N > 7)
        {
            std::get< I + 7 >(tuple) = bool_val & (1 << 7) ? true : false;
        }
    }

    template < std::size_t I, std::size_t N, typename... Ts >
    std::uint8_t tuple_get_bool_sequence(std::tuple< Ts... > const& tuple)
    {
        static_assert(N >= 1 && N <= 8);
        std::uint8_t output = 0;
        
        if (std::get< I >(tuple))
        {
            output |= (1 << 0);
        }

        if constexpr (N > 1)
        {
            if (std::get< I + 1 >(tuple)) 
            {
                output |= (1 << 1);
            }
        }
        if constexpr (N > 2)
        {
            if (std::get< I + 2 >(tuple))
            {
                output |= (1 << 2);
            }
        }
        if constexpr (N > 3)
        {
            if (std::get< I + 3 >(tuple))
            {
                output |= (1 << 3);
            }
        }
        if constexpr (N > 4)
        {
            if (std::get< I + 4 >(tuple))
            {
                output |= (1 << 4);
            }
        }
        if constexpr (N > 5)
        {
            if (std::get< I + 5 >(tuple))
            {
                output |= (1 << 5);
            }
        }
        if constexpr (N > 6)
        {
            if (std::get< I + 6 >(tuple))
            {
                output |= (1 << 6);
            }
        }
        if constexpr (N > 7)
        {
            if (std::get< I + 7 >(tuple))
            {
                output |= (1 << 7);
            }
        }
        return output;

    }
}

#endif