#pragma once

#include <string>

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
#define COD1 15
#define COD2 16

class RobotManager {
	private:
		static std::string getName(int);
		static void setDirection(int, int);
		static void setSpeed(int, int);
		static void setDirections(int, int);
		static void setSpeeds(int, int);	
	public:
		static void init();
		static void handleSignal(int);
		static void reset();
		static void handle(std::string);
};