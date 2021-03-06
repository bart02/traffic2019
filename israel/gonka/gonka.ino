#include <Servo.h>
#include <Wire.h>
#include <Octoliner.h>

#include "func.h"

Octoliner lineleft(42);
Octoliner lineright(45);

#define ENCODER_INT 0 // прерывание энкодера
// текущее значение энкодера в переменной enc

#define RED 40
#define YELLOW 39
#define GREEN 38

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[16] = { -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8 };


void setup() {
	attachInterrupt(ENCODER_INT, encoder, RISING);
	myservo.attach(14);
	Serial1.begin(115200);
	Serial.begin(9600);

	enc = -5;



	Wire.begin();
	// начало работы с датчиками линии
	lineleft.begin();
	// выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
	lineleft.setSensitivity(220);
	// выставляем яркость свечения ИК-светодиодов в диапазоне от 0 до 255
	lineleft.setBrightness(255);
	// начало работы с датчиками линии
	lineright.begin();
	// выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
	lineright.setSensitivity(220);
	// выставляем яркость свечения ИК-светодиодов в диапазоне от 0 до 255
	lineright.setBrightness(255);
	pinMode(RED, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(GREEN, OUTPUT);
	//waitGreen();
}

void loop() {
	byte speed = 120;
	float kp = 3; //40
	float ki = 0; //40
	float kd = 10; //80

	if (enc > 15) {
		speed = 90;
		digitalWrite(RED, 1);
	}
	if (enc > 30) {
		kp = 4;
		kd = 80;
		ki = 0.25;
		digitalWrite(YELLOW, 1);
	}
	if (enc > 65) {
		speed = 120;
		kp = 3;
		kd = 10;
	}
	if (enc > 75) {
		speed = 80;
	}
	if (enc > 80) {
		kp = 10;
		kd = 50;
	}
	if (enc > 110) {
		speed = 120;
		kp = 3;
		kd = 10;
	}
	if (enc > 130) {
		speed = 90;
	}
	if (enc > 135) {
		kp = 8;
		kd = 30;
	}
	if (enc > 195) {
		speed = 120;
		kp = 3;
		kd = 10;
	}
	if (enc > 200) {
		speed = 90;
	}
	if (enc > 205) {
		kp = 8;
		kd = 30;
	}

	//Serial.println(enc);

	go(motor, speed);
	int d[16] = { lineleft.analogRead(7), lineleft.analogRead(6), lineleft.analogRead(5), lineleft.analogRead(4), lineleft.analogRead(3), lineleft.analogRead(2), lineleft.analogRead(1), lineleft.analogRead(0), lineright.analogRead(7), lineright.analogRead(6), lineright.analogRead(5), lineright.analogRead(4), lineright.analogRead(3), lineright.analogRead(2), lineright.analogRead(1), lineright.analogRead(0) };
	//printSensors(d);
	float err = senOut(d, w);

	float pd = PID(err, kp, ki, kd);
	servo(pd);
	Serial.println(enc);
	delay(5);
}
