#pragma once

#include <chrono>
#include <string>
#include <thread>

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

//Pour la camera
#define LR_SERVO 7
#define UD_SERVO 17

//Ultrasonic Ranging Module
#define TRIG 20
#define ECHO 18

#define FRONTWARDS 1
#define BACKWARDS 0
#define COD1 16
#define COD2 15

class RobotManager {
	private:
		static int lrFD[2], udFD[2];
		static std::thread lrThread, udThread;
		static bool lrMoving, udMoving, blocked;
		static unsigned int lCnt, rCnt;
		static float lSpeed, rSpeed, speed;
		static std::chrono::time_point<std::chrono::system_clock> codTime, distTime;
		static std::string getName(int);
		static void checkDistance();
		static void setCameraPosition(int, int);
		static void getDistance();
		static void setDirection(int, int);
		static void setSpeed(int, int);
		static void setDirections(int, int);
		static void setSpeeds(int, int);	
		static void incLeftEncoder();
		static void incRightEncoder();
		static void checkTime();
		static void reset();
	public:
		static void init();
		static void initServo();
		static void closeServo();
		static void handleSignal(int);
		static std::string handle(std::string);
};
