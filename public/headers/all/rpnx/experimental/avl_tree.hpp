#ifndef RPNX_AVL_TREE_IMPLEMENTATION2_HPP
#define RPNX_AVL_TREE_IMPLEMENTATION2_HPP

#include <utility>
#include <memory>
#include "rpnx/assert.hpp"

namespace rpnx::experimental
{
    template <typename K, typename T, typename Compare = std::less<K>, typename Allocator = std::allocator< std::pair< K const, T > > >
    class avl_tree;


    template <typename K, typename T, typename Compare, typename Allocator>
    class avl_tree_const_iterator;


    template <typename K, typename T, typename Compare, typename Allocator>
    class avl_tree_iterator;

    template <typename K, typename T, typename Compare, typename Allocator>
    class avl_tree_node;

    template< typename K, typename T, typename Compare, typename Allocator>
    class avl_tree_node
    {
        friend class avl_tree_iterator<K, T, Compare, Allocator>;
        friend class avl_tree_const_iterator<K, T, Compare, Allocator>;
        friend class avl_tree<K, T, Compare, Allocator>;

        using tree = avl_tree<K, T, Compare, Allocator>;

        using node = avl_tree_node<K, T, Compare, Allocator>;
        using value_type = typename tree::value_type;


        value_type m_value;
        std::array<node*, 2> m_child {};
        node * m_parent {};

        avl_tree_node(value_type const & val)
        : m_value(val)
        {

        }



    };

    template <typename K, typename T, typename Compare, typename Allocator>
    class avl_tree
        :
        private Compare,
        private Allocator
    {
      public:
        using value_type = std::pair<K const, T>;
        using iterator = avl_tree_iterator<K,T,Compare, Allocator>;
        using const_iterator = avl_tree_const_iterator<K,T,Compare, Allocator>;


      private:
        using node = avl_tree_node<K, T, Compare, Allocator>;
        using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

        using node_allocator_traits = std::allocator_traits<node_allocator>;

      public:
        inline avl_tree() noexcept(noexcept(Allocator()))
        {}

        // TODO: Copy constructor
        inline avl_tree(avl_tree const & ) = delete;

        Allocator get_allocator() const
        {
            return static_cast<Allocator&>(*this);
        }

        iterator insert(value_type const & value_to_insert)
        {
            node* node_to_insert = create_node(value_to_insert);





        }

      private:

        template <typename T2>
        auto get_rebound_allocator() const noexcept
        {
            typename std::allocator_traits<Allocator>::template rebind_alloc<T2> rebound_alloc(get_allocator());
            return rebound_alloc;
        }

        auto get_node_allocator() const noexcept
        {
            return get_rebound_allocator<node>();
        }

        node * create_node(value_type const & value)
        {
            node_allocator node_alloc = get_node_allocator();
            node * nptr = node_alloc.allocate(1);
            try
            {
                node_allocator_traits::construct(node_alloc, nptr, value_to_insert);
            }
            catch (...)
            {
                node_alloc.deallocate(nptr, 1);
                throw;
            }
            return std::launder(nptr);
        }

        void delete_node(node * n) noexcept
        {
            node_allocator node_alloc = get_node_allocator();
            node_allocator_traits::destroy(node_alloc, nptr);
            node_allocator_traits::deallocate(node_alloc, nptr);
        }

      private:
        node * m_root = nullptr;
        std::size_t m_size = 0;

    };

}


#endif