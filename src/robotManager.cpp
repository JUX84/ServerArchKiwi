#include <wiringPi.h>
#include <softPwm.h>

#include "robotManager.hpp"
#include "logger.hpp"

unsigned int RobotManager::lCnt, RobotManager::lSpeed, RobotManager::rCnt, RobotManager::rSpeed;
std::chrono::time_point<std::chrono::system_clock> RobotManager::time;

void RobotManager::init() {
	wiringPiSetup();
	reset();
	time = std::chrono::system_clock::now();
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
		lCnt = 0;
		rCnt = 0;
		time = now;
	}
}

void RobotManager::handle(std::string str) {
        std::string target;
        int angle = 0, power = 0;
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
        }
        return "Undefined Wheel";
}

void RobotManager::handleSignal(int signal) {
	reset();
	exit(0);
}

void RobotManager::reset() {
        Logger::log("Setting PWM pins...");

        softPwmCreate(FRONT_LEFT_WHEEL, 0, 100);
        softPwmCreate(FRONT_RIGHT_WHEEL, 0, 100);
        softPwmCreate(REAR_LEFT_WHEEL, 0, 100);
        softPwmCreate(REAR_RIGHT_WHEEL, 0, 100);

        Logger::log("Setting OUTPUT pins...");

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
}

void RobotManager::setDirection(int wheel, int frontwards) {
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
        //Logger::log(msg);

        digitalWrite(w_front, frontwards);
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
