#pragma once

#include <Platform/Platform.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

#include <thread>

#include <Network/Network.hpp>
#include <Utilities/ThreadSafe.hpp>
#include <Utilities/Utilities.hpp>

#define VERIFY(x)                                                              \
    if (!(x))                                                                  \
        SC_APP_ERROR("ERROR READING/WRITING BUFFER");
