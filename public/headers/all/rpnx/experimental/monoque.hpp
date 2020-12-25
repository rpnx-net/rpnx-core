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
#include <climits>
#include <limits.h>

#include "rpnx/assert.hpp"

namespace rpnx
{
    namespace experimental
    {
        // Probably will reimplement this later more efficiently.
        inline namespace monoque_abi_v1
        {
            template < typename T, typename Alloc = std::allocator< T > >
            class monoque : private Alloc
            {
              public:
                using value_type = T;
                using allocator_type = Alloc;
                using const_reference = typename allocator_type::const_reference;
                using pointer = typename allocator_type::pointer;
                using const_pointer = typename allocator_type::const_pointer;
                using size_type = typename allocator_type::size_type;
                using reference = typename allocator_type::reference;

                class const_iterator;
                class iterator
                {
                    friend class monoque< T, Alloc >;
                    friend class const_iterator;

                  private:
                    monoque< T, Alloc >* m_at;
                    std::size_t m_index;

                  public:
                    iterator() : m_at(nullptr), m_index(0)
                    {
                    }

                    iterator(iterator const&) = default;

                    value_type& operator*() const
                    {
                        RPNX_ASSERT(m_at != nullptr);
                        RPNX_ASSERT(m_at->m_block_list[index1(m_index)] != nullptr);
                        T* first_pointer = reinterpret_cast< value_type* >(m_at->m_block_list[index1(m_index)]);
                        RPNX_ASSERT(first_pointer != nullptr);
                        return std::launder(first_pointer)[index2(m_index)];
                    }

                    iterator operator+(std::size_t n) const
                    {
                        iterator v_copy = *this;
                        v_copy.m_index += n;
                        return v_copy;
                    }
                };

              private:
                std::size_t m_size = 0;
                std::size_t m_allocated_blocks = 0;
                std::size_t m_capacity_blocks = 0;
                void** m_block_list = nullptr;

                static inline std::size_t index1(std::size_t at)
                {
                    if (at <= 1)
                        return 0;
                    std::size_t a = sizeof(at) * CHAR_BIT - countl_zero(at);
                    return a - 1;
                }

                static inline std::size_t index2(std::size_t at)
                {
                    std::size_t mask = (bit_floor(at) - 1) | 1;
                    return at & mask;
                }

                static std::size_t size_of_primary_block(std::size_t index)
                {
                    if (index == 0)
                        return 2;
                    return std::size_t(1) << index;
                }

                void add_block()
                {
                    if (m_allocated_blocks == m_capacity_blocks)
                    {
                        if (m_allocated_blocks == 0)
                        {
                            void* v_storage_block_list = (typename std::allocator_traits< Alloc >::template rebind_alloc< void* >(get_allocator())).allocate(sizeof(void*) * 8);
                            void** v_blocks_list = new (v_storage_block_list) void*[8];
                            for (int i = 0; i < m_capacity_blocks; i++)
                            {
                                v_blocks_list[i] = nullptr;
                            }
                            m_block_list = v_blocks_list;
                            m_capacity_blocks = 8;
                        }
                        else
                        {
                            void* v_storage_block_list = (typename std::allocator_traits< Alloc >::template rebind_alloc< void* >(get_allocator())).allocate(sizeof(void*) * 8);
                            void** v_blocks_list = new (v_storage_block_list) void*[sizeof(std::size_t) * CHAR_BIT];

                            for (int i = 0; i < m_capacity_blocks; i++)
                            {
                                v_blocks_list[i] = m_block_list[i];
                            }

                            for (int i = m_capacity_blocks; i < sizeof(std::size_t) * CHAR_BIT; i++)
                            {
                                v_blocks_list[i] = nullptr;
                            }

                            std::swap(m_block_list, v_blocks_list);
                            (typename std::allocator_traits< Alloc >::template rebind_alloc< void* >(get_allocator()))
                                .deallocate(reinterpret_cast< void** >(v_blocks_list), m_capacity_blocks * sizeof(void*));

                            m_capacity_blocks = sizeof(std::size_t) * CHAR_BIT;
                        }
                    }

                    std::size_t index_of_secondary_block_to_create = m_allocated_blocks;
                    void* v_allocated_block_storage =
                        (typename std::allocator_traits< Alloc >::template rebind_alloc< void* >(get_allocator())).allocate(sizeof(T) * size_of_primary_block(index_of_secondary_block_to_create));
                    m_block_list[index_of_secondary_block_to_create] = v_allocated_block_storage;
                    m_allocated_blocks++;
                }

              public:
                monoque()
                {
                }

                monoque(Alloc const& ac) : Alloc(ac)
                {
                }

                ~monoque()
                {
                    // TODO: implement this
                    return;
                    while (size())
                    {
                        pop_back();
                    }
                }

                void pop_back()
                {
                    // TODO
                }

                value_type& operator[](std::size_t n)
                {
                    return *(begin() + n);
                }

                template < typename... Ts >
                void emplace_back(Ts&&... ts)
                {
                    if (size() == capacity())
                    {
                        add_block();
                    }
                    void* v_storage_block = m_block_list[index1(size())];
                    void* v_storage_location = (void*)((T*)v_storage_block + index2(size()));
                    new (v_storage_location) T(std::forward< Ts >(ts)...);
                    m_size++;
                    // TODO: Catch error and deallocate storage
                }

                /*
                value_type const & operator [](std::size_t n) const
                {
                    return *(cbegin()+n);
                }
                */

                inline iterator begin()
                {
                    iterator v_it;
                    v_it.m_at = this;
                    v_it.m_index = 0;
                    return v_it;
                }

                inline iterator end()
                {
                    iterator v_it;
                    v_it.m_at = this;
                    v_it.m_index = size();
                    return v_it;
                }

                std::size_t size() const
                {
                    return m_size;
                }

                std::size_t capacity() const
                {
                    // 0, [2, 4, 8, 16, 32, 64, 128, 256], 512, 1024 ...
                    if (m_allocated_blocks == 0)
                        return 0;
                    else
                        return std::size_t(1) << m_allocated_blocks;
                }

                Alloc get_allocator() const
                {
                    return static_cast< Alloc const& >(*this);
                }
            };
        }
    }
}

#endif // RPNXCORE_MONOQUE_HPP
