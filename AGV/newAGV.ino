#include <SPI.h>
#include <MFRC522.h>

#define SENSOR_NUMBER 6
const float Kp = 2.9;
const float Ki = 0.00015;
const float Kd = 1;
const int ssr[SENSOR_NUMBER] = {4, 5, 6, 7, 8, 9};

typedef enum AGV_opcode {
    MOV = 'W', RIGHT = 'A', LEFT = 'D', POS = 'P', BACK = 'S', SLOW = 'E', 
    HALT = 'Q' 
} AGV_opcode;

struct Pos {
    bool success;
    String rfid;
};

struct Station {
    bool loading;
    bool unloading;
    bool turning;
}

class Sensors {
    private:
        int sensorvalues[SENSOR_NUMBER];
        int activeSensor;
        float avgSensor;
        float totalSensor;
        float error;
    public :
        const int weights[SENSOR_NUMBER] = {-6, -2, -1, 1 , 2, 6};
        void update(int *);
        float getTotal();
        float getAvg();
        int   getActiveSensor();
        void setTotal();
        void setActiveSensor();
        void setError();
        void getError();
        void setAvg();
};

Sensors::Sensors() {
    sensorvalues = {0,0,0,0,0,0};
    activeSensor = avgSensor = totalSensor = error = 0;
}

void Sensors::setActiveSensor() {
    for (int i = 0; i < SENSOR_NUMBER; i++)
        activeSensor += sensorvalues[i]
}

void Sensors::readSensors() {
    for (int i = 0; i < SENSOR_NUMBER; i++)
        sensorvalues[i] = digitalRead(ssr[i]);
}

void Sensors::setSensorValues(int *new_values) {
    for (int i = 0; i < SENSOR_NUMBER; i++) 
        sensorvalues[i] = new_values[i];
}

void Sensors::setTotal() {
    for (int i = 0; i < SENSOR_NUMBER; i++) 
        totalSensor += sensorvalues[i] * weights[i];
}

void Sensors::setAvg() {
    avgSensor = totalSensor / activeSensor;
}

void Sensors::setError() {
    error = avgSensor;
}

int Sensors::getActiveSensor() {

    return activeSensor;
}

float Sensors::getTotal() {
    return totalSensor;
}

float Sensors::getAvg() {
    return avgSensor;
}

int Sensors::getError() {
    return error;
}

void Sensors::setError() {
    error = avgSensor;
}


#define RST_PIN 40 //9
#define SS_PIN 53 //10

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
    for (int i = 0; i < SENSOR_NUMBER; i++)  {
        pinMode(ssr[i], INPUT);
    }
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(100);
}

void read_sensor() {

}

void loop() {
    if (Serial.available()) {
        char inst = Serial.read();
        if (inst == POS)
            Serial.println(report_pos().rfid);
        else
            lineFollow(inst);
    }
}

void lineFollow(char inst)
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
    Pos cur_pos;
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
        case POS:
            cur_pos = report_pos();
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

void PID_program()
{
  int MAX_SPEED = 18;

  read_sensor();
  previousError = error; // save previous error for differential
  error = avgSensor; // Count how much robot deviate from center
  totalError += error;

  power = (Kp * error) + (Kd * (error - previousError)); // pid hesaplamasi

  if ( power > MAX_SPEED ) {
    power = MAX_SPEED;
  }
  if ( power < -MAX_SPEED ) {
    power = -MAX_SPEED;
  }

  if (power > 0) // Turn left
  {
    PWM_Right = MAX_SPEED + (5 * abs(int(power))) / 10;
    PWM_Left = MAX_SPEED -  (6 * abs(int(power))) / 10;
  }
  else // Turn right
  {
    PWM_Right = MAX_SPEED - (6 * abs(int(power))) / 10;
    PWM_Left = MAX_SPEED +  (5 * abs(int(power))) / 10;
  }

}