#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Ticker flipper;

const char* ssid     = "Ntk-39";
const char* password = "506938506938";
IPAddress ip(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char* host = "narodmon.ru";
const int httpPort = 8283;

const int interval = 10 * 60; // 10 минут

int tm = interval;
int deviceCount = 0;

Adafruit_BME280 bme; // I2C

void flip() {
  tm--;
}

// 60:01:94:35:CE:B7
// SCL - D4, SDA - D3
void setup() {
  Serial.begin(115200);
  
  delay(10);

  Wire.begin(D3, D4);
  Wire.setClock(100000);

  Serial.println();
  Serial.println(F("BME280 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  Serial.println();
  
  delay(100);

  Send();                                  // при включении отправляем данные

  flipper.attach(1, flip);                 // запускаем таймер
}

/// функция отправляет данные на народмон.ру
void Send() {

  // Подключаемся к wifi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();                       // отправляем в Serial данные о подключении
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println();

  // подключаемся к серверу
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // отправляем данные
  Serial.println("Sending...");
  // заголовок
  client.print("#");
  client.print(WiFi.macAddress()); // отправляем МАС нашей ESP8266
  client.print("#");
  client.print("ESP8266"); // название устройства
  client.print("#");
  client.print("54.940994#83.184592"); // координаты местонахождения датчика
  client.println();

  client.print("#T1#");
  client.print(bme.readTemperature());
  client.print("#Температура");
  client.println();

  client.print("#H1#");
  client.print(bme.readHumidity());
  client.print("#Влажность");
  client.println();

  client.print("#P1#");
  client.print(bme.readPressure() / 100 * 0.75);
  client.print("#Атм. давление");
  client.println();

  client.print("##");
  //

  delay(10);

  // читаем ответ с и отправляем его в сериал
  // вообще на ответ нужно както реагировать
  Serial.print("Requesting: ");
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  client.stop();
  Serial.println();
  Serial.println();
  Serial.println("Closing connection");

  WiFi.disconnect(); // отключаемся от сети
  Serial.println("Disconnect WiFi.");
}

void loop() {
  if (tm == 0) {                     // если таймер отработал
    flipper.detach();                 // выключаем
    tm = interval;                    // сбрасываем переменную таймера
    //sensors.requestTemperatures();    // забераем температуру с градусника
    delay(10);
    Send();                           // отправляем
    flipper.attach(1, flip);          // включаем прерывание по таймеру
  }
}
