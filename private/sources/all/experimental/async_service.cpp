//
// Created by Ryan on 1/10/2021.
//
#include "rpnx/experimental/network.hpp"

rpnx::experimental::async_service & rpnx::experimental::default_async_service()
{
    static async_service s_service;
    return s_service;
}