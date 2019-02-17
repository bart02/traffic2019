#include <Ultrasonic.h>
#include <Servo.h>
#include "func.h"

Ultrasonic ultrasonic(48, 49);

#define ENCODER_INT 2 // прерывание энкодера
// текущее значение энкодера в переменной enc
#define SPEEDTEK 45

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[7] = { -3, -2, -1, 0, 1, 2, 3 };
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
		speed = 45;
		if (analogRead(A9) > 300 && analogRead(A10) > 300 && analogRead(A11) > 300 && analogRead(A12) > 300) {	
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
		//go(motor, speed);
		//delay(1000);
		polin = 1;
	}
	if ((ir == 2 || ir == 3) && polin) {
		if (analogRead(A9) > 300 && analogRead(A10) > 300 && analogRead(A11) > 300 && analogRead(A12) > 300) {
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
		int d[7] = { analogRead(A6), analogRead(A7), analogRead(A8), analogRead(A9), analogRead(A10), analogRead(A11), analogRead(A12) };
		//printSensors(d);
		float err = senOut(d, w);
		float pd = PD(err, kp, kd);
		servo(pd);
		go(motor, speed);
	}
}
