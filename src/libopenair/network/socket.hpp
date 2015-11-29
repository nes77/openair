//
// socket.hpp
// Cross-platform implementation of a BSD-like TCP Network socket
// Created by nsamson on 11/26/15.
//

#ifndef OPENAIR_SOCKET_HPP_HPP
#define OPENAIR_SOCKET_HPP_HPP

#include "../../libopenair/common.hpp"
#include <string>
#include <cstring>

#ifdef __linux__

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>

#elif WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef ssize_t
#define ssize_t int
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#endif

namespace openair {

    namespace network {

        enum class SocketType {
            CLIENT, SERVER
        };

        enum class IOType {
            RD, WR, RDWR
        };




        class TCPSocket {
#ifdef __linux__
            std::shared_ptr<int> sockfd;
			TCPSocket(int sockfd, SocketType type);
#elif (defined WIN32)
			std::shared_ptr<SOCKET> socket;
			TCPSocket(std::shared_ptr<SOCKET> socket, SocketType type);
#endif

            SocketType sock_type;
			std::shared_ptr<bool> _shutdown;
			std::shared_ptr<bool> _closed;

        public:

            TCPSocket(std::string hostname, uint16_t port); // Client socket
            TCPSocket(uint16_t port, uint32_t max_queued_connections); // Server socket
            ~TCPSocket();

            static TCPSocket loopback(uint16_t port); // System-independent

            SocketType get_type(); // System-independent

            TCPSocket accept();

            bool is_shutdown(); // System-independent

            bool is_closed(); // System-independent

            void shutdown(IOType type);

            void close(); // System-independent

            bool poll(IOType type, int timeout_millis = 0);

            ssize_t recv(void *buf, size_t len, int flags);

            ssize_t send(const void *buf, size_t len, int flags);



        };

        class NetworkingException : public std::runtime_error {

        public:
            NetworkingException(const std::string &__arg) : runtime_error(__arg) { }
        };

        class InterruptedException : public std::runtime_error {
        public:
            InterruptedException(const std::string &__arg) : runtime_error(__arg) { }
        };
    }
}

#endif //OPENAIR_SOCKET_HPP_HPP
