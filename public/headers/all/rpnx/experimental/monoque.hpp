/*
Monoque Data Structure

Copyright (c) 2017, 2018, 2020 Ryan P. Nicholl <rnicholl@protonmail.com> http://rpnx.net/
 -- Please let me know if you find this structure useful, thanks!

All rights reserved.

See rpnx-core/LICENSE.txt
*/

#ifndef RPNXCORE_MONOQUE_HPP
#define RPNXCORE_MONOQUE_HPP

#include <vector>
#include <memory>

namespace rpnx
{
    // Probably will reimplement this later more efficiently.
    inline namespace monoque_abi_v1
    {
        template < typename T, typename Alloc = std::allocator< T > >
        class monoque
        : private Alloc
        {
          private:
            std::size_t m_size;
            std::size_t m_capacity1;
            std::size_t m_capacity2;
            T** m_block_list;

            static inline std::size_t index1(std::size_t at)
            {
                if (at <= 1) return 0;
                std::size_t a = sizeof(at)*CHAR_BIT - countl_zero(at);
                return a-1;
            }

            static inline std::size_t index2(std::size_t at)
            {
                std::size_t mask = (bit_floor(at) -1) | 1;
                return at & mask;
            }
          public:
            monoque()
            {

            }

            monoque(Alloc const & ac)
                : Alloc(ac)
            {}


            class const_iterator;
            class iterator
            {
                friend class monoque<T, Alloc>;
                friend class const_iterator;
              private:
                monoque<T, Alloc> * m_at;
                std::size_t m_index;
              public:
                iterator()
                :m_at(nullptr), m_index(0)
                {}

                iterator(iterator const &) = default;

                value_type & operator *() const
                {
                    return m_at->m_block_list1[index1(m_index)][index2(m_index)];
                }

                iterator operator+(std::size_t n) const
                {
                    iterator v_copy = *this;
                    v_copy.m_index += n;
                    return v_copy;
                }
            };

            Alloc get_allocator() const
            {
                return static_cast<Alloc&>(*this);
            }
        };
    }
}

#endif // RPNXCORE_MONOQUE_HPP
