#include <signal.h>

#include "robotManager.hpp"
#include "logger.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
	signal(SIGINT, RobotManager::handleSignal);
	signal(SIGTERM, RobotManager::handleSignal);

	Logger::init();
	RobotManager::init();

	Server* srv = new Server();
	srv->run();

	return 0;
}
