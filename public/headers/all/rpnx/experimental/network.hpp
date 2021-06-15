#ifndef RPNXCORE_NETWORK_HPP
#define RPNXCORE_NETWORK_HPP

#include <future>
#include "rpnx/assert.hpp"
#include "rpnx/experimental/channel.hpp"

#ifdef _WIN32
#include <WinSock2.h>
#include <in6addr.h>
#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)

#if defined(__FreeBSD__)
#include <sys/types.h>
#endif

#include <fcntl.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#if defined(__FreeBSD__)

#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if defined(__linux__)
#include <sys/epoll.h>
#endif

#include <array>
#include <assert.h>
#include <climits>
#include <functional>
#include <iostream>
#include <system_error>
#include <vector>

#include "result.hpp"
#include "rpnx/network_error.hpp"
#include <set>

#include <cstring>

#include <type_traits>
#include <utility>

namespace rpnx
{
    namespace experimental
    {
#ifdef _WIN32
        namespace detail
        {

            struct async_ip6_tcp_autoacceptor_binding;

            class wsa_intializer
            {
              private:
                WSADATA wsaData;

              public:
                wsa_intializer(int a, int b)
                {
                    auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
                    if (result != 0)
                        throw std::system_error(std::error_code(result, std::system_category()), "WSA intialization failed");
                }
                ~wsa_intializer()
                {
                    WSACleanup();
                }

                static wsa_intializer& singleton()
                {
                    static wsa_intializer wsa(2, 2);
                    return wsa;
                }
            };
        } // namespace detail
        inline std::error_code get_os_network_error_code()
        {
            int er = WSAGetLastError();
            int erx = 1;

            switch (er)

            {
            case WSA_NOT_ENOUGH_MEMORY:
                return std::make_error_code(std::errc::not_enough_memory);
            case WSA_INVALID_PARAMETER:
                return std::make_error_code(std::errc::invalid_argument);
            case WSA_OPERATION_ABORTED:
                return std::make_error_code(std::errc::operation_canceled);
            case WSA_IO_INCOMPLETE:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSA_IO_PENDING:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSAEINTR:
                return std::make_error_code(std::errc::interrupted);
            case WSAEBADF:
                return std::make_error_code(std::errc::bad_file_descriptor);
            case WSAEACCES:
                return std::make_error_code(std::errc::permission_denied);
            case WSAEFAULT:
                return std::make_error_code(std::errc::bad_address);
            case WSAEMFILE:
                return std::make_error_code(std::errc::too_many_files_open);
            case WSAEWOULDBLOCK:
                return std::make_error_code(std::errc::operation_would_block);
            case WSAEINPROGRESS:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSAEALREADY:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSAEMSGSIZE:
                return std::make_error_code(std::errc::message_size);
            case WSAEPFNOSUPPORT:
                return std::make_error_code(std::errc::protocol_not_supported);
            case WSAEADDRINUSE:
                return std::make_error_code(std::errc::address_in_use);
            case WSAEADDRNOTAVAIL:
                return std::make_error_code(std::errc::address_not_available);
            case WSAENETDOWN:
                return std::make_error_code(std::errc::network_down);
            case WSAENETUNREACH:
                return std::make_error_code(std::errc::network_unreachable);
            case WSAENETRESET:
                return std::make_error_code(std::errc::network_reset);
                // case WSAEACCES: return std::make_error_code(std::errc::permission_denied);
                // case WSAEACCES: return std::make_error_code(std::errc::permission_denied);
                //            return std::make_error_code(network_errc::permission_denied);
            }

            return std::error_code(er, std::system_category());
        }
        using native_socket_type = SOCKET;
        using native_sockaddr_length_type = int;
        static const constexpr native_socket_type native_invalid_socket_value = INVALID_SOCKET;
        inline auto native_close_socket(native_socket_type skt)
        {
            return ::closesocket(skt);
        }
#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        inline std::error_code get_os_network_error_code()
        {
            return std::error_code(errno, std::system_category());
        }
        using native_socket_type = int;
        using native_sockaddr_length_type = socklen_t;
        static const constexpr native_socket_type native_invalid_socket_value = -1;
        inline auto native_close_socket(native_socket_type skt)
        {
            return ::close(skt);
        }
#endif

        template <class T1, class T2>
        constexpr T1 bit_cast(T2 const& value) noexcept
        {
            static_assert(sizeof(T1) == sizeof(T2));
            static_assert(std::is_trivially_copyable_v<T1>);            
            static_assert(std::is_trivially_copyable_v<T2>);
            T1 t;
            std::memcpy(&t, &value, sizeof(value));
            return t;
        }

        struct network_enabled_context
        {
#ifdef _WIN32
            network_enabled_context()
                : obj(2,2)
            {}
          private:
            detail::wsa_intializer obj;
#endif
        };

        class ip4_udp_socket;
        class ip4_udp_socket_ref;

        class ip4_udp_endpoint;

        class ip4_tcp_acceptor;
        class ip4_tcp_acceptor_ref;

        class ip4_tcp_connection;
        class ip4_tcp_endpoint;

        class ip6_udp_socket;
        class ip6_udp_endpoint;
        class ip6_tcp_acceptor;
        class ip6_tcp_connection;
        class ip6_tcp_endpoint;

        class async_service;

        class async_ip4_udp_socket;

        class async_ip6_tcp_acceptor;
        class async_ip6_tcp_acceptor_ref;

        class async_ip6_tcp_connection;
        class async_ip6_tcp_connection_ref;

        class async_ip6_tcp_autoacceptor;


        template < typename It >
        void net_send(ip4_udp_socket_ref socket, ip4_udp_endpoint const& to, It input_begin, It input_end);

        template < typename It >
        void net_send(ip4_tcp_connection socket, It input_begin, It input_end);

        template < typename It >
        auto net_receive(ip4_udp_socket_ref socket, ip4_udp_endpoint& from, It output_begin, It output_bounds_check) -> It;

        template < typename It >
        auto net_receive(ip4_udp_socket_ref socket, ip4_udp_endpoint& from, It output_begin) -> It;

        template < typename It >
        auto net_receive(ip4_tcp_connection& socket, It output_begin, It output_end) -> void;

        template < typename It >
        auto net_receive(ip4_tcp_connection& socket, size_t count, It output) -> void;


        void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const& bind);
        void net_bind(async_ip4_udp_socket& socket, ip4_udp_endpoint const& bind);

        void net_listen(ip4_tcp_acceptor_ref socket);

        ip4_udp_endpoint net_endpoint(ip4_udp_socket& socket);
        ip4_udp_endpoint net_endpoint(async_ip4_udp_socket& socket);

        ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor_ref);
        void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection);

        class ip6_address 
        {
#ifdef _WIN32
            IN6_ADDR m_data;
#else
            in6_addr m_data;
#endif
        public:
            constexpr ip6_address() noexcept
            : m_data{}
            { }
            
            constexpr ip6_address(ip6_address const &) = default;
            
            #ifdef _WIN32
            constexpr ip6_address(IN6_ADDR const & other)
                : m_data{}
            {
                for (int i = 0; i < size(); i++)
                {
                    m_data.u.Byte[i] = *(reinterpret_cast<unsigned char const*>(&other)+i);
                }
            }

            inline constexpr IN6_ADDR const& native() const
            {
                return m_data;
            }

            constexpr inline std::size_t size() const noexcept { return 16; }

            constexpr std::uint8_t & operator[](int index)
            {
                //RPNX_ASSERT(index < size());
                return m_data.u.Byte[index];
            }

            constexpr std::uint8_t const & operator[](int index) const
            {
                //RPNX_ASSERT(index < size());
                return m_data.u.Byte[index];
            }


            #else
            inline constexpr in6_addr const & native() const
            {
                return m_data;
            }
            #endif
            
            
            
            
        };

        class ip6_tcp_endpoint
        {
            ip6_address m_addr;
            std::uint16_t m_port;
          public:
            ip6_address& address()
            {
                return m_addr;
            }
            std::uint16_t& port()
            {
                return m_port;
            }

            ip6_address const& address() const
            {
                return m_addr;
            }
            std::uint16_t const& port() const
            {
                return m_port;
            }
        };

        class ip4_address final
        {
          private:
            using data_type = std::array< std::uint8_t, 4 >;
            data_type m_addr;

          public:
            ip4_address() noexcept : m_addr{0, 0, 0, 0}
            {
            }

            ip4_address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept : m_addr{a, b, c, d}
            {
            }

            explicit ip4_address(in_addr const& addr)
            {
#ifdef _WIN32
                m_addr[0] = addr.S_un.S_un_b.s_b1;
                m_addr[1] = addr.S_un.S_un_b.s_b2;
                m_addr[2] = addr.S_un.S_un_b.s_b3;
                m_addr[3] = addr.S_un.S_un_b.s_b4;
#elif defined(__linux__) || defined(__APPLE__)
                m_addr[0] = reinterpret_cast< char const* >(&addr.s_addr)[0];
                m_addr[1] = reinterpret_cast< char const* >(&addr.s_addr)[1];
                m_addr[2] = reinterpret_cast< char const* >(&addr.s_addr)[2];
                m_addr[3] = reinterpret_cast< char const* >(&addr.s_addr)[3];
#else
#error Implement
#endif
            }

            inline in_addr native() const noexcept
            {

#ifdef _WIN32
                in_addr addr;
                addr.S_un.S_un_b.s_b1 = m_addr[0];
                addr.S_un.S_un_b.s_b2 = m_addr[1];
                addr.S_un.S_un_b.s_b3 = m_addr[2];
                addr.S_un.S_un_b.s_b4 = m_addr[3];
                return addr;
#endif
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
                return {static_cast< in_addr_t >(m_addr[0] | (m_addr[1] << 8) | (m_addr[3] << 16) | (m_addr[3] << 24))};
#endif
            }

            using value_type = data_type::value_type;
            using iterator = data_type::iterator;
            using const_iterator = data_type::const_iterator;
            using pointer = data_type::pointer;
            using const_pointer = data_type::const_pointer;
            using reference = data_type::reference;
            using const_reference = data_type::const_reference;

            inline value_type& operator[](int n)
            {
                return m_addr[n];
            }

            inline value_type const& operator[](int n) const
            {
                return m_addr[n];
            }

            iterator begin()
            {
                return std::begin(m_addr);
            }
            const_iterator begin() const
            {
                return std::begin(m_addr);
            }

            iterator end()
            {
                return std::end(m_addr);
            }
            const_iterator end() const
            {
                return std::end(m_addr);
            }

            static ip4_address any()
            {
                ip4_address addr;
                addr[0] = 0;
                addr[1] = 0;
                addr[2] = 0;
                addr[3] = 0;
                return addr;
            }
        };

        class ip4_udp_endpoint;
        class ip4_tcp_endpoint
        {
            ip4_address m_addr;
            uint16_t m_port;

          public:
            inline ip4_tcp_endpoint() : m_addr(), m_port(0)
            {
            }
            inline ip4_tcp_endpoint(ip4_address addr, uint16_t port) : m_addr(addr), m_port(port)
            {
            }

            inline explicit ip4_tcp_endpoint(sockaddr_in const& in)
            {
                assert(in.sin_family == AF_INET);
                m_port = ntohs(in.sin_port);
                m_addr = ip4_address(in.sin_addr);
            }

            sockaddr_in native() const
            {
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(m_port);
                addr.sin_addr = m_addr.native();
                return addr;
            }

            ip4_address& address()
            {
                return m_addr;
            }
            uint16_t& port()
            {
                return m_port;
            }

            ip4_address const& address() const
            {
                return m_addr;
            }
            uint16_t const& port() const
            {
                return m_port;
            }
        };

        class ip4_udp_endpoint
        {
            ip4_address m_addr;
            uint16_t m_port;

          public:
            inline ip4_udp_endpoint() : m_port(0), m_addr()
            {
            }
            inline ip4_udp_endpoint(sockaddr_in const& native_addr) : ip4_udp_endpoint()
            {
                m_port = ntohs(native_addr.sin_port);
#ifdef _WIN32
                m_addr[0] = native_addr.sin_addr.S_un.S_un_b.s_b1;
                m_addr[1] = native_addr.sin_addr.S_un.S_un_b.s_b2;
                m_addr[2] = native_addr.sin_addr.S_un.S_un_b.s_b3;
                m_addr[3] = native_addr.sin_addr.S_un.S_un_b.s_b4;
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
                m_addr[0] = native_addr.sin_addr.s_addr >> 24;
                m_addr[1] = native_addr.sin_addr.s_addr >> 16;
                m_addr[2] = native_addr.sin_addr.s_addr >> 8;
                m_addr[3] = native_addr.sin_addr.s_addr >> 0;
#else
#error Not implemented
#endif
            }

            inline ip4_udp_endpoint(ip4_address addr, uint16_t port) : m_addr(addr), m_port(port)
            {
            }

            inline ip4_udp_endpoint(ip4_udp_endpoint const&) = default;

            explicit inline ip4_udp_endpoint(ip4_tcp_endpoint const& other) : m_addr(other.address()), m_port(other.port())
            {
            }

            sockaddr_in native() const
            {
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(m_port);
                addr.sin_addr = m_addr.native();
                return addr;
            }

            ip4_address& address()
            {
                return m_addr;
            }
            uint16_t& port()
            {
                return m_port;
            }

            ip4_address const& address() const
            {
                return m_addr;
            }
            uint16_t const& port() const
            {
                return m_port;
            }
        };

        class ip4_udp_socket
        {
          public:
            native_socket_type m_s;

          public:
            ip4_udp_socket() noexcept
            {
                m_s = native_invalid_socket_value;
            }

            ip4_udp_socket(ip4_udp_socket const&) = delete;

            inline ip4_udp_socket(ip4_udp_socket&& other) noexcept : ip4_udp_socket()
            {
                std::swap(m_s, other.m_s);
            }

            ~ip4_udp_socket()
            {
                close();
            }

            template < typename It >
            auto receive(ip4_udp_endpoint& from, It out_iterator) -> It
            {
                return net_receive(*this, from, out_iterator);
            }

            template < typename It >
            auto receive(ip4_udp_endpoint& from, It out_iterator, It out_iterator_bounds) -> It
            {
                return net_receive(*this, from, out_iterator, out_iterator_bounds);
            }

            template < typename It >
            void send(ip4_udp_endpoint const& destination, It begin, It end)
            {
                net_send(*this, destination, begin, end);
            }

            void bind(ip4_udp_endpoint ep)
            {
                net_bind(*this, ep);
            }

            void bind(ip4_address addr)
            {
                ip4_udp_endpoint ep;
                ep.address() = addr;
                ep.port() = 0;
                net_bind(*this, ep);
            }

            void bind(uint16_t portno)
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = portno;
                net_bind(*this, ep);
            }

            void bind()
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = 0;
                net_bind(*this, ep);
            }

            ip4_udp_endpoint endpoint()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                sockaddr_in addr;
                int len = sizeof(addr);

                auto result = ::getsockname(m_s, (sockaddr*)&addr, &len);

                if (result == SOCKET_ERROR)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#else
                sockaddr_in addr;
                socklen_t len = sizeof(addr);

                auto result = ::getsockname(m_s, (sockaddr*)&addr, &len);

                if (result == native_invalid_socket_value)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#endif
            }

            void close()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
#endif
                if (m_s != native_invalid_socket_value)
                {
                    native_close_socket(m_s);
                    m_s = native_invalid_socket_value;
                }

            }

            void open()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                close();
                m_s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (m_s == native_invalid_socket_value)
                    throw network_error("udp_ip4_socket::open()", get_os_network_error_code());
#else
                this->close();
                m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (m_s == native_invalid_socket_value)
                    throw network_error("udp_ip4_socket::open()", get_os_network_error_code());
#endif
            }

            native_socket_type native() const noexcept
            {
                return m_s;
            }
        };

        class ip4_tcp_acceptor
        {

          private:
            native_socket_type m_sock;

          public:
            ip4_tcp_acceptor() noexcept : m_sock(native_invalid_socket_value)
            {
            }

            ~ip4_tcp_acceptor()
            {
                close();
            }

            ip4_tcp_acceptor(ip4_tcp_acceptor const&) = delete;

            ip4_tcp_acceptor(ip4_tcp_acceptor&& other) noexcept : m_sock(native_invalid_socket_value)
            {
                std::swap(m_sock, other.m_sock);
            }

            void close() noexcept
            {
                if (m_sock != native_invalid_socket_value)
                {
                    native_close_socket(m_sock);
                    m_sock = native_invalid_socket_value;
                }

            }

            void open()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                close();
                m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (m_sock == INVALID_SOCKET)
                    throw network_error("tcp_ip4_socket::open()+::socket", get_os_network_error_code());
#else
                close();
                m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (m_sock == native_invalid_socket_value)
                    throw network_error("tcp_ip4_socket::open()", get_os_network_error_code());
#endif
            }

            void listen(ip4_tcp_endpoint const& listen_addr)
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                open();
                ::sockaddr_in sock_addr = listen_addr.native();
                if (::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
                {
                    throw network_error("tcp_ip4_socket::open()+::bind", get_os_network_error_code());
                }
                if (::listen(m_sock, SOMAXCONN) == SOCKET_ERROR)
                {
                    throw network_error("tcp_ip4_socket::open()+::listen", get_os_network_error_code());
                }
#else
                this->open();
                ::sockaddr_in sock_addr = listen_addr.native();
                ::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in));
                if (::listen(m_sock, SOMAXCONN) == native_invalid_socket_value)
                {
                    throw network_error("tcp_ip4_socket::open()", get_os_network_error_code());
                }
#endif
            }

#ifdef _WIN32
            [[nodiscard]] SOCKET native() const noexcept
            {
                return m_sock;
            }
#else
            int native() const noexcept
            {
                return m_sock;
            }
#endif
        };

        class [[maybe_unused]] ip4_udp_socket_ref
        {
            native_socket_type m_socket;

          public:
            [[maybe_unused]] ip4_udp_socket_ref() : m_socket(-1) {}
            [[maybe_unused]] ip4_udp_socket_ref(ip4_udp_socket_ref const &) = default;
            [[maybe_unused]] ip4_udp_socket_ref(ip4_udp_socket const & other)
                : m_socket(other.native())
            {}

            auto native() const noexcept
            {
                return m_socket;
            }

            [[maybe_unused]] static inline ip4_udp_socket_ref from_native(native_socket_type native_value)
            {
                ip4_udp_socket_ref result;
                result.m_socket = native_value;
                return result;
            }
        };

        class [[maybe_unused]] ip4_tcp_acceptor_ref
        {
            native_socket_type m_socket;

          public:
            [[maybe_unused]] ip4_tcp_acceptor_ref() : m_socket(-1) {}
            [[maybe_unused]] ip4_tcp_acceptor_ref(ip4_tcp_acceptor_ref const &) = default;
            [[maybe_unused]] ip4_tcp_acceptor_ref(ip4_tcp_acceptor const & other)
                : m_socket(other.native())
            {}

            auto native() const noexcept
            {
                return m_socket;
            }

            [[maybe_unused]] static inline ip4_tcp_acceptor_ref from_native(decltype(m_socket) native_value)
            {
                ip4_tcp_acceptor_ref result;
                result.m_socket = native_value;
                return result;
            }
        };

        class [[maybe_unused]] ip4_tcp_connection_ref
        {
            friend class ip4_tcp_connection;

            native_socket_type m_socket;

          public:

            ip4_tcp_connection_ref() : m_socket(-1){}
            ip4_tcp_connection_ref(ip4_tcp_connection_ref const &) = default;
            ip4_tcp_connection_ref(ip4_tcp_connection const &);

            native_socket_type native() const noexcept
            {
                return m_socket;
            }

            [[maybe_unused]] static inline ip4_tcp_connection_ref from_native(decltype(m_socket) native_value)
            {
                ip4_tcp_connection_ref result;
                result.m_socket = native_value;
                return result;
            }
        };

        class ip4_tcp_connection
        {
          private:
            native_socket_type m_socket;
          public:
            ip4_tcp_connection() noexcept : m_socket(-1)
            {
            }

#ifndef _WIN32
            ip4_tcp_connection(ip4_tcp_endpoint const& ep)
            {
                m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (m_socket == -1)
                {
                    throw network_error("ip4_tcp_connection::ip4_tcp_connection", get_os_network_error_code());
                }

                sockaddr_in addr = ep.native();
                int c = ::connect(m_socket, (sockaddr const*)&addr, sizeof(addr));
                if (m_socket == -1)
                {
                    throw network_error("ip4_tcp_connection::ip4_tcp_connection", get_os_network_error_code());
                }
            }
#endif
            // TODO: Implement on windows

            ip4_tcp_connection(ip4_tcp_connection const&) = delete;

            ip4_tcp_connection(ip4_tcp_connection&& other) noexcept : m_socket(-1)
            {
                std::swap(other.m_socket, m_socket);
            }

            /** Destructor of the socket. Either calls abort() or close() (it is unspecified which is called)
             */
            ~ip4_tcp_connection()
            {
                close(); // TODO: Should be 'abort' by default
                // abort isn't yet implemented.
            }

            /**
              Create a socket from a native SOCKET object.
              Ownership of the SOCKET is assumed by this class.
            */
            explicit inline ip4_tcp_connection(native_socket_type s) noexcept : m_socket(s)
            {
            }

            inline ip4_tcp_connection& operator=(ip4_tcp_connection&& other) noexcept
            {
                std::swap(other.m_socket, m_socket);
                return *this;
            }

            ip4_tcp_endpoint endpoint()
            {
                sockaddr_in saddr;
#ifdef _WIN32
                int sz = sizeof(saddr);
#else
                socklen_t sz = sizeof(saddr);
#endif
                if (getsockname(m_socket, (sockaddr*)&saddr, &sz) != 0)
                {
                    throw network_error("ip4_tcp_connection::endpoint", get_os_network_error_code());
                }

                return ip4_tcp_endpoint(saddr);
            }

            ip4_tcp_endpoint peer_endpoint()
            {

                sockaddr_in saddr;
#ifdef _WIN32
                int sz = sizeof(saddr);
#else
                socklen_t sz = sizeof(saddr);
#endif
                if (getpeername(m_socket, (sockaddr*)&saddr, &sz) != 0)
                {
                    throw network_error("peer_endpoint", get_os_network_error_code());
                }

                return ip4_tcp_endpoint(saddr);
            }

            /**
              Returns a copy of the internal SOCKET. This class retains ownership of the SOCKET.
            */
            [[nodiscard]] native_socket_type native() const noexcept
            {
                return m_socket;
            }

            /**
             Extracts the internal SOCKET. The function relinquishes ownership of the socket and the class's internal socket becomes INVALID_SOCKET.
            */
            [[nodiscard]] native_socket_type extract_native() noexcept
            {
                native_socket_type skt = m_socket;
                m_socket = -1;
                return skt;
            }

            [[nodiscard]] bool valid() const noexcept
            {
                return m_socket != -1;
            }

            template < typename It >

            void send(It begin, It end)
            {
                net_send(*this, begin, end);
            }

            /* TODO
            void abort()
            {

            }*/

            void close()
            {
                if (m_socket != -1)
                {
#ifdef _WIN32
                    shutdown(m_socket, SD_BOTH);
                    closesocket(m_socket);
#else
                    shutdown(m_socket, SHUT_RDWR);
                    ::close(m_socket);
#endif
                    m_socket = -1;
                }
            }
        };

        inline ip4_tcp_connection_ref::ip4_tcp_connection_ref(const ip4_tcp_connection& other)
        :m_socket(other.native())
        {

        }

        template < typename It >
        void net_send(ip4_udp_socket_ref socket, ip4_udp_endpoint const& to, It begin_packet, It end_packet)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
            sockaddr_in dest = to.native();
            std::vector< char > data;
            for (auto it = begin_packet; it != end_packet; ++it)
            {
                data.emplace_back((char)*it);
            }

            auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
            if (result == SOCKET_ERROR)
            {
                throw network_error("upd_ip4_socket::send()", get_os_network_error_code());
            }
#else
            sockaddr_in dest = to.native();
            std::vector< std::byte > data(begin_packet, end_packet);

            auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
            if (result == -1)
            {
                throw network_error("upd_ip4_socket::send()", get_os_network_error_code());
            }
            return;
#endif
        }

        template < typename It >
        void net_send(ip4_tcp_connection_ref socket, It begin_data, It end_data)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();

            std::vector< char > data;
            data.assign(begin_data, end_data);

            // TODO: Right now we don't support sending more than 2^32 bytes at once on windows
            // Ideally we should fix this
            if (data.size() >= INT_MAX)
                throw std::invalid_argument("send too large");

            std::size_t st = 0;

            while (st != data.size())
            {
                auto count = ::send(socket.native(), (const char*)data.data() + st, data.size() - st, 0);

                if (count != SOCKET_ERROR)
                {
                    st += count;
                }
                else
                {
                    throw network_error("rpnx::net_send", get_os_network_error_code());
                }
            }

#else

            std::vector< char > data(begin_data, end_data);

            std::size_t st = 0;

            while (st != data.size())
            {
                auto count = ::send(socket.native(), (const char*)data.data() + st, data.size() - st, 0);

                if (count != -1)
                {
                    st += count;
                }
                else
                {
                    throw network_error("rpnx::net_send", get_os_network_error_code());
                }
            }

#endif
        }

        template < typename It >
        auto net_receive(ip4_udp_socket_ref socket, ip4_udp_endpoint& fr, It begin_packet, It end_packet) -> It
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            std::array< std::byte, 256 * 256 > buffer;
            sockaddr_in from;
            static_assert(sizeof(from) < INT_MAX);
#ifdef _WIN32
            int fromlen = sizeof(from);
#else
            socklen_t fromlen = sizeof(from);
#endif
            auto count = recvfrom(socket.native(), reinterpret_cast< char* >(buffer.data()), (int)buffer.size(), 0, reinterpret_cast< sockaddr* >(&from), &fromlen);
            if (count == -1)
            {
                throw network_error("upd_ip4_socket::receive()", get_os_network_error_code());
            }

            It it = begin_packet;
            auto buf_it = buffer.begin();
            auto buf_end = buffer.begin() + count;
            while (it != end_packet && buf_it != buf_end)
            {
                *it = *buf_it;
                ++it;
                ++buf_it;
            }

            if (it == end_packet && buf_it != buf_end)
            {
                throw std::out_of_range("Iterator out of range in rpnx::net_receive");
            }

            fr = ip4_udp_endpoint(from);

            return it;
        }

        template < typename It >
        auto net_receive(ip4_udp_socket_ref socket, ip4_udp_endpoint& fr, It begin_packet) -> It
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            std::array< char, 256 * 256 > buffer;
            sockaddr_in from{};
            static_assert(sizeof(from) < INT_MAX);

#ifdef _WIN32
            int from_len = sizeof(from);
#else
            socklen_t from_len = sizeof(from);
#endif

            auto count = recvfrom(socket.native(), reinterpret_cast< char* >(buffer.data()), (int)buffer.size(), 0, reinterpret_cast< sockaddr* >(&from), &from_len);
            if (count == -1)
            {
                throw network_error("upd_ip4_socket::receive()", get_os_network_error_code());
            }

            It it = begin_packet;
            auto buf_it = buffer.begin();
            auto buf_end = buffer.begin() + count;
            while (buf_it != buf_end)
            {
                *it = std::byte(*buf_it);
                ++it;
                ++buf_it;
            }
            fr = ip4_udp_endpoint(from);
            return it;
        }

        inline void net_bind(ip4_udp_socket_ref socket, ip4_udp_endpoint const& bind_addr)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            sockaddr_in socket_addr = bind_addr.native();
            auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
            if (result == -1)
            {
                throw network_error("upd_ip4_socket::bind()", get_os_network_error_code());
            }
            return;
        }



        inline ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor_ref socket)
        {
#ifdef _WIN32
            ip4_tcp_connection con = ip4_tcp_connection(::accept(socket.native(), nullptr, nullptr));
            if (con.native() == INVALID_SOCKET)
            {
                throw network_error("rpnx::net_accept_connection", get_os_network_error_code());
            }

            return std::move(con);
#else
            ip4_tcp_connection con = ip4_tcp_connection(::accept(socket.native(), nullptr, nullptr));
            if (con.native() == -1)
            {
                throw network_error("rpnx::net_accept_connection", get_os_network_error_code());
            }

            return std::move(con);
#endif
        }

        inline void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection)
        {
            connection = net_accept_connection(socket);
        }



        struct async_ip4_udp_send_request
        {
            ip4_udp_socket* socket;
            void const* data;
            std::size_t data_len;
            std::function< void() > callback;
        };

        class async_ip4_udp_socket
        {
            friend void net_bind(async_ip4_udp_socket &, ip4_udp_endpoint const &);
          private:
            native_socket_type m_socket;

          public:

            auto endpoint()
            {
                return net_endpoint(*this);
            }

#ifndef _WIN32
            bool is_open() const { return m_socket != -1; }
            void open()
            {
                m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (m_socket == -1)
                {
                    throw std::runtime_error("SOCKET");
                }
            }

            [[nodiscard]] native_socket_type native() const
            {
                return m_socket;
            }

#endif


#ifdef _WIN32
            async_ip4_udp_socket()
                : m_socket(INVALID_SOCKET)
            {


            }
            bool is_open() const
            {
                return m_socket != -1;
            }

            void close()
            {
                if (is_open()) return;

                ::closesocket(m_socket);

                m_socket = -1;
            }

            [[nodiscard]] auto native() const
            {
                return m_socket;
            }

            void open()
            {
                if (is_open()) close();
                m_socket = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
                if (m_socket == -1)
                {
                    throw std::runtime_error("WSA SOCKET");
                }
            }

            void bind(ip4_udp_endpoint ep)
            {
                net_bind(*this, ep);
            }

            void bind(ip4_address addr)
            {
                ip4_udp_endpoint ep;
                ep.address() = addr;
                ep.port() = 0;
                net_bind(*this, ep);
            }

            void bind(uint16_t portno)
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = portno;
                net_bind(*this, ep);
            }

            void bind()
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = 0;
                net_bind(*this, ep);
            }


#endif
        };

        class async_service
        {
            friend class async_ip6_tcp_autoacceptor;
          private:
            void * m_pimpl;


          public:
            async_service();
            ~async_service();

          private:
            [[maybe_unused]] void bind_autoaccept6(detail::async_ip6_tcp_autoacceptor_binding &binding);
            [[maybe_unused]] void unbind_autoaccept6(detail::async_ip6_tcp_autoacceptor_binding &binding);

            //void submit(async_ip4_udp_send_request const& req);
            //void cancel_all();

        };

        template<typename Submitter, typename T>
        class action_channel_transfer
        {
            friend class async_service;
            Submitter & m_sub_ref;
            channel<T> & m_channel_ref;
          public:
            action_channel_transfer(Submitter & sub, channel<T> & chan)
            : m_sub_ref(sub), m_channel_ref(chan)
            {}
        };

        //template <typename MainAction, typename ... AuxActions>
        class async_ip6_tcp_autoacceptor;

        class async_ip6_tcp_acceptor
        {
            native_socket_type m_socket;
          public:
#ifdef _WIN32
            async_ip6_tcp_acceptor()
            {
                m_socket = WSASocketW( AF_INET6, SOCK_STREAM, IPPROTO_TCP,
                                      nullptr, 0, WSA_FLAG_OVERLAPPED);
                if (m_socket == INVALID_SOCKET) throw network_error("async_ip6_tcp_acceptor::async_ip6_tcp_acceptor()", get_os_network_error_code());
            }


#else
            async_ip6_tcp_acceptor()
            {
                m_socket = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

                ::fcntl(m_socket, F_SETFL, O_NONBLOCK);
                // TODO: Error checking here
            }
#endif

            native_socket_type native() const
            {
                return m_socket;
            }
        };

        class async_ip6_tcp_acceptor_ref
        {
            native_socket_type m_socket;

          public:
            async_ip6_tcp_acceptor_ref() : m_socket(native_invalid_socket_value) {}
            async_ip6_tcp_acceptor_ref(async_ip6_tcp_acceptor_ref const &) = default;
            async_ip6_tcp_acceptor_ref(async_ip6_tcp_acceptor const & other)
            : m_socket(other.native())
            {}

            [[nodiscard]] native_socket_type native() const noexcept
            {
                return m_socket;
            }
        };

        namespace detail
        {
            struct async_ip6_tcp_autoacceptor_binding
            {
                std::function<void(rpnx::experimental::result<async_ip6_tcp_connection>)> m_mainaction;
                std::vector<std::function<void()> > m_auxactions;
                async_ip6_tcp_acceptor_ref m_socket;
#ifdef _WIN32
                OVERLAPPED m_overlapped;
                SOCKET m_accept_on_socket;
#endif
            };
        }

        class async_ip6_tcp_autoacceptor
        {

            async_service & m_async;
            detail::async_ip6_tcp_autoacceptor_binding m_binding;

          public:
            template <typename MainAction, typename ... AuxActions>
            [[maybe_unused]] async_ip6_tcp_autoacceptor(async_ip6_tcp_acceptor_ref socket
                                       , async_service & async
                                       , ip6_tcp_endpoint ep
                                       , MainAction completion_action
                                       , AuxActions && ... aux_actions
                                       );

            ~async_ip6_tcp_autoacceptor();

          private:
            void run_loop()
            {
                // TODO

            }
        };


        class async_ip6_tcp_connection
        {
          private:
            native_socket_type m_socket;

          public:
            async_ip6_tcp_connection()
             : m_socket(native_invalid_socket_value)
            {}

            ~async_ip6_tcp_connection()
            {
                if (m_socket != native_invalid_socket_value)
                {
                    native_close_socket(m_socket);
                }
            }
        };




        inline void net_bind(async_ip4_udp_socket& socket, ip4_udp_endpoint const& bind_addr)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            sockaddr_in socket_addr = bind_addr.native();
            auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
            if (result == -1)
            {
                throw network_error("upd_ip4_socket::bind()", get_os_network_error_code());
            }
            return;
        }

        // "asynchronously" binds a socket.
        // Note: Synchronous sockets that are bound asynchronously, actually bind synchronously in a different thread.
        inline std::future<void> net_bind_async(ip4_udp_socket& socket, ip4_udp_endpoint const& bind_addr)
        {
            return std::async([&socket, bind_addr] {
#ifdef _WIN32
               detail::wsa_intializer::singleton();
#endif
               sockaddr_in socket_addr = bind_addr.native();
               auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
               if (result == -1)
               {
                   throw network_error("upd_ip4_socket::bind()", get_os_network_error_code());
               }
               return;
            });
        }

        inline ip4_udp_endpoint net_endpoint(ip4_udp_socket& socket)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
            sockaddr_in addr;
            int len = sizeof(addr);

            auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

            if (result == SOCKET_ERROR)
            {
                throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
            }
            assert(len == sizeof(addr));
            return addr;
#else
            sockaddr_in addr;
                socklen_t len = sizeof(addr);

                auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

                if (result == -1)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#endif
        }
        inline ip4_udp_endpoint net_endpoint(async_ip4_udp_socket& socket)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
            sockaddr_in addr;
            int len = sizeof(addr);

            auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

            if (result == SOCKET_ERROR)
            {
                throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
            }
            assert(len == sizeof(addr));
            return addr;
#else
            sockaddr_in addr;
                socklen_t len = sizeof(addr);

                auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

                if (result == -1)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#endif
        }

        async_service& default_async_service();



        template < typename MainAction, typename... AuxActions >
        async_ip6_tcp_autoacceptor::async_ip6_tcp_autoacceptor(async_ip6_tcp_acceptor_ref socket, async_service& async, ip6_tcp_endpoint ep, MainAction completion_action, AuxActions&&... aux_actions)
        {

            m_binding.m_socket = socket;
            m_binding.m_mainaction = completion_action;
            (m_binding.m_auxactions.push_back(std::forward<AuxActions>(aux_actions)),...);
            m_async.bind_autoaccept6(this->m_binding);
        }

        inline async_ip6_tcp_autoacceptor::~async_ip6_tcp_autoacceptor()
        {
            m_async.unbind_autoaccept6(this->m_binding);
        }

    } // namespace experimental
} // namespace rpnx

inline std::ostream& operator<<(std::ostream& lhs, rpnx::experimental::ip4_address const& addr)
{
    return lhs << (int)addr[0] << "." << (int)addr[1] << "." << (int)addr[2] << "." << (int)addr[3];
}

inline std::ostream& operator<<(std::ostream& lhs, rpnx::experimental::ip4_udp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}

inline std::ostream& operator<<(std::ostream& lhs, rpnx::experimental::ip4_tcp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}

#endif
