//
// Created by nsamson on 11/26/15.
//

#include "../../libopenair/network/socket.hpp"

#ifdef __linux__

void disconnect_socket(int *fid) {
    int result = shutdown(*fid, SHUT_RDWR);
    if (result < 0 && errno != ENOTCONN) {
        perror("Could not shutdown socket: ");
        std::terminate();
    }

    result = close(*fid);
    if (result < 0) {
        perror("Could not close socket: ");
        std::terminate();
    }

    delete fid;
}

openair::network::Socket::Socket(std::string hostname, uint16_t port) {

    this->sock_type = SocketType::CLIENT;

    addrinfo hint;

    std::memset(&hint, 0, sizeof(addrinfo));

    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);

    addrinfo *dest_info;
    int result = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hint, &dest_info);

    if (result != 0) {
        std::string errstr(gai_strerror(result));
        throw NetworkingException(std::string("Failed hostname resolution: ").append(errstr));
    }

    std::unique_ptr<addrinfo, void (*)(addrinfo *)> ap(dest_info, freeaddrinfo);


    int sock = -1;

    addrinfo *rp;

    for (rp = ap.get(); rp != nullptr; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sock < 0) {
            continue;
        }

        if (connect(sock, rp->ai_addr, rp->ai_addrlen) >= 0) {
            break;
        }

        result = close(sock);

        if (result < 0) {
            throw NetworkingException("Failed to close socket");
        }
    }

    if (rp == nullptr) {
        throw NetworkingException("Error opening socket");
    }

    this->sockfd = std::shared_ptr<int>(new int(sock), disconnect_socket);


}

openair::network::Socket::Socket(int sockfd, openair::network::SocketType type) {
    this->sock_type = type;
    this->sockfd = std::shared_ptr<int>(new int(sockfd), disconnect_socket);
}


openair::network::Socket::Socket(uint16_t port, uint32_t max_queued_connections) {

    this->sock_type = SocketType::SERVER;

    addrinfo hints;
    addrinfo *results, *rp;

    int sockfd, result;
    sockfd = -1;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    result = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &results);

    if (result != 0) {
        std::string errstr(gai_strerror(result));
        throw NetworkingException(std::string("Failed hostname resolution: ").append(errstr));
    }

    std::unique_ptr<addrinfo, void (*)(addrinfo *)> ap(results, freeaddrinfo);

    for (rp = ap.get(); rp != nullptr; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd < 0) {
            continue;
        }

        if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }

        result = close(sockfd);

        if (result < 0) {
            throw NetworkingException("Failed to close socket.");
        }
    }

    if (rp == nullptr) {
        throw NetworkingException("Could not bind socket.");
    }

    this->sockfd = std::shared_ptr<int>(new int(sockfd), disconnect_socket);

    if (listen(*this->sockfd, max_queued_connections) < 0) {
        throw NetworkingException("Could not listen on socket.");
    }

}

openair::network::Socket openair::network::Socket::accept(int timeout_millis) {
    if (this->sock_type != SocketType::SERVER) {
        throw NetworkingException("Cannot accept connections on a client socket.");
    }

    if (timeout_millis == 0) {
        int sockfd = ::accept(*this->sockfd, nullptr, nullptr);
        if (sockfd < 0) {
            int error = errno;
            std::string errstr(strerror(error));
            throw NetworkingException(std::string("Could not accept connection: ").append(errstr));
        }

        return Socket(sockfd, SocketType::CLIENT);
    } else {
        pollfd fd[1];

        fd->fd = *this->sockfd;
        fd->events = POLLIN | POLLPRI;

        int result = poll(fd, 1, timeout_millis);
        int error = errno;

        if (result > 0) {
            int sockfd = ::accept(*this->sockfd, nullptr, nullptr);
            return Socket(sockfd, SocketType::CLIENT);
        } else if (result < 0 && error != EINTR) {
            throw NetworkingException("poll() indicated an error.");
        } else if (error == EINTR) {
            throw InterruptedException("poll() was interrupted by a signal.");
        }
    }

    return Socket();
}

openair::network::Socket openair::network::Socket::loopback(uint16_t port) {
    return Socket("127.0.0.1", port);
}

openair::network::SocketType openair::network::Socket::get_type() {
    return this->sock_type;
}

openair::network::Socket::Socket() {
    this->sock_type = SocketType::EMPTY;
}

#endif // defined __linux__



