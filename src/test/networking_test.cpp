//
// Created by nsamson on 11/28/15.
//

#include "libopenair/network/socket.hpp"
#include <string>
#include <assert.h>
#include <iostream>
#include <thread>
#include <mutex>

namespace opnet = openair::network;

int main(int argc, char **argv) {
    opnet::TCPSocket server(7777, 5);
	auto client = opnet::TCPSocket::loopback(7777);

	opnet::TCPSocket serv_conn = server.accept();
	char buf[256] = { '\0' };

	std::string test_str = "Hello, world!";

	std::move(test_str.begin(), test_str.end(), buf);
	ssize_t bytes_sent = serv_conn.send(buf, 256, 0);
	assert(bytes_sent > 0);

	char recv_buf[256] = { '\0' };
	ssize_t bytes_recv = client.recv(recv_buf, 256, 0);
	assert(bytes_recv > 0);
	std::cout << std::string(recv_buf) << std::endl;
}
