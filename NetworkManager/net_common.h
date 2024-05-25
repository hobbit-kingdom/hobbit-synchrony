#pragma once

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ASIO_STANDALONE

#include "../asio-1.30.2/include/asio.hpp"
#include "../asio-1.30.2/include/asio/ts/buffer.hpp"
#include "../asio-1.30.2/include/asio/ts/internet.hpp"
