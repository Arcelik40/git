#include <MFRC522.h>

typedef enum AGV_opcode {
    MOV = 'A', RIGHT = 'B', LEFT = 'C', RPT = 'D', BACK = 'F', HALT = 'G' 
} AGV_opcode;

#define RST_PIN 40 //9
#define SS_PIN 53 //10


String lastRfid;


MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
void setup() {
    Serial.begin(9600);
    mfrc522.PCD_Init();
    delay(100);
}

void loop() {
    de_cycle(fetch(Serial), Serial);
}

String fetch(HardwareSerial &serial) {
    return (serial.readStringUntil('\n'));
}

void de_cycle(String inst, HardwareSerial &serial) {
    while (true) {
        int opcode  = inst[0];
        inst.remove(0,1);
        switch (opcode) {
            case MOV:
                mov(serial);
                break;
            case RIGHT:
                turn_right(serial);
                break;
            case LEFT:
                turn_left(serial);
                break;
            case BACK:
                turn_back(serial);
                break;
            case RPT:
                report_pos(serial);
                break;
            case HALT:
                halt(serial);
                return;
            default:
                halt(serial);
                return;
    }
  }
}

static inline void mov(HardwareSerial &serial) {
    serial.println("ILERI");
}

static inline void turn_right(HardwareSerial &serial) {
    serial.println("SAG");
}

static inline void turn_left(HardwareSerial &serial) {
    serial.println("SOL");
}

static inline void halt(HardwareSerial &serial) {
  serial.println("HALT");
}

static inline void turn_back(HardwareSerial &serial) {
  serial.println("GERI");
}

static inline  report_pos(HardwareSerial &serial) {
    if (!mfrc522.PICC_IsNewCardPresent())
        return;
    if (!mfrc522.PICC_ReadCardSerial())
        return;
    String rfid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        rfid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
        rfid += String(mfrc522.uid.uidByte[i], HEX);
    }
  //string'in boyutunu ayarla ve tamamÄ±nÄ± bÃ¼yÃ¼k harfe Ã§evir
    rfid.trim();
    rfid.toUpperCase();
    serial.println(serial);


   if (rfid == lastRfid) /// bu Ã§Ä±kartÄ±labilir
       return;
  lastRfid = rfid;
}

static inline void failure(HardwareSerial &serial) {
  serial.println("FAIL");
}

static inline void echo(String inst, HardwareSerial &serial) {
    serial.println(inst);
}
