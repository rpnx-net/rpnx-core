#ifndef RPNXCORE_WINDOWS_NETWORK_HPP
#define RPNXCORE_WINDOWS_NETWORK_HPP

//#include <winsock.h>
#include <winsock2.h>
#include <array>
#include <system_error>
#include <assert.h>
#include <vector>
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

  class ip4_address final
  {
  private:
      using data_type = std::array<std::uint8_t, 4>;
      data_type m_val;
  public:
    inline in_addr native() const
    {
          in_addr addr;
          std::copy(m_val.begin(), m_val.end(), reinterpret_cast<std::uint8_t*>(&addr.S_un.S_un_b));
          return *std::launder(&addr);
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
      inline ip4_tcp_endpoint(ip4_address addr, uint16_t port)
      : m_addr(addr), m_port(port)
      {}

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
      inline ip4_udp_endpoint() {
          
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
          detail::wsa_intializer::singleton();
      }

      inline ip4_udp_endpoint(ip4_udp_endpoint const&) = default;
      explicit ip4_udp_endpoint(ip4_tcp_endpoint const&);

      

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
      ip4_udp_socket()
      {
          detail::wsa_intializer::singleton();
          m_s = INVALID_SOCKET;
      }


      ~ip4_udp_socket()
      {
          close();
      }

      std::pair<ip4_udp_endpoint, std::vector<std::byte>> receive()
      {
          thread_local std::array<std::byte, 256 * 256> buffer;
          sockaddr_in from;
          static_assert(sizeof(from) < INT_MAX);
          int fromlen = sizeof(from);
          auto count = recvfrom(m_s, reinterpret_cast<char*>(buffer.data()), (int) buffer.size(), 0, reinterpret_cast<sockaddr*>( &from ), &fromlen);
          if (count == SOCKET_ERROR)
          {
              throw network_error("upd_ip4_socket::receive()", get_wsa_error_code());
          }
          return { from, std::vector<std::byte>(buffer.data(), buffer.data()+count) };
      }

      void send(std::vector<std::byte> const & data, ip4_udp_endpoint destination)
      {
          sockaddr_in dest = destination.native();
          auto result = sendto(m_s, (char const *)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
          if (result == SOCKET_ERROR)
          {
              throw network_error("upd_ip4_socket::send()", get_wsa_error_code());
          }
          return;
      }

      void bind(ip4_udp_endpoint ep)
      {
          sockaddr_in socket_addr = ep.native();
          auto result = ::bind(m_s, (const sockaddr*)&socket_addr, sizeof(socket_addr));
          if (result == SOCKET_ERROR)
          {
              throw network_error("upd_ip4_socket::bind()", get_wsa_error_code());
          }
          //assert(result == 0);
          return;
      }

      ip4_udp_endpoint endpoint()
      {
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
          if (m_s != INVALID_SOCKET)
          {
              closesocket(m_s);
              m_s = INVALID_SOCKET;
          }
      }

      void open()
      {
          close();
          m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
          if (m_s == INVALID_SOCKET) throw network_error("upd_ip4_socket::open()", get_wsa_error_code());
      }

  };
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_address const& addr)
{
    return lhs << (int) addr[0] << "." << (int) addr[1] << "." << (int) addr[2] << "." << (int) addr[3];
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_udp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}


#endif
