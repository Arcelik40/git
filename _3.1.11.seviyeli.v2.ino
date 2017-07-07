#include <SoftwareSerial.h>
#include "RoboClaw.h"

SoftwareSerial serial(11, 12);
RoboClaw roboclaw(&serial, 10000);
#define address 0x80

int sensor[6] = {  4, 5, 6, 7, 8, 9};
float weights[6] = { -6, -2, -1, 1 , 2, 6 };

const int GUVENLIK = 22; // guvenlik Arduino'muzun outputu bizim inputumuz

float activeSensor = 0;
int aktifsens = 0;
float totalSensorweight = 0;
float avgSensor = 0; // sensor araliklari toplami

float Kp = 2.9;
float Ki = 0.00015;
float Kd = 1;
float error = 0;
float previousError = 0;
float totalError = 0;

float power = 0;
int PWM_Left;
int PWM_Right;

const int hizlimod = 10; // hızlı mod inputu

// soldaki motor M2

int MAX_SPEED = 22;
//25 ok oldu

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define RST_PIN 40 //9
#define SS_PIN 53 //10

//UNO -> MEGA
//13 -> 52
//12 -> 50
//11 -> 51
//10 -> 53

MFRC522 mfrc522(SS_PIN, RST_PIN);

String lastRfid = "";
String kart1 = "";
String kart2 = "";
String kart3 = "";
String kart4 = "";
String kart5 = "";
String kart6 = "";
String kart7 = "";
String kart8 = "";
String kart9 = "";
String kart10 = "";
String kart11 = "";
String kart12 = "";
String kart13 = "";
String kart14 = "";

MFRC522::MIFARE_Key key;

int yuklemeist = 0;
int bosaltmaist = 0;

int birakmaguvenlikrfid = 0;
int yavasrfid = 0;
int dengeliyavasrfid = 0;
int ekstrayavasrfid = 0;

const int varliksensoru = 3;
const int varliksensoru2 = 2;

const int acilbuton = 13;
//////////////////////////

void setup() {
  roboclaw.begin(38400);
  Serial.begin(9600);

  pinMode(GUVENLIK, INPUT) ; // guvenlik check

  for (int i = 0; i < 6; i++)  {
    pinMode(sensor[i], INPUT);
  }

  ///////////////////////////
  pinMode(varliksensoru, INPUT);
  pinMode(varliksensoru2, INPUT);

  pinMode(hizlimod, INPUT);
  pinMode(acilbuton, INPUT);
  /////////////////////////////

  SPI.begin();
  mfrc522.PCD_Init();
  readEEPROM();
  //////////////////////////
}

void loop()
{
  lineFollow();
}

void lineFollow(void)
{
  rfid();
  PID_program();
  go(PWM_Left, PWM_Right);
}

void PID_program()
{
  readSensor();
  previousError = error; // save previous error for differential
  error = avgSensor; // Count how much robot deviate from center
  totalError += error;

  if (digitalRead(hizlimod))
  {
    MAX_SPEED = 35;
    if (ekstrayavasrfid == 1)
    {
      MAX_SPEED =  MAX_SPEED / 5;
    }
    else if (yavasrfid == 1)
    {
      MAX_SPEED =  MAX_SPEED / 4;
    }
    else if (dengeliyavasrfid == 1)
    {
      MAX_SPEED =  MAX_SPEED / 3;
    }
  }
  else
  {
    MAX_SPEED = 22;
    if (ekstrayavasrfid == 1)
    {
      MAX_SPEED =  MAX_SPEED / 4;
    }
    else if (yavasrfid == 1)
    {
      MAX_SPEED =  MAX_SPEED / 3;
    }
    else if (dengeliyavasrfid == 1)
    {
      MAX_SPEED =  MAX_SPEED / 2;
    }
  }

  power = (Kp * error) + (Kd * (error - previousError));
  
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

void rfid()
{
  //yeni kart okununmadÄ±kÃ§a devam etme
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //kartÄ±n UID'sini oku, rfid isimli string'e kaydet
  String rfid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    rfid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
    rfid += String(mfrc522.uid.uidByte[i], HEX);
  }
  //string'in boyutunu ayarla ve tamamÄ±nÄ± bÃ¼yÃ¼k harfe Ã§evir
  rfid.trim();
  rfid.toUpperCase();

  if (rfid == lastRfid) /// bu Ã§Ä±kartÄ±labilir
    return;
  lastRfid = rfid;
  //1 nolu kart okunduysa LED'i yak, 2 nolu kart okunduysa LED'i sÃ¶ndÃ¼r

  if (rfid == kart1 || rfid == kart2 || rfid == kart3 || rfid == kart4)
  {
    yavasrfid = 1;
  }
  if (rfid == kart5 || rfid == kart6 || rfid == kart7)
  {
    yuklemeist = 1;
  }
  if (rfid == kart8 || rfid == kart9 || rfid == kart10 )
  {
    bosaltmaist = 1;
  }
  if (rfid == kart11 || rfid == kart12 || rfid == kart13)
  {
    birakmaguvenlikrfid = 0;
    yavasrfid = 0;
    dengeliyavasrfid = 0;
    ekstrayavasrfid = 0;
  }
}

void go(int speedLeft, int speedRight) {
  if (digitalRead(acilbuton))
  {
    if (yuklemeist == 1 && (digitalRead(varliksensoru) == 1 || digitalRead(varliksensoru2) == 1))
    {
      for (int wait = 0; wait < 60; wait++)
      {
        roboclaw.ForwardM1(address, 0);
        roboclaw.ForwardM2(address, 0);
        speedLeft = 0;
        speedRight = 0;

        yavasrfid = 0;
        dengeliyavasrfid = 0;
        ekstrayavasrfid = 0;
        Serial.println("Yukleme basladi");
        if (digitalRead(varliksensoru) == 0 && digitalRead(varliksensoru2) == 0)
        {
          Serial.println("Bosaltma beklemesine giris yasandı");
          for (int yukist = 0; yukist < 2500; yukist++)
          {
            Serial.println(yukist);
          }
          Serial.println("Bosaltma tamamlandı ");
          Serial.print("c                    Bosaltma tamamlandı");
          yuklemeist = 0;
          return;
        }
        if (digitalRead(varliksensoru) == 1 || digitalRead(varliksensoru2) == 1)
        {
          wait = 0;
          Serial.print("wait sıfırlandı");
        }
      }
    }
    else if (bosaltmaist == 1 && digitalRead(varliksensoru) == 0 && digitalRead(varliksensoru2) == 0)
    {
      Serial.println("bosaltma dongusu");
      for (int wait = 0; wait < 60; wait++)
      {
        roboclaw.ForwardM1(address, 0);
        roboclaw.ForwardM2(address, 0);
        speedLeft = 0;
        speedRight = 0;

        ekstrayavasrfid = 0;
        yavasrfid = 0;
        dengeliyavasrfid = 1;

        birakmaguvenlikrfid = 1;

        Serial.println("Bosaltma basladi");
        if (digitalRead(varliksensoru) == 1 && digitalRead(varliksensoru2) == 1)
        {
          Serial.println("Bosaltma beklemesine giris yasandı");
          for (int ist = 0; ist < 2000; ist++)
          {
            Serial.println(ist);
          }
          //          // sensorler parazit yapabilirse diye aşağıdaki check yapılabilir. bu check ya üstteki ist for loopunda yada allta tek olarak yapılabilir.
          //          if (digitalRead(varliksensoru) == 0 || digitalRead(varliksensoru2) == 0)
          //          {
          //            return;
          //          }
          Serial.println("Bosaltma tamamlandı ");
          Serial.print("Bosaltma tamamlandı                    Bosaltma tamamlandı                     Bosaltma tamamlandı");
          bosaltmaist = 0;
          return;
        }
        if (digitalRead(varliksensoru) == 0 || digitalRead(varliksensoru2) == 0)
        {
          wait = 0;
          Serial.print("wait sıfırlandı");
        }
      }
    }
    else
    {
      yuklemeist = 0;
      bosaltmaist = 0;
      if (( aktifsens == 1 ) && ( digitalRead(GUVENLIK) == HIGH || birakmaguvenlikrfid == 1))
      {
        roboclaw.ForwardM1(address, abs(speedRight));
        roboclaw.BackwardM2(address, abs(speedLeft));
      }
      else  ////////gÃ¼venlik ihlali var ise ya frenleri devreye alÄ±rÄ±z, yada pwmleri sÄ±fÄ±rlarÄ±z
      {
        roboclaw.ForwardM1(address, 0);
        roboclaw.BackwardM2(address, 0);
      }
    }
  }
  else
  {
    speedLeft = 0;
    speedRight = 0;
    roboclaw.ForwardM1(address, 0);
    roboclaw.ForwardM2(address, 0);
    Serial.println("Acil stop");
  }
}

void readSensor() {
  //avgSensor=0;
  for (int i = 0; i < 6; i++)
  {
    if (digitalRead(sensor[i]) == 1)
    {
      activeSensor += 1;
      totalSensorweight += weights[i];
    }
  }
  if (activeSensor == 0)
  {
    aktifsens = 0;
  }
  else
  {
    aktifsens = 1;
  }

  avgSensor = totalSensorweight / activeSensor;
  activeSensor = 0; totalSensorweight = 0;
}

void readEEPROM()
{
  //EEPROM'dan ilk kartÄ±n UID'sini oku (ilk 4 byte)
  for (int i = 0 ; i < 4 ; i++)
  {
    kart1 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart1 += String(EEPROM.read(i), HEX);
  }
  kart1.trim();
  kart1.toUpperCase();

  for (int i = 4 ; i < 8 ; i++)
  {
    kart2 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart2 += String(EEPROM.read(i), HEX);
  }
  kart2.trim();
  kart2.toUpperCase();
  for (int i = 8 ; i < 12 ; i++)
  {
    kart3 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart3 += String(EEPROM.read(i), HEX);
  }
  kart3.trim();
  kart3.toUpperCase();
  for (int i = 12 ; i < 16 ; i++)
  {
    kart4 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart4 += String(EEPROM.read(i), HEX);
  }
  kart4.trim();
  kart4.toUpperCase();
  for (int i = 16 ; i < 20 ; i++)
  {
    kart5 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart5 += String(EEPROM.read(i), HEX);
  }
  kart5.trim();
  kart5.toUpperCase();
  for (int i = 20 ; i < 24 ; i++)
  {
    kart6 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart6 += String(EEPROM.read(i), HEX);
  }
  kart6.trim();
  kart6.toUpperCase();
  for (int i = 24 ; i < 28 ; i++)
  {
    kart7 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart7 += String(EEPROM.read(i), HEX);
  }
  kart7.trim();
  kart7.toUpperCase();
  for (int i = 28 ; i < 32 ; i++)
  {
    kart8 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart8 += String(EEPROM.read(i), HEX);
  }
  kart8.trim();
  kart8.toUpperCase();
  for (int i = 32 ; i < 36 ; i++)
  {
    kart9 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart9 += String(EEPROM.read(i), HEX);
  }
  kart9.trim();
  kart9.toUpperCase();
  for (int i = 36 ; i < 40 ; i++)
  {
    kart10 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart10 += String(EEPROM.read(i), HEX);
  }
  kart10.trim();
  kart10.toUpperCase();
  for (int i = 40 ; i < 44 ; i++)
  {
    kart11 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart11 += String(EEPROM.read(i), HEX);
  }
  kart11.trim();
  kart11.toUpperCase();
  for (int i = 44 ; i < 48 ; i++)
  {
    kart12 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart12 += String(EEPROM.read(i), HEX);
  }
  kart12.trim();
  kart12.toUpperCase();
  for (int i = 48 ; i < 52 ; i++)
  {
    kart13 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart13 += String(EEPROM.read(i), HEX);
  }
  kart13.trim();
  kart13.toUpperCase();
  for (int i = 52 ; i < 56 ; i++)
  {
    kart14 += EEPROM.read(i) < 0x10 ? " 0" : " ";
    kart14 += String(EEPROM.read(i), HEX);
  }
  kart14.trim();
  kart14.toUpperCase();
}
