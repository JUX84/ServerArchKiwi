#pragma once

#include <netinet/in.h>
#include <ev++.h>

#include "robotManager.hpp"

class Server {
	public:
		Server();
		~Server();
		void run();
	private:
		int sock;
		ev::io watcher;
		void init(int, uint16_t, sockaddr*, socklen_t);
		void handle(int);
		void acceptClient(ev::io&, int);
		void setSocketOptions() const;
};
