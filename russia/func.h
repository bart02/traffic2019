#include <Arduino.h>
#include <Servo.h>

Servo myservo;

volatile long enc = 0;
float oldin = 0;
bool useold = 0;

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

float senOut(int d[7], float w[7]) {
	float ch = 0;
	for (int i = 0; i < 7; i++) {
		//    Serial.print(d[i] * w[i]);
		//    Serial.print("\t");
		ch = ch + d[i] * w[i];
	}
	//  Serial.println();
	float zn = 0;
	for (int i = 0; i < 7; i++) zn = zn + d[i];

	float frac = float(ch) / float(zn);
	return frac;
}

float PD(float in, float kp, float kd) {
	float out = 0;
	if (useold) out = in * kp + (in - oldin) * kd;
	else out = in * kp;
	useold = 1;
	oldin = in;
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

void printSensors(int d[7]) {
	Serial.print(d[0]);
	Serial.print("\t");
	Serial.print(d[1]);
	Serial.print("\t");
	Serial.print(d[2]);
	Serial.print("\t");
	Serial.print(d[3]);
	Serial.print("\t");
	Serial.print(d[4]);
	Serial.print("\t");
	Serial.print(d[5]);
	Serial.print("\t");
	Serial.println(d[6]);
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
