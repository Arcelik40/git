import serial, time, sys

control = True;
arduino = serial.Serial('COM4', 9600, timeout = .1)
time.sleep(2) # ONEMLI

while control:
    inst = raw_input()
    arduino.write(inst)
    time.sleep(1);
    while (arduino.in_waiting > 0):
        data = arduino.readline()[:-2]
        if data:
            if (data == "HALT"):
                control = False;
            else:
                print data
        time.sleep(0.009)
arduino.close()
