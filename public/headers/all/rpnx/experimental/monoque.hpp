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
                T** m_block_list = nullptr;

                template <typename T2>
                auto get_rebound_allocator() const
                {
                    return typename std::allocator_traits< Alloc >::template rebind_alloc< T2 >(get_allocator());
                }

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

                static std::size_t size_of_block(std::size_t index)
                {
                    if (index == 0)
                        return 2;
                    return std::size_t(1) << index;
                }

                void expand_block_list()
                {
                    auto storage_block_list_allocator = get_rebound_allocator<T*>();

                    if (m_allocated_blocks == 0)
                    {

                        T** v_block_list = storage_block_list_allocator.allocate(8);
                        m_capacity_blocks = 8;
                        for (int i = 0; i < m_capacity_blocks; i++)
                        {
                            std::allocator_traits<decltype(storage_block_list_allocator)>::construct(storage_block_list_allocator, v_block_list + i, nullptr);
                            v_block_list[i] = std::launder(v_block_list[i]);
                        }

                        m_block_list = std::launder(v_block_list);
                        m_capacity_blocks = 8;
                    }
                    else
                    {
                        T** v_block_list = storage_block_list_allocator.allocate(sizeof(std::size_t) * CHAR_BIT);

                        for (int i = 0; i < m_capacity_blocks; i++)
                        {
                            std::allocator_traits<decltype(storage_block_list_allocator)>::construct(storage_block_list_allocator, v_block_list + i, m_block_list[i]);
                            v_block_list[i] = std::launder(v_block_list[i]);
                        }

                        for (int i = m_capacity_blocks; i < sizeof(std::size_t) * CHAR_BIT; i++)
                        {
                            std::allocator_traits<decltype(storage_block_list_allocator)>::construct(storage_block_list_allocator, v_block_list + i, nullptr);
                            v_block_list[i] = std::launder(v_block_list[i]);
                        }

                        std::swap(m_block_list, v_block_list);


                        for (int i = 0; i < m_capacity_blocks; i++)
                        {
                            std::allocator_traits<decltype(storage_block_list_allocator)>::destroy(storage_block_list_allocator, v_block_list+i);
                        }

                        storage_block_list_allocator.deallocate(v_block_list, m_capacity_blocks);

                        m_capacity_blocks = sizeof(std::size_t) * CHAR_BIT;
                    }
                }

                void add_block()
                {
                    RPNX_ASSERT(m_allocated_blocks < sizeof(std::size_t) * CHAR_BIT);

                    auto storage_block_allocator = get_rebound_allocator<T>();

                    if (m_allocated_blocks == m_capacity_blocks)
                    {
                        expand_block_list();
                    }

                    std::size_t index_of_block_to_create = m_allocated_blocks;
                    m_block_list[index_of_block_to_create] = storage_block_allocator.allocate(size_of_block(index_of_block_to_create));
                    m_allocated_blocks++;
                }

                void shrink_block_list()
                {

                    auto storage_block_list_allocator = get_rebound_allocator<T*>();

                    if (m_allocated_blocks == 0 && m_capacity_blocks != 0)
                    {
                        RPNX_ASSERT(m_capacity_blocks == 8 || m_capacity_blocks == sizeof(std::size_t)*CHAR_BIT);
                        for (int i = 0; i < m_allocated_blocks; i++)
                        {
                            std::allocator_traits<decltype(storage_block_list_allocator)>::destroy(storage_block_list_allocator, m_block_list+i);
                        }
                        storage_block_list_allocator.deallocate(m_block_list, m_capacity_blocks);
                    }

                }

                // Deallocates the top storage block
                // Does not deconstruct any elements
                void remove_block()
                {
                    RPNX_ASSERT(m_allocated_blocks != 0);

                    auto storage_block_allocator = get_rebound_allocator<T>();
                    storage_block_allocator.deallocate(m_block_list[m_allocated_blocks-1],size_of_block(m_allocated_blocks - 1));
                    m_block_list[m_allocated_blocks-1] = nullptr;
                    m_allocated_blocks--;
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
                    while (size())
                    {
                        pop_back();
                    }
                    shrink_to_fit();
                }



                void shrink_to_fit()
                {
                    while (size() < std::numeric_limits<std::size_t>::max() / 2 && ((size() * 2 <= capacity() && size() >=2) || (size()*2 < capacity())))
                    {
                        remove_block();
                    }

                    shrink_block_list();
                }

                void pop_back()
                {
                    auto storage_block_allocator = get_rebound_allocator<T>();
                    T* v_storage_block = m_block_list[index1(size()-1)];
                    T* v_object_to_destroy = std::launder(v_storage_block + index2(size()-1));

                    m_size--;
                    std::allocator_traits<decltype(storage_block_allocator)>::destroy(storage_block_allocator, v_object_to_destroy);

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
