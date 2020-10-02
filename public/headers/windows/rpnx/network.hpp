#ifndef RPNXCORE_WINDOWS_NETWORK_HPP
#define RPNXCORE_WINDOWS_NETWORK_HPP

//#include <winsock.h>
#include <winsock2.h>
#include <array>
#include <system_error>
#include <assert.h>
#include <vector>
#include <iostream>
#include "rpnx/network_error.hpp"

namespace rpnx
{
    namespace detail
    {
        class wsa_intializer
        {
        private:
            WSADATA wsaData;
        public:
            wsa_intializer(int a, int b)
            {
                auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
                if (result != 0) throw std::system_error(std::error_code(result, std::system_category()), "WSA intialization failed");
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
    }
    inline std::error_code get_wsa_error_code()
    {
        int er = WSAGetLastError();
        int erx = 1;

        if (er == WSAEACCES)
        {
            return std::make_error_code(network_errc::permission_denied);
        }

        return std::error_code(er, std::system_category());
    }


    class ip4_udp_socket;
    class ip4_udp_endpoint;
    class ip4_tcp_acceptor;
    class ip4_tcp_connection;
    class ip4_tcp_endpoint;

    template <typename It>
    void net_send(ip4_udp_socket& socket, ip4_udp_endpoint const& to, It input_begin, It input_end);

    template <typename It>
    auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& from, It output_begin, It output_bounds_check)->It;

    template <typename It>
    auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& from, It output_begin)->It;

    void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const& bind);

    void net_listen(ip4_tcp_acceptor& socket, ip4_tcp_endpoint const& addr);

    ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor&);
    void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection);

    template <typename It>
    auto net_receive(ip4_tcp_connection& socket, It output_begin, It output_end)->void;

    template <typename It>
    auto net_receive(ip4_tcp_connection& socket, size_t count, It output ) -> void;

   

  class ip4_address final
  {
  private:
      using data_type = std::array<std::uint8_t, 4>;
      data_type m_val;
  public:
      ip4_address() noexcept
          : m_val{0,0,0,0}
      {

      }

      ip4_address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept
          : m_val{a,b,c,d}
      {

      }

      explicit ip4_address(in_addr const& addr)
      {
          m_val[0] = addr.S_un.S_un_b.s_b1;
          m_val[1] = addr.S_un.S_un_b.s_b2;
          m_val[2] = addr.S_un.S_un_b.s_b3;
          m_val[3] = addr.S_un.S_un_b.s_b4;
      }

    inline in_addr native() const noexcept
    {
      in_addr addr;
      addr.S_un.S_un_b.s_b1 = m_val[0];
      addr.S_un.S_un_b.s_b2 = m_val[1];
      addr.S_un.S_un_b.s_b3 = m_val[2];
      addr.S_un.S_un_b.s_b4 = m_val[3];
      return addr;
    }

    
    using value_type = data_type::value_type;
    using iterator = data_type::iterator;
    using const_iterator = data_type::const_iterator;
    using pointer = data_type::pointer;
    using const_pointer = data_type::const_pointer;
    using reference = data_type::reference;
    using const_reference = data_type::const_reference;

    inline value_type & operator [] (int n)
    {
        return m_val[n];
    }

    inline value_type const& operator [](int n) const
    {
        return m_val[n];
    }

    iterator begin() { return std::begin(m_val); }
    const_iterator begin() const { return std::begin(m_val); }

    iterator end() { return std::end(m_val); }
    const_iterator end() const { return std::end(m_val); }


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

      inline ip4_tcp_endpoint()
          :m_addr(), m_port(0)
      {

      }
      inline ip4_tcp_endpoint(ip4_address addr, uint16_t port)
      : m_addr(addr), m_port(port)
      {}

      inline explicit ip4_tcp_endpoint(sockaddr_in const & in)
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

      ip4_address& address() { return m_addr; }
      uint16_t& port() { return m_port;  }

      ip4_address const& address() const { return m_addr; }
      uint16_t const& port() const { return m_port;  }
  };


  class ip4_udp_endpoint
  {
      ip4_address m_addr;
      uint16_t m_port;
  public:
      inline ip4_udp_endpoint() :m_port(0), m_addr() {
          
      }
      inline ip4_udp_endpoint(sockaddr_in const & native_addr)
          : ip4_udp_endpoint()
      {
          m_port = ntohs(native_addr.sin_port);
          m_addr[0] = native_addr.sin_addr.S_un.S_un_b.s_b1;
          m_addr[1] = native_addr.sin_addr.S_un.S_un_b.s_b2;
          m_addr[2] = native_addr.sin_addr.S_un.S_un_b.s_b3;
          m_addr[3] = native_addr.sin_addr.S_un.S_un_b.s_b4;
      }

      inline ip4_udp_endpoint(ip4_address addr, uint16_t port)
          : m_addr(addr), m_port(port)
      {
          
      }

      inline ip4_udp_endpoint(ip4_udp_endpoint const&) = default;
      
      explicit inline ip4_udp_endpoint(ip4_tcp_endpoint const& other)
          : m_addr(other.address()), m_port(other.port())
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

      ip4_address& address() { return m_addr; }
      uint16_t& port() { return m_port; }

      ip4_address const& address() const { return m_addr; }
      uint16_t const& port() const { return m_port; }
  };


  class ip4_udp_socket
  {
      SOCKET m_s;
  public:
      ip4_udp_socket() noexcept
      {
          m_s = INVALID_SOCKET;
      }

      ip4_udp_socket(ip4_udp_socket const&) = delete;

      inline ip4_udp_socket(ip4_udp_socket&& other) noexcept
          : ip4_udp_socket()
      {
          std::swap(m_s, other.m_s);
      }


      ~ip4_udp_socket()
      {
          close();
      }

      template <typename It> 
      auto receive(ip4_udp_endpoint & from, It  out_iterator) -> It
      {
          return net_receive(*this, from, out_iterator);
      }

      template <typename It>
      auto receive(ip4_udp_endpoint& from, It out_iterator, It out_iterator_bounds) -> It
      {
          return net_receive(*this, from, out_iterator, out_iterator_bounds);
      }

      template <typename It>
      void send(ip4_udp_endpoint const & destination, It begin, It end)
      {
          net_send(*this, destination, begin, end);
      }

      void bind(ip4_udp_endpoint ep)
      {
          net_bind(*this, ep);
      }

      ip4_udp_endpoint endpoint()
      {
          detail::wsa_intializer::singleton();
          sockaddr_in addr;
          int len = sizeof(addr);
          auto result = getsockname(m_s, (sockaddr*)&addr, &len);
          if (result == SOCKET_ERROR)
          {
              throw network_error("upd_ip4_socket::endpoint()", get_wsa_error_code());
          }
          assert(len == sizeof(addr));
          return addr;
      }

      void close()
      {
          detail::wsa_intializer::singleton();
          if (m_s != INVALID_SOCKET)
          {
              closesocket(m_s);
              m_s = INVALID_SOCKET;
          }
      }

      void open()
      {
          detail::wsa_intializer::singleton();
          close();
          m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
          if (m_s == INVALID_SOCKET) throw network_error("udp_ip4_socket::open()", get_wsa_error_code());
      }

      SOCKET native() const noexcept
      {
          return m_s;
      }

  };

  class ip4_tcp_acceptor

  {
  private:
      SOCKET m_sock;
  public:
      ip4_tcp_acceptor() noexcept
          : m_sock(INVALID_SOCKET)
      {

      }

      ~ip4_tcp_acceptor()
      {
          close();
      }

      ip4_tcp_acceptor(ip4_tcp_acceptor const&) = delete;

      ip4_tcp_acceptor(ip4_tcp_acceptor&& other) noexcept
          :m_sock(INVALID_SOCKET)
      {
          std::swap(m_sock, other.m_sock);
      }


      void close() noexcept
      {
          //detail::wsa_intializer::singleton();
          if (m_sock != INVALID_SOCKET)
          {
              ::closesocket(m_sock);
              m_sock = INVALID_SOCKET;
          }
      }

      void open()
      {
          detail::wsa_intializer::singleton();
          close();
          m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
          if (m_sock == INVALID_SOCKET) throw network_error("tcp_ip4_socket::open()", get_wsa_error_code());
      }

      void listen(ip4_tcp_endpoint const & listen_addr)
      {
          detail::wsa_intializer::singleton();
          open();
          ::sockaddr_in sock_addr = listen_addr.native();
          ::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in));
          if (::listen(m_sock, SOMAXCONN) == SOCKET_ERROR) 
          {
              throw network_error("tcp_ip4_socket::open()", get_wsa_error_code());
          }
              
      }

      SOCKET native() const noexcept
      {
          return m_sock;
      }
  };

  class ip4_tcp_connection
  {
  private:
      SOCKET m_socket;
  public:
      ip4_tcp_connection() noexcept
          : m_socket(INVALID_SOCKET)
      {

      }

      ip4_tcp_connection(ip4_tcp_connection const&) = delete;

      ip4_tcp_connection(ip4_tcp_connection&& other) noexcept
          : m_socket(INVALID_SOCKET)
      {
          std::swap(other.m_socket, m_socket);
      }

      /**
        Create a socket from a native SOCKET object.
        Ownership of the SOCKET is assumed by this class.
      */
      explicit inline ip4_tcp_connection(SOCKET s) noexcept
          :m_socket(s)
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
          int sz = sizeof(saddr);
          if (getsockname(m_socket, (sockaddr*)&saddr, &sz) != 0)
          {
              throw network_error("ip4_tcp_connection::endpoint", get_wsa_error_code());
          }

          return ip4_tcp_endpoint(saddr);
      }

      ip4_tcp_endpoint peer_endpoint()
      {
          sockaddr_in saddr;
          int sz = sizeof(saddr);
          if (getpeername(m_socket, (sockaddr*)&saddr, &sz) != 0)
          {
              throw network_error("peer_endpoint", get_wsa_error_code());
          }

          return ip4_tcp_endpoint(saddr);

      }

      /**
        Returns a copy of the internal SOCKET. This class retains ownership of the SOCKET.
      */
      SOCKET native() const noexcept
      {
          return m_socket;
      }

      /**
       Extracts the internal SOCKET. The function relinquishes ownership of the socket and the class's internal socket becomes INVALID_SOCKET.
      */
      SOCKET extract_native() noexcept
      {
          SOCKET skt = m_socket;
          m_socket = INVALID_SOCKET;
          return skt;
      }

      bool valid() const noexcept
      {
          return m_socket != INVALID_SOCKET;
      }
  };

  template <typename It>
  void net_send(ip4_udp_socket& socket, ip4_udp_endpoint const & to, It begin_packet, It end_packet)
  {
      detail::wsa_intializer::singleton();
      sockaddr_in dest = to.native();
      std::vector<std::byte> data(begin_packet, end_packet);

      auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
      if (result == SOCKET_ERROR)
      {
          throw network_error("upd_ip4_socket::send()", get_wsa_error_code());
      }
      return;
  }


  template <typename It>
  void net_send(ip4_tcp_connection& socket, It begin_data, It end_data)
  {
      detail::wsa_intializer::singleton();
      std::vector<std::byte> data(begin_data, end_data);

      // TODO: Right now we don't support sending more than 2^32 bytes at once
      // Ideally we should fix this
      if (data.size() >= INT_MAX) throw std::bad_alloc("send too large");
      
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
              throw network_error("rpnx::net_send", get_wsa_error_code());
          }

      }
  }

  template <typename It>
  auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& fr, It begin_packet, It end_packet) -> It
  {
      detail::wsa_intializer::singleton();
      std::array<std::byte, 256 * 256> buffer;
      sockaddr_in from;
      static_assert(sizeof(from) < INT_MAX);
      int fromlen = sizeof(from);
      auto count = recvfrom(socket.native(), reinterpret_cast<char*>(buffer.data()), (int)buffer.size(), 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
      if (count == SOCKET_ERROR)
      {
          throw network_error("upd_ip4_socket::receive()", get_wsa_error_code());
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

  template <typename It>
  auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& fr, It begin_packet) -> It
  {
      detail::wsa_intializer::singleton();
      std::array<std::byte, 256 * 256> buffer;
      sockaddr_in from;
      static_assert(sizeof(from) < INT_MAX);
      int fromlen = sizeof(from);
      auto count = recvfrom(socket.native(), reinterpret_cast<char*>(buffer.data()), (int)buffer.size(), 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
      if (count == SOCKET_ERROR)
      {
          throw network_error("upd_ip4_socket::receive()", get_wsa_error_code());
      }

      It it = begin_packet;
      auto buf_it = buffer.begin();
      auto buf_end = buffer.begin() + count;
      while (buf_it != buf_end)
      {
          *it = *buf_it;
          ++it;
          ++buf_it;
      }
      fr = ip4_udp_endpoint(from);
      return it;
  }

  inline void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const & bind_addr)
  {
      detail::wsa_intializer::singleton();
      sockaddr_in socket_addr = bind_addr.native();
      auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
      if (result == SOCKET_ERROR)
      {
          throw network_error("upd_ip4_socket::bind()", get_wsa_error_code());
      }
      return;
  }


  inline void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection)
  {
      connection = net_accept_connection(socket);
  }

  inline ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor& socket)
  {
      ip4_tcp_connection con = ip4_tcp_connection(::accept(socket.native(), nullptr, nullptr));
      if (con.native() == INVALID_SOCKET)
      {
          throw network_error("rpnx::net_accept_connection", get_wsa_error_code());
      }

      return std::move(con);
  }
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_address const& addr)
{
    return lhs << (int) addr[0] << "." << (int) addr[1] << "." << (int) addr[2] << "." << (int) addr[3];
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_udp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_tcp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}


#endif
