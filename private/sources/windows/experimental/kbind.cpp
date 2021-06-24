//
// Created by rnicholl on 6/15/2021.
//

#include "rpnx/kbind.hpp"

#include <winsock2.h>
#include <iostream>

rpnx::kbind rpnx::g_kbind{};

rpnx::kbind::kbind()
{
    std::cerr << "Initializing kbind" << std::endl;

    WSAData woo;
    WSAStartup(MAKEWORD(2, 2), &woo);

    SOCKET socket = INVALID_SOCKET;

    socket = WSASocketW(AF_INET6, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    GUID GuidAcceptEx = WSAID_ACCEPTEX;

    DWORD v_bytes{};
    [[maybe_unused]] int iResult = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                                            &GuidAcceptEx, sizeof (GuidAcceptEx),
                                            &winnt_AcceptEx, sizeof (winnt_AcceptEx),
                                            &v_bytes, nullptr, nullptr);

    closesocket(socket);

}