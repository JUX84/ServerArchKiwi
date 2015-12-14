#include <string>
#include <cstring>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.hpp"
#include "robotManager.hpp"
#include "logger.hpp"

Server::Server() {
	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = PF_INET;
	address.sin_port = htons(20000);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		Logger::log("error creating socket");

	if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
		Logger::log("error binding socket");

	if (listen(sock, 1) == -1)
		Logger::log("error listening socket");
}

void Server::run() {
	while(1) {
		int conn = accept(sock, nullptr, nullptr);
		if (conn == -1)
			continue;
		while (1) {
			char buffer[16];
			int err = recv(conn, buffer, 16, 0);
			if (err <= 0)
				break;
			Logger::log("recv: " + std::string(buffer));
			std::string response = RobotManager::handle(buffer);
			if (response.size() > 0) {
				Logger::log("send: " + response);
				send(sock, response.data(), response.size(), 0);
			}
		}
	}
}
