//
// Created by nsamson on 11/26/15.
//

#include "../../libopenair/network/socket.hpp"

#ifdef __linux__

openair::network::TCPSocket::TCPSocket(std::string hostname, uint16_t port) {

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

        result = ::close(sock);

        if (result < 0) {
            throw NetworkingException("Failed to close socket");
        }
    }

    if (rp == nullptr) {
        throw NetworkingException("Error opening socket");
    }

    this->sockfd = std::shared_ptr<int>(new int(sock));


}

openair::network::TCPSocket::TCPSocket(int sockfd, openair::network::SocketType type) {
    this->sock_type = type;
    this->sockfd = std::shared_ptr<int>(new int(sockfd));
}


openair::network::TCPSocket::TCPSocket(uint16_t port, uint32_t max_queued_connections) {

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

        result = ::close(sockfd);

        if (result < 0) {
            throw NetworkingException("Failed to close socket.");
        }
    }

    if (rp == nullptr) {
        throw NetworkingException("Could not bind socket.");
    }

    this->sockfd = std::shared_ptr<int>(new int(sockfd));

    if (listen(*this->sockfd, max_queued_connections) < 0) {
        throw NetworkingException("Could not listen on socket.");
    }

}

openair::network::TCPSocket openair::network::TCPSocket::accept() {
    if (this->sock_type != SocketType::SERVER) {
        throw NetworkingException("Cannot accept connections on a client socket.");
    }
        int sockfd = ::accept(*this->sockfd, nullptr, nullptr);
        if (sockfd < 0) {
            int error = errno;
            std::string errstr(strerror(error));
            throw NetworkingException(std::string("Could not accept connection: ").append(errstr));
        }

    return TCPSocket(sockfd, SocketType::CLIENT);
}

openair::network::TCPSocket openair::network::TCPSocket::loopback(uint16_t port) {
    return TCPSocket("127.0.0.1", port);
}

openair::network::SocketType openair::network::TCPSocket::get_type() {
    return this->sock_type;
}

openair::network::TCPSocket::~TCPSocket() {
    if (this->sockfd.unique()) { // last copy; need to kill connection
        this->close();
    }
}

bool openair::network::TCPSocket::is_shutdown() {
    return this->_shutdown;
}

bool openair::network::TCPSocket::is_closed() {
    return this->_closed;
}

void openair::network::TCPSocket::shutdown(openair::network::IOType type) {

    int how;

    switch (type) {
        case IOType::RD:
            how = SHUT_RD;
            break;

        case IOType::WR:
            how = SHUT_WR;
            break;

        case IOType::RDWR:
            how = SHUT_RDWR;
            break;
    }

    int result = ::shutdown(*this->sockfd, how);

    if (result < 0) {
        int error = errno;
        std::string errstr = strerror(error);
        throw NetworkingException(std::string("Error when shutting down socket: ").append(errstr));
    }

    this->_shutdown = true;

}

void openair::network::TCPSocket::close() {

    if (this->is_closed()) {
        return;
    }

    int result = ::close(*this->sockfd);

    if (result < 0) {
        int error = errno;
        std::string errstr = strerror(error);
        throw NetworkingException(std::string("Error when closing socket: ").append(errstr));
    }
}

bool openair::network::TCPSocket::poll(openair::network::IOType type, int timeout_millis) {
    pollfd fd[1];

    fd->fd = *this->sockfd;
    short flags;

    switch (type) {
        case IOType::RD:
            flags = POLLIN | POLLPRI;
            break;

        case IOType::WR:
            flags = POLLWRBAND | POLLOUT;
            break;

        case IOType::RDWR:
            flags = POLLIN | POLLPRI | POLLWRBAND | POLLOUT;
            break;
    }

    fd->events = flags;

    int result = ::poll(fd, 1, timeout_millis);
    int error = errno;

    if (result > 0) {
        return true;

    } else if (result < 0 && error != EINTR) {
        throw NetworkingException("poll() indicated an error.");
    } else if (error == EINTR) {
        throw InterruptedException("poll() was interrupted by a signal.");
    }

    return false;
}

ssize_t openair::network::TCPSocket::recv(void *buf, size_t len, int flags) {
    ssize_t bytes_read = ::recv(*this->sockfd, buf, len, flags);

    if (bytes_read < 0) {
        int error = errno;
        if (error == EINTR) {
            throw InterruptedException("recv() was interrupted while blocking for receive.");
        } else if (error == EAGAIN || error == EWOULDBLOCK) {
            return -1;
        } else {
            std::string errmsg(strerror(errno));
            throw NetworkingException(std::string("recv() returned an error: ").append(errmsg));
        }
    }

    return bytes_read;
}

ssize_t openair::network::TCPSocket::send(const void *buf, size_t len, int flags) {
    ssize_t bytes_sent = ::send(*this->sockfd, buf, len, flags);

    if (bytes_sent < 0) {
        int error = errno;
        if (error == EINTR) {
            throw InterruptedException("sent() was interrupted while blocking for receive.");
        } else if (error == EAGAIN || error == EWOULDBLOCK) {
            return -1;
        } else {
            std::string errmsg(strerror(errno));
            throw NetworkingException(std::string("sent() returned an error: ").append(errmsg));
        }
    }

    return bytes_sent;
}
#endif // defined __linux__



