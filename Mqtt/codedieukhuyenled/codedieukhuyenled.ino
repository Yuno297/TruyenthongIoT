#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

/* ====== CONFIG ====== */
const char* ssid = "Trần Hiệp";          // Tên WiFi của bạn
const char* password = "29072006";      // Mật khẩu WiFi

const char* mqtt_server = "192.168.1.104"; // IP Home Assistant
const int mqtt_port = 1883;

const char* mqtt_user = "hiep1";
const char* mqtt_pass = "123";

const char* student_id = "10124131";     // Mã sinh viên của bạn

String topic_cmd = "iot/lab2/" + String(student_id) + "/cmd";
String topic_state = "iot/lab2/" + String(student_id) + "/state";

// Cấu hình cho LED RGB tích hợp trên ESP32-S3
#define PIN_RGB 48    // Chân GPIO 48 nối với LED RGB tích hợp trên board S3
#define NUM_PIXELS 1  // Số lượng bóng LED (board S3 có 1 bóng)

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_RGB, NEO_GRB + NEO_KHZ800);

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (String(topic) == topic_cmd) {
    if (message == "ON" || message == "1") {
      // Bật đèn RGB màu trắng (Độ sáng tùy chỉnh từ 0 đến 255 ở mỗi màu R, G, B)
      pixels.setPixelColor(0, pixels.Color(50, 50, 50)); 
      pixels.show();
      client.publish(topic_state.c_str(), "ON");
    } 
    else if (message == "OFF" || message == "0") {
      // Tắt đèn RGB
      pixels.setPixelColor(0, pixels.Color(0, 0, 0)); 
      pixels.show();
      client.publish(topic_state.c_str(), "OFF");
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32S3_Client_" + String(student_id);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("MQTT connected");
      client.subscribe(topic_cmd.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  pixels.begin(); // Khởi tạo NeoPixel
  pixels.clear();
  pixels.show();
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}