#pragma once

class Server {
	public:
		static void init();
		static void run();
		static void stop();
	private:
		static int sock;
};
