#include <Arduino.h>
#include <Servo.h>

Servo myservo;

volatile long enc = 0;
float oldin = 0;
bool useold = 0;
int sum = 0;

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

int senSum(int d[16]) {
	int sum = 0;
	for (int i = 0; i < 16; i++) sum = sum + d[i];
	return sum;
}

float PD(float in, float kp, float kd) {
	float out = 0;
	if (useold) out = in * kp + (in - oldin) * kd;
	else out = in * kp;
	useold = 1;
	oldin = in;
	return out;
}

float PID(float in, float kp, float ki, float kd) {
	float out = 0;
	sum += in;
	sum = constrain(sum * ki, -100, 100);
	if (useold) out = in * kp + (in - oldin) * kd + sum * ki;
	else out = in * kp;
	useold = 1;
	oldin = in;
	return out;
}

void servo(float pos, int k = -1) {
	pos = constrain(pos, -100, 100) * k;
	if (pos > 0) {
		pos = map(pos, 0, 100, 90, 120);
	}
	else {
		pos = map(pos, -100, 0, 40, 90);
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
