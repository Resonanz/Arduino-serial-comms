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

#define BUFFER_SIZE 255

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
  byte buff[255];
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

static uint16_t commsTimeoutTickCounter = 0;


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerTick_1ms); // blinkLED to run every 1 ms
}

// the loop function runs over and over again until power down or reset
void loop() {
  if (cb_t.head != cb_t.tail) {

    byte b = cb_t.cBuff[cb_t.tail];
    incrementCBTail();

    switch (nextState) {

    case READING_HEADER_1:
      commsTimeoutTickCounter = 0; // reset the interrupt driver tick counter; if max time exceeded, return state machine to beginning
      ledTodo(OFF);
      if (b == HEADER_BYTE)
        nextState = READING_HEADER_2;
      else
        nextState = READING_HEADER_1;
      break;

    case READING_HEADER_2:
      if (b == HEADER_BYTE)
        nextState = READING_HEADER_3;
      else
        nextState = READING_HEADER_1;
      break;

    case READING_HEADER_3:
      if (b == HEADER_BYTE)
        nextState = READING_ID;
      else
        nextState = READING_HEADER_1;
      break;

    case READING_ID:
      if (b == ID_BYTE) {
        nextState = READING_ADDR;
        inPacket_t.id = b;
      }
      else {
        nextState = READING_HEADER_1;
      }
      break;

    case READING_ADDR:
      if (b == ADDR_BYTE) {
        inPacket_t.addr = b;
        nextState = READING_NUMBYTES;
      }
      else {
        nextState = READING_HEADER_1;
      }
      break;

    case READING_NUMBYTES:
      inPacket_t.numBytes = b;
      inPacket_t.tempNumBytes = b;
      inPacket_t.buffIdx = 0;   //   <<<<< prob should have some init code to do this
      nextState = READING_BYTES;
      break;

    case READING_BYTES:
      inPacket_t.buff[inPacket_t.buffIdx] = b;
      inPacket_t.buffIdx++;
      inPacket_t.tempNumBytes--;
      if (inPacket_t.tempNumBytes == 0) { // decrement the numBytes value received previously
//        nextState = READING_FOOTER;
        nextState = VALID_PACKET;
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

    case VALID_PACKET:
      commsTimeoutTickCounter = 0; // reset the interrupt driver tick counter; if max time exceeded, return state machine to beginning
      printMetaState();
      break;

    default:
      break;
    }
  }
}

/*
*  1 ms tick interrupt
*/
void timerTick_1ms() {
  commsTimeoutTickCounter++;
  if (commsTimeoutTickCounter == 150) { // if this counter reaches this number, reset the comms state machine back to searching for the header bytes
    commsTimeoutTickCounter = 0;
    nextState = READING_HEADER_1;
    ledTodo(ON);
  }
}

/*
*  Set the onboard LED to ON, OFF, or TOGGLE current state
*/
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

/*
  For debugging, this displays the storage details from the last incoming data packet
*/
void printMetaState() {
  Serial.print("Head = ");         Serial.print(cb_t.head, HEX);
  Serial.print("  Tail = ");     Serial.print(cb_t.tail, HEX);

  Serial.print("  id = ");       Serial.print(inPacket_t.id, HEX);
  Serial.print("  addr = ");     Serial.print(inPacket_t.addr, HEX);
  Serial.print("  numbytes = "); Serial.print(inPacket_t.numBytes, HEX);

  Serial.print("  Buff = ");
  for (byte i = 0; i < inPacket_t.numBytes; i++) {
    Serial.print(inPacket_t.buff[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void incrementCBHead() {
  if (++cb_t.head == BUFFER_SIZE)
    cb_t.head = 0;
}

void incrementCBTail() {
  if (++cb_t.tail == BUFFER_SIZE)
    cb_t.tail = 0;
}

void serialEvent() {
  while (Serial.available()) {
    cb_t.cBuff[cb_t.head] = Serial.read();
    incrementCBHead();
  }
}

