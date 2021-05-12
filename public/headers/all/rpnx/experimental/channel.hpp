//
// Created by Ryan on 4/26/2021.
//

#ifndef RPNXCORE_CHANNEL_HPP
#define RPNXCORE_CHANNEL_HPP
#include <memory>
#include <atomic>
#include <list>
#include <deque>
#include <condition_variable>


namespace rpnx::experimental
{
    template <typename T>
    class channel
    {
        //std::array<std::atomic<T*>, 64> m_arry;
        std::mutex m_mtx;
        std::condition_variable m_cond;
        std::deque<T*> m_queue;

      public:
        channel()
        {}

        void submit(T value)
        {
            T* copy = new T(std::move(value));
            try
            {
                std::unique_lock v_lock(m_mtx);
                m_queue.push_back(copy);
            }
            catch (...)
            {
                delete copy;
                throw;
            }
            m_cond.notify_all();
        }

        T receive()
        {
            std::unique_lock v_lock(m_mtx);
            m_cond.wait(v_lock, [&]{ return ! m_queue.empty(); });
            T result(std::move(m_queue.front()));
            m_queue.pop_front();
            return result;

        }
    };

}

#endif // RPNXCORE_CHANNEL_HPP
