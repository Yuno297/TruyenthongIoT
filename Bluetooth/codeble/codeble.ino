#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// LED RGB trên board ESP32-S3
#define RGB_PIN     48  // Đổi thành 38 nếu LED board dùng GPIO38
#define BRIGHTNESS  40  // Độ sáng (0 - 255)

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789000"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-123456789001"

const char *DEVICE_NAME = "Yuno9";

BLEServer *pServer;

void setColor(uint8_t r, uint8_t g, uint8_t b) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  rgbLedWrite(RGB_PIN, r, g, b);
#else
  neopixelWrite(RGB_PIN, r, g, b);
#endif
}

// Hàm hỗ trợ chuyển đổi mã HEX (ví dụ: "FF8000") thành giá trị RGB
void setHexColor(String hex) {
  if (hex.startsWith("#")) {
    hex = hex.substring(1); // Bỏ dấu # nếu người dùng gửi kèm
  }
  if (hex.length() == 6) {
    long number = strtol(hex.c_str(), NULL, 16);
    uint8_t r = (number >> 16) & 0xFF;
    uint8_t g = (number >> 8) & 0xFF;
    uint8_t b = number & 0xFF;

    // Quy đổi theo độ sáng (Brightness) chung để tránh đèn quá chói
    r = (r * BRIGHTNESS) / 255;
    g = (g * BRIGHTNESS) / 255;
    b = (b * BRIGHTNESS) / 255;

    setColor(r, g, b);
  }
}

class ServerCallback : public BLEServerCallbacks {
  void onConnect(BLEServer *s) override {
    Serial.println("Dien thoai da ket noi");
  }

  void onDisconnect(BLEServer *s) override {
    Serial.println("Da ngat ket noi -> phat BLE lai");
    BLEDevice::startAdvertising();
  }
};

class CommandCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue().c_str();

    value.trim();
    value.toUpperCase(); // Viết hoa toàn bộ để dễ so sánh

    Serial.print("Nhan lenh: ");
    Serial.println(value);

    // 1. Kiểm tra nếu gửi theo dạng mã HEX (Bắt đầu bằng dấu # hoặc đủ 6 ký tự hex)
    if (value.startsWith("#") || value.length() == 6) {
      setHexColor(value);
      Serial.print("LED -> Mau HEX: ");
      Serial.println(value);
      return;
    }

    // 2. Tối ưu hóa phần kiểm tra màu bằng mảng cấu trúc 
    struct ColorConfig {
      const char* name1;
      const char* name2;
      uint8_t r, g, b;
    };

    ColorConfig colors[] = {
      {"RED",     "",           BRIGHTNESS, 0, 0},
      {"GREEN",   "",           0, BRIGHTNESS, 0},
      {"BLUE",    "",           0, 0, BRIGHTNESS},
      {"WHITE",   "",           BRIGHTNESS, BRIGHTNESS, BRIGHTNESS},
      {"YELLOW",  "",           BRIGHTNESS, BRIGHTNESS, 0},
      {"CYAN",    "LIGHTBLUE",  0, BRIGHTNESS, BRIGHTNESS},
      {"MAGENTA", "PURPLE",     BRIGHTNESS, 0, BRIGHTNESS},
      {"ORANGE",  "",           BRIGHTNESS, BRIGHTNESS / 2, 0},
      {"PINK",    "",           BRIGHTNESS, BRIGHTNESS / 4, BRIGHTNESS / 4},
      {"OFF",     "",           0, 0, 0}
    };

    bool found = false;
    for (auto const& c : colors) {
      if (value == c.name1 || (c.name2[0] != '\0' && value == c.name2)) {
        setColor(c.r, c.g, c.b);
        found = true;
        break;
      }
    }

    if (!found) {
      Serial.println("Lenh khong hop le");
      Serial.println("Dung: RED, GREEN, BLUE, WHITE, YELLOW, PURPLE, ORANGE, PINK, CYAN, OFF hoặc mã HEX (VD: #FF00FF)");
      return;
    }

    Serial.print("LED -> ");
    Serial.println(value);
  }
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  setColor(0, 0, 0);

  // Khoi tao BLE voi ten Toan
  BLEDevice::init(DEVICE_NAME);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallback());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic =
      pService->createCharacteristic(
          CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_WRITE
      );

  pCharacteristic->setCallbacks(new CommandCallback());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

  // Ten thiet bi trong goi quang ba chinh
  BLEAdvertisementData advData;
  advData.setFlags(0x06);
  advData.setName(DEVICE_NAME);
  pAdvertising->setAdvertisementData(advData);

  // UUID dich vu trong goi scan response
  BLEAdvertisementData scanData;
  scanData.setCompleteServices(BLEUUID(SERVICE_UUID));
  pAdvertising->setScanResponseData(scanData);

  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  // Nhay xanh la bao khoi dong
  setColor(0, BRIGHTNESS, 0);
  delay(300);
  setColor(0, 0, 0);

  Serial.println("--------------------------");
  Serial.println("Bluetooh da khoi dong");
  Serial.println("Ho tro mau: RED, GREEN, BLUE, WHITE, YELLOW, PURPLE, ORANGE, PINK, CYAN, OFF");
  Serial.println("Hoac gui ma HEX truc tiep (VD: #FF00FF)");
  Serial.println("--------------------------");
}

void loop() {
}