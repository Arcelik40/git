void setup() {
  Serial.begin();
}

void loop() {
  if (Serial.begin()) {
    char  data[] = Serial.readStringUntil('\n');
    Serial.print(data)
    
  }

}
