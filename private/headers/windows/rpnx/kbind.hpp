//
// Created by rnicholl on 6/15/2021.
//

#ifndef RPNXCORE_KBIND_HPP
#define RPNXCORE_KBIND_HPP


#include <winsock2.h>
#include <mswsock.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <cinttypes>

namespace rpnx
{
    class kbind
    {
      public:
        kbind();

        int (*winnt_AcceptEx) (
            SOCKET       sListenSocket,
            SOCKET       sAcceptSocket,
            void*        lpOutputBuffer,
            int        dwReceiveDataLength,
            int        dwLocalAddressLength,
            int        dwRemoteAddressLength,
            int*      lpdwBytesReceived,
            _OVERLAPPED* lpOverlapped
        );
    };

    [[maybe_unused]]

    [[maybe_unused]] extern kbind g_kbind;
}

#endif // RPNXCORE_KBIND_HPP
