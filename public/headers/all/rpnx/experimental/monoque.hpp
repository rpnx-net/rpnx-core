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
#include <limits>
#include <iterator>
#include <climits>

#include "rpnx/assert.hpp"

namespace rpnx
{
    namespace experimental
    {
        // Probably will reimplement this later more efficiently.
        inline namespace monoque_abi_v1
        {
            template <typename T, typename Alloc>
            class monoque_iterator;

            template <typename T, typename Alloc>
            class monoque_const_iterator;

            template < typename T, typename Alloc = std::allocator< T > >
            class monoque : private Alloc
            {
                friend class monoque_iterator< T, Alloc >;
                friend class monoque_const_iterator< T, Alloc >;

                static_assert(noexcept(Alloc()));

                static_assert(std::is_same_v<typename std::allocator_traits<Alloc>::size_type, std::size_t>, "Not implemented");
                static_assert(std::is_same_v<typename std::allocator_traits<Alloc>::pointer, T*>, "Not implemented");
                static_assert(std::is_same_v<typename std::allocator_traits<Alloc>::difference_type, std::ptrdiff_t>, "Not implemented");

              public:
                using value_type = T;
                using allocator_type = Alloc;
                using const_reference = typename allocator_type::const_reference;
                using pointer = typename allocator_type::pointer;
                using const_pointer = typename allocator_type::const_pointer;
                using size_type = typename allocator_type::size_type;
                using reference = typename allocator_type::reference;

                using iterator = monoque_iterator<T, Alloc>;
                using const_iterator = monoque_const_iterator<T, Alloc>;

              private:
                std::size_t m_size = 0;
                std::size_t m_allocated_blocks = 0;
                std::size_t m_capacity_blocks = 0;
                T** m_block_list = nullptr;

                template <typename T2>
                auto get_rebound_allocator() const noexcept
                {
                    static_assert(noexcept(typename std::allocator_traits< Alloc >::template rebind_alloc< T2 >(get_allocator())));
                    return typename std::allocator_traits< Alloc >::template rebind_alloc< T2 >(get_allocator());
                }

                static inline std::size_t index1(std::size_t at)
                {
                    if (at <= 1)
                        return 0;
                    auto v1 = sizeof(at) * CHAR_BIT;
                    auto v2 = countl_zero(at);
                    std::size_t a = v1 - v2;
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
                void remove_block() noexcept
                {
                    RPNX_ASSERT(m_allocated_blocks != 0);

                    auto storage_block_allocator = get_rebound_allocator<T>();
                    storage_block_allocator.deallocate(m_block_list[m_allocated_blocks-1],size_of_block(m_allocated_blocks - 1));
                    m_block_list[m_allocated_blocks-1] = nullptr;
                    m_allocated_blocks--;
                }

              public:
                monoque() noexcept(noexcept(Alloc()))
                {
                }

                explicit monoque(Alloc const& ac) noexcept(noexcept(Alloc())) : Alloc(ac)
                {
                }

                // TODO explicit monoque( size_type count, const T & value, Alloc const & alloc = Allocator());

                // TODO explicit monoque( size_type count, const Alloc & alloc = Alloc())

                // TODO template <typename It> monoque(It begin, It end, const Alloc & alloc = Alloc())

                // TODO monoque(std::initializer_list<T> ilist)

                // TODO clear
                // todo insert
                // TODO erase

                ~monoque()
                {
                    while (size())
                    {
                        pop_back();
                    }
                    shrink_to_fit();
                }

                monoque(monoque<T, Alloc> const & other)
                    : Alloc(other.get_allocator())
                {
                    for (auto const & x : other)
                        emplace_back(x);
                }

                monoque(monoque<T, Alloc> && other)
                : Alloc(other.get_allocator())
                {
                    swap(*this, other);
                }

                // TODO monoque<A, Alloc> & operator=(monoque const & other);
                // TODO monoque<T, Alloc> & operator=(monoque && other);
                // TODO monoque<T, Alloc> & operator=(std::initializer_list<T> ilist);

                value_type & front()
                {
                    return *begin();
                }

                value_type const & front() const
                {
                    return *cbegin();
                }

                value_type & back()
                {
                    return *(begin()+(size()-1));
                }


                value_type const & back() const
                {
                    return *(cbegin()+(size()-1));
                }


                void shrink_to_fit()
                {
                    while (size() < std::numeric_limits<std::size_t>::max() / 2 && ((size() * 2 <= capacity() && size() >=2) || (size()*2 < capacity())))
                    {
                        remove_block();
                    }

                    shrink_block_list();
                }

                bool empty() const noexcept
                {
                    return size() == 0;
                }

                void pop_back() noexcept
                {
                    RPNX_ASSERT(size() != 0);
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
                    RPNX_ASSERT(capacity() > size());
                    RPNX_ASSERT(index1(2) == 1);
                    auto i1 = index1(size());
                    auto i2 = index2(size());
                    T* v_storage_block = m_block_list[i1];
                    T* v_object_location = v_storage_block + i2;
                    new (v_object_location) T(std::forward< Ts >(ts)...);
                    m_size++;
                    // TODO: Catch error and deallocate storage
                }

                inline iterator begin() noexcept
                {
                    iterator v_it;
                    v_it.m_which = this;
                    v_it.m_index = 0;
                    return v_it;
                }

                inline iterator end() noexcept
                {
                    iterator v_it;
                    v_it.m_which = this;
                    v_it.m_index = size();
                    return v_it;
                }

                inline const_iterator begin() const noexcept
                {
                    return cbegin();
                }

                inline const_iterator end() const noexcept
                {
                    return cend();
                }

                inline const_iterator cbegin() const noexcept
                {
                    const_iterator v_it;
                    v_it.m_which = this;
                    v_it.m_index = 0;
                    return v_it;
                }

                inline const_iterator cend() const noexcept
                {
                    const_iterator v_it;
                    v_it.m_which = this;
                    v_it.m_index = size();
                    return v_it;
                }

                std::size_t size() const noexcept
                {
                    return m_size;
                }

                void reserve(std::size_t n)
                {
                    while (n > capacity())
                    {
                        add_block();
                    }
                }

                void resize(std::size_t n)
                {
                    while (n < size())
                    {
                        emplace_back();
                    }
                    while (n > size())
                    {
                        pop_back();
                    }
                }

                void clear() noexcept
                {
                    resize(0);
                }

                std::size_t capacity() const noexcept
                {
                    // 0, [2, 4, 8, 16, 32, 64, 128, 256], 512, 1024 ...
                    if (m_allocated_blocks == 0)
                        return 0;
                    else
                        return std::size_t(1) << m_allocated_blocks;
                }

                Alloc get_allocator() const noexcept
                {
                    return static_cast< Alloc const& >(*this);
                }

                void swap(monoque<T, Alloc> & other) noexcept
                {
                    std::swap(m_block_list, other.m_block_list);
                    std::swap(m_capacity_blocks, other.m_capacity_blocks);
                    std::swap(m_allocated_blocks, other.m_allocated_blocks);
                    std::swap(m_size, other.m_size);
                }

            };

            template <typename T, typename Alloc>
            class monoque_iterator
            {
                friend class monoque< T, Alloc >;
                friend class monoque_const_iterator<T, Alloc>;
              public:
                using value_type = typename monoque<T, Alloc>::value_type;
                typedef std::ptrdiff_t difference_type;
                using pointer  = value_type *;
                using reference = value_type&;
                using iterator_category = std::random_access_iterator_tag ;
              private:
                monoque< T, Alloc >* m_which;
                std::size_t m_index;

              public:
                monoque_iterator() noexcept
                    : m_which(nullptr), m_index(0)
                {
                }

                monoque_iterator(monoque_iterator<T, Alloc> const&) = default;

                monoque_iterator<T, Alloc>& operator =(monoque_iterator<T, Alloc> const & other) noexcept
                {
                    m_which = other.m_which;
                    m_index = other.m_index;
                    return *this;
                }

                value_type& operator*() const
                {
                    RPNX_ASSERT(m_which != nullptr);
                    RPNX_ASSERT(m_index < m_which->size());
                    RPNX_ASSERT(m_which->m_block_list != nullptr);
                    auto index1 = monoque<T, Alloc>::index1(m_index);
                    auto index2 = monoque<T, Alloc>::index2(m_index);

                    T* first_pointer = m_which->m_block_list[index1];
                    RPNX_ASSERT(first_pointer != nullptr);
                    return std::launder(first_pointer)[index2];
                }



                inline monoque_iterator<T, Alloc> & operator +=(difference_type n) noexcept
                {
                    m_index += n;
                    return *this;
                }

                inline monoque_iterator<T, Alloc> & operator -=(difference_type n) noexcept
                {
                    m_index += n;
                    return *this;
                }



                inline monoque_iterator<T, Alloc> operator+(difference_type n) const noexcept
                {
                    monoque_iterator<T, Alloc> v_copy = *this;
                    v_copy.m_index += n;
                    return v_copy;
                }

                inline monoque_iterator<T, Alloc> operator-(difference_type n) const noexcept
                {
                    monoque_iterator<T, Alloc> v_copy = *this;
                    v_copy.m_index -= n;
                    return v_copy;
                }

                inline bool operator!=(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index != other.m_index;
                }

                inline bool operator==(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index == other.m_index;
                }


                inline bool operator<(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index < other.m_index;
                }

                inline bool operator<=(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index <= other.m_index;
                }

                inline bool operator>=(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index >= other.m_index;
                }

                inline bool operator>(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index > other.m_index;
                }

                inline monoque_iterator<T, Alloc> operator++(int) noexcept
                {
                    monoque_iterator<T, Alloc> copy = *this;
                    m_index++;
                    return copy;
                }


                inline monoque_iterator<T, Alloc>& operator++() noexcept
                {
                    m_index++;
                    return *this;
                }

                inline monoque_iterator<T, Alloc> operator--(int) noexcept
                {
                    monoque_iterator<T, Alloc> copy = *this;
                    m_index--;
                    return copy;
                }


                inline monoque_iterator<T, Alloc>& operator--() noexcept
                {
                    m_index--;
                    return *this;
                }

                inline difference_type operator-(monoque_iterator<T, Alloc> const & other) const noexcept
                {
                    return m_index - other.m_index;
                }

                inline value_type & operator[](difference_type dif) const noexcept
                {
                    return *(*this + dif);
                }

            };


            template <typename T, typename Alloc>
            class monoque_const_iterator
            {
                friend class monoque< T, Alloc >;
              public:
                using value_type = typename monoque<T, Alloc>::value_type;
                typedef std::ptrdiff_t difference_type;
                using pointer  = value_type *;
                using reference = value_type&;
                using iterator_category = std::random_access_iterator_tag ;
              private:
                monoque< T, Alloc > const* m_which;
                std::size_t m_index;

              public:
                inline monoque_const_iterator() : m_which(nullptr), m_index(0)
                {
                }

                monoque_const_iterator(monoque_const_iterator<T, Alloc> const&) = default;

                inline monoque_const_iterator(monoque_iterator<T, Alloc> const & other)
                    : m_index(other.m_index), m_which(other.m_which)
                {
                }

                inline monoque_const_iterator<T, Alloc>& operator =(monoque_const_iterator<T, Alloc> const & other)
                {
                    m_which = other.m_which;
                    m_index = other.m_index;
                    return *this;
                }

                inline value_type& operator*() const
                {
                    RPNX_ASSERT(m_which != nullptr);
                    RPNX_ASSERT(m_index < m_which->size());
                    auto index1 = monoque<T, Alloc>::index1(m_index);
                    auto index2 = monoque<T, Alloc>::index2(m_index);
                    RPNX_ASSERT((m_which->m_block_list[monoque<T, Alloc>::index1(m_index)] != nullptr));
                    T* first_pointer = m_which->m_block_list[index1];
                    RPNX_ASSERT(first_pointer != nullptr);
                    return std::launder(first_pointer)[index2];
                }



                inline monoque_const_iterator<T, Alloc> & operator +=(difference_type n)
                {
                    m_index += n;
                    return *this;
                }

                inline monoque_const_iterator<T, Alloc> & operator -=(difference_type n)
                {
                    m_index += n;
                    return *this;
                }

                inline monoque_const_iterator<T, Alloc> operator+(difference_type n) const
                {
                    monoque_const_iterator<T, Alloc> v_copy = *this;
                    v_copy.m_index += n;
                    return v_copy;
                }

                inline monoque_const_iterator<T, Alloc> operator-(difference_type n) const
                {
                    monoque_const_iterator<T, Alloc> v_copy = *this;
                    v_copy.m_index -= n;
                    return v_copy;
                }

                inline bool operator!=(monoque_const_iterator<T, Alloc> const & other) const
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index != other.m_index;
                }

                inline bool operator==(monoque_const_iterator<T, Alloc> const & other) const
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index == other.m_index;
                }


                inline bool operator<(monoque_const_iterator<T, Alloc> const & other) const
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index < other.m_index;
                }

                inline bool operator<=(monoque_const_iterator<T, Alloc> const & other) const
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index <= other.m_index;
                }

                inline bool operator>=(monoque_const_iterator<T, Alloc> const & other) const
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index >= other.m_index;
                }

                inline bool operator>(monoque_const_iterator<T, Alloc> const & other) const
                {
                    RPNX_ASSERT(m_which == other.m_which);
                    return m_index > other.m_index;
                }

                inline monoque_const_iterator<T, Alloc> operator++(int)
                {
                    monoque_const_iterator<T, Alloc> copy = *this;
                    m_index++;
                    return copy;
                }


                inline monoque_const_iterator<T, Alloc>& operator++()
                {
                    m_index++;
                    return *this;
                }

                inline monoque_const_iterator<T, Alloc> operator--(int)
                {
                    monoque_const_iterator<T, Alloc> copy = *this;
                    m_index--;
                    return copy;
                }


                inline monoque_const_iterator<T, Alloc>& operator--()
                {
                    m_index--;
                    return *this;
                }

                inline difference_type operator-(monoque_const_iterator<T, Alloc> const & other) const
                {
                    return m_index - other.m_index;
                }

                inline value_type & operator[](difference_type dif) const
                {
                    return *(*this + dif);
                }

            };


        }
    }
}
template <typename T, typename Alloc>
auto operator+(std::ptrdiff_t lhs, typename rpnx::experimental::monoque_iterator<T,Alloc> const & rhs)
{
    return rhs+lhs;
}

template <typename T, typename Alloc>
auto operator-(std::ptrdiff_t lhs, typename rpnx::experimental::monoque_iterator<T,Alloc> const & rhs)
{
    return rhs-lhs;
}

template <typename T, typename Alloc>
auto operator+(std::ptrdiff_t lhs, typename rpnx::experimental::monoque_const_iterator<T,Alloc> const & rhs)
{
    return rhs+lhs;
}

template <typename T, typename Alloc>
auto operator-(std::ptrdiff_t lhs, typename rpnx::experimental::monoque_const_iterator<T,Alloc> const & rhs)
{
    return rhs-lhs;
}

#endif // RPNXCORE_MONOQUE_HPP
