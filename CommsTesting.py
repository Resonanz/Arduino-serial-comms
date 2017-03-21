This code is currently broken
http://stackoverflow.com/questions/5649407/hexadecimal-string-to-byte-array-in-python

import serial

ser.close()

ser = serial.Serial('COM3', 9600, timeout=3)  # open serial port

sleep(2) # wait for the Arduino to reset (reset is driven by pening the serial port via the DTR line)

ser.write(b'\xaa\xaa\xaa\x20\x21\x05\x55\x55\x55\x55\x55')

counter = 0

while (counter < 100):
    bytesToRead = ser.inWaiting()
    print(bytesToRead)
    incoming = ser.read(bytesToRead)
    print(incoming)
    counter = counter + 1

print(incoming)
ser.close()
