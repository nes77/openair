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

#endif

namespace openair {

    namespace network {

        enum class SocketType {
            CLIENT, SERVER
        };

        enum class IOType {
            RD, WR, RDWR
        };

#ifdef __linux__


        class TCPSocket {
            std::shared_ptr<int> sockfd;
            SocketType sock_type;
            bool _shutdown = false;
            bool _closed = false;


            TCPSocket(int sockfd, SocketType type);

        public:

            TCPSocket(std::string hostname, uint16_t port); // Client socket
            TCPSocket(uint16_t port, uint32_t max_queued_connections); // Server socket
            ~TCPSocket();

            static TCPSocket loopback(uint16_t port);

            SocketType get_type();

            TCPSocket accept();

            bool is_shutdown();

            bool is_closed();

            void shutdown(IOType type);

            void close();

            bool poll(IOType type, int timeout_millis = 0);

            ssize_t recv(void *buf, size_t len, int flags);

            ssize_t send(const void *buf, size_t len, int flags);



        };

#endif

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
