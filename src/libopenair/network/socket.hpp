//
// socket.hpp
// Cross-platform implementation of a BSD-like TCP Network Socket
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
            CLIENT, SERVER, EMPTY
        };

#ifdef __linux__


        class Socket {
            std::shared_ptr<int> sockfd;
            SocketType sock_type;


            Socket(int sockfd, SocketType type);

        public:
            Socket();

            Socket(std::string hostname, uint16_t port); // Client socket
            Socket(uint16_t port, uint32_t max_queued_connections); // Server socket

            static Socket loopback(uint16_t port);

            SocketType get_type();

            Socket accept(int timeout_millis = 0);


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
