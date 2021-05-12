//
// Created by Ryan Nicholl on 12/23/20.
//

#ifndef RPNXCORE_PRIORITY_DISPATCHER_HPP
#define RPNXCORE_PRIORITY_DISPATCHER_HPP

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>



namespace rpnx
{
    namespace experimental
    {


        class priority_dispatcher
        {

          public:
            enum class completion_state
            {
                completed,
                completed_with_exception,
                cancelled
            };
          private:
            void * m_implementation;

          private:
            template <typename T>
            static void delete_functor(void * t, completion_state) noexcept
            {
                delete reinterpret_cast<T*>(t);
            }

            template <typename T>
            static void exec_functor(void * t)
            {
                (*reinterpret_cast<T*>(t))();
            }


          public:
            priority_dispatcher();
            ~priority_dispatcher();

            /** Cancels all running operations. After this function completes, there are no pending actions.
             * Note: A job could still complete normally instead of being cancelled.
             * Note: This call will not terminate until all running executions have completed. If a running job exists, this call will block until it completes.
             * Note: It is undefined behavior to submit() a request while a call to cancel_all() is still pending.
             * Note: It is also undefined behavior to call finish_all() or cancel_all_quick() concurrently with this call.
             */
            void cancel_all();

            /** Like cancel_all, but does not wait for currently running jobs to complete before returning.
             * Therefore, it is possible the queue contains in-progress jobs when this call returns.
             * If the cleanup operation is non-blocking, then this call should not block.
             * Note: If cancel_all_quick and submit execute concurrently, then whether or not the
             * concurrently submitted job is cancelled is unspecified, but the program's behavior is
             * otherwise well defined.
             * Note: It is undefined behavior to call this function concurrently with cancel_all.
             * */
            void cancel_all_quick();

            /** Waits until all operations complete normally. This call blocks.
             * It is undefined behavior to call finish_all and any of submit, cancel_all, or cancel_all_quick concurrently.
             */
            void finish_all();

            void set_service_thread_count(std::size_t n);




            /**
             * Submit a job for asynchronous processing by this priority dispatcher.
             * @param exec The function pointer to execute during a normal execution routine. caller_data is passed as a parameter
             * @param cleanup The function pointer to execute after a normal execution of the job,
             * or when the job is cancelled. After normal execution, the bool is false, if the job
             * was cancelled, the bool is true.
             * @param caller_data Data passed into the previous callbacks.
             * @param priority An integer priority for the submission.
             */
            void submit(void(*exec)(void*), void(*cleanup) (void*, completion_state) noexcept, void* caller_data, std::int64_t priority);

            /**
             * RAII style wrapper around the C style submit call
             * @tparam F
             * @param f
             * @param priority
             */
            template <typename F>
            void submit(F f, std::int64_t priority)
            {
                F * f_copy = new F(std::move(f));

                void (*ef)(void*);
                ef = &exec_functor<F>;
                submit(ef, &delete_functor<F>, reinterpret_cast<void*>(f_copy), priority);
            }
        };

        class priority_submitter
        {
            std::int64_t m_pri;
            priority_dispatcher & m_disp;
          public:
            priority_submitter(priority_dispatcher & disp, std::int64_t priority)
            : m_pri(priority), m_disp(disp)
            {

            }

            template <typename F>
            void submit(F && f)
            {
                m_disp.submit(std::forward<F>(f), m_pri);
            }
        };
    }
}

#endif // RPNXCORE_PRIORITY_DISPATCHER_HPP
