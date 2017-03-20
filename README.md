# Arduino-serial-comms
Arduino UART serial communications software for comms between a computer and Arduino boards.

Testing of the serial comms on an Arduino can be performed using Termite (Windows) and sending HEX bytes. To do this, install Termite with all the plugins and activate the HEX option. To send HEX use the format "0x55" for simple 8-bit bytes or "0xab0xcd0xef" and the like for multiple byte transmissions.

## Notes

- TimerThree -- this popular timer interrupt library does not work on the Arduino Uno.  I think it is designed for the Mega chips. TimerOne however works fine.

TimerOne URL: https://github.com/PaulStoffregen/TimerOne
