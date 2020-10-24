#ifndef RPNX_DERIVATOR_HPP
#define RPNX_DERIVATOR_HPP

#include <type_traits>
#include <memory>
#include <tuple>
#include <typeindex>

namespace rpnx
{

    struct foo {};


    template <typename Allocator>
    struct derivator_vtab
    {
        std::type_index m_type_index;
        void (*m_deleter)(Allocator::void_pointer *);
        void (*m_copier)(Allocator::void_pointer, Allocator::void_pointer);
        bool (*m_equals)(Allocator::void_pointer, Allocator::void_pointer);
        bool (*m_less)(void const*, void const *);
        int m_index;
    };




    template <typename Alloc, typename ... Types>
    class basic_derivator
    : private Alloc
    {
        typename std::allocator_traits<Alloc>::void_pointer m_value;
        derivator_vtab * m_vtab;

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
                m_vtab->m_deleter(m_value);
                m_value = nullptr;
            }

        }

        template <size_t I, typename ... Ts>
        void emplace(Ts &&  ...ts)
        {

            if constexpr(std::is_void_v< std::tuple_element_t<I, std::tuple<Types...>> >)
            {
                destroy();
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
                    (typename std::allocator_traits<Alloc>::template rebind_alloc<std::tuple_element_t<I, std::tuple<Types...>>>((Alloc&)*this)).deallocate(ptr);
                    throw;
                }
                destroy();
                m_vtab = derivator_vtable_v<I, Ts...>;
            }
        }


    };

    template <typename ... Ts>
    using derivator = basic_derivator<std::allocator<void>, Ts...>;
}


#endif
