# Arduino-serial-comms
Arduino UART serial communications software for comms between a computer and Arduino boards.

Testing of the serial comms on an Arduino can be performed using a terminal emulator on a computer (see below) to send HEX bytes to the Arduino. To do this, first install a terminal emulator, then send the necessary packets to the Arduino.

## Packet layout

+--------+--------+--------+------+------+-------+------+------+------+------+-------+-------+-------+-------+------+
| Header | Header | Header |  ID  | Addr | Data  | Data | Data | Data | Data | Cksum | Cksum | Cksum | Cksum | Tail |  
|  byte  |  byte  |  byte  | byte | byte | bytes | byte | byte | byte | byte | byte  | byte  | byte  | byte  | byte |
|   1    |   2    |   3    |      |      | 0-255 |  #1  |  #2  |  ... | #255 |   1   |   2   |   3   |   4   |      |
+--------+--------+--------+------+------+-------+------+------+------+------+-------+-------+-------+-------+------+

## Arduino coding

- TimerThree -- this popular timer interrupt library does not work on the Arduino Uno.  I think it is designed for the Mega chips. TimerOne however works fine.

- TimerOne -- https://github.com/PaulStoffregen/TimerOne

## Serial port emulation (send, receive, HEX, binary, raw...)

- Serial port terminal emulators:
  - RealTerm (Windows .exe) is not the prettiest thing around, and hardly simple to use, but it does provide the best feature set for serial port thrashing
  - Termite (Windows .exe) has a HEX plugin for sending HEX bytes. To send HEX using Termite, formatting styles are "0x55" for simple 8-bit bytes and "0xab0xcd0xef" for multiple byte transmissions
  - Various: https://learn.sparkfun.com/tutorials/terminal-basics/all
  - Putty (cannot do HEX): http://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

### RealTerm

- On the "Send" tab, use `0xaa 0xaa 0xaa 0x20 0x21 0x02 0x02 0x02` format for sending hex bytes, and transmit using the "Send numbers" button.
- On the "Send" tab you can also send files to the serial port with inter-byte and inter-packet delays, but the files need to be *raw* binary which you can make using HexEdit 2.0 (http://www.catch22.net/software/hexedit). You need to enter the HEX characters and export as raw binary.

### HEX-to-binary converters

- HexEdit does the job: http://www.catch22.net/software/hexedit
- HxD might be a goody: https://mh-nexus.de/en/hxd/
