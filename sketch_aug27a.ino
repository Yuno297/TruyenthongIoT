int led = 3;
void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  // Sáng dần
  for (int i = 0; i <= 255; i++) {
    analogWrite(led, i);
    delay(10);
  }

  // Tắt dần
  for (int i = 255; i >= 0; i--) {
    analogWrite(led, i);
    delay(10);
  }

  delay(500);
}