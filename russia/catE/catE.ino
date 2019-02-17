#include <Servo.h>
#include "func.h"

#define ENCODER_INT 2 // прерывание энкодера
// текущее значение энкодера в переменной enc

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[7] = { -4, -2, -1, 0, 1, 2, 3 };
bool polin = 1;
byte step = 0;
bool firststep;
int per = 0;
bool inper = 0;


void setup() {
	attachInterrupt(ENCODER_INT, encoder, RISING);
	myservo.attach(14);
	Serial1.begin(115200);
	Serial.begin(9600);
	pinMode(37, INPUT);
	waitGreen();
}

void loop() {
	Serial.println(step);
	while (step == 0)
	{
		int speed = 80;
		float kp = 12; //40
		float kd = 40; //80

		//Serial.println(per);

		if (enc > 10) inper = 1;

		if (analogRead(A6) > 100 && analogRead(A12) > 100 && inper) {
			polin = 0;
			servo(0);
			speed = 60;
			go(motor, speed);
			delay(250);
			firststep = 1;
			step = 1;
		}

		if (polin) {
			go(motor, speed);
			int d[7] = { analogRead(A6), analogRead(A7), analogRead(A8), analogRead(A9), analogRead(A10), analogRead(A11), analogRead(A12) };
			printSensors(d);
			float err = senOut(d, w);
			float pd = PD(err, kp, kd);
			servo(pd);
		}
	}
	while (step == 1)
	{
		servo(-100);
		go(motor, 60);
		if (firststep) {
			firststep = 0;
			delay(600);
		}
		if (analogRead(A6) > 200) {
			go(motor, -255);
			servo(100);
			step = 2;
		}
	}
	while (step == 2)
	{
		go(motor, -255);
		servo(100);
		if (digitalRead(37) == 1) {
			servo(0);
			go(motor, 60);
			delay(450);
			servo(-100);
			step = 3;
		}
	}
	while (step == 3)
	{
		go(motor, 90);
		servo(-100);
		if (analogRead(A12) > 200) {
			go(motor, 50);
			servo(-100);
			firststep = 1;
			step = 4;
		}
	}
	while (step == 4)
	{
		go(motor, 90);
		servo(-100);
		if (firststep) {
			firststep = 0;
			delay(100);
		}
		if (analogRead(A9) > 200) {
			go(motor, 60);
			step = 7;
			per = 0;
			inper = 0;
			polin = 1;
			enc = 0;
		}
	}
	while (step = 7)
	{
		int speed = 50;
		float kp = 37; //40
		float kd = 40; //80

		Serial.println(enc);
		if (enc > 5) {
			speed = 100;
			kp = 5; //40
			kd = 80; //80

			/*if (analogRead(A6) > 300 && analogRead(A12) > 300) {
				polin = 0;
				servo(0);
				speed = -255;
				go(motor, speed);
				delay(500);
				go(motor, 0);
				while (1);
			}*/
		}
		//if (enc > 17) {
		//	speed = 20;
		//	kp = 12; //40
		//	kd = 40; //80

		//	if (analogRead(A6) > 300 && analogRead(A12) > 300) {
		//		polin = 0;
		//		servo(0);
		//		speed = -255;
		//		go(motor, speed);
		//		delay(100);
		//		go(motor, 0);
		//		while (1);
		//	}
		//}
		Serial.println(kp);
		//if (analogRead(A6) > 400 && analogRead(A12) > 400) {
		//	polin = 0;
		//	servo(0);
		//	speed = -255;
		//	go(motor, speed);
		//	delay(150);
		//	while (true)
		//	{
		//		go(motor, 0);
		//	}
		//}

		if (polin) {
			go(motor, speed);
			int d[7] = { analogRead(A6), analogRead(A7), analogRead(A8), analogRead(A9), analogRead(A10), analogRead(A11), analogRead(A12) };
			printSensors(d);
			float err = senOut(d, w);
			float pd = PD(err, kp, kd);
			servo(pd);
		}
	}
}
