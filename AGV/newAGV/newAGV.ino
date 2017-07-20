#include <SPI.h>
#include <MFRC522.h>
#include "RoboClaw.h"
#include <Sensors.h>
#include <SoftwareSerial.h>

SoftwareSerial serial(11, 12);
RoboClaw roboclaw(&serial, 10000);
#define address 0x80


#define RST_PIN 40 //9
#define SS_PIN 53 //10
#define ACIL 13
MFRC522 mfrc522(SS_PIN, RST_PIN);

bool turning_started = false;
bool turning_finished = false;
float error = 0;

const float Kp = 2.9;
const float Ki = 0.00015;
const float Kd = 1;

typedef enum AGV_opcode {
    WAIT = 0, NORMAL, FORWARD, RIGHT, LEFT,  HALT
} AGV_opcode;

#define INTERVAL 100
int opcode = 0;
unsigned long cur_time = 0;
unsigned long prev_time = 0;


void setup() {
    pinMode(ACIL, INPUT);
    Serial.begin(9600);
    roboclaw.begin(38400);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(100);
    cur_time = millis();
    prev_time = cur_time;
}

void loop() {
    cur_time = millis();
    if (cur_time - prev_time > INTERVAL) {
        String cur_rfid = read_rfid();
//        Serial.println(cur_rfid);
        prev_time = cur_time; 
    }
    if (Serial.available()) {
        updateStatus(Serial.parseInt());   
    }
    PID(opcode);
}

void updateStatus(int s) {
    opcode = s;
}

void PID(int opcode) {
    Sensors s;
    Serial.println(opcode);
    s.readSensors();
    s.setActiveSensor();
    int aktif = s.getActiveSensor();
    if (turning_started && aktif > 3) {
        Serial.print("girdim");
        turning_finished = true;
    }
    if (turning_finished && aktif < 3) {
        turning_started = false;
        turning_finished = false;
        updateStatus(1);
    }

    if (aktif > 3 && opcode != 1) {
      
        int newSensorV [6] = {0,0,0,0,0,0};
        if (opcode == 2) {
            turning_started = true;
            newSensorV[2] = 1;
            newSensorV[3] = 1;
        }
        else if (opcode == 3) {
            turning_started = true;
            newSensorV[0] = 1;
            newSensorV[1] = 1;
        }
        else if (opcode == 4) {
            turning_started = true;
            newSensorV[4] = 1;
            newSensorV[5] = 1       ;
        }
        s.setSensorValues(newSensorV);
        s.printSensors();
    }
    aktif = s.getActiveSensor();
    s.setTotal();
    s.setAvg();
    float avg = s.getAvg();
   /* Serial.println(aktif);
    delay(200);*/
    
  
    if (aktif > 0 && digitalRead(ACIL))
        go(avg);
    else { 
        roboclaw.ForwardM1(address, 0);
        roboclaw.BackwardM2(address,0);
    }
}
void go(float avg){
    int previousError = error; // save previous error for differential
    error = avg; // Count how much robot deviate from center
    int MAX_SPEED = 16;
    int PWM_Right;
    int PWM_Left;
    int power = (Kp * error) + (Kd * (error - previousError));
  
    if ( power > MAX_SPEED )
        power = MAX_SPEED;
    if ( power < -MAX_SPEED ) 
        power = -MAX_SPEED;

    if (power > 0) {
        PWM_Right = MAX_SPEED + (5 * abs(int(power))) / 10;
        PWM_Left = MAX_SPEED -  (6 * abs(int(power))) / 10;
    }
    else   {
        PWM_Right = MAX_SPEED - (6 * abs(int(power))) / 10;
        PWM_Left = MAX_SPEED +  (5 * abs(int(power))) / 10;
    }
/*   Serial.print("SAG: ");
   Serial.println(PWM_Right);
   delay(100);
   Serial.print("SOL: ");
   Serial.println(PWM_Left);
*/
    roboclaw.ForwardM1(address, abs(PWM_Right));
    roboclaw.BackwardM2(address, abs(PWM_Left));
}

static inline String read_rfid() {
    String rfid = "";
    mfrc522.PICC_IsNewCardPresent();
    if (!mfrc522.PICC_ReadCardSerial())
        return rfid;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        rfid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
        rfid += String(mfrc522.uid.uidByte[i], HEX);
    }

    rfid.trim();
    rfid.toUpperCase();
    return rfid;
}

