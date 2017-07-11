#include <SPI.h>
#include <MFRC522.h>


typedef enum AGV_opcode {
    MOV = 'A', RIGHT = 'B', LEFT = 'C', RPT = 'D', BACK = 'F', HALT = 'G' 
} 
AGV_opcode;

struct Pos
{
    bool success;
    String rfid;
};

#define RST_PIN 40 //9
#define SS_PIN 53 //10


MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
    Serial.begin(9600);

    SPI.begin();
    mfrc522.PCD_Init();
    delay(100);
}

void loop() {
    if (Serial.available()) {
        char inst = Serial.read();
        de(inst);
    }
    }

//---------------------------------------------------------------
void lineFollow(void)
{
  Pos cu_pos= report_pos();
  Serial.println (cu_pos.rfid)
}

static inline Pos report_pos() {
    Pos position;
    position.success = false;
    if (! mfrc522.PICC_IsNewCardPresent()) {
        position.rfid = "NO NEW CARD PRESENT";
        return position;
    }
    if (! mfrc522.PICC_ReadCardSerial()) {

        position.rfid = "COULD NOT READ RFID CARD";
        return position;
    }

    position.success = true;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        position.rfid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
        position.rfid += String(mfrc522.uid.uidByte[i], HEX);
    }
    position.rfid.trim();
    position.rfid.toUpperCase();
    return position;
  }

void de(char inst) {
    switch (opcode) {
        case MOV:
            lineFollow();
            break;
        case RIGHT:
            turn_right();
            lineFollow()
            break;
        case LEFT:
            turn_left();
            lineFollow()
            break;
        case BACK:
            turn_back();
            lineFollow()
            break;
        case RPT:
            report_pos();
            break;
        case HALT:
            halt = true;
            break;
        default:
            halt = true;
    }
}

static inline void mov() {
    Serial.println("ILERI");
}

static inline void turn_right() {
    Serial.println("SAG");
}

static inline void turn_left() {
    Serial.println("SOL");
}

static inline void turn_back() {
    Serial.println("GERI");
}

static inline Pos report_pos() {
    Pos position;
    position.success = false;
    if (! mfrc522.PICC_IsNewCardPresent()) {
        position.rfid = "NO NEW CARD PRESENT";
        return position;
    }
    if (! mfrc522.PICC_ReadCardSerial()) {

        position.rfid = "COULD NOT READ RFID CARD";
        return position;
    }

    position.success = true;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        position.rfid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
        position.rfid += String(mfrc522.uid.uidByte[i], HEX);
    }
    position.rfid.trim();
    position.rfid.toUpperCase();
    return position;
}

static inline void echo(String inst) {
    Serial.println(inst);
}
