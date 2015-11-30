#include <signal.h>

#include "robotManager.hpp"
#include "server.hpp"

void handler(int sig) {
	exit(0);
}

int main(int argc, char** argv) {
	signal(SIGINT, RobotManager::handleSignal);
	signal(SIGTERM, RobotManager::handleSignal);

	RobotManager::init();

	Server* srv = new Server();
	srv->run();

	return 0;
}
