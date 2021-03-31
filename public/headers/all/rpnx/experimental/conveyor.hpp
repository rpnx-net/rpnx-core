//
// Created by Ryan on 3/30/2021.
//

#ifndef RPNXCORE_CONVEYOR_HPP
#define RPNXCORE_CONVEYOR_HPP

#include <list>
#include <memory>

namespace rpnx
{
    namespace experimental
    {
        template < typename T, typename Alloc = std::allocator< T > >
        class conveyor : private Alloc
        {
          public:
            conveyor(Alloc const& alloc = Alloc()) : Alloc(alloc), m_block_allocs(alloc)
            {
            }

            conveyor(conveyor< T, Alloc > const&) = delete;

            template < typename... Ts >
            T* emplace(Ts&&... ts)
            {
                if (!have_storage())
                {
                    make_more_storage();
                }

                T* position = get_current_position();
                std::allocator_traits< Alloc >::construct(get_allocator(), position, std::forward< Ts >(ts)...);
                advance_current_position();

                return std::launder(position);
            }

            void clear()
            {
                delete_everything();
            }

          private:
            void delete_everything()
            {
                while (have_block_allocated())
                {
                    while (current_block_has_object())
                    {
                        destroy_most_recent_object();
                    }
                    deallocate_current_block();

                    if (have_deallocation_block())
                    {
                        move_deallocation_block_to_current_block();
                    }
                }
            }

            void deallocate_current_block()
            {
                std::allocator_traits< Alloc >::deallocate(get_allocator(), m_current_block, 64);
                m_current_block = nullptr;
            }

            bool current_block_has_object() const
            {
                return m_current_allocs != 0;
            }

            bool have_deallocation_block() const
            {
                return !m_block_allocs.empty();
            }

            void move_deallocation_block_to_current_block()
            {
                m_current_block = m_block_allocs.back();
                m_block_allocs.pop_back();
                m_current_allocs = 64;
            }

            Alloc& get_allocator() noexcept
            {
                return (Alloc&)*this;
            }

            bool have_storage() const
            {
                return m_current_allocs != 64 && m_current_block != nullptr;
            }

            void make_more_storage() const
            {
                if (have_block_allocated())
                {
                    move_current_block_to_dealloc_list();
                }

                allocate_block();
            }

            void move_current_block_to_dealloc_list()
            {
                m_block_allocs.push_back(m_current_block);
                m_current_block = nullptr;
            }

            bool have_block_allocated() const
            {
                return m_current_block != nullptr;
            }

            T* get_current_position() const
            {
                return m_current_block + m_current_allocs;
            }

            void advance_current_position()
            {
                m_current_allocs++;
            }

            void allocate_block()
            {
                m_current_block = std::allocator_traits< Alloc >::allocate(get_allocator(), 64);
                m_current_allocs = 0;
            }

          private:
            T* m_current_block;
            size_t m_current_allocs;
            std::list< T*, Alloc > m_block_allocs;
        };
    } // namespace experimental

} // namespace rpnx

#endif // RPNXCORE_CONVEYOR_HPP
