#include <Ultrasonic.h>
#include <Servo.h>
#include "func.h"
#include <Wire.h>
#include <Octoliner.h>

Ultrasonic ultrasonic(48, 49);
Octoliner lineleft(42);
Octoliner lineright(45);

#define ENCODER_INT 2 // прерывание энкодера
// текущее значение энкодера в переменной enc
#define SPEEDTEK 60

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[16] = { -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8 };
bool polin = 1;
int irtek = -1;
bool ultraon = 0;
bool svetofor = 1;
unsigned long mil = 0;
unsigned long vrem = 1500;
bool stopped = 0;


int speed = SPEEDTEK;

void setup() {
	attachInterrupt(ENCODER_INT, encoder, RISING);
	myservo.attach(14);
	Serial1.begin(115200);
	Serial.begin(9600);

	Wire.begin();
	// начало работы с датчиками линии
	lineleft.begin();
	// выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
	lineleft.setSensitivity(210);
	// выставляем яркость свечения ИК-светодиодов в диапазоне от 0 до 255
	lineleft.setBrightness(255);
	// начало работы с датчиками линии
	lineright.begin();
	// выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
	lineright.setSensitivity(210);
	// выставляем яркость свечения ИК-светодиодов в диапазоне от 0 до 255
	lineright.setBrightness(255);
	pinMode(13, OUTPUT);
	digitalWrite(13, 1);
	//waitGreen();

}

void loop() {
	float kp = 26; //40
	float kd = 100; //80
	int ir = IRread();
	if (ultraon) {
		int dist = ultrasonic.Ranging(CM);
		Serial.println(dist);
		if (dist < 30 && dist > 10) {
			polin = 0;
			servo(0);
			go(motor, 0);
			delay(1000);
		}
		if ((dist >= 60 || dist <= 5) && !polin) {
			ultraon = 0;
			polin = 1;
			digitalWrite(13, 0);
		}
	}
	if (ir != -1 && ir != 2 && ir != 3 && ir < 7 && svetofor && polin) {
		speed = SPEEDTEK - 10;
		if (lineright.analogRead(0) > 300 && lineright.analogRead(1) > 300 && lineright.analogRead(2) > 300 && lineright.analogRead(3) > 300 && lineright.analogRead(4) > 300 && lineright.analogRead(5) > 300) {
			if (ir == 6) {
				mil = millis();
				polin = 0;
				servo(0);
				go(motor, -255);
				delay(70);
				go(motor, 0);
				delay(5000);
				go(motor, 60);
				delay(500);
				polin = 1;
			}
			else {
				mil = millis();
				polin = 0;
				servo(0);
				go(motor, -255);
				delay(45);
				go(motor, 0);
				delay(500);
			}
		}
	}
	if (ir != -1 && ir == 2 && !polin) {
		servo(0);
		speed = SPEEDTEK;
		//go(motor, speed);
		//delay(1000);
		polin = 1;
	}
	if ((ir == 2 || ir == 3) && polin) {
		if (lineright.analogRead(0) > 300 && lineright.analogRead(1) > 300 && lineright.analogRead(2) > 300 && lineright.analogRead(3) > 300 && lineright.analogRead(4) > 300 && lineright.analogRead(5) > 300) {
			speed = SPEEDTEK + 5;
			svetofor = 0;
			mil = millis();
		}
	}
	if (millis() - mil > vrem) {
		speed = SPEEDTEK;
		svetofor = 1;
	}
	if (polin) {
		float kp = 7; //40
		float ki = 0.1; //40
		float kd = 30; //80
		int d[16] = { lineleft.analogRead(7), lineleft.analogRead(6), lineleft.analogRead(5), lineleft.analogRead(4), lineleft.analogRead(3), lineleft.analogRead(2), lineleft.analogRead(1), lineleft.analogRead(0), lineright.analogRead(7), lineright.analogRead(6), lineright.analogRead(5), lineright.analogRead(4), lineright.analogRead(3), lineright.analogRead(2), lineright.analogRead(1), lineright.analogRead(0) };

		float err = senOut(d, w);
		float pd = PID(err, kp, ki, kd);
		servo(pd);
		go(motor, speed);
	}
}
