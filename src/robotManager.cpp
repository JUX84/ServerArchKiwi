#include <thread>

#include <wiringPi.h>
#include <softPwm.h>

#include "robotManager.hpp"
#include "logger.hpp"
#include "server.hpp"

bool RobotManager::lrMoving, RobotManager::udMoving;
unsigned int RobotManager::lCnt, RobotManager::rCnt;
float RobotManager::lSpeed, RobotManager::rSpeed, RobotManager::speed;
std::chrono::time_point<std::chrono::system_clock> RobotManager::time;

void RobotManager::init() {
	wiringPiSetup();
	reset();
	time = std::chrono::system_clock::now();
	speed = lSpeed = rSpeed = 0;
}

void RobotManager::incLeftEncoder() {
	checkTime();
	++lCnt;
}

void RobotManager::incRightEncoder() {
	checkTime();
	++rCnt;
}

void RobotManager::checkTime() {
	auto now = std::chrono::system_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now-time).count() > 1000) {
		lSpeed = lCnt*0.01;
		rSpeed = rCnt*0.01;
		speed = (lSpeed+rSpeed)/2.f;
		lCnt = 0;
		rCnt = 0;
		time = now;
	}
}

void RobotManager::setCameraPosition(int servo, int delay) {
	//Logger::log("Setting camera position on servo " + getName(servo) + " for " + std::to_string(delay) + "ms");
	digitalWrite(servo, HIGH);
	delayMicroseconds(delay);
	digitalWrite(servo, LOW);
	delayMicroseconds(20000-delay);
	servo == LR_SERVO ? lrMoving = false : udMoving = false;
}

std::string RobotManager::handle(std::string str) {
	Logger::log(str);
	std::string target;
	int angle = 0, power = 0;
	std::size_t first, second, third;
	first = str.find(';', 0);
	if (first != std::string::npos)
		target = str.substr(0, first);
	if (target == "M" || target == "C") {
		second = str.find(';', first+1);
		if (second != std::string::npos)
			angle = std::stoi(str.substr(first+1, second-first-1));
		third = str.find(';', second+1);
		if (third != std::string::npos)
			power = std::stoi(str.substr(second+1, third-second-1));
	}

	if (target == "E") {
		return std::to_string(speed);
	}
	if (target == "M") { // MOTOR
		if (angle > -80 && angle <= 80) {
			setDirections(LEFT, FRONTWARDS);
			setDirections(RIGHT, FRONTWARDS);
			if (angle <= 0) {
				setSpeeds(LEFT, power*(1.f-(angle/(-75.f))));
				setSpeeds(RIGHT, power);
			} else {
				setSpeeds(LEFT, power);
				setSpeeds(RIGHT, power*(1.f-(angle/75.f)));
			}
		} else if (angle <= -100 || angle > 100) {
			setDirections(LEFT, BACKWARDS);
			setDirections(RIGHT, BACKWARDS);
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
	if(target == "C") //CAMERA
	{
		int delay = 0;
		if(angle >= 0 && !lrMoving)
		{
			switch(angle) {
				case 0:
					delay = 600;
					break;
				case 180:
					delay = 2400;
					break;
			}
			lrMoving = true;
			std::thread thread(RobotManager::setCameraPosition, LR_SERVO, delay);
			thread.detach();
		}
		if(power >= 0 && !udMoving)
		{
			switch(power)
			{
				case 0:
					delay = 600;
					break;
				case 180:
					delay = 2400;
					break;  
			}
			udMoving = true;
			std::thread thread(RobotManager::setCameraPosition, UD_SERVO, delay);
			thread.detach();
		}
	}
	return "";
}

std::string RobotManager::getName(int pin) {
	switch(pin) {
		case FRONT_LEFT_WHEEL:
			return "Front Left Wheel";
		case FRONT_RIGHT_WHEEL:
			return "Front Right Wheel";
		case REAR_LEFT_WHEEL:
			return "Rear Left Wheel";
		case REAR_RIGHT_WHEEL:
			return "Rear Right Wheel";
		case LR_SERVO:
			return "Left-Right Servo";
		case UD_SERVO:
			return "Up-Down Servo";
	}
	return "Undefined Wheel";
}

void RobotManager::handleSignal(int signal) {
	reset();
	Server::stop();
	Logger::stop();
	exit(0);
}

void RobotManager::reset() {
	Logger::log("Setting pins...");

	softPwmCreate(FRONT_LEFT_WHEEL, 0, 100);
	softPwmCreate(FRONT_RIGHT_WHEEL, 0, 100);
	softPwmCreate(REAR_LEFT_WHEEL, 0, 100);
	softPwmCreate(REAR_RIGHT_WHEEL, 0, 100);

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

	pinMode(COD1, INPUT);
	wiringPiISR(COD1, INT_EDGE_RISING, RobotManager::incLeftEncoder);

	pinMode(COD2, INPUT);
	wiringPiISR(COD2, INT_EDGE_RISING, RobotManager::incRightEncoder);

	pinMode(LR_SERVO, OUTPUT);
	pinMode(UD_SERVO, OUTPUT);
}

void RobotManager::setDirection(int wheel, int frontwards) {
	int w_front = -1, w_back = -1;
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
	//Logger::log(msg);

	if (w_front != -1)
		digitalWrite(w_front, frontwards);
	if (w_back != -1)
		digitalWrite(w_back, !frontwards);
}

void RobotManager::setSpeed(int wheel, int speed) {
	if (speed < 0)
		speed = 0;
	if (speed > 100)
		speed = 100;

	std::string msg = "Setting speed of ";
	msg += std::to_string(speed);
	msg += " for ";
	msg += getName(wheel);
	msg += "...";
	//Logger::log(msg);

	softPwmWrite(wheel, speed);
}

void RobotManager::setDirections(int side, int direction) {
	if (side == 0) {
		setDirection(FRONT_LEFT_WHEEL, direction);
		setDirection(REAR_LEFT_WHEEL, direction);
	} else {
		setDirection(FRONT_RIGHT_WHEEL, direction);
		setDirection(REAR_RIGHT_WHEEL, direction);
	}
}

void RobotManager::setSpeeds(int side, int speed) {
	if (side == 0) {
		setSpeed(FRONT_LEFT_WHEEL, speed);
		setSpeed(REAR_LEFT_WHEEL, speed);
	} else {
		setSpeed(FRONT_RIGHT_WHEEL, speed);
		setSpeed(REAR_RIGHT_WHEEL, speed);
	}
}
