#define BLYNK_TEMPLATE_ID "TMPL3ljJ3Anak"
#define BLYNK_TEMPLATE_NAME "Air Quality Montoring System"
#define BLYNK_AUTH_TOKEN "bhqwFBHQez9JeTN8mEHTZhLkci9Dwdo2"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Pin Definitions
#define DHTPIN D4
#define DHTTYPE DHT11
#define MQ135_PIN A0
#define BUZZER_PIN D6

// Objects Initialization
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp; // Default I2C address 0x76
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Arjit Mittal";
char pass[] = "Arjit@3005";

// Blynk Timer
BlynkTimer timer;

void setup() {
  // Serial Monitor
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Initialize DHT11
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

   // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("GROUP 4");
  lcd.setCursor(0, 1);
  lcd.print("AIR MONITORING");
  delay(2000);

  // Set Buzzer pin
  pinMode(12, OUTPUT);

    // Initialize BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 sensor!");
    while (1);
  }

  // Timer to read sensors and update Blynk
  timer.setInterval(2000L, readSensors);
}

void readSensors() {
  // Read DHT11 data
  float temperatureDHT = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read BMP280 data
  float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
  float altitude = bmp.readAltitude(1013.25); // Standard sea level pressure

  // Read MQ135 data
  int airQuality = analogRead(MQ135_PIN);

  // Display data on LCD
   lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureDHT);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");
  
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Air Quality: ");
  lcd.print(airQuality);
  lcd.setCursor(0, 1);
  lcd.print("Pressure: ");
  lcd.print(pressure);
  lcd.print("hPa");

  delay(4000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Altitude: ");
  lcd.print(altitude);

  delay(3000);
  

  // Push data to Blynk
  Blynk.virtualWrite(V0, temperatureDHT);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, airQuality);
  Blynk.virtualWrite(V3, pressure);
  Blynk.virtualWrite(V4, altitude);

  // Trigger buzzer if air quality exceeds threshold
  if (airQuality > 350 || temperatureDHT > 30) {
    digitalWrite(12, HIGH);
    // Blynk.email("prakharg.1981@gmail.com", "Alert", "Temperature over 28C!");
    Blynk.logEvent("pollution_alert","High Air Quality Detected!");
  } else {
    digitalWrite(12, LOW);
  }
}

void loop() {
  Blynk.run();
  timer.run();
}