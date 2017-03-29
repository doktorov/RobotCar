#include <OLED_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#define SDA               8
#define SCL               9
#define BTNPIN            10
#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

OLED  myOLED(SDA, SCL, 8); // SDA - 8pin, SCL - 9pin

extern uint8_t RusFont[]; // Русский шрифт
extern uint8_t MediumNumbers[]; // Подключение больших шрифтов
extern uint8_t SmallFont[]; // Базовый шрифт без поддержки русских символов.

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
int btnVal;

void setup() {
  pinMode(BTNPIN, INPUT);

  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  
  myOLED.begin();
}

void loop() {
  btnVal = digitalRead (BTNPIN);
  if (btnVal == LOW) {
    sensors_event_t event;  
    
    dht.temperature().getEvent(&event);   

    myOLED.clrScr(); // очищаем экран
    myOLED.setFont(RusFont); // Устанавливаем русский шрифт
    myOLED.print("Ntvgthfnehf", CENTER, 0); // Выводим надпись "Температура"
    myOLED.setFont(MediumNumbers);
    myOLED.print(String(event.temperature , 1), CENTER, 9);   // Отображение температуры
  
    dht.humidity().getEvent(&event);

    myOLED.setFont(RusFont); // Устанавливаем русский шрифт
    myOLED.print("Dkf;yjcnm", CENTER, 30); // Выводим надпись "Температура"
    myOLED.setFont(MediumNumbers);
    myOLED.print(String(event.relative_humidity , 1), CENTER, 39);   // Отображение температуры
    myOLED.update();

    delay(2000);
    
    delay(delayMS);
  } else {
    myOLED.clrScr(); // Стираем все с экрана
    myOLED.update();// Обновляем информацию на дисплее
  }
}
