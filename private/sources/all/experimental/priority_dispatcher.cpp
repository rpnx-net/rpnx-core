//
// Created by Ryan Nicholl on 12/23/20.
//
#include "rpnx/experimental/priority_dispatcher.hpp"
#include "rpnx/experimental/scoped_action.hpp"
#include <queue>
#include <map>
#include "rpnx/assert.hpp"

namespace rpnx
{
    namespace experimental
    {
        namespace impl
        {
            struct submitted_job
            {
                std::int64_t m_priority;
                void* m_data;
                void (*m_exec)(void*);
                void (*m_cleanup)(void*, priority_dispatcher::completion_state) noexcept;
            };

            struct submitted_job_comp
            {
                bool operator()(submitted_job const& a, submitted_job const& b) const noexcept
                {
                    return a.m_priority < b.m_priority;
                }
            };

            struct priority_dispatcher_impl
            {
                priority_dispatcher_impl()
                {
                    std::unique_lock v_lock(m_mtx);
                    m_tc_target = std::thread::hardware_concurrency();

                    while (m_tc_running < m_tc_target) start_thread(v_lock);
                }

                ~priority_dispatcher_impl()
                {
                    std::unique_lock sc_lock(m_mtx);
                    scoped_increment_with_lock sc_incr(m_waiting_thread_abort, sc_lock);

                    m_tc_target = 0;

                    static_assert(noexcept(m_cond.notify_all()));
                    m_cond.notify_all();

                    // static_assert doesn't work here, but this behavior is guaranteed noexcept by the C++ standard.
                    // (unless assert failure throws an exception, but we don't talk about that)
                    m_cond.wait(sc_lock, [&] {
                        RPNX_ASSERT(m_tc_target == 0);
                        return m_tc_running == 0;
                    });

                    for (auto& x : m_thread_objects)
                    {
                        x.second.join();
                    }

                    return;
                }

                std::mutex m_mtx;
                std::condition_variable m_cond;
                std::priority_queue< submitted_job, std::deque< submitted_job >, submitted_job_comp > m_queue;
                std::map<std::size_t, std::thread> m_thread_objects;
                std::deque<std::size_t> m_joinable_threads;
                std::deque<std::size_t> m_reusable_ids;
                std::size_t m_thread_min_id = 0;
                std::size_t m_async_executions = 0;
                std::size_t m_waiting_completion_routines = 0;
                std::size_t m_waiting_thread_abort = 0;
                std::size_t m_tc_running = 0;
                std::size_t m_tc_target = 0;

                std::size_t alloc_thread_id(std::unique_lock<std::mutex> &)
                {

                    if (m_reusable_ids.empty())
                    {
                        if (m_thread_min_id == std::numeric_limits<std::size_t>::max())
                        {
                            // If this ever gets called, are you F***-ing nuts?
                            // This can only happen if you allocate std::numeric_limits<std::size_t>::max()/2 threads simultaneously.
                            // Not possible today, but who knows what future processors might be able to do?
                            // In any case, we can't handle this condition.
                            std::terminate();
                        }

                        return m_thread_min_id++;
                    }

                    auto id = m_reusable_ids.back();
                    m_reusable_ids.pop_back();
                    return id;
                }

                void start_thread(std::unique_lock<std::mutex> & lock)
                {
                    auto thread_id = alloc_thread_id(lock);

                    m_thread_objects[thread_id] = std::thread([this, thread_id]{ service_thread(thread_id); });
                    m_tc_running++;
                }

                void join_stopped_threads(std::unique_lock<std::mutex> & sc_lock)
                {
                    while(!m_joinable_threads.empty())
                    {

                        auto th_id = m_joinable_threads.back();
                        m_joinable_threads.pop_back();

                        {
                            std::thread& th = m_thread_objects[th_id];
                            RPNX_ASSERT(th.joinable());
                            th.join();
                        }

                        m_thread_objects.erase(th_id);

                        if (th_id > std::numeric_limits<std::size_t>::max()/ 2)
                        {
                            // to avoid the unlikely scenario of duplicating an in-use thread ID,
                            // there is some limit after which we can begin to recycle thread IDs.

                            m_reusable_ids.push_back(th_id);
                        }
                    }
                }


                void finish_all()
                {
                    std::unique_lock< std::mutex > v_lock(m_mtx);
                    scoped_increment sc_incr(m_waiting_completion_routines);
                    m_cond.wait(v_lock, [&]{
                        return m_async_executions == 0 && m_queue.empty();
                    });
                }

                void service_thread(std::size_t thread_id)
                {
                    while (true)
                    {
                        std::unique_lock< std::mutex > v_lock(m_mtx);

                        m_cond.wait(v_lock, [&] {
                            return m_tc_running > m_tc_target || !m_queue.empty();
                        });

                        if (m_tc_running > m_tc_target)
                        {
                            m_tc_running--;

                            // we cant join ourselves, so let the struct know we are disposable now
                            m_joinable_threads.push_back(thread_id);

                            if (m_waiting_thread_abort)
                            {
                                m_cond.notify_all();
                            }

                            return;
                        }


                        if (!m_queue.empty())
                        {
                            auto item = m_queue.top();
                            m_queue.pop();

                            {
                                scoped_increment_with_lock scope_incr(m_async_executions, v_lock);
                                // using this class because there is a (very small) chance an exception might be thrown by .lock() which
                                // would cause a race condition with scoped_increment
                                // Note: scoped_incrment_with_lock will call std::terminate in the failure case.

                                v_lock.unlock();
                                // The mutex is released in order to allow new items to be submitted to the queue while we are busy executing the function.

                                try
                                {
                                    item.m_exec(item.m_data);

                                    static_assert(noexcept(item.m_cleanup(item.m_data, priority_dispatcher::completion_state::completed)));
                                    item.m_cleanup(item.m_data, priority_dispatcher::completion_state::completed);
                                }
                                catch (...)
                                {
                                    // TODO: Exception handler
                                    // if (exception_handler)
                                    item.m_cleanup(item.m_data, priority_dispatcher::completion_state::completed_with_exception);
                                }

                                v_lock.lock();
                            }

                            // We re-lock the mutex and check if there are cancellors that need to be notified of the queue being emptied by us.

                            if (m_async_executions==0 && m_waiting_completion_routines != 0)
                            {
                                m_cond.notify_all();
                            }
                        }
                    }

                }

                void submit(void (*a_exec)(void*), void (*a_cleanup)(void*, priority_dispatcher::completion_state) noexcept,
                            void* a_data, std::int64_t a_priority)
                {
                    std::unique_lock lock(m_mtx);

                    RPNX_ASSERT(m_waiting_completion_routines == 0);
                    // Race condition
                    // Use cancel_all_quick if there could be operations submitted at the same time.

                    submitted_job v_job;

                    v_job.m_priority = a_priority;
                    v_job.m_exec = a_exec;
                    v_job.m_cleanup = a_cleanup;
                    v_job.m_data = a_data;

                    m_queue.push(v_job);

                    static_assert(noexcept(m_cond.notify_all()));
                    // If cond could throw exceptions, the caller wouldn't know if they needed to run the cleanup function or not

                    m_cond.notify_all();
                }

                void cancel_all_quick()
                {
                    std::unique_lock lock(m_mtx);

                    RPNX_ASSERT(m_waiting_completion_routines == 0);

                    while (!m_queue.empty())
                    {
                        m_queue.top().m_cleanup(m_queue.top().m_data, priority_dispatcher::completion_state::cancelled);
                        m_queue.pop();
                    }

                    // Technically this would be a race condition, but unsure if there is performance gain by avoiding it.
                    if (m_async_executions==0  && m_waiting_completion_routines != 0)
                    {
                        m_cond.notify_all();
                    }
                }

                void set_service_thread_count(std::size_t ct)
                {
                    std::unique_lock sc_lock(m_mtx);

                    // If we call this over and over with different args it could cause a memory leak because we don't wait for threads to actually
                    // finish when we reduce the thread count, so clean up any immediately joinable threads now
                    join_stopped_threads(sc_lock);

                    m_tc_target = ct;

                    if (m_tc_running > m_tc_target)
                    {
                        m_cond.notify_all();
                        return;
                        // the threads will stop by themselves later
                        // We should wake them up though so they can quit
                    }

                    else while (m_tc_running < m_tc_target)
                    {
                        start_thread(sc_lock);
                    }



                }

                void cancel_all()
                {
                    std::unique_lock sc_lock(m_mtx);

                    scoped_increment sc_increment(m_waiting_completion_routines);
                    // We always have the sc_lock


                    while (true)
                    {
                        // wait is guaranteed not to throw unless the predicate throws. our predicate cannot throw.
                        m_cond.wait(sc_lock, [&] {
                            return !m_queue.empty() || m_async_executions == 0;
                        });

                        RPNX_ASSERT(sc_lock.owns_lock());
                        // Guaranteed to be true by C++ standard, otherwise std::terminate would be called.

                        while (!m_queue.empty())
                        {
                            m_queue.top().m_cleanup(m_queue.top().m_data, priority_dispatcher::completion_state::cancelled);
                            m_queue.pop();
                        }
                        // ! noexcept(m_queue.pop());
                        // TODO: Under some low memory condition, this might throw? Investigate.

                        if (m_async_executions == 0)
                            return;
                    }
                }
            };
        }
    }


}

void rpnx::experimental::priority_dispatcher::cancel_all()
{
    reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation)->cancel_all();
}

rpnx::experimental::priority_dispatcher::priority_dispatcher()
{
    m_implementation = new impl::priority_dispatcher_impl;
}

rpnx::experimental::priority_dispatcher::~priority_dispatcher()
{
    reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation)->cancel_all();
    delete reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation);
}

void rpnx::experimental::priority_dispatcher::submit(void (*a_exec)(void*), void (*a_cleanup)(void*, completion_state) noexcept, void* a_caller_data, std::int64_t priority)
{
    RPNX_ASSERT(m_implementation != nullptr);
    reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation)->submit(a_exec, a_cleanup, a_caller_data, priority);
}

void rpnx::experimental::priority_dispatcher::cancel_all_quick()
{
    RPNX_ASSERT(m_implementation != nullptr);
    reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation)->cancel_all_quick();
}
void rpnx::experimental::priority_dispatcher::finish_all()
{
    RPNX_ASSERT(m_implementation != nullptr);
    reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation)->finish_all();
}
void rpnx::experimental::priority_dispatcher::set_service_thread_count(std::size_t n)
{
    RPNX_ASSERT(m_implementation != nullptr);
    reinterpret_cast<impl::priority_dispatcher_impl*>(m_implementation)->set_service_thread_count(n);
}
