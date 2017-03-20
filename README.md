# Arduino-serial-comms
Arduino UART serial communications software for comms between a computer and Arduino boards.

Testing of the serial comms on an Arduino can be performed using Termite (Windows) and sending HEX bytes. To do this, install Termite with all the plugins and activate the HEX option. To send HEX use the format "0x55" for simple 8-bit bytes or "0xab0xcd0xef" and the like for multiple byte transmissions.

## Notes

- TimerThree -- this popular timer interrupt library does not work on the Arduino Uno.  I think it is designed for the Mega chips. TimerOne however works fine.

- TimerOne -- https://github.com/PaulStoffregen/TimerOne

- Serial port terminal emulators:
  - Various: https://learn.sparkfun.com/tutorials/terminal-basics/all
  - Putty (cannot do HEX): http://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

  ### RealTerm

  - On the "Send" tab, use `0xaa 0xaa 0xaa 0x20 0x21 0x02 0x02 0x02` format for sending hex bytes, and transmit using the "Send numbers" button
  - On the "Send" tab you can also send files to the serial port, but the files need to be *raw* binary which you can make using HexEdit (http://www.catch22.net/software/hexedit)
