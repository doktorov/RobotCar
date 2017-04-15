#include <Thread.h>  // подключение библиотеки ArduinoThread
#include <DHT_U.h>
#include <OLED_I2C.h>
#include <Adafruit_Sensor.h>

#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

#define SDA               8
#define SCL               7

#define BTNPIN            22

#define LEDPIN            13

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
String temperature;
String humidity;

OLED myOLED(SDA, SCL, 8);

extern uint8_t RusFont[]; // Русский шрифт
extern uint8_t MediumNumbers[]; // Подключение больших шрифтов
extern uint8_t SmallFont[]; // Базовый шрифт без поддержки русских символов.

int btnVal;

Thread ledThread = Thread(); // создаём поток управления светодиодом
Thread dhtThread = Thread();
Thread oledThread = Thread();

void setup() {
  Serial.begin(9600);

  pinMode(LEDPIN, OUTPUT);   // объявляем пин 13 как выход.
  pinMode(BTNPIN, INPUT);

  sensor_t sensor;

  dht.begin();
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  myOLED.begin();

  dhtThread.onRun(dhtData);
  dhtThread.setInterval(delayMS);

  oledThread.onRun(oledShow);
  oledThread.setInterval(50);

  ledThread.onRun(ledBlink);
  ledThread.setInterval(1000);
}

void loop() {
  // Проверим, пришло ли время переключиться светодиоду:
  if (ledThread.shouldRun())
    ledThread.run(); // запускаем поток

  if (dhtThread.shouldRun())
    dhtThread.run(); // запускаем поток

  if (oledThread.shouldRun())
    oledThread.run(); // запускаем поток
}

// Поток светодиода:
void ledBlink() {
  static bool ledStatus = false;    // состояние светодиода Вкл/Выкл
  ledStatus = !ledStatus;           // инвертируем состояние
  digitalWrite(LEDPIN, ledStatus);  // включаем/выключаем светодиод
}

void dhtData() {
  sensors_event_t event;

  dht.temperature().getEvent(&event);
  temperature = String(event.temperature, 1);

  Serial.print("Temperature (oC): ");
  Serial.println(temperature);

  dht.humidity().getEvent(&event);
  humidity = String(event.relative_humidity, 1);

  Serial.print("Humidity (%): ");
  Serial.println(humidity);
}

void oledShow() {
  btnVal = digitalRead (BTNPIN);
  if (btnVal == LOW) {
    myOLED.clrScr(); // очищаем экран
    myOLED.setFont(RusFont); // Устанавливаем русский шрифт
    myOLED.print("Ntvgthfnehf", CENTER, 0); // Выводим надпись "Температура"
    myOLED.setFont(MediumNumbers);
    myOLED.print(temperature, CENTER, 9);   // Отображение температуры

    myOLED.setFont(RusFont); // Устанавливаем русский шрифт
    myOLED.print("Dkf;yjcnm", CENTER, 30); // Выводим надпись "Температура"
    myOLED.setFont(MediumNumbers);
    myOLED.print(humidity, CENTER, 39);   // Отображение температуры
    myOLED.update();
  } else {
    myOLED.clrScr(); // Стираем все с экрана
    myOLED.update();// Обновляем информацию на дисплее
  }
}
