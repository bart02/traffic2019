#include <Servo.h>
#include <Wire.h>
#include <Octoliner.h>
#include <MPU6050.h>

#include "func.h"

MPU6050 mpu;

Octoliner lineleft(42);
Octoliner lineright(45);

#define ENCODER_INT 0 // прерывание энкодера
// текущее значение энкодера в переменной enc

#define RED 40
#define YELLOW 39
#define GREEN 38

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
//float w[16] = { -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8 };

float timeStep = 0.01;
float yaw = 0;
unsigned long stepbegin;

int step = 1;

void povorot(bool minus = 1) {
	int step = 1;
	enc = 0;
	while (step == 1) {
		digitalWrite(RED, 1);
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(-10 + enc * (-4));
		Serial.println(enc);

		if (enc > 17) {
			step = 2;
			enc = -15;
		}
		else go(motor, 50);
		// Wait to full timeStep period 
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 2) {
		digitalWrite(YELLOW, 1);
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(enc * (2.6));
		Serial.println(enc);

		if (enc > 0 || yaw >= 85) {
			step = 3;
			enc = 0;
			yaw = 0;
		}
		else go(motor, 50);
		// Wait to full timeStep period 
		delay((timeStep * 1000) - (millis() - timer));
	}
}

void fullpovorot() {
	int step = 1;
	enc = 0;
	while (step == 1) {
		digitalWrite(RED, 1);
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(-10 + enc * (-3));
		Serial.println(enc);

		if (enc > 16) {
			step = 2;
			enc = 0;
			servo(-30);
		}
		else go(motor, 50);
		// Wait to full timeStep period 
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 2) {
		digitalWrite(YELLOW, 1);
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		Serial.println(enc);

		if (enc > 24) {
			step = 3;
			enc = -17;
		}
		else go(motor, 50);
		// Wait to full timeStep period 
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 3) {
		digitalWrite(GREEN, 1);
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(enc * (2.6));
		Serial.println(enc);

		if (enc > 0 || yaw >= 180) {
			step = 4;
			enc = 0;
			yaw = 0;
		}
		else go(motor, 50);
		// Wait to full timeStep period 
		delay((timeStep * 1000) - (millis() - timer));
	}
}

void setup() {
	attachInterrupt(ENCODER_INT, encoder, RISING);
	myservo.attach(14);
	Serial1.begin(115200);
	Serial.begin(9600);
	//waitGreen();
	enc = 0;

	// Initialize MPU6050 
	while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
	{
		Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
		delay(500);
	}
	//delay(5000);
	// Calibrate gyroscope. The calibration must be at rest. 
	// If you don't want calibrate, comment this line. 
	mpu.calibrateGyro();

	// Set threshold sensivty. Default 3. 
	// If you don't want use threshold, comment this line or set 0. 
	mpu.setThreshold(0); 

	//Wire.begin();
	//// начало работы с датчиками линии
	//lineleft.begin();
	//// выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
	//lineleft.setSensitivity(210);
	//// выставляем яркость свечения ИК-светодиодов в диапазоне от 0 до 255
	//lineleft.setBrightness(255);
	//// начало работы с датчиками линии
	//lineright.begin();
	//// выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
	//lineright.setSensitivity(210);
	//// выставляем яркость свечения ИК-светодиодов в диапазоне от 0 до 255
	//lineright.setBrightness(255);
	stepbegin = millis();
	pinMode(RED, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(GREEN, OUTPUT);
}

void loop() {
	//fullpovorot();
	//go(motor, -255);
	//delay(70);
	//go(motor, 0);
	//while (true);
	while (step == 1) {
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(yaw * 3);
		if (enc > 45) {
			step = 2;
			enc = 0;
		}
		else go(motor, 50);
		// Wait to full timeStep period 
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 2)
	{
		povorot();
		step = 3;
	}

	while (step == 3) {
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(yaw * 3);
		Serial.println(enc);

		if (enc > 27) {
			step = 4;
		}
		else go(motor, 50);
		// Wait to full timeStep period
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 4) {
		povorot();
		step = 5;
	}

	while (step == 5) {
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(yaw * 3);
		Serial.println(enc);

		if (enc > 28) {
			step = 6;
		}
		else go(motor, 50);
		// Wait to full timeStep period
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 6) {
		fullpovorot();
		step = 7;
		
	}

	while (step == 7) {
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(yaw * 3);
		Serial.println(enc);

		if (enc > 18) {
			step = 8;
		}
		else go(motor, 50);
		// Wait to full timeStep period
		delay((timeStep * 1000) - (millis() - timer));
	}

	while (step == 8) {
		povorot();
		step = 9;
	}

	while (step == 9) {
		unsigned long timer = millis();
		Vector norm = mpu.readNormalizeGyro();
		yaw = yaw + norm.ZAxis * timeStep;
		servo(yaw * 3);
		Serial.println(enc);

		if (enc > 19) {
			step = 10;
			go(motor, -255);
			delay(200);
			go(motor, 0);
			while (true);
		}
		else go(motor, 50);
		// Wait to full timeStep period
		delay((timeStep * 1000) - (millis() - timer));
	}
	//unsigned long timer = millis();
	//Vector norm = mpu.readNormalizeGyro();
	//yaw = yaw + norm.ZAxis * timeStep;
	//servo(-10 + enc * (-3));
	//Serial.println(enc);
	//
	//if (enc > 15) {
	//	go(motor, -255);
	//	delay(70);
	//	go(motor, 0);
	//	while (true);
	//}
	//else go(motor, 50);
	//// Wait to full timeStep period 
	//delay((timeStep * 1000) - (millis() - timer));


	//byte speed = 80;
	//float kp = 7; //40
	//float ki = 0.1; //40
	//float kd = 30; //80

	////Serial.println(enc);

	//go(motor, speed);
	//int d[16] = { lineleft.analogRead(7), lineleft.analogRead(6), lineleft.analogRead(5), lineleft.analogRead(4), lineleft.analogRead(3), lineleft.analogRead(2), lineleft.analogRead(1), lineleft.analogRead(0), lineright.analogRead(7), lineright.analogRead(6), lineright.analogRead(5), lineright.analogRead(4), lineright.analogRead(3), lineright.analogRead(2), lineright.analogRead(1), lineright.analogRead(0) };
	////printSensors(d);
	//float err = senOut(d, w);
	//
	//float pd = PID(err, kp, ki, kd);
	//servo(pd);
	//Serial.println(pd);
	//delay(10);
}
