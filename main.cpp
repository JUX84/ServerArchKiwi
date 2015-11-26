/* INCLUDE STD */
#include <iostream>
#include <cstring>
#include <ctime>
#include <signal.h>

/* INCLUDE SOCKETS */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* INCLUDE wiringPi */
#include <wiringPi.h>
#include <softPwm.h>

/* DEFINE GPIO PINS */
#define FRONT_LEFT_WHEEL 1
#define FRONT_RIGHT_WHEEL 6
#define REAR_LEFT_WHEEL 12
#define REAR_RIGHT_WHEEL 0
#define FL_FRONTWARDS 5
#define FL_BACKWARDS 4
#define FR_FRONTWARDS 10
#define FR_BACKWARDS 11
#define RL_FRONTWARDS 13
#define RL_BACKWARDS 14
#define RR_FRONTWARDS 3
#define RR_BACKWARDS 2
#define LEFT 0
#define RIGHT 1
#define FRONTWARDS 1
#define BACKWARDS 0

/* GET WHEEL ASSOCIATED TO pin */
std::string getName(int pin) {
	switch(pin) {
		case FRONT_LEFT_WHEEL:
			return "Front Left Wheel";
		case FRONT_RIGHT_WHEEL:
			return "Front Right Wheel";
		case REAR_LEFT_WHEEL:
			return "Rear Left Wheel";
		case REAR_RIGHT_WHEEL:
			return "Rear Right Wheel";
	}
	return "ERROR";
}

/* LOG */
void info(std::string message) {
	std::time_t time = std::time(nullptr);
	char timestr[9];
	if (std::strftime(timestr, sizeof(timestr), "%H:%M:%S", std::localtime(&time)))
		std::cout << "[" << timestr << "] ";
	std::cout << message << '\n';
}

/* SET PIN MODES (PWM/OUTPUT) */
void reset() {
	info("Setting PWM pins...");

	softPwmCreate(FRONT_LEFT_WHEEL, 0, 100);
	softPwmCreate(FRONT_RIGHT_WHEEL, 0, 100);
	softPwmCreate(REAR_LEFT_WHEEL, 0, 100);
	softPwmCreate(REAR_RIGHT_WHEEL, 0, 100);

	info("Setting OUTPUT pins...");

	pinMode(FL_FRONTWARDS, OUTPUT);
	digitalWrite(FL_FRONTWARDS, LOW);

	pinMode(FL_BACKWARDS, OUTPUT);
	digitalWrite(FL_BACKWARDS, LOW);

	pinMode(FR_FRONTWARDS, OUTPUT);
	digitalWrite(FR_FRONTWARDS, LOW);

	pinMode(FR_BACKWARDS, OUTPUT);
	digitalWrite(FR_BACKWARDS, LOW);

	pinMode(RL_FRONTWARDS, OUTPUT);
	digitalWrite(RL_FRONTWARDS, LOW);

	pinMode(RL_BACKWARDS, OUTPUT);
	digitalWrite(RL_BACKWARDS, LOW);

	pinMode(RR_FRONTWARDS, OUTPUT);
	digitalWrite(RR_FRONTWARDS, LOW);

	pinMode(RR_BACKWARDS, OUTPUT);
	digitalWrite(RR_BACKWARDS, LOW);
}

/* SET DIRECTION FOR wheel */
void setDirection(int wheel, int frontwards) {
	int w_front, w_back;
	switch(wheel) {
		case FRONT_LEFT_WHEEL:
			w_front = FL_FRONTWARDS;
			w_back = FL_BACKWARDS;
			break;
		case FRONT_RIGHT_WHEEL:
			w_front = FR_FRONTWARDS;
			w_back = FR_BACKWARDS;
			break;
		case REAR_LEFT_WHEEL:
			w_front = RL_FRONTWARDS;
			w_back = RL_BACKWARDS;
			break;
		case REAR_RIGHT_WHEEL:
			w_front = RR_FRONTWARDS;
			w_back = RR_BACKWARDS;
			break;
	}

	std::string msg = "Setting ";
	msg += (frontwards ? "frontwards" : "backwards");
	msg += " direction for ";
	msg += getName(wheel);
	msg += "...";
	info(msg);

	digitalWrite(w_front, frontwards);
	digitalWrite(w_back, !frontwards);
}

/* SET SPEED FOR wheel */
void setSpeed(int wheel, int speed) {
	if (speed < 0)
		speed = 0;
	if (speed > 100)
		speed = 100;

	std::string msg = "Setting speed of ";
	msg += std::to_string(speed);
	msg += " for ";
	msg += getName(wheel);
	msg += "...";
	info(msg);

	softPwmWrite(wheel, speed);
}

/* SET DIRECTION FOR LEFT OR RIGHT SIDE */
void setDirections(int side, int direction) {
	if (side == 0) {
		setDirection(FRONT_LEFT_WHEEL, direction);
		setDirection(REAR_LEFT_WHEEL, direction);
	} else {
		setDirection(FRONT_RIGHT_WHEEL, direction);
		setDirection(REAR_RIGHT_WHEEL, direction);
	}
}

/* SET SPEED FOR LEFT OR RIGHT SIDE */
void setSpeeds(int side, int speed) {
	if (side == 0) {
		setSpeed(FRONT_LEFT_WHEEL, speed);
		setSpeed(REAR_LEFT_WHEEL, speed);
	} else {
		setSpeed(FRONT_RIGHT_WHEEL, speed);
		setSpeed(REAR_RIGHT_WHEEL, speed);
	}
}

/* SET DIRECTION FOR ALL WHEELS */
void setAllDirections(int direction) {
	setDirections(LEFT, direction);
	setDirections(RIGHT, direction);
}

void interrupt(int sig) {
	reset();
	exit(EXIT_SUCCESS);
}

void parse(std::string str) {
	std::string target;
	int angle, power;
	std::size_t first, second, third;
	first = str.find(';', 0);
	if (first != std::string::npos)
		target = str.substr(0, first);
	second = str.find(';', first+1);
	if (second != std::string::npos)
		angle = std::stoi(str.substr(first+1, second-first-1));
	third = str.find(';', second+1);
	if (third != std::string::npos)
		power = std::stoi(str.substr(second+1, third-second-1));

	/*std::string s = "Target: ";
	s += (target == "M" ? "Motor" : "Camera");
	info(s);
	info("Angle: " + angle);
	info("Power: " + power);*/

	if (target == "M") { // MOTOR
		if (angle > -80 && angle <= 80) {
			setAllDirections(FRONTWARDS);
			if (angle <= 0) {
				setSpeeds(LEFT, power*(1.f-(angle/(-75.f))));
				setSpeeds(RIGHT, power);
			} else {
				setSpeeds(LEFT, power);
				setSpeeds(RIGHT, power*(1.f-(angle/75.f)));
			}
		} else if (angle <= -100 || angle > 100) {
			setAllDirections(BACKWARDS);
			if (angle <= 0) {
				setSpeeds(LEFT, power);
				setSpeeds(RIGHT, power*(1.f-(angle/75.f)));
			} else {
				setSpeeds(LEFT, power*(1.f-(angle/(-75.f))));
				setSpeeds(RIGHT, power);
			}
		} else if (angle <= -80 && angle > -100) {
			setDirections(LEFT, BACKWARDS);
			setDirections(RIGHT, FRONTWARDS);
			setSpeeds(LEFT, power);
			setSpeeds(RIGHT, power);
		} else if (angle > 80 && angle <= 100) {
			setDirections(LEFT, FRONTWARDS);
			setDirections(RIGHT, BACKWARDS);
			setSpeeds(LEFT, power);
			setSpeeds(RIGHT, power);
		}
	}
}

int main(int argc, char** argv) {
	wiringPiSetup(); // wiringpi initialization
	reset(); // pin modes initialization
	
	/*setDirection(FRONT_LEFT_WHEEL, FRONTWARDS);	
	setSpeed(FRONT_LEFT_WHEEL, 10);

	while(1);*/

	struct sigaction exitAction;
        exitAction.sa_handler = interrupt;
	sigaction(SIGINT, &exitAction, nullptr);

	struct sockaddr_in sa;

	int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sockfd) {
		info("error create socket");
		return 1;
	}

	memset(&sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(20000);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(sockfd, (struct sockaddr*)&sa, sizeof(sa))) {
		info("error bind socket");
		close(sockfd);
		return 1;
	}

	if (-1 == listen(sockfd, 10)) {
		info("error listen socket");
		close(sockfd);
		return 1;
	}

	while(1) {
		int connfd = accept(sockfd, nullptr, nullptr);
		if (0 > connfd) {
			info("error connect socket");
			close(sockfd);
			return 1;
		}

		char buffer[12];
		read(connfd, buffer, 11);

		parse(buffer);

		if (-1 == shutdown(connfd, SHUT_RDWR)) {
			close(connfd);
			close(sockfd);
			return 1;
		}
		close(connfd);
	}

	close(sockfd);

	return 0;
}
