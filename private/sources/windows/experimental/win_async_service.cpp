#ifdef _WIN32
// Above is just to help the IDEs find the right place to stick functions...
// we should not be compiling this file unless we are on Windows...

#include "rpnx/experimental/network.hpp"

#include <windows.h>
#include <ioapiset.h>

#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace rpnx
{
    namespace experimental
    {
        inline namespace implementation
        {
            struct win32_async_service;

            struct iocp_handler
            {
                virtual ~iocp_handler() {};
                [[maybe_unused]] virtual void iocp_respond(win32_async_service& response) = 0;
            };

            struct ip6_acceptor_handler
                : public iocp_handler
            {
                virtual ~ip6_acceptor_handler();
                void iocp_respond(win32_async_service& srv) override;

                void submit_accept6(win32_async_service& srv, std::function<void(rpnx::experimental::result<async_ip6_tcp_connection>)>);
            };

            struct win32_async_service
            {

                std::vector<std::thread> m_threads;
                std::mutex m_mtx;
                std::condition_variable m_cond;
                std::atomic<bool> m_stop_signal;


                std::map<ULONG_PTR, std::unique_ptr<iocp_handler>> m_allocated_objects;

                win32_async_service();
                ~win32_async_service();

                HANDLE m_iocp_handle;

                void submit(const async_ip4_udp_send_request&)
                {
                    // TODO
                }

                void run_thread();
            };



            ip6_acceptor_handler::~ip6_acceptor_handler()
            {
            }

            void ip6_acceptor_handler::iocp_respond(win32_async_service& srv)
            {
            }

            enum class io_object_type
            {
                socket_acceptor6,

            };

        } // namespace implementation
    }     // namespace experimental
} // namespace rpnx

static inline rpnx::experimental::win32_async_service* impl(void* ptr)
{

    return reinterpret_cast< rpnx::experimental::win32_async_service* >(ptr);
}

rpnx::experimental::async_service::async_service() : m_pimpl(nullptr)
{
    m_pimpl = reinterpret_cast< void* >(new win32_async_service());
}

rpnx::experimental::async_service::~async_service()
{
    delete reinterpret_cast< win32_async_service* >(m_pimpl);
}
/*
void rpnx::experimental::async_service::submit(const async_ip4_udp_send_request& req)
{
    // TODO
   // impl(m_pimpl)->submit(req);
}
*/

/*
void rpnx::experimental::async_service::cancel_all()
{
    // TODO
}
 */

rpnx::experimental::win32_async_service::win32_async_service()
{
    m_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, ULONG_PTR(0), 0);
    // TODO: Use proper error handling

    if (m_iocp_handle == INVALID_HANDLE_VALUE) throw std::runtime_error("failed");

    m_stop_signal.store(false, std::memory_order_relaxed);

    for (int i = 0; i < std::thread::hardware_concurrency()*2; i++)
    {
        m_threads.emplace_back([this]{ run_thread(); });
    }



}

rpnx::experimental::win32_async_service::~win32_async_service()
{

    m_stop_signal.store(true);

    for (int i = 0; i < m_threads.size(); i++)
    {
        PostQueuedCompletionStatus(m_iocp_handle, 0, 0, nullptr);
    }

    for (std::thread & th : m_threads)
    {
        th.join();
    }

    CloseHandle(m_iocp_handle);

}

void rpnx::experimental::win32_async_service::run_thread()
{
   // std::unique_lock m_lock(m_mtx);
    while (true)
    {
        DWORD count{};
        ULONG_PTR ptr{};
        OVERLAPPED* overlapped_ptr {};
        auto ok = GetQueuedCompletionStatus(m_iocp_handle, &count, &ptr, &overlapped_ptr, INFINITE);

        if (!ok)
        {
            std::cerr << "Err" << get_os_network_error_code().message() << std::endl;
            return;
        }

        if (ptr == 0)
        {
            if (m_stop_signal)
            {
                std::cerr << "STOP" << std::endl;
                return;
            }
        }

        // TODO: Think of a strategy to keep track of IOCP handlers

        std::unique_lock lock(m_mtx);

        m_allocated_objects.at(ptr)->iocp_respond(*this);



    }
}

#endif
