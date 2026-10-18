#include <SoftwareSerial.h>

// Định nghĩa chân RX và TX mềm trên Arduino Uno
// Chân RX của Arduino (10) nối với TX (GPIO 4) của ESP32
// Chân TX của Arduino (11) nối với RX (GPIO 5) của ESP32
#define ARDUINO_RX_PIN 10
#define ARDUINO_TX_PIN 11

SoftwareSerial mySerial(ARDUINO_RX_PIN, ARDUINO_TX_PIN);

const int ledPin = 13; // Chân LED tích hợp trên Arduino Uno

void setup() {
  Serial.begin(115200);
  
  // Khởi động SoftwareSerial với tốc độ 9600
  mySerial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  Serial.println("Arduino SoftwareSerial Ready!");
}

void loop() {
  // Đọc dữ liệu từ ESP32 gửi sang
  if (mySerial.available() > 0) {
    String incoming = mySerial.readStringUntil('\n');
    incoming.trim(); // Loại bỏ khoảng trắng hoặc ký tự xuống dòng thừa
    
    Serial.print("Nhan tu ESP32: ");
    Serial.println(incoming);

    // Xử lý điều khiển đèn LED dựa trên dữ liệu nhận được
    if (incoming.equalsIgnoreCase("ON")) {
      digitalWrite(ledPin, HIGH);
      Serial.println("Da BAT den LED!");
      mySerial.println("Arduino: LED ON");
    } 
    else if (incoming.equalsIgnoreCase("OFF")) {
      digitalWrite(ledPin, LOW);
      Serial.println("Da TAT den LED!");
      mySerial.println("Arduino: LED OFF");
    }
  }
}