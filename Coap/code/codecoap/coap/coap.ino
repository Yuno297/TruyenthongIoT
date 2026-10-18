#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h> // ESP32 sẽ tự động nhận diện thư viện này
#endif

#include <WiFiUdp.h>
#include <coap-simple.h>

/* ========== CONFIG (Thông tin mạng của bạn) ========== */
const char* WIFI_SSID = "Trần Hiệp";
const char* WIFI_PASS = "29072006";

/* CoAP Server (IP máy tính hoặc Server nhận CoAP trong mạng LAN) */
IPAddress coap_server_ip(192, 168, 1, 111);
const int coap_port = 5683;

/* CoAP Resource */
const char* coap_resource = "sensor/temp";

/* Send interval (ms): Thời gian gửi gói tin định kỳ (5 giây) */
const unsigned long SEND_INTERVAL = 5000;
/* ============================================================ */

WiFiUDP udp;
Coap coap(udp);

unsigned long lastSend = 0;

/* ============================ */
void setup_wifi() {
  Serial.begin(115200);
  delay(10);
  
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected successfully!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

/* ============================ */
void setup() {
  setup_wifi();

  coap.start();
  Serial.println("CoAP Client started on ESP32");
}

/* ============================ */
void loop() {
  // Lắng nghe và xử lý phản hồi từ server
  coap.loop();

  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    /* Payload mẫu (Dữ liệu gửi đi) */
    String payload = "temp=30.5";

    Serial.print("Sending CoAP CON POST to IP: ");
    Serial.print(coap_server_ip);
    Serial.print(" Resource: ");
    Serial.print(coap_resource);
    Serial.print(" Payload: ");
    Serial.println(payload);

    // Gửi gói tin CoAP dạng CON (Confirmable - Yêu cầu ACK phản hồi)
    // Đã thêm 2 tham số nullptr và 0 cho Token và Token Length
    coap.send(
      coap_server_ip,
      coap_port,
      coap_resource,
      COAP_CON,                  // Confirmable message
      COAP_POST,                 // Method: POST
      nullptr,                   // Token (truyền nullptr nếu không dùng)
      0,                         // Token length
      (uint8_t*)payload.c_str(), // Payload
      payload.length()           // Payload length
    );
  }
}