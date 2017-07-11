#include <SPI.h>
#include <MFRC522.h>

typedef enum AGV_opcode {
    MOV = 'A', RIGHT = 'B', LEFT = 'C', RPT = 'D', BACK = 'F', HALT = 'G' 
} AGV_opcode;

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

void lineFollow(Pos cur_pos)
{
    if (cur_pos.rfid == "NO NEW CARD PRESENT") {
        cur_pos = report_pos();
    }
    while (1) {
        Serial.println(cur_pos.rfid);
        Pos new_pos = report_pos();
        if (new_pos.success && new_pos.rfid != cur_pos.rfid ) {
            Serial.println(new_pos.rfid);
            return;
        }
        PID_and_go();
    }
}

void PID_and_go() {
    Serial.println("YOLDA");
}

void de(char inst) {
    Pos cur_pos = report_pos();
    switch (inst) {
        case MOV:
            lineFollow(cur_pos);
            break;
        case RIGHT:
            turn_right();
            lineFollow(cur_pos);
            break;
        case LEFT:
            turn_left();
            lineFollow(cur_pos);
            break;
        case BACK:
            turn_back();
            lineFollow(cur_pos);
            break;
        case RPT:
            Serial.println(cur_pos.rfid);
            break;
        case HALT:
            Serial.println("G");
            Serial.println("HALT");
        default:
            Serial.println("HALT");
            break;
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
