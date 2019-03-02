#include <Servo.h>
#include <Wire.h>
#include <Octoliner.h>

#include "func.h"

Octoliner lineleft(42);
Octoliner lineright(45);

#define ENCODER_INT 2 // прерывание энкодера
// текущее значение энкодера в переменной enc

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[16] = { -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8 };


void setup() {
	attachInterrupt(ENCODER_INT, encoder, RISING);
	myservo.attach(14);
	Serial1.begin(115200);
	Serial.begin(9600);
	//waitGreen();
	enc = -5;



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
}

void loop() {
	byte speed = 80;
	float kp = 10; //40
	float kd = 20; //80

	//Serial.println(enc);

	go(motor, speed);
	int d[16] = { lineleft.analogRead(7), lineleft.analogRead(6), lineleft.analogRead(5), lineleft.analogRead(4), lineleft.analogRead(3), lineleft.analogRead(2), lineleft.analogRead(1), lineleft.analogRead(0), lineright.analogRead(7), lineright.analogRead(6), lineright.analogRead(5), lineright.analogRead(4), lineright.analogRead(3), lineright.analogRead(2), lineright.analogRead(1), lineright.analogRead(0) };
	//printSensors(d);
	float err = senOut(d, w);
	
	float pd = PD(err, kp, kd);
	servo(pd);
	Serial.println(pd);
	delay(10);
}
