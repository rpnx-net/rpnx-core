// Copyright 2015, 2016, 2020 Ryan P. Nicholl <rnicholl@protonmail.com>
// All rights reserved
// See rpnx-core/LICENSE.txt

// Warning: basic_derivator custom allocators DO NOT currently work.

#ifndef RPNX_DERIVATOR_HPP
#define RPNX_DERIVATOR_HPP

#include <memory>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <variant>
#include <assert.h>

#include <rpnx/meta.hpp>
#include <rpnx/assert.hpp>

namespace rpnx
{

    template < typename Allocator >
    struct derivator_vtab
    {
        void (*m_deleter)(Allocator const& a, typename std::allocator_traits< Allocator >::void_pointer) = nullptr;
        void* (*m_construct_copy)(Allocator const & alloc, typename std::allocator_traits< Allocator >::const_void_pointer) = nullptr;
        void* (*m_construct_move)(Allocator const & alloc, typename std::allocator_traits< Allocator >::void_pointer) = nullptr;
        bool (*m_equals)(typename std::allocator_traits< Allocator >::void_pointer, typename std::allocator_traits< Allocator >::void_pointer) = nullptr;
        bool (*m_less)(void const*, void const*) = nullptr;
        int m_index = -1;
        bool is_void = false;
    };

    namespace detail
    {

        // There is some bug in visual studio that causes this not to work
        template < typename T, typename Alloc >
        void derivator_deletor(Alloc const & alloc, void* src)
        {
            if constexpr (!std::is_void_v< T >)
            {
                ((T*)src)->T::~T();
                (typename std::allocator_traits< Alloc >::template rebind_alloc< T >(alloc)).deallocate((T*)src, sizeof(T));
            }
        }

        template <typename T, typename Alloc>
        void * derivator_construct_copy(Alloc const & alloc, void const * src)
        {
            if constexpr (!std::is_void_v< T >)
            {
                void* dest = (typename std::allocator_traits< Alloc >::template rebind_alloc< T >(alloc)).allocate(sizeof(T));
                try
                {
                    return (void*)new (dest) T(*reinterpret_cast< T const* >(src));
                }
                catch (...)
                {
                    (typename std::allocator_traits< Alloc >::template rebind_alloc< T >(alloc)).deallocate((T*)dest, sizeof(T));
                    throw;
                }
            }
            else
                return nullptr;
        }

        template <typename T, typename Alloc>
        void * derivator_construct_move(Alloc const & alloc, void * src)
        {
            if constexpr (!std::is_void_v< T >)
            {
                void* dest = (typename std::allocator_traits< Alloc >::template rebind_alloc< T >(alloc)).allocate(sizeof(T));
                try
                {
                    return (void*)new (dest) T(std::move(*reinterpret_cast< T const* >(src)));
                }
                catch (...)
                {
                    (typename std::allocator_traits< Alloc >::template rebind_alloc< T >(alloc)).deallocate((T*)dest, sizeof(T));
                    throw;
                }
            }
            else
                return nullptr;
        }
    } // namespace detail
    
    
    template < int I, typename T, typename Allocator >
    constexpr derivator_vtab< Allocator > init_vtab_for()
    {
        derivator_vtab< Allocator > tb;
           
        tb.m_construct_copy = &detail::derivator_construct_copy< T, Allocator >;
        tb.m_construct_move = &detail::derivator_construct_move< T, Allocator >;
        tb.m_deleter = &detail::derivator_deletor< T, Allocator >;
        
        // TODO: It would be nice to support comparisons where possible, fix this.
        tb.m_equals = nullptr;
        tb.m_less = nullptr;

        tb.m_index = I;
        tb.is_void = std::is_void_v<T>;

        return tb;
    }

    template < int I, typename T, typename Allocator >
    inline constexpr const derivator_vtab< Allocator > derivator_vtab_v = init_vtab_for< I, T, Allocator >();

    template < typename Allocator, typename... Types >
    class basic_derivator : private Allocator
    {
        static_assert(std::is_same_v<std::allocator<void>, Allocator>, "Custom allocators don't work yet.");

        typename std::allocator_traits< Allocator >::void_pointer m_value;
        derivator_vtab< Allocator > const* m_vtab;
      private:
        void make_void()
        {
            m_vtab = &derivator_vtab_v< tuple_type_index<void, std::tuple<Types...>>::value, void, Allocator >;
            m_value = nullptr;
        }

        void destroy()
        {
            if (m_value && m_vtab)
            {
                m_vtab->m_deleter(get_allocator(), m_value);
            }
            make_void();
        }

      public:
        using allocator_type = Allocator;

        basic_derivator() noexcept(noexcept(Allocator()))
            : m_value(nullptr), m_vtab(nullptr) { emplace< 0 >(); }
        ~basic_derivator() { destroy(); }

        basic_derivator(basic_derivator<Allocator, Types...> const& other) 
            : Allocator(other) 
        {
            if (other.m_value)
            {
                m_value = other.m_vtab->m_construct_copy(get_allocator(), other.m_value);
                m_vtab = other.m_vtab;
            }
        }

        basic_derivator(basic_derivator<Allocator, Types...> && other )
        : Allocator(other.get_allocator())
        {
            make_void();
            std::swap(m_vtab, other.m_vtab);
            std::swap(m_value, other.m_value);

        }

        basic_derivator<Allocator, Types...> & operator =(basic_derivator<Allocator, Types...> && other)
        {
            static_assert(std::is_same_v<Allocator, std::allocator<void>>, "Not implemented");
            /*
            if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value)
            {
                //if (get_allocator() != other.get_allocator())
                *(allocator_type*)this = static_cast<allocator_type&>(other); 
            }
            */
           std::swap(m_value, other.m_value);
           std::swap(m_vtab, other.m_vtab);

           return *this;
        }

        basic_derivator<Allocator, Types...> & operator =(basic_derivator<Allocator, Types...> const & other)
        {
            static_assert(std::is_same_v<Allocator, std::allocator<void>>, "Not implemented");
            assert(other.m_vtab != nullptr);
            void *value = other.m_vtab->m_construct_copy(other, other.m_value );
            m_value = value;
            m_vtab = other.m_vtab;
            return *this;
        }

        Allocator const& get_allocator() const noexcept 
        {
            return *this;
        }

        

        template < typename T, typename... Ts >
        void emplace(Ts&&... ts)
        {
            static_assert(tuple_type_index< T, std::tuple< Types... >>::value != -1, "The target type T is not allowed");
            emplace< tuple_type_index< T, std::tuple< Types... > >::value >(std::forward< Ts >(ts)...);
        }

        template <typename T>
        basic_derivator<Allocator, Types...> & operator=(T const & value)
        {
            constexpr const int I = tuple_type_index<T, std::tuple<Types...> >::value;
            static_assert(I != -1, "Cannot assign type T to incompatible derivator");

            void* ptr = (typename std::allocator_traits< Allocator >::template rebind_alloc< T >(get_allocator())).allocate(sizeof(T));
            try
            {
                ptr = (void*)new (ptr) T(value);
            }
            catch (...)
            {
                (typename std::allocator_traits< Allocator >::template rebind_alloc< T >(get_allocator())).deallocate((T*)ptr, sizeof(T));
                throw;
            }

            destroy();
            m_vtab = &derivator_vtab_v< I, T, Allocator >;
            m_value = reinterpret_cast< void* >(ptr);

            return *this;
        }

        template <typename T>
        basic_derivator<Allocator, Types...> & operator=(T && value)
        {
            constexpr const int I = tuple_type_index<T, std::tuple<Types...> >::value;
            static_assert(I != -1, "Cannot assign type T to incompatible derivator");

            void* ptr = (typename std::allocator_traits< Allocator >::template rebind_alloc< T >(get_allocator())).allocate(sizeof(T));
            try
            {
                ptr = (void*)new (ptr) T(std::move(value));
            }
            catch (...)
            {
                (typename std::allocator_traits< Allocator >::template rebind_alloc< T >(get_allocator())).deallocate((T*)ptr, sizeof(T));
                throw;
            }

            destroy();
            m_vtab = &derivator_vtab_v< I, T, Allocator >;
            m_value = reinterpret_cast< void* >(ptr);
            return *this;
        }

        template < size_t I, typename... Ts >
        void emplace(Ts&&... ts)
        {
            if constexpr (std::is_void_v< std::tuple_element_t< I, std::tuple< Types... > > >)
            {
                destroy();
                m_vtab = &derivator_vtab_v< I, std::tuple_element_t< I, std::tuple< Types... > >, Allocator >;
            }
            else
            {
                using T = std::tuple_element_t< I, std::tuple< Types... > >;
                void* ptr = (typename std::allocator_traits< Allocator >::template rebind_alloc< T >(get_allocator())).allocate(sizeof(T));
                try
                {
                    ptr = (void*)new (ptr) T(std::forward< Ts >(ts)...);
                }
                catch (...)
                {
                    (typename std::allocator_traits< Allocator >::template rebind_alloc< T >(get_allocator())).deallocate((T*)ptr, sizeof(T));
                    throw;
                }

                destroy();
                m_vtab = &derivator_vtab_v< I, std::tuple_element_t< I, std::tuple< Types... > >, Allocator >;
                m_value = reinterpret_cast< void* >(ptr);
            }
        }

        /** Returns the index of the currently held type
         * Note: this can be -1 if no value is held and void does not appear in the type list.
         */
        int index() const noexcept { return m_vtab->m_index; }

        template < int I >
        std::tuple_element_t< I, std::tuple< Types... > >& as_checked()
        {
            if (m_vtab->m_index != I) throw std::invalid_argument("derivator");

            return *reinterpret_cast< std::tuple_element_t< I, std::tuple< Types... > >* >(m_value);
        }

        template < int I >
        std::tuple_element_t< I, std::tuple< Types... > > const& as_checked() const
        {
            if (m_vtab->m_index != I) throw std::invalid_argument("derivator");

            return *reinterpret_cast< std::tuple_element_t< I, std::tuple< Types... > > const* >(m_value);
        }

        template < typename T >
        auto& as_checked()
        {
            static_assert(tuple_type_index< T, std::tuple< Types... > >::value != -1, "The type T in derivator<Types...>::as_checked<T>() is not present "
                                                                                      "in Types...");
            return as_checked< tuple_type_index< T, std::tuple< Types... > >::value >();
        }

        template < typename T >
        auto const& as_checked() const
        {
            static_assert(tuple_type_index< T, std::tuple< Types... > >::value != -1, "The type T in derivator<Types...>::as_checked<T>() is not present "
                                                                                      "in Types...");
            return as_checked< tuple_type_index< T, std::tuple< Types... > >::value >();
        }

        template < int I >
        std::tuple_element_t< I, std::tuple< Types... > >& as()
        {
            return *reinterpret_cast< std::tuple_element_t< I, std::tuple< Types... > >* >(m_value);
        }

        template < int I >
        std::tuple_element_t< I, std::tuple< Types... > > const& as() const
        {
            return *reinterpret_cast< std::tuple_element_t< I, std::tuple< Types... > > const* >(m_value);
        }

        template < typename T >
        auto& as()
        {
            static_assert(tuple_type_index< T, std::tuple< Types... > >::value != -1, "The type T in derivator<Types...>::as_checked<T>() is not present "
                                                                                      "in Types...");
            return as< tuple_type_index< T, std::tuple< Types... > >::value >();
        }

        template < typename T >
        auto const& as() const
        {
            static_assert(tuple_type_index< T, std::tuple< Types... > >::value != -1, "The type T in derivator<Types...>::as_checked<T>() is not present "
                                                                                      "in Types...");
            return as< tuple_type_index< T, std::tuple< Types... > >::value >();
        }

        void swap(basic_derivator< Allocator, Types... >& other) noexcept
        {
            // TODO: this should only be noexcept if the allocator void_pointer type is
            // noexcept swappable
            if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
            {
                std::swap(static_cast<allocator_type&>(*this),static_cast<allocator_type&>(other));
            }
            else
            {
                // Swap with incompatible allocators is undefined per standard.
                RPNX_ASSERT(static_cast<allocator_type&>(*this) == static_cast<allocator_type&>(other));
            }
            std::swap(m_vtab, other.m_vtab);
            std::swap(m_value, other.m_value);
        }

        template < typename T >
        bool holds_alternative() const noexcept
        {
            return tuple_type_index< T, std::tuple< Types... > >::value == m_vtab->m_index;
        }

        bool has_value() const noexcept
        {
            return m_value != nullptr;
        }
    };

    /** The class rpnx::derivator is a bit like std::variant, except that it
     * allocates memory indirectly. This allows derivators that can refer to the
     * enclosing class, and also can save memory if some possible values are much
     * larger than others (variant always allocates enough space for the largest).
     * If you need control over memory allocation, use
     * rpnx::basic_derivator<Allocator, Types...>
     *
     * */
    template < typename... Ts >
    using derivator = basic_derivator< std::allocator< void >, Ts... >;

    namespace detail
    {
        template<typename Visitor, typename Derivator, size_t I>
        void derivator_dispatch(Visitor && visitor, Derivator && derivator)
        {
            using derivator_t = std::remove_all_extents_t<std::remove_reference_t<Derivator>>;
            if constexpr (! std::is_void_v< derivator_element<I, derivator_t>::type >)
            {
                visitor(std::forward<Derivator>(derivator).as<I>());
            }
            else
            {
                visitor();
            }
        }

        template<size_t I, typename Derivator>
        struct derivator_element;

        template <size_t I, typename Allocator, typename ... Types>
        struct derivator_element<I, basic_derivator<Allocator, Types...>>
        {
            using type = typename std::tuple_element<I, std::tuple<Types...>>::type;
        };



        template<typename Visitor, typename Derivator, size_t N>
        struct derivator_dispatch_table
        {
            using dispatch_function = void(*)(Visitor &&, Derivator&&);
            dispatch_function dispatch_array[N];

            constexpr derivator_dispatch_table()
            :dispatch_array()
            {
                for (int i = 0; i < N; i++)
                {
                    dispatch_array[i] = nullptr;
                }
            }

            template <size_t I>
            static constexpr void fill_dispatch_table(derivator_dispatch_table<Visitor, Derivator, N> & table)
            {
                table.dispatch_array[I] = &derivator_dispatch<Visitor, Derivator, I>;
                if constexpr (I+1 != N)
                {
                    fill_dispatch_table<I+1>(table);
                }
            }
            static constexpr derivator_dispatch_table<Visitor, Derivator, N> generate()
            {
                derivator_dispatch_table<Visitor, Derivator, N> result;
                fill_dispatch_table<0>(result);
                return result;
            }
        };

        
    }

    template <typename Visitor, typename Allocator, typename ... Types>
    void visit(Visitor && vistor, basic_derivator<Allocator, Types...> && derivator)
    {
        static const constexpr auto dispatch_table = derivator_dispatch_table<Visitor, basic_derivator<Allocator, Types...> &&, std::tuple_size<std::tuple<Types...>>::value>::generate();
        RPNX_ASSERT(derivator.index() != -1);
        auto function_pointer = dispatch_table[derivator.index()];
        function_pointer(std::forward<Visitor>(vistor), std::forward<basic_derivator<Allocator, Types...> &&>(derivator));        
    }

    template <typename Visitor, typename Allocator, typename ... Types>
    void visit(Visitor && vistor, basic_derivator<Allocator, Types...> const & derivator)
    {
        static const constexpr auto dispatch_table = derivator_dispatch_table<Visitor, basic_derivator<Allocator, Types...> const&, std::tuple_size<std::tuple<Types...>>::value>::generate();
        RPNX_ASSERT(derivator.index() != -1);
        auto  function_pointer = dispatch_table[derivator.index()];
        function_pointer(std::forward<Visitor>(vistor), std::forward<basic_derivator<Allocator, Types...> const &>(derivator));        
    }

    template <typename Visitor, typename Allocator, typename ... Types>
    void visit(Visitor && vistor, basic_derivator<Allocator, Types...> & derivator)
    {
        static const constexpr auto dispatch_table = detail::derivator_dispatch_table<Visitor, basic_derivator<Allocator, Types...> &, std::tuple_size<std::tuple<Types...>>::value>::generate();
        RPNX_ASSERT(derivator.index() != -1);
        auto const & function_pointer = dispatch_table.dispatch_array[derivator.index()];
        function_pointer(std::forward<Visitor>(vistor), std::forward<basic_derivator<Allocator, Types...> &>(derivator));        
    }

} // namespace rpnx

#endif
