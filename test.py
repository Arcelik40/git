import serial, time, sys

control = True;
arduino = serial.Serial('COM4', 9600, timeout = .1)
time.sleep(1) # ONEMLI

instruction = sys.argv[1]
arduino.write(instruction)
while control:
    data = arduino.readline()[:-2]
    if data:
        if (data == "HALT"):
            control = False;
        else:
         print data
arduino.close()
