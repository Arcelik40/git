const int sinyal = 11;
const int sinyal2 = 12;

typedef enum AGV_opcode {
    MOVE = 0, RIGT, LEFT, RPT, BACK, 
} AGV_opcode;

void setup() {
    Serial.begin(9600);
    pinMode(sinyal, OUTPUT);
    pinMode(sinyal2, INPUT);
    delay(100);
}

void loop() {
    decode(fetch(Serial), Serial);
}

String fetch(HardwareSerial &serial) {
  return (serial.readStringUntil('\n'));
}

void decode(String inst, HardwareSerial &serial) {
  while (true) {
    char cur  = inst[0];
    inst.remove(0,1);
    case (opcode) {
      case MOVE:
    }
  }
}

void go_north(HardwareSerial &serial) {
  serial.println("ILERI");
  digitalWrite(sinyal, HIGH);
}

void go_east(HardwareSerial &serial) {
  serial.println("SAG");
}

void go_west(HardwareSerial &serial) {
  serial.println("SOL");
}

void halt(HardwareSerial &serial) {
  serial.println("HALT");
}

void go_south(HardwareSerial &serial) {
  serial.println("GERI");
  digitalWrite(sinyal, LOW);
}

