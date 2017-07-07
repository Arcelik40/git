const int sinyal = 11;
const int sinyal2 = 12;

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
    if (cur == 'N')
      go_north(serial);
    else if (cur == 'E')
      go_east(serial);
    else if (cur == 'W')
      go_west(serial);
    else if (cur == '!')
      halt(serial);
    else if (cur == 'S')
      go_south(serial);
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

