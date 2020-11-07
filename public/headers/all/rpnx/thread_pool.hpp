#ifndef RPNX_ALL_THREAD_POOL_HPP
#define RPNX_ALL_THREAD_POOL_HPP

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <set>
#include <condition_variable>
#include "rpnx/assert.hpp"

namespace rpnx
{
    class thread_pool
    {
        


        struct post_item
        {
            void (*m_func)(void*);
            void *m_data;
        };

        std::condition_variable m_cond;
        std::mutex m_mtx;
        std::size_t m_thread_count_target;
        std::size_t m_thread_count;


        std::vector< post_item > m_tasks;

        static void run(thread_pool &pool, int i)
        {
            while (true) 
            {
                {
                    std::unique_lock lock(pool.m_mtx);

                    pool.m_cond.wait(lock, [&]() { return pool.m_thread_count_target < pool.m_thread_count || pool.m_tasks.size() != 0; });

                    if (pool.m_thread_count_target < pool.m_thread_count && (pool.m_thread_count != 1 || pool.m_tasks.empty()))
                        return;

                    RPNX_ASSERT(!pool.m_tasks.empty());
                }
            }
        }


      public:
        thread_pool() : m_thread_count(0)
        {
            set_thread_count_target(std::thread::hardware_concurrency());            
        }

        ~thread_pool()
        {
        }

        void set_thread_count_target(std::size_t num)
        {            
            std::unique_lock lock(m_mtx);
            m_thread_count_target = num;
           
        }
    };

} // namespace rpnx

#endif