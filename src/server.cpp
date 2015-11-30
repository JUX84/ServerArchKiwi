#include <string>
#include <cstring>
#include <system_error>
#include <stdexcept>
#include <errno.h>
#include <sys/socket.h>

#include "server.hpp"
#include "connectionHandler.hpp"
#include "logger.hpp"

Server::Server() : sock(-1), watcher(EV_DEFAULT) {
	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = PF_INET;
	address.sin_port = htons(20000);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	init(PF_INET, 20000, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

Server::~Server() {}

void Server::init(int domain, uint16_t port, sockaddr* address, socklen_t addrlen) {
	if ((sock = socket(domain, SOCK_STREAM, 0)) == -1)
		throw std::system_error(errno, std::system_category());

	setSocketOptions();

	if (bind(sock, address, addrlen) == -1)
		throw std::system_error(errno, std::system_category());

	if (listen(sock, 1) == -1)
		throw std::system_error(errno, std::system_category());

	watcher.set<Server, &Server::acceptClient>(this);
	watcher.start(sock, ev::READ);
}

void Server::setSocketOptions() const {
	const int opt = 1;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::system_error(errno, std::system_category());
}

void Server::run() {
	ev_run(EV_DEFAULT);
}

void Server::acceptClient(ev::io& w, int revents) {
	int responseSock = accept(sock, nullptr, nullptr);

	if (responseSock != -1) {
		try {
			handle(responseSock);
		} catch (const std::exception& e) {
			Logger::log(e.what());
		}
	} else {
		Logger::log("accept client failed");
	}
}

void Server::handle(int responseSock) {
	new ConnectionHandler(responseSock);
}
