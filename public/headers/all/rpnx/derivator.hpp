// Copyright 2015, 2016, 2020 Ryan P. Nicholl <rnicholl@protonmail.com>
// All rights reserved
// See rpnx-core/LICENSE.txt

#ifndef RPNX_DERIVATOR_HPP
#define RPNX_DERIVATOR_HPP

#include <type_traits>
#include <memory>
#include <tuple>
#include <typeindex>
#include <variant>

#include <rpnx/meta.hpp>

namespace rpnx
{

    struct foo {};


    template <typename Allocator>
    struct derivator_vtab
    {
        std::type_index m_type_index = typeid(void);
        void (*m_deleter)(Allocator const& a, typename std::allocator_traits<Allocator>::void_pointer) = nullptr;
        void (*m_copier)(typename std::allocator_traits<Allocator>::const_void_pointer, typename std::allocator_traits<Allocator>::void_pointer) = nullptr;
        bool (*m_equals)(typename std::allocator_traits<Allocator>::void_pointer, typename std::allocator_traits<Allocator>::void_pointer) = nullptr;
        bool (*m_less)(void const*, void const *) = nullptr;
        int m_index = -1;
    };

    template <int I, typename T, typename Alloc>
    constexpr derivator_vtab<Alloc> init_vtab_for()
    {
        derivator_vtab<Alloc> tb;
        tb.m_type_index = typeid(T);
        // TODO: We assume actual void* here, change to use the allocator traits "void*" to support fancy pointers
        tb.m_copier = [](void const* src, void * dest)
        {
            if constexpr (! std::is_void_v<T>)
            {
                new (dest) T(*reinterpret_cast<T const *>(src));
            }
        };

        // TODO: We assume actual void* here, change to use the allocator traits "void*" to support fancy pointers
        tb.m_deleter = [](Alloc const &a, void *val)
        {
            if constexpr (! std::is_void_v<T>)
            {
                reinterpret_cast<T*>(val)->T::~T();
            (typename std::allocator_traits<Alloc>::template rebind_alloc<T>(a)).deallocate(reinterpret_cast<T*>(val), sizeof(T));
            }
        };

        // TODO: It would be nice to support comparisons where possible, fix this.
        tb.m_equals = nullptr;
        tb.m_less = nullptr;

        tb.m_index = I;

        return tb;
    }

    template <int I, typename T, typename Alloc>
    inline derivator_vtab<Alloc> derivator_vtab_v =  init_vtab_for<I, T, Alloc>();

    
    template <typename Alloc, typename ... Types>
    class basic_derivator
    : private Alloc
    {
        typename std::allocator_traits<Alloc>::void_pointer m_value;
        derivator_vtab<Alloc> * m_vtab;

    public:

        basic_derivator()
            :m_value(nullptr),
              m_vtab(nullptr)
        {
            emplace<0>();
        }



        void destroy()
        {
            if (m_value && m_vtab)
            {
                m_vtab->m_deleter((Alloc&)*this, m_value);
                m_value = nullptr;
            }

        }


        template <typename T, typename ...Ts>
        void emplace (Ts && ... ts)
        {
            emplace< tuple_type_index<T, std::tuple<Types...>>::value >(std::forward<Ts>(ts)...);
        }

        template <size_t I, typename ... Ts>
        void emplace(Ts &&  ...ts)
        {
            

            if constexpr(std::is_void_v< std::tuple_element_t<I, std::tuple<Types...>> >)
            {
                destroy();
                m_vtab = &derivator_vtab_v<I,  std::tuple_element_t<I, std::tuple<Types...>>, Alloc>;
            }
            else
            {
                
                auto ptr = (typename std::allocator_traits<Alloc>::template rebind_alloc<std::tuple_element_t<I, std::tuple<Types...>>>((Alloc&)*this)).allocate(sizeof(std::tuple_element_t<I, std::tuple<Types...>>));
                try
                {
                    new (ptr) std::tuple_element_t<I, std::tuple<Types...>>(std::forward<Ts>(ts)...);
                }
                catch (...)
                {
                    (typename std::allocator_traits<Alloc>::template rebind_alloc<std::tuple_element_t<I, std::tuple<Types...>>>((Alloc&)*this)).deallocate(ptr, sizeof(std::tuple_element_t<I, std::tuple<Types...>>));
                    throw;
                }
                destroy();
                m_vtab = &derivator_vtab_v<I,  std::tuple_element_t<I, std::tuple<Types...>>, Alloc>;
                m_value = reinterpret_cast<void*>(ptr);
            }
        }

        std::size_t index() const noexcept
        {
           return m_vtab->m_index;
        }

        template <int I>
        std::tuple_element_t<I, std::tuple<Types...>> & as()
        {
            if (m_vtab->m_index != I) throw std::invalid_argument("derivator");

            return *reinterpret_cast<std::tuple_element_t<I, std::tuple<Types...>>*>(m_value);
        }

        template <int I>
        std::tuple_element_t<I, std::tuple<Types...>> const & as() const
        {
            if (m_vtab->m_index != I) throw std::invalid_argument("derivator");

            return *reinterpret_cast<std::tuple_element_t<I, std::tuple<Types...>> const*>(m_value);
        }

        template <typename T>
        auto & as()
        {
            static_assert(tuple_type_index<T, std::tuple<Types...>>::value != -1, "The type T in derivator<Types...>::as<T>() is not present in Types...");
            return as<tuple_type_index<T, std::tuple<Types...>>::value>();
        }

        template <typename T>
        auto const & as() const
        {
            static_assert(tuple_type_index<T, std::tuple<Types...>>::value != -1, "The type T in derivator<Types...>::as<T>() is not present in Types...");
            return as<tuple_type_index<T, std::tuple<Types...>>::value>();
        }


    };

   
    /** The class rpnx::derivator is a bit like std::variant, except that it allocates memory indirectly.
     * This allows derivators that can refer to the enclosing class, and also can save memory if some possible 
     * values are much larger than others (variant always allocates enough space for the largest). If you 
     * need control over memory allocation, use rpnx::basic_derivator<Allocator, Types...>
     * 
     * */
    template <typename ... Ts>
    using derivator = basic_derivator<std::allocator<void>, Ts...>;
}


#endif
