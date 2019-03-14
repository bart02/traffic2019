#include <Ultrasonic.h>
#include <Servo.h>
#include "func.h"
#include <Wire.h>
#include <Octoliner.h>
#include <SharpIR.h>

Ultrasonic ultrasonic(48, 49);
Octoliner lineleft(42);
Octoliner lineright(45);
SharpIR sharp(A6, 1080);


#define SPEEDYY 60
#define ENCODER_INT 2 // прерывание энкодера
// текущее значение энкодера в переменной enc
int SPEEDTEK = SPEEDYY - 20;

#define PLEFT 52
#define PRIGHT 53
#define STOPSIG 51

#define RED 40
#define YELLOW 39
#define GREEN 38

//            [en, in1, in2]
int motor[3] = { 3, 24, 25 };
float w[16] = { -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8 };
bool polin = 1;
int irtek = -1;
bool ultraon = 0;
bool svetofor = 1;
unsigned long mil = 0;
unsigned long vrem = 3000;
bool stopped = 0;

byte blink = 0; //1red 2 y 3g
bool blink_state = 0;
unsigned long blinkmil = 0;

unsigned long irdamil = 0;
unsigned long timerirdamil = 0;

int count = 1;

int speed = SPEEDTEK;
bool wasstopu = 0;
bool infra = 1;

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
	pinMode(PLEFT, OUTPUT);
	pinMode(PRIGHT, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(STOPSIG, OUTPUT);
	waitGreen();
}

void loop() {
	float kp = 26; //40
	float kd = 100; //80
	int ir = IRread();
	int d[16] = { lineleft.analogRead(7), lineleft.analogRead(6), lineleft.analogRead(5), lineleft.analogRead(4), lineleft.analogRead(3), lineleft.analogRead(2), lineleft.analogRead(1), lineleft.analogRead(0), lineright.analogRead(7), lineright.analogRead(6), lineright.analogRead(5), lineright.analogRead(4), lineright.analogRead(3), lineright.analogRead(2), lineright.analogRead(1), lineright.analogRead(0) };
	//if (ir == 5) { 
	if (infra) {
		int dist = sharp.distance();
		Serial.println(dist);
		if (dist < 30 && polin) {
			SPEEDTEK = SPEEDYY;
			speed = SPEEDTEK;
			wasstopu = 1;
			digitalWrite(STOPSIG, 1);
			polin = 0;
			servo(0);
			go(motor, -255);
			delay(200);
			go(motor, 0);
			delay(1000);
		}
		if ((dist >= 60) && !polin && wasstopu) {
			wasstopu = 0;
			ultraon = 0;
			polin = 1;
			digitalWrite(STOPSIG, 0);
			SPEEDTEK = SPEEDYY;
		}
		//}
	}

	if (ir != -1 && ir != 2 && ir != 3 && ir != 5 && ir < 7 && svetofor && polin) {
		//speed = SPEEDTEK - 20;
		if (lineright.analogRead(0) > 300 && lineright.analogRead(1) > 300 && lineright.analogRead(2) > 300 && lineright.analogRead(3) > 300 && lineright.analogRead(4) > 300 && lineright.analogRead(5) > 300) {
			if (ir == 6) {
				digitalWrite(STOPSIG, 1);
				polin = 0;
				servo(0);
				go(motor, -255);
				delay(200);
				go(motor, 0);
				delay(5000);
				digitalWrite(STOPSIG, 0);
				go(motor, 60);
				delay(500);
				polin = 1;
				mil = millis();
				svetofor = 0;
			}
			else {
				digitalWrite(STOPSIG, 1);
				polin = 0;
				servo(0);
				go(motor, -255);
				delay(200);
				go(motor, 0);
				delay(500);
				mil = millis();
				svetofor = 0;
			}
		}
	}
	if (ir != -1 && ir == 2 && !polin) {
		digitalWrite(STOPSIG, 0);
		servo(0);
		speed = SPEEDTEK;
		//go(motor, speed);
		//delay(1000);
		polin = 1;
	}
	if ((ir == 2 || ir == 3) && polin) {
		if ( (lineright.analogRead(0) > 300 && lineright.analogRead(1) > 300 && lineright.analogRead(2) > 300 && lineright.analogRead(3) > 300 && lineright.analogRead(4) > 300 && lineright.analogRead(5) > 300)  ) {
			speed = SPEEDTEK + 5;
			svetofor = 0;
			mil = millis();
		}
	}
	if (millis() - mil > vrem && !svetofor) {
		speed = SPEEDTEK;
		svetofor = 1;
	}

	if (millis() - mil > vrem) {
		if (blink == PLEFT || blink == PRIGHT) blink = 0;
		digitalWrite(PLEFT, 0);
		digitalWrite(PRIGHT, 0);
		infra = 1;
	}

	if (blink && millis() - blinkmil > 500) {
		blink_state = !blink_state;
		digitalWrite(blink, blink_state);
		blinkmil = millis();
	}

	if (millis() - irdamil > 200) {
		if (ir > -1 && ir < 7) {
			Serial.println(blink);
			if (ir == 0) {
				digitalWrite(RED, 1);
				digitalWrite(YELLOW, 0);
				digitalWrite(GREEN, 0);
				blink = 0;
			}
			else if (ir == 1) {
				digitalWrite(RED, 1);
				digitalWrite(YELLOW, 1);
				digitalWrite(GREEN, 0);
				blink = 0;
			}
			else if (ir == 2) {
				digitalWrite(RED, 0);
				digitalWrite(YELLOW, 0);
				digitalWrite(GREEN, 1);
				blink = 0;
			}
			else if (ir == 3) {
				digitalWrite(RED, 0);
				digitalWrite(YELLOW, 0);
				digitalWrite(GREEN, blink_state);
				blink = GREEN;
			}
			else if (ir == 4) {
				digitalWrite(RED, 0);
				digitalWrite(YELLOW, 1);
				digitalWrite(GREEN, 0);
				blink = 0;
			}
			else if (ir == 5) {
				digitalWrite(RED, 0);
				digitalWrite(YELLOW, blink_state);
				digitalWrite(GREEN, 0);
				blink = YELLOW;
			}
			else if (ir == 6) {
				digitalWrite(RED, blink_state);
				digitalWrite(YELLOW, 0);
				digitalWrite(GREEN, 0);
				blink = RED;
			}
			irdamil = millis();
			timerirdamil = millis();
		}
		else {
			if (millis() - timerirdamil > 1000) {
				digitalWrite(RED, 0);
				digitalWrite(YELLOW, 0);
				digitalWrite(GREEN, 0);
			}
		}
	}

	if (ir == 7 && senSum(d) > 6500 && senSum(d) < 8000 && millis() - mil > 500) {
		infra = 0;
		if (count % 2 == 0) {
			digitalWrite(PLEFT, 1);
			blink = PLEFT;
			polin = 0;
			servo(0);
			delay(500);
			polin = 1;
			//speed = SPEEDTEK;
		} else {
			digitalWrite(PRIGHT, 1);
			blink = PRIGHT;
			polin = 0;
			go(motor, SPEEDTEK - 20);
			servo(70);
			delay(800);
			speed = SPEEDTEK;
			go(motor, speed);
			polin = 1;
		}
		count++;
		mil = millis();
	}
	//if (count % 2 == 0) {
	//	digitalWrite(39, 0);
	//	digitalWrite(40, 1);
	//}
	//else {
	//	digitalWrite(39, 1);
	//	digitalWrite(40, 0);
	//}
	if (polin) {
		float kp = 14; //40
		float ki = 0.09; //40
		float kd = 50; //80
		
		//Serial.println(count);
		//printSensors(d);
		float err = senOut(d, w);
		float pd = PID(err, kp, ki, kd);
		servo(pd);
		go(motor, speed);
		delay(5);
	}
}
