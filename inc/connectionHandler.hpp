#pragma once

#include <string>
#include <memory>
#include <arpa/inet.h>
#include <ev++.h>

class ConnectionHandler
{
	public:
		ConnectionHandler(int);
		~ConnectionHandler();
		void readRequest(ev::io&, int);
	private:
		int sock;
		std::string request;
		ev::io readWatcher;

		void init();
		void setSocketOptions() const;
		void destroy();
};
