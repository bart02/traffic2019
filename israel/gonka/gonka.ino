#include <Servo.h>
#include "func.h"

#define ENCODER_INT 2 // прерывание энкодера
// текущее значение энкодера в переменной enc

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[7] = { -3.2, -2, -1, 0, 1, 2, 3.2 };


void setup() {
	attachInterrupt(ENCODER_INT, encoder, RISING);
	myservo.attach(14);
	Serial1.begin(115200);
	Serial.begin(9600);
	waitGreen();
	enc = -5;

}

void loop() {
	byte speed = 120;
	float kp = 3; //40
	float kd = 80; //80

	Serial.println(enc);

	if (enc > 20) {
		speed = 0;
	}
	if (enc > 30) {
		speed = 40;
		kp = 25; //40
		kd = 80;
	}
	if (enc > 60) {
		speed = 65;
		kp = 30;
		kd = 100;
	}
	if (enc > 100) {
		speed = 90;
		kp = 10;
		kd = 20;
	}
	if (enc > 115) {
		speed = 50;
		kp = 20;
		kd = 30;
	}
	if (enc > 140) {
		speed = 60;
		kp = 23;
		kd = 50;
	}
	if (enc > 200) {
		speed = 150;
		kp = 3;
		kd = 70;
	}
	//if (enc > 210) {
	//	speed = 140;
	//	kp = 5;
	//	kd = 200;
	//}
	//if (enc > 90) {
	//	speed = 40;
	//	kp = 35;
	//	kd = 40;
	//}
	//if (enc > 120) {
	//	speed = 60;
	//	kp = 40;
	//	kd = 40;
	//}
	//if (enc >215) {
	//	go(motor, 90);
	//	kp = 12;
	//	kd = 40;
	//}
	//if (enc > 250) {
	//	go(motor, 0);
	//}
	//if (enc > 275) {
	//	go(motor, 60);
	//	kp = 40;
	//	kd = 40;
	//}
	//if (enc > 360) {
	//	go(motor, 100);
	//	kp = 13;
	//	kd = 40;
	//}
	//if (enc > 390) {
	//	go(motor, 120);
	//	kp = 10;
	//	kd = 40;
	//}

	go(motor, speed);
	int d[7] = { analogRead(A6), analogRead(A7), analogRead(A8), analogRead(A9), analogRead(A10), analogRead(A11), analogRead(A12) };
	printSensors(d);
	float err = senOut(d, w);
	float pd = PD(err, kp, kd);
	servo(pd);
}
