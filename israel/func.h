#include <Arduino.h>
#include <Servo.h>

Servo myservo;

volatile long enc = 0;
float oldin = 0;
bool useold = 0;
const int len_arr = 10;
float err_arr[len_arr];
unsigned int err_kol = 0;

void encoder() {
	enc++;
}


void go(int motor[3], int speedd, bool forward = 1) {
	if (speedd < 0) {
		speedd = abs(speedd);
		forward = 0;
	}
	analogWrite(motor[0], speedd);
	if (forward) {
		digitalWrite(motor[1], 0);
		digitalWrite(motor[2], 1);
	}
	else {
		digitalWrite(motor[1], 1);
		digitalWrite(motor[2], 0);
	}
}

float senOut(int d[16], float w[16]) {
	float ch = 0;
	for (int i = 0; i < 16; i++) {
		//    Serial.print(d[i] * w[i]);
		//    Serial.print("\t");
		ch = ch + d[i] * w[i];
	}
	//  Serial.println();
	float zn = 0;
	for (int i = 0; i < 16; i++) zn = zn + d[i];

	float frac = float(ch) / float(zn);
	return frac;
}

float PD(float in, float kp, float kd) {
	float out = 0;
	err_arr[err_kol % 10] = in;
	err_kol++;
	if (useold) out = in * kp + (err_arr[err_kol] - err_arr[(err_kol + 11) % 10]) * kd;
	else out = in * kp;
	if (err_kol > 10) useold = 1;
	return out;
}

void servo(float pos, int k = -1) {
	pos = constrain(pos, -100, 100) * k;
	if (pos > 0) {
		pos = map(pos, 0, 100, 90, 140);
	}
	else {
		pos = map(pos, -100, 0, 30, 90);
	}
	myservo.write(int(pos));
}

void printSensors(int d[16]) {
	for (int i = 0; i < 16; i++) {
		Serial.print(d[i]);
		Serial.print("\t");
	}
	Serial.println();
}

int IRread() {
	int in = -1;
	while (Serial1.available()) {
		in = Serial1.read();
	}
	return in;
}

void waitGreen() {
	int ir = IRread();
	while (ir != 2) {
		do {
			ir = IRread();
		} while (ir == -1);
		Serial.println(ir);
	}
}
