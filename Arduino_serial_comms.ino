#include <TimerOne.h>

/*
Name:    circBuffer.ino
Created: 3/17/2017 12:09:57 AM
Author:  robwa
*/

// ledTodo() calls
enum {
	OFF = 0,
	ON = 1,
	TOGGLE = 2
};

// Special protocol bytes
enum {
	HEADER_BYTE = 0xaaU,
	ID_BYTE = 0x20U,
	ADDR_BYTE = 0x21U,
	STUFF_BYTE = 0x55U,
	FOOTER_BYTE = 0x55U,
};

#define BUFFER_SIZE 100

// Receiving state machine
enum {
	READING_HEADER_1,
	READING_HEADER_2,
	READING_HEADER_3,
	READING_ID,
	READING_ADDR,
	READING_NUMBYTES,
	READING_BYTES,
	READING_CKSUM1,
	READING_CKSUM2,
	READING_CKSUM3,
	READING_CKSUM4,
	READING_FOOTER,
	VALID_PACKET
};

typedef struct incomingPacket {
	byte id = 0;
	byte addr = 0;
	byte numBytes = 0;
	byte tempNumBytes = 0;
	byte buff[50];
	byte buffIdx = 0;
} incomingPacket_t;

typedef struct circBuff {
	byte cBuff[BUFFER_SIZE]; // receive buffer, 255 bytes max. limited by size(byte)
							 //byte cBuffIdx = 0;       // index into buff[]
	byte head = 0;
	byte tail = 0;
} circBuffer_t;

circBuffer_t cb_t;
incomingPacket_t inPacket_t;

static byte nextState = READING_HEADER_1;

static uint16_t tickCounter = 0;

/*
void init_cb_t () {
for (byte i = 0; i < BUFFER_SIZE; i++) { cb_t[i] = 0; }
cb_t.buffIdx = 0;
cb_t.head = 0;
cb_t.tail = 0;
}

void resetStateMachine() {
init_cb_t();
nextState = READING_HEADER_1;

}
*/

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(19200);
	pinMode(13, OUTPUT);
	Timer1.initialize(10000);
	Timer1.attachInterrupt(timerTick_10ms); // blinkLED to run every 10 ms
}

// the loop function runs over and over again until power down or reset
void loop() {
	byte b;
	if (cb_t.head != cb_t.tail) {

		b = cb_t.cBuff[cb_t.tail];
		incrementCBTail();

		switch (nextState) {

		case READING_HEADER_1:
			if (b == HEADER_BYTE) {
				tickCounter = 0; // reset the interrupt driver tick counter; if max time exceeded, return state machine to beginning 
				nextState = READING_HEADER_2;
				ledTodo(TOGGLE);
				Serial.write(0x33);
			}
			else {
				nextState = READING_HEADER_1;
			}
			break;

		case READING_HEADER_2:
			if (b == HEADER_BYTE) {
				nextState = READING_HEADER_3;
				ledTodo(TOGGLE);
				Serial.write(0x34);
			}
			else {
				nextState = READING_HEADER_1;
			}
			break;

		case READING_HEADER_3:
			if (b == HEADER_BYTE) {
				nextState = READING_ID;
				ledTodo(TOGGLE);
				Serial.write(0x35);
			}
			else {
				nextState = READING_HEADER_1;
			}
			break;

		case READING_ID:
			if (b == ID_BYTE) {
				nextState = READING_ADDR;
				inPacket_t.id = b;
				ledTodo(TOGGLE);
				Serial.write(0x36);
			}
			else {
				nextState = READING_HEADER_1;
			}
			break;

		case READING_ADDR:
			if (b == ADDR_BYTE) {
				inPacket_t.addr = b;
				ledTodo(TOGGLE);
				Serial.write(0x37);
				nextState = READING_NUMBYTES;
			}
			else {
				nextState = READING_HEADER_1;
			}
			break;

		case READING_NUMBYTES:
			inPacket_t.numBytes = b;
			inPacket_t.tempNumBytes = b;
			ledTodo(TOGGLE);
			Serial.write(b);
			inPacket_t.buffIdx = 0;   //   <<<<< prob should have some init code to do this
			nextState = READING_BYTES;
			break;

		case READING_BYTES:
			inPacket_t.buff[inPacket_t.buffIdx] = b;
			inPacket_t.buffIdx++;
			ledTodo(TOGGLE);
			inPacket_t.tempNumBytes--;
			if (inPacket_t.tempNumBytes == 0) { // decrement the numBytes value received previously
				nextState = READING_FOOTER;
				for (byte i = 0; i < inPacket_t.numBytes; i++) {
					Serial.write(inPacket_t.buff[i]);
				}
			}
			break;

		case READING_FOOTER:
			if (b == FOOTER_BYTE) {
				inPacket_t.numBytes = b;
				nextState = VALID_PACKET;
			}
			else {
				nextState = VALID_PACKET;
			}
			break;

		default:
			break;
		}
	}
	//delay(100);
}

void timerTick_10ms() {
	tickCounter++;
	if (tickCounter == 4) { // if this counter reaches this number, reset the comms state machine back to searching for the header bytes
		tickCounter = 0;
		ledTodo(OFF);
		nextState = READING_HEADER_1;
	}
}

void ledTodo(byte todo) {
	static bool ledState; // hold the LED state 

	switch (todo) {
	case OFF:
		digitalWrite(13, LOW);
		break;
	case ON:
		digitalWrite(13, HIGH);
		break;
	case TOGGLE:
		digitalWrite(13, ~ledState);
	default:
		break;
	}
}

void printMetaState() {
	Serial.print("Head = "); Serial.println(cb_t.head, HEX);
	Serial.print("Tail = "); Serial.println(cb_t.tail, HEX);
	Serial.print("Next state = "); Serial.println(nextState, HEX);
	Serial.print("Buff = ");
	for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
		Serial.print(cb_t.cBuff[i], HEX);
		Serial.print(" ");
	}
	Serial.println("");
}

void incrementCBHead() {
	//cb.head++;
	if (++cb_t.head == BUFFER_SIZE)
		cb_t.head = 0;

}

void incrementCBTail() {
	//cb.tail++;
	if (++cb_t.tail == BUFFER_SIZE)
		cb_t.tail = 0;

}

void serialEvent() {
	while (Serial.available()) {
		cb_t.cBuff[cb_t.head] = Serial.read();
		incrementCBHead();
	}
}