// ========================================================
// IMPORT THƯ VIỆN
// ========================================================
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// ========================================================
// CẤU HÌNH THÔNG SỐ (THAY ĐỔI THEO THÔNG TIN CỦA BẠN)
// ========================================================
const char* device_name = "ESP8266_Node";
const char* student_id  = "10124131"; // Thay bằng Mã số sinh viên của bạn

const char* ssid        = "Trần Hiệp";     // Tên WiFi
const char* password    = "29072006"; // Mật khẩu WiFi

// Cấu hình MQTT Broker (Home Assistant)
const char* mqtt_server = "192.168.1.104";        // Địa chỉ IP của Home Assistant
const int   mqtt_port   = 1883;                 // Cổng MQTT mặc định
const char* mqtt_user   = "hiep1";      // Username MQTT trên HA (nếu có)
const char* mqtt_pass   = "123";      // Password MQTT trên HA (nếu có)

// Topic MQTT để gửi dữ liệu
const char* mqtt_topic  = "iot/lab1/10124131/sensor";

// ========================================================
// KHỞI TẠO ĐỐI TƯỢNG
// ========================================================
WiFiClient espClient;
PubSubClient client(espClient);

// ========================================================
// HÀM KẾT NỐI WIFI
// ========================================================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Dang ket noi voi WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi da ket noi!");
  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());
}

// ========================================================
// HÀM KẾT NỐI RECONNECT MQTT
// ========================================================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Dang ket noi MQTT Broker...");
    
    // Tạo Client ID duy nhất bằng cách kết hợp tên thiết bị và MSV
    String clientId = String(device_name) + "_" + String(student_id);

    // Kết nối đến MQTT Broker
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println(" thanh cong!");
    } else {
      Serial.print(" That bai, ma loi rc=");
      Serial.print(client.state());
      Serial.println(" (Thu lai sau 2 giay)");
      delay(2000);
    }
  }
}

// ========================================================
// SETUP
// ========================================================
void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  // Thiết lập server và port cho MQTT client
  client.setServer(mqtt_server, mqtt_port);
}

// ========================================================
// LOOP
// ========================================================
void loop() {
  // Kiểm tra duy trì kết nối MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Tạo dữ liệu JSON giả lập nhiệt độ, độ ẩm
  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0; // Random từ 25.0 đến 35.0 °C
  doc["hum"]  = random(400, 800) / 10.0; // Random từ 40.0 đến 80.0 %

  char payload[128];
  serializeJson(doc, payload);

  // Gửi gói tin lên MQTT Topic
  Serial.print("Gui payload: ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload);

  // Đợi 5 giây trước khi gửi lần tiếp theo
  delay(5000);
}