#include <string>
#include <cstring>
#include <system_error>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "connectionHandler.hpp"
#include "robotManager.hpp"

ConnectionHandler::ConnectionHandler(int socket) : sock(socket) {
	init();
}

void ConnectionHandler::init() {
	setSocketOptions();

	request.reserve(16);

	readWatcher.set<ConnectionHandler, &ConnectionHandler::readRequest>(this);
	readWatcher.start(sock, ev::READ);
}

ConnectionHandler::~ConnectionHandler() {}

void ConnectionHandler::setSocketOptions() const {
	int flags = fcntl(sock, F_GETFL);

	if (flags == -1 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::system_error(errno, std::system_category());
}

void ConnectionHandler::readRequest(ev::io& w, int revents) {
	char buffer[16];
	memset(buffer, 0, 16);
	size_t size = recv(sock, buffer, 16, 0);

	if (size > 0) {
		request.append(buffer, size);
		size = request.size();

		if (size >= 6) {
			readWatcher.stop();
			RobotManager::handle(request);
			destroy();
		}
	} else {
		destroy();
	}
}

void ConnectionHandler::destroy()
{
	readWatcher.stop();
	close(sock);
	delete this;
}
