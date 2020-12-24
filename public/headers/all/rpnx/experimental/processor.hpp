//
// Created by Ryan Nicholl on 12/23/20.
//

#ifndef RPNXCORE_PROCESSOR_HPP
#define RPNXCORE_PROCESSOR_HPP
#include <thread>
#include <mutex>
#include <functional>
#include <deque>
#include <condition_variable>



namespace rpnx
{
    namespace experimental
    {

        template <typename T>
        class processor
        {

            std::function<void(T)> m_func;
            std::thread m_processor_thread;

            std::mutex m_mutex;
            std::condition_variable m_cond;

            bool m_should_stop = false;
            std::deque<T> m_queue;
            std::size_t m_unprocessed_items = 0;

          public:
            processor(std::function<void(T)> f)
                : m_func(std::move(f)),
                  m_processor_thread([this]{run();})
            {}

            void submit(T value)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_queue.push_back(std::move(value));
                m_cond.notify_all();
            }

            void finish_all()
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                m_cond.wait(lock, [&]{
                  return m_queue.empty() && m_unprocessed_items == 0;
                });
            }

            void stop()
            {
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_should_stop = true;
                    m_cond.notify_all();
                }

                if (m_processor_thread.joinable()) {
                    m_processor_thread.join();
                }
            }

            ~processor()
            {
                stop();
            }

          private:
            void run()
            {
                while (true)
                {
                    std::unique_lock<std::mutex> lock(m_mutex);

                    m_cond.wait(lock, [&]{
                      return m_should_stop || ! m_queue.empty();
                    });

                    if (m_should_stop) return;

                    if (m_queue.empty() == false) {
                        T item = std::move(m_queue.front());
                        m_queue.pop_front();
                        m_unprocessed_items++;
                        lock.unlock();

                        try
                        {
                            m_func(item);
                        }
                        catch (...) {}

                        lock.lock();
                        m_unprocessed_items--;
                        if (m_unprocessed_items == 0 && m_queue.empty())
                        {
                            m_cond.notify_all();
                        }

                    }

                }
            }


        };




    }

}

#endif // RPNXCORE_PROCESSOR_HPP
