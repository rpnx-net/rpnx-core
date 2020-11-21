/*
Copyright (c) 2016, 2017, 2018, 2020 Ryan P. Nicholl <rnicholl@protonmail.com> http://rpnx.net/
RPNX Serial Traits 2
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS
IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
This is currently a work in progress. Iterator's a version 2 of the original version I published as a separate library.
This version should perform better due to the inclusion of generator interfaces, which should, at least in theory,
optimize much better than a strictly iterator based approach, while maintaining safety.


*/
#ifndef RPNX_SERIAL_TRAITS_2_HPP
#define RPNX_SERIAL_TRAITS_2_HPP

#include <algorithm>
#include <assert.h>
#include <cinttypes>
#include <cstddef>
#include <iterator>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "rpnx/meta.hpp"

namespace rpnx
{

    // These are specialization placeholders
    // They are used to represent integers of any
    // size in a machine independent format.

    // This is a "wire type", it only exists as a serial encoding,
    // and objects of this type cannot be created.
    struct intany;

    // This is a "wire type", it only exists as a serial encoding,
    // and objects of this type cannot be created.
    struct uintany;


    // This is a "wire type", it only exists as a serial encoding,
    // and objects of this type cannot be created.
    template < typename I >
    struct big_endian;

    // This is a "wire type", it only exists as a serial encoding,
    // and objects of this type cannot be created.
    template < typename I >
    struct little_endian;

    // This is a "wire type", it only exists as a serial encoding,
    // and objects of this type cannot be created.
    template < typename C >
    struct with_32bit_size;

    // This is a "wire type", it only exists as a serial encoding,
    // and objects of this type cannot be created.
    template < typename C >
    struct with_64bit_size;

    template < typename T >
    struct serial_traits;

    template <typename K, typename V>
    struct map_serial_traits;

    template < typename T, typename Generator >
    struct synchronous_generator_serial_traits;

    template < typename T, typename Iterator >
    struct synchronous_iterator_serial_traits;

    template < typename T, typename Iterator, std::size_t I, typename... Ts >
    struct synchronous_iterator_tuple_serial_traits;

    template < typename K, typename V, typename Iterator >
    struct synchronous_iterator_map_serial_traits;

    template < typename T, typename Generator, std::size_t I, typename... Ts >
    struct synchronous_generator_tuple_serial_traits;

    template < typename K, typename V, typename Iterator >
    struct synchronous_generator_map_serial_traits;

    template < typename T, std::size_t I, typename... Ts >
    struct tuple_serial_traits;

    template < typename T >
    struct c_fixed_serial_size;

    template < typename T, typename Iterator >
    auto quick_serialize(T const& val, Iterator iterator) -> Iterator;

   
    template <>
    struct serial_traits< bool >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 1;
        }
        static inline constexpr std::size_t serial_size(const bool&) noexcept
        {
            return 1;
        }
    };

    // The following 4 specializations are traits for unsigned integers
    // has_fixed_serial_size should return true, and returns the number of bytes in
    // an integer
    template <>
    struct serial_traits< std::uint8_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 1;
        }
        static inline constexpr std::size_t serial_size(const std::uint8_t&) noexcept
        {
            return 1;
        }
    };



    template <>
    struct serial_traits< std::uint16_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 2;
        }
        static inline constexpr std::size_t serial_size(const std::uint16_t&) noexcept
        {
            return 2;
        }
    };

    template <>
    struct serial_traits< std::uint32_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 4;
        }
        static inline constexpr std::size_t serial_size(const std::uint32_t&) noexcept
        {
            return 4;
        }
    };

    template <>
    struct serial_traits< std::uint64_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 8;
        }
        static inline constexpr std::size_t serial_size(const std::uint64_t&) noexcept
        {
            return 8;
        }
    };

    template <>
    struct serial_traits< std::int8_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 1;
        }
        static inline constexpr std::size_t serial_size(const std::int8_t&) noexcept
        {
            return 1;
        }
    };

    template <>
    struct serial_traits< std::int16_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 2;
        }
        static inline constexpr std::size_t serial_size(const std::int16_t&) noexcept
        {
            return 2;
        }
    };

    template <>
    struct serial_traits< std::int32_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 4;
        }
        static inline constexpr std::size_t serial_size(const std::int32_t&) noexcept
        {
            return 4;
        }
    };

    template <>
    struct serial_traits< std::int64_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 8;
        }
        static inline constexpr std::size_t serial_size(const std::int64_t&) noexcept
        {
            return 8;
        }
    };

    template <>
    struct serial_traits< char > : serial_traits< int8_t >
    {
    };

    // The following 4 specializations are for serial synchronous generator interfaces for the
    // unsigned integers

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::uint8_t, Generator >
    {
        static inline constexpr auto serialize(std::uint8_t val, Generator g)
        {
            auto it = g(1);
            *it++ = val;
        }

        static inline constexpr auto deserialize(std::uint8_t& val, Generator g)
        {
            auto it = g(1);
            val = *it++;
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::uint16_t, Generator >
    {
        static inline constexpr auto serialize(std::uint16_t val, Generator g)
        {
            auto it = g(2);
            *it++ = val & 0xFF;
            *it = (val >> 8) & 0xFF;
        }

        static inline constexpr auto deserialize(std::uint16_t& val, Generator g)
        {
            auto it = g(2);
            val = *it++;
            val |= (*it << 8);
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::uint32_t, Generator >
    {
        static inline constexpr auto serialize(std::uint32_t val, Generator g)
        {
            auto it = g(4);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it = (val >> 24) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::uint32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::uint32_t& val, Generator g)
        {
            auto it = g(4);
            val = *it++;
            val |= (*it++ << 8);
            val |= (*it++ << 16);
            val |= (*it++ << 24);
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::uint64_t, Generator >
    {
        static inline constexpr auto serialize(std::uint64_t val, Generator g)
        {
            auto it = g(8);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it++ = (val >> 24) & 0xFF;
            *it++ = (val >> 32) & 0xFF;
            *it++ = (val >> 40) & 0xFF;
            *it++ = (val >> 48) & 0xFF;
            *it = (val >> 56) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::uint32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::uint64_t& val, Generator g)
        {
            auto it = g(8);
            val = std::uint64_t(*it++);
            val |= (std::uint64_t(*it++) << 8);
            val |= (std::uint64_t(*it++) << 16);
            val |= (std::uint64_t(*it++) << 24);
            val |= (std::uint64_t(*it++) << 32);
            val |= (std::uint64_t(*it++) << 40);
            val |= (std::uint64_t(*it++) << 48);
            val |= (std::uint64_t(*it) << 56);
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< bool, Generator >
    {
        static inline constexpr auto serialize(bool val, Generator g)
        {
            auto it = g(1);
            *it++ = val == true ? 1 : 0;
        }

        static inline constexpr auto deserialize(bool& val, Generator g)
        {
            auto it = g(1);
            val = (*it++ == 0 ? false : true);
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::int8_t, Generator >
    {
        static inline constexpr auto serialize(std::int8_t val, Generator g)
        {
            auto it = g(1);
            *it++ = val;
        }

        static inline constexpr auto deserialize(std::int8_t& val, Generator g)
        {
            auto it = g(1);
            val = *it++;
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::int16_t, Generator >
    {
        static inline constexpr auto serialize(std::int16_t val, Generator g)
        {
            auto it = g(2);
            *it++ = val & 0xFF;
            *it = (val >> 8) & 0xFF;
        }

        static inline constexpr auto deserialize(std::int16_t& val, Generator g)
        {
            auto it = g(2);
            val = *it++;
            val |= (*it << 8);
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::int32_t, Generator >
    {
        static inline constexpr auto serialize(std::int32_t val, Generator g)
        {
            auto it = g(4);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it = (val >> 24) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::int32_t& val, Generator g)
        {
            auto it = g(4);
            val = *it++;
            val |= (*it++ << 8);
            val |= (*it++ << 16);
            val |= (*it++ << 24);
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::int64_t, Generator >
    {
        static inline constexpr auto serialize(std::int64_t val, Generator g)
        {
            auto it = g(8);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it++ = (val >> 24) & 0xFF;
            *it++ = (val >> 32) & 0xFF;
            *it++ = (val >> 40) & 0xFF;
            *it++ = (val >> 48) & 0xFF;
            *it = (val >> 56) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::int64_t& val, Generator g)
        {
            auto it = g(8);
            val = *it++;
            val |= (*it++ << 8);
            val |= (*it++ << 16);
            val |= (*it++ << 24);
            val |= (std::uint64_t(*it++) << 32);
            val |= (std::uint64_t(*it++) << 40);
            val |= (std::uint64_t(*it++) << 48);
            val |= (std::uint64_t(*it) << 56);
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::uint8_t, Iterator >
    {
        static inline constexpr auto serialize(std::uint8_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::uint8_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< bool, Iterator >
    {
        static inline constexpr auto serialize(bool val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val ? 1 : 0;
            return outIterator;
        }

        static inline constexpr auto deserialize(bool& val, Iterator in) -> Iterator
        {
            val = (*in++ == 0 ? false : true);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< char, Iterator >
    {
        static inline constexpr auto serialize(char val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            return outIterator;
        }

        static inline constexpr auto deserialize(char& val, Iterator in) -> Iterator
        {
            val = *in++;
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::uint16_t, Iterator >
    {
        static inline constexpr auto serialize(std::uint16_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            *outIterator++ = val >> 8;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::uint16_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            val |= (*in++ << 8);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::int16_t, Iterator >
    {
        static inline constexpr auto serialize(std::int16_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            *outIterator++ = val >> 8;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::int16_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            val |= (*in++ << 8);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::int32_t, Iterator >
    {
        static inline constexpr auto serialize(std::int32_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            *outIterator++ = val >> 8;
            *outIterator++ = val >> 16;
            *outIterator++ = val >> 24;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::int32_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::uint32_t, Iterator >
    {
        static inline constexpr auto serialize(std::uint32_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            *outIterator++ = val >> 8;
            *outIterator++ = val >> 16;
            *outIterator++ = val >> 24;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::uint32_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::uint64_t, Iterator >
    {
        static inline constexpr auto serialize(std::uint64_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            *outIterator++ = val >> 8;
            *outIterator++ = val >> 16;
            *outIterator++ = val >> 24;
            *outIterator++ = val >> 32;
            *outIterator++ = val >> 40;
            *outIterator++ = val >> 48;
            *outIterator++ = val >> 56;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::uint64_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            val |= (std::uint64_t(*in++) << 32);
            val |= (std::uint64_t(*in++) << 40);
            val |= (std::uint64_t(*in++) << 48);
            val |= (std::uint64_t(*in++) << 56);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::int64_t, Iterator >
    {
        static inline constexpr auto serialize(std::int64_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            *outIterator++ = val >> 8;
            *outIterator++ = val >> 16;
            *outIterator++ = val >> 24;
            *outIterator++ = val >> 32;
            *outIterator++ = val >> 40;
            *outIterator++ = val >> 48;
            *outIterator++ = val >> 56;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::int64_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            val |= (std::uint64_t(*in++) << 32);
            val |= (std::uint64_t(*in++) << 40);
            val |= (std::uint64_t(*in++) << 48);
            val |= (std::uint64_t(*in++) << 56);
            return in;
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::int8_t, Iterator >
    {
        static inline constexpr auto serialize(std::int8_t val, Iterator outIterator) -> Iterator
        {
            *outIterator++ = val;
            return outIterator;
        }

        static inline constexpr auto deserialize(std::int8_t& val, Iterator in) -> Iterator
        {
            val = *in++;
            return in;
        }
    };

    template< typename ... Ts, std::size_t I, typename T, typename ... Ts2>
    struct tuple_serial_traits<std::tuple<Ts...>, I, T, Ts2...>
    {
        static constexpr bool has_fixed_serial_size()
        {
            if (!serial_traits< T >::has_fixed_serial_size()) {
                return false;
            }                
            else
            {
                return tuple_serial_traits< std::tuple< Ts... >, I + 1, Ts2... >::has_fixed_serial_size();
            }
        }

        static constexpr std::size_t serial_size(std::tuple<Ts...> const & tuple)
        {
            return serial_traits< T >::serial_size(std::get< I >(tuple)) + tuple_serial_traits< std::tuple< Ts... >, I + 1, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            static_assert(serial_traits< T >::has_fixed_serial_size());
            return serial_traits< T >::fixed_serial_size() + tuple_serial_traits< std::tuple< Ts... >, I + 1, Ts2... >::fixed_serial_size();
        }

        static constexpr std::size_t serial_size2(std::tuple< Ts... > const& tuple, std::size_t n = 0)
        {
            return tuple_serial_traits< std::tuple< Ts... >, I + 1, Ts2... >::serial_size2(tuple, n + serial_traits< T >::serial_size(std::get< I >(tuple)));
        }
    };

    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
                return tuple_serial_traits< std::tuple< Ts... >, I + 8, Ts2... >::has_fixed_serial_size();
            
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 8, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 8, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool, bool, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };

    
    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
                return tuple_serial_traits< std::tuple< Ts... >, I + 7, Ts2... >::has_fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 7, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 7, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };

    
    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return tuple_serial_traits< std::tuple< Ts... >, I + 6, Ts2... >::has_fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 6, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 6, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };


    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool,  Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return tuple_serial_traits< std::tuple< Ts... >, I + 5, Ts2... >::has_fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 5, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 5, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };

    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool, Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return tuple_serial_traits< std::tuple< Ts... >, I + 4, Ts2... >::has_fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 4, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 4, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };

    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool,  Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return tuple_serial_traits< std::tuple< Ts... >, I + 3, Ts2... >::has_fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 3, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 3, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };

    template < typename... Ts, std::size_t I, typename... Ts2 >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool, Ts2... >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return tuple_serial_traits< std::tuple< Ts... >, I + 2, Ts2... >::has_fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 2, Ts2... >::serial_size(tuple);
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1 + tuple_serial_traits< std::tuple< Ts... >, I + 2, Ts2... >::fixed_serial_size();
        }
    };

    template < typename... Ts, std::size_t I >
    struct tuple_serial_traits< std::tuple< Ts... >, I, bool, bool >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return true;
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return 1;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return 1;
        }
    };



    template < typename... Ts, std::size_t I, typename T >
    struct tuple_serial_traits< std::tuple< Ts... >, I, T >
    {
        static constexpr bool has_fixed_serial_size()
        {
            if (!serial_traits< T >::has_fixed_serial_size())
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        static constexpr std::size_t serial_size(std::tuple< Ts... > const& tuple)
        {
            return serial_traits< T >::serial_size(std::get< I >(tuple));
        }

        static constexpr std::size_t fixed_serial_size()
        {
            static_assert(serial_traits< T >::has_fixed_serial_size());
            return serial_traits< T >::fixed_serial_size();
        }

        static constexpr std::size_t serial_size2(std::tuple< Ts... > const& tuple, std::size_t n = 0)
        {
            return n + serial_traits< T >::serial_size(std::get< I >(tuple));
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I, typename T, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, T, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            outit = synchronous_iterator_serial_traits< T, Iterator >::serialize(std::get< I >(value), outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 1, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            in = synchronous_iterator_serial_traits< T, Iterator >::deserialize(std::get< I >(value), in);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 1, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I, typename T >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, T >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            return synchronous_iterator_serial_traits< T, Iterator >::serialize(std::get< I >(value), outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            return synchronous_iterator_serial_traits< T, Iterator >::deserialize(std::get< I >(value), in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename T, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, T, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            synchronous_generator_serial_traits< T, Generator >::serialize(std::get< I >(value), out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 1, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            synchronous_generator_serial_traits< T, Generator >::deserialize(std::get< I >(value), in);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 1, Ts2... >::deserialize(value, in);
        }

    };

    // Bool Specializations (x8)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 8, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 8, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 8, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 8, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 8, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 8, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 8, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 8, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, bool, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 8, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 8, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, bool, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 8, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 8, Ts... >(value, bool_val);
        }
    };
    // Bool Specializations (x7)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 7, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 7, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 7, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 7, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 7, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 7, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 7, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 7, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 7, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 7, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 7, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 7, Ts... >(value, bool_val);
        }
    };

    // Bool Specializations (x6)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 6, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 6, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 6, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 6, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 6, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 6, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 6, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 6, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 6, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 6, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 6, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 6, Ts... >(value, bool_val);
        }
    };

    // Bool Specializations (x5)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 5, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 5, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 5, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 5, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 5, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 5, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 5, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 5, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 5, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 5, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 5, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 5, Ts... >(value, bool_val);
        }
    };

    // Bool Specializations (x4)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 4, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 4, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 4, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 4, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 4, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 4, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 4, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 4, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 4, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 4, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 4, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 4, Ts... >(value, bool_val);
        }
    };

    // Bool Specializations (x3)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 3, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 3, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 3, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 3, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 3, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 3, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 3, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 3, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 3, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 3, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 3, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 3, Ts... >(value, bool_val);
        }
    };

    // Bool Specializations (x2)

    template < typename... Ts, typename Iterator, std::size_t I, typename... Ts2 >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool, Ts2... >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 2, Ts... >(value);
            outit = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 2, Ts2... >::serialize(value, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 2, Ts... >(value, bool_val);
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I + 2, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename... Ts2 >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool, Ts2... >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 2, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 2, Ts2... >::serialize(value, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 2, Ts... >(value, bool_val);
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I + 2, Ts2... >::deserialize(value, in);
        }
    };

    template < typename... Ts, typename Iterator, std::size_t I >
    struct synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, I, bool, bool >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& value, Iterator outit) -> Iterator
        {
            std::uint8_t bool_val = tuple_get_bool_sequence< I, 2, Ts... >(value);
            return synchronous_iterator_serial_traits< std::uint8_t, Iterator >::serialize(bool_val, outit);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& value, Iterator in) -> Iterator
        {
            std::uint8_t bool_val = 0;
            in = synchronous_iterator_serial_traits< std::uint8_t, Iterator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 2, Ts... >(value, bool_val);
            return in;
        }
    };

    template < typename... Ts, typename Generator, std::size_t I >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, bool, bool >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            std::uint8_t bool_val = 0;
            bool_val = tuple_get_bool_sequence< I, 2, Ts... >(value);
            synchronous_generator_serial_traits< std::uint8_t, Generator >::serialize(bool_val, out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator in)
        {
            std::uint8_t bool_val = 0;
            synchronous_generator_serial_traits< std::uint8_t, Generator >::deserialize(bool_val, in);
            tuple_set_bool_sequence< I, 2, Ts... >(value, bool_val);
        }
    };

    template < typename... Ts, typename Generator, std::size_t I, typename T >
    struct synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, I, T >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& value, Generator out)
        {
            synchronous_generator_serial_traits< T, Generator >::serialize(std::get< I >(value), out);
        }

        static inline constexpr void deserialize(std::tuple< Ts... >& value, Generator out)
        {
            synchronous_generator_serial_traits< T, Generator >::deserialize(std::get< I >(value), out);
        }
    };

    template < typename Iterator, typename... Ts >
    struct synchronous_iterator_serial_traits< std::tuple< Ts... >, Iterator >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& val, Iterator outIterator) -> Iterator
        {
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::serialize(val, outIterator);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& val, Iterator inIterator) -> Iterator
        {
            return synchronous_iterator_tuple_serial_traits< std::tuple< Ts... >, Iterator, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::deserialize(val, inIterator);
        }
    };

    template < typename... Ts, typename Generator >
    struct synchronous_generator_serial_traits< std::tuple< Ts... >, Generator >
    {
        static inline constexpr auto serialize(std::tuple< Ts... > const& val, Generator g) -> void
        {
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::serialize(val, g);
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& val, Generator in) -> void
        {
            synchronous_generator_tuple_serial_traits< std::tuple< Ts... >, Generator, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::deserialize(val, in);
            return;
        }
    };

    template < typename T1, typename T2, typename Generator >
    struct synchronous_generator_serial_traits< std::pair< T1, T2 >, Generator >
    {
        static inline constexpr auto serialize(std::pair< T1, T2 > const& val, Generator g) -> void
        {
            synchronous_generator_tuple_serial_traits< std::tuple< T1 const &, T2 const & >, Generator, 0, std::remove_cv_t< std::remove_reference_t< T1 > >, std::remove_cv_t< std::remove_reference_t< T2 > > >::serialize(std::tie(val.first, val.second), g);
        }

        static inline constexpr auto deserialize(std::pair< T1, T2 >& val, Generator in) -> void
        {
            // TODO: Allow tuple lvalues
            auto lvalue = std::tie(val.first, val.second);
            synchronous_generator_tuple_serial_traits< std::tuple< T1 &, T2 & >, Generator, 0, std::remove_cv_t< std::remove_reference_t< T1 > >, std::remove_cv_t< std::remove_reference_t< T2 > > >::deserialize(lvalue, in);
            return;
        }
    };

    template < typename T1, typename T2, typename Iterator >
    struct synchronous_iterator_serial_traits< std::pair< T1, T2 >, Iterator >
    {
        static inline constexpr auto serialize(std::pair< T1, T2 > const& val, Iterator out) -> Iterator
        {
            return synchronous_iterator_tuple_serial_traits< std::tuple< T1 const &, T2 const & >, Iterator, 0, std::remove_cv_t< std::remove_reference_t< T1 > >, std::remove_cv_t< std::remove_reference_t< T2 > > >::serialize(std::tie(val.first, val.second), out);
        }

        static inline constexpr auto deserialize(std::pair< T1, T2 >& val, Iterator in) -> Iterator
        {
            // TODO: Make it accept rvalues for tuples of references
            auto lvalue = std::tie(val.first, val.second);
            return synchronous_iterator_tuple_serial_traits< std::tuple< T1 &, T2 & >, Iterator, 0, std::remove_cv_t< std::remove_reference_t< T1 > >, std::remove_cv_t< std::remove_reference_t< T2 > > >::deserialize(lvalue, in);
        }
    };


    template <>
    struct serial_traits< uintany >
    {
        // TODO: Make this specailize on each type of integer 
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        static inline constexpr std::size_t serial_size(std::uint64_t value)
        {
            if (value < 128ull)
                return 1;
            else if (value < 16512ull)
                return 2;
            else if (value < 2113664ull)
                return 3;
            else if (value < 270549120ull)
                return 4;
            else if (value < 34630287488ull)
                return 5;
            else if (value < 4432676798592ull)
                return 6;
            else if (value < 567382630219904ull)
                return 7;
            else if (value < 72624976668147840ull)
                return 8;
            else if (value < 9295997013522923648ull)
                return 9;
            else
                return 10; // 10 units can represent all 64-bit integers.
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< uintany, Iterator >
    {
        static inline Iterator serialize(uintmax_t in, Iterator out)
        {
            // This isn't the best function, it might give wrong results for really large values.
            // Iterator's good enough for now.
            // Also, optimize this, it's not very fast

            uintmax_t base = in;

            uintmax_t bytecount = 1;
            uintmax_t max = 0;

            max = (1ull << (7)) - 1;
            while (base > max)
            {
                bytecount++;
                base -= max + 1;
                max = (1ull << ((7) * bytecount)) - 1;
            }

            for (uintmax_t i = 0; i < bytecount; i++)
            {
                uint8_t val = base & ((uintmax_t(1) << (7)) - 1);
                if (i != bytecount - 1)
                {
                    val |= 0b10000000;
                }
                *out++ = val;
                base >>= 7;
            }
            return out;
        }

        static inline Iterator deserialize(uintmax_t& n, Iterator in)
        {
            // This function is slow, optimize it
            n = 0;
            uintmax_t n2 = 0;
            while (true)
            {
                uint8_t a = *in++;
                uint8_t read_value = a & 0b1111111;
                n += (uintmax_t(read_value) << (n2 * 7));
                if (!(a & 0b10000000))
                    break;
                n2++;
            }
            for (uintmax_t i = 1; i <= n2; i++)
            {
                n += (uintmax_t(1) << (i * 7));
            }
            return in;
        }
    };

    template < typename IteratorF >
    struct synchronous_generator_serial_traits< uintany, IteratorF >
    {
        template < typename Integral >
        static inline constexpr void deserialize(Integral& i, IteratorF generator)
        {
            static_assert(std::is_integral_v< Integral >);

            i = 0;
            Integral i2 = 0;
            while (true)
            {
                auto it = generator(1);
                uint8_t val = *it++;
                i2 <<= 7;
                i2 |= val & 0b1111111;
                if (val & 0b10000000)
                {
                    // There are additional bytes
                    i <<= 7;
                    // TODO: Check for overflow here and throw an exception if the
                    // variable length integer is too large to be encoded in the destination type

                    i |= 0b10000000;
                }
                else
                    break;
            }

            i += i2;
        }
    };

    template < typename T, typename A >
    struct serial_traits< std::vector< T, A > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }


        template <typename Vec>
        static inline constexpr std::size_t serial_size(Vec const& value)
        {
            if constexpr (serial_traits< T >::has_fixed_serial_size())
            {
                return serial_traits< uintany >::serial_size(value.size()) + serial_traits< T >::fixed_serial_size() * value.size();
            }
            else
            {
                std::size_t result = 0;
                result += serial_traits< uintany >::serial_size(value.size());
                for (auto const& x : value)
                {
                    result += serial_traits< T >::serial_size(x);
                }
                return result;
            }
        }
    };

    template < typename... Ts >
    struct serial_traits< std::tuple< Ts... > >
    {
        // TODO: Handle reference tuples

        static inline constexpr bool has_fixed_serial_size()
        {
            return tuple_serial_traits< std::tuple< Ts... >, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::has_fixed_serial_size();
            //return is_true_for_all< c_fixed_serial_size, Ts... >::value;
        }

        static constexpr std::size_t fixed_serial_size()
        {
            static_assert(has_fixed_serial_size(), "Type must have a fixed size to use fixed_serial_size()");
            return tuple_serial_traits< std::tuple< Ts... >, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::tuple<Ts...> const & tuple)
        {
            return tuple_serial_traits< std::tuple< Ts... >, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::serial_size(tuple);
        }

        static constexpr std::size_t serial_size2(std::tuple< Ts... > const& tuple)
        {   
            return tuple_serial_traits< std::tuple< Ts... >, 0, std::remove_cv_t< std::remove_reference_t< Ts > >... >::serial_size2(tuple, 0);
        }
    };

    template <typename First, typename Second>
    struct serial_traits< std::pair<First, Second> >
    {
        static constexpr bool has_fixed_serial_size()
        {
            return serial_traits< std::tuple< First, Second > >::has_fixed_serial_size();
        }

        static constexpr std::size_t fixed_serial_size()
        {
            return serial_traits< std::tuple< First, Second > >::fixed_serial_size();
        }

        static constexpr std::size_t serial_size(std::pair<First, Second> const & value)
        {
            return serial_traits< std::tuple< First const &, Second const & > >::serial_size( std::tie(value.first, value.second));
        }
    };

    
    template <typename K, typename V>
    struct map_serial_traits
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        
        template <typename Map>
        static inline constexpr std::size_t serial_size(Map const& value)
        {
            using value_type = typename Map::value_type;
            if constexpr (serial_traits< value_type >::has_fixed_serial_size())
            {
                return serial_traits< uintany >::serial_size(value.size()) + serial_traits< value_type >::fixed_serial_size() * value.size();
            }
            else
            {
                std::size_t result = 0;
                result += serial_traits< uintany >::serial_size(value.size());
                for (auto const& x : value)
                {
                    result += serial_traits< value_type >::serial_size(x);
                }
                return result;
            }
        }
    };

    template < typename T, typename K, typename Comparator, typename Allocator >
    struct serial_traits< std::map< T, K, Comparator, Allocator > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }

        static inline constexpr std::size_t serial_size(std::map< T, K, Comparator, Allocator > const& value)
        {
            return map_serial_traits<T, K>::serial_size(value);
        }
    };

    template < typename K, typename V, typename C,  typename A >
    struct serial_traits< std::multimap< K, V, C, A > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }

        static inline constexpr std::size_t serial_size(std::multimap< K, V, C, A > const& value)
        {
            return map_serial_traits<K, V>::serial_size(value);
        }
    };

    template < typename K, typename V, typename H, typename E, typename A >
    struct serial_traits< std::unordered_map< K, V, H, E, A > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }

        static inline constexpr std::size_t serial_size(std::unordered_map< K, V, H, E, A > const& value)
        {
            return map_serial_traits<K, V>::serial_size(value);
        }
    };
    

    template <>
    struct serial_traits< std::string >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        static inline std::size_t serial_size(std::string const& value)
        {
            return serial_traits< uintany >::serial_size(value.size()) + value.size();
        }
    };

    template < typename Generator >
    struct synchronous_generator_serial_traits< std::string, Generator >
    {
        static inline constexpr auto serialize(std::string const& val, Generator g)
        {
            auto it = g(serial_traits< std::string >::serial_size(val));
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
            std::copy(val.begin(), val.end(), it);
        }

        static inline constexpr auto deserialize(std::string& val, Generator g)
        {
            std::size_t count = 0;
            synchronous_generator_serial_traits< uintany, Generator >::deserialize(count, g);
            auto it = g(count);
            val.resize(count);
            std::copy_n(it, count, val.begin());
        }
    };

    template < typename T, typename Alloc, typename Iterator >
    struct synchronous_iterator_serial_traits< std::vector< T, Alloc >, Iterator >
    {
        //    static_assert(false, "debug message");
        template <typename Vec>
        static inline constexpr auto serialize(Vec const& val, Iterator it) -> Iterator
        {
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
            for (auto const& x : val)
            {
                it = synchronous_iterator_serial_traits< T, decltype(it) >::serialize(x, it);
            }
            return it;
        }

        template <typename Vec>
        static inline constexpr auto deserialize(Vec & value, Iterator it) -> Iterator
        {
            // TODO: Optimize
            value.clear();
            std::size_t size = 0;
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::deserialize(size, it);
            for (std::size_t i = 0; i != size; i++)
            {
                typename Vec::value_type t;
                it = synchronous_iterator_serial_traits< T, decltype(it) >::deserialize(t, it);
                value.push_back(std::move(t));
            }
        }
    };

    template < typename Iterator >
    struct synchronous_iterator_serial_traits< std::string, Iterator >
    {
        //    static_assert(false, "debug message");
        static inline constexpr auto serialize(std::string const& val, Iterator it) -> Iterator
        {
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
            return std::copy(val.cbegin(), val.cend(), it);
        }

        static inline constexpr auto deserialize(std::string& value, Iterator it) -> Iterator
        {
            value.clear();
            std::size_t size = 0;
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(size, it);
            // TODO: This is not efficient with all types of iterators.
            // May not even be legal in some cases.
            value.reserve(size);
            std::copy_n(it, size, std::back_inserter(value));
            std::advance(it, size);
            return it;
        }
    };

    template < typename T, typename Alloc, typename Generator >
    struct synchronous_generator_serial_traits< std::vector< T, Alloc >, Generator >
    {
        template <typename Vec>
        static inline constexpr auto serialize(Vec const& val, Generator g)
        {
            if constexpr (serial_traits< T >::has_fixed_serial_size())
            {
                auto it = g(serial_traits< Vec >::serial_size(val));
                it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
                for (auto const& x : val)
                {
                    it = synchronous_iterator_serial_traits< T, decltype(it) >::serialize(x, it);
                }
            }
            else
            {
                auto it = g(serial_traits< uintany >::serial_size(val.size()));
                it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
                for (auto const& x : val)
                {
                    synchronous_generator_serial_traits< T, Generator >::serialize(x, g);
                }
            }
        }

        template <typename Vec>
        static inline constexpr auto deserialize( Vec& val, Generator g)
        {
            val.clear();
            if constexpr (serial_traits< T >::has_fixed_serial_size())
            {
                std::size_t sz = 0;
                synchronous_generator_serial_traits< uintany, Generator >::deserialize(sz, g);
                std::size_t total_size = sz * serial_traits< T >::fixed_serial_size();

                auto it = g(total_size);
                for (std::size_t i = 0; i != sz; i++)
                {
                    typename Vec::value_type t;
                    it = synchronous_iterator_serial_traits< T, decltype(it) >::deserialize(t, it);
                    val.emplace_back(std::move(t));
                }

                return;
            }
            else
            {
                std::size_t sz = 0;
                synchronous_generator_serial_traits< uintany, Generator >::deserialize(sz, g);

                for (std::size_t i = 0; i != sz; i++)
                {
                    typename Vec::value_type t;
                    synchronous_generator_serial_traits< T, Generator >::deserialize(t, g);
                    val.emplace_back(std::move(t));
                }

                return;
            }
        }
    };

    template < typename K, typename V, typename Alloc, typename Generator >
    struct synchronous_generator_serial_traits<std::map<K, V, Alloc>, Generator>
    {
        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::serialize(val, g);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::deserialize(val, g);
        }
    };

    template < typename K, typename V, typename Alloc, typename Generator >
    struct synchronous_generator_serial_traits<std::multimap<K, V, Alloc>, Generator>
    {
        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::serialize(val, g);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::deserialize(val, g);
        }
    };

    template < typename K, typename V, typename Alloc, typename Generator >
    struct synchronous_generator_serial_traits<std::unordered_map<K, V, Alloc>, Generator>
    {
        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::serialize(val, g);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::deserialize(val, g);
        }
    };

    template < typename K, typename V, typename Alloc, typename Generator >
    struct synchronous_generator_serial_traits<std::unordered_multimap<K, V, Alloc>, Generator>
    {
        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::serialize(val, g);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Generator g)
        {
            synchronous_generator_map_serial_traits<K, V, Generator>::deserialize(val, g);
        }
    };

    template < typename K, typename V, typename Generator >
    struct synchronous_generator_map_serial_traits
    {
        using T = typename std::pair<K, V>;

        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Generator g)
        {
            if constexpr (serial_traits< typename Map::value_type >::has_fixed_serial_size())
            {
                auto it = g(serial_traits< typename Map::value_type >::serial_size(val));
                it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
                for (auto const& x : val)
                {
                    it = synchronous_iterator_serial_traits< T, decltype(it) >::serialize(x, it);
                }
            }
            else
            {
                auto it = g(serial_traits< uintany >::serial_size(val.size()));
                it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
                for (auto const& x : val)
                {
                    synchronous_generator_serial_traits< T, Generator >::serialize(x, g);
                }
            }
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Generator g)
        {
            val.clear();
            if constexpr (serial_traits< typename Map::value_type >::has_fixed_serial_size())
            {
                std::size_t sz = 0;
                synchronous_generator_serial_traits< uintany, Generator >::deserialize(sz, g);
                std::size_t total_size = sz * serial_traits< T >::fixed_serial_size();

                auto it = g(total_size);
                for (std::size_t i = 0; i != sz; i++)
                {
                    T t;
                    it = synchronous_iterator_serial_traits< T, decltype(it) >::deserialize(t, it);
                    val.insert(std::move(t));
                }

                return;
            }
            else
            {
                std::size_t sz = 0;
                synchronous_generator_serial_traits< uintany, Generator >::deserialize(sz, g);

                for (std::size_t i = 0; i != sz; i++)
                {
                    std::pair<K, V> t;
                    synchronous_generator_serial_traits< std::pair<K, V>, Generator >::deserialize(t, g);
                    val.insert(std::move(t));
                }

                return;
            }
        }
    };

    template <typename K, typename V, typename Iterator>
    struct synchronous_iterator_map_serial_traits
    {
        using T = typename std::pair<K, V>;

        template <typename Map>
        static inline constexpr auto serialize(Map const & val, Iterator out) -> Iterator
        {
            out = synchronous_iterator_serial_traits< uintany, Iterator >::serialize(val.size(), out);
            for (auto const& x : val)
            {
                out = synchronous_iterator_serial_traits< T, Iterator >::serialize(x, out);
            }
            return out;
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Iterator in) -> Iterator
        {
            val.clear();
            std::size_t sz = 0;
            in = synchronous_iterator_serial_traits< uintany, Iterator >::deserialize(sz, in);
            std::size_t total_size = sz * serial_traits< T >::fixed_serial_size();
            
            for (std::size_t i = 0; i != sz; i++)
            {
                T t;
                in = synchronous_iterator_serial_traits< T, Iterator >::deserialize(t, in);
                val.emplace_back(std::move(t));
            }

            return in;
        }
    };

    template < typename K, typename V, typename Alloc, typename Iterator >
    struct synchronous_iterator_serial_traits< std::map< K, V, Alloc >, Iterator >
    {
        using T = typename std::pair<K, V>;

        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Iterator out) -> Iterator
        {
            return synchronous_iterator_map_serial_traits<K, V, Iterator>::serialize(val, out);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Iterator in) -> Iterator
        {
            return synchronous_iterator_map_serial_traits<K, V, Iterator>::deserialize(val, in);
        }
    };


    template < typename K, typename V, typename Alloc, typename Iterator >
    struct synchronous_iterator_serial_traits< std::multimap< K, V, Alloc >, Iterator >
    {
        using T = typename std::pair<K, V>;

        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Iterator out) -> Iterator
        {
            return synchronous_iterator_map_serial_traits<K, V, Iterator>::serialize(val, out);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Iterator in) -> Iterator
        {
            return synchronous_iterator_map_serial_traits<K, V, Iterator>::deserialize(val, in);
        }
    };

    template < typename K, typename V, typename Alloc, typename Iterator >
    struct synchronous_iterator_serial_traits< std::unordered_multimap< K, V, Alloc >, Iterator >
    {
        using T = typename std::pair<K, V>;

        template <typename Map>
        static inline constexpr auto serialize(Map const& val, Iterator out) -> Iterator
        {
            return synchronous_iterator_map_serial_traits<K, V, Iterator>::serialize(val, out);
        }

        template <typename Map>
        static inline constexpr auto deserialize(Map & val, Iterator in) -> Iterator
        {
            return synchronous_iterator_map_serial_traits<K, V, Iterator>::deserialize(val, in);
        }
    };

    


    template < typename T, typename IteratorF >
    inline void quick_generator_serialize(T const& t, IteratorF f)
    {
        synchronous_generator_serial_traits< T, IteratorF >::serialize(t, f);
    }

    template < typename T, typename Iterator >
    inline auto quick_iterator_serialize(T const& t, Iterator i) -> Iterator
    {
        return synchronous_iterator_serial_traits< T, Iterator >::serialize(t, i);
    }

    template < typename T, typename IteratorF >
    inline void quick_generator_deserialize(T& t, IteratorF f)
    {
        synchronous_generator_serial_traits< T, IteratorF >::deserialize(t, f);
    }

    template < typename T, typename Iterator >
    inline auto quick_iterator_deserialize(T& t, Iterator i) -> Iterator
    {
        return synchronous_iterator_serial_traits< T, Iterator >::deserialize(t, i);
    }

    template < typename T >
    inline std::size_t get_serial_size(T const& t)
    {
        return serial_traits< T >::serial_size(t);
    }

    template < typename T >
    struct c_fixed_serial_size
    {
        static const constexpr bool value = serial_traits< T >::has_fixed_serial_size();
    };

    static_assert(serial_traits< std::tuple< std::int32_t, std::int32_t > >::has_fixed_serial_size() == true);
    static_assert(serial_traits< std::tuple< std::int32_t, std::string > >::has_fixed_serial_size() == false);
    static_assert(serial_traits< std::tuple< std::int32_t, std::int32_t > >::fixed_serial_size() == 8);
    static_assert(serial_traits< std::tuple< bool, bool, bool > >::fixed_serial_size() == 1);
    static_assert(serial_traits< std::tuple< bool, bool, std::uint8_t, bool, bool > >::fixed_serial_size() == 3);
    static_assert(serial_traits< std::tuple< bool, bool, bool, bool, std::uint8_t > >::fixed_serial_size() == 2);
    static_assert(serial_traits< std::tuple< bool, bool, bool, bool, bool, bool, bool, bool, std::uint8_t > >::fixed_serial_size() == 2);
    static_assert(serial_traits< std::tuple< bool, bool, bool, bool, bool, bool, bool, bool, bool, std::uint8_t > >::fixed_serial_size() == 3);
    //static_assert(serial_traits< std::tuple< std::bitset<4>, std::bitset<4> >::fixed_serial_size() == 1);


} // namespace rpnx

#endif