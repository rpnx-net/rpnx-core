//
// Created by rnicholl on 6/15/2021.
//

#ifndef RPNXCORE_KBIND_HPP
#define RPNXCORE_KBIND_HPP

#include <Windows.h>

namespace rpnx
{
    class kbind
    {
      public:
        kbind();
    };

    BOOL (*winnt_AcceptEx = nullptr) (
        SOCKET       sListenSocket,
        SOCKET       sAcceptSocket,
        PVOID        lpOutputBuffer,
        DWORD        dwReceiveDataLength,
        DWORD        dwLocalAddressLength,
        DWORD        dwRemoteAddressLength,
        LPDWORD      lpdwBytesReceived,
        LPOVERLAPPED lpOverlapped
    );

    extern kbind g_kbind;
}

#endif // RPNXCORE_KBIND_HPP
