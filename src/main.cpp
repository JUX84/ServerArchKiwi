#include <signal.h>

#include "robotManager.hpp"
#include "logger.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
	signal(SIGINT, RobotManager::handleSignal);

	Logger::init();
	RobotManager::init();
	Server::init();
	Server::run();

	return 0;
}
