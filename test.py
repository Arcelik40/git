import serial, time, sys

arduino = serial.Serial('COM4', 9600, timeout = .1)
time.sleep(2)
agv = AGV(arduino)

inst = sys.argv[1]

while control:
    com = inst[0]
    inst = inst[1:]
    arduino.write(com)
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

class AGV:

    serial
    curRfid
    curOrder

    def __init__(self, connection) :
        while

    def _initialize(self):
        rfid = initalRfid()
        if (rfid == "COULD NOT RFID")

    def initialRfid():
        self.connection.reset_input_buffer()
        rfid = getRfid()
        if (rfid == "COULD NOT READ RFID CARD"):
            rfid = getRfid()
            if (rfid == "COULD NOT READ RFID CARD"):
                print("LOCATION NOT KNOWN, FOLLOWING LINE UNTIL RFID")
                giveOrder(1)
                while (not isNewRfidPresent()):




    def getRfid():


