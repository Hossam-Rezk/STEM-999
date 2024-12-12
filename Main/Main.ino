#include <Wire.h>
#include <WiFi.h>
#include <DHT.h>
#include <MQ135.h>
#include <WebServer.h>

// DHT22 Setup
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// MQ135 Setup
#define MQ135_PIN 34  // Analog input pin for MQ135 sensor
MQ135 gasSensor(MQ135_PIN);

// Flame Sensor Setup (Digital mode)
#define FLAME_PIN 15  // Flame sensor digital pin
#define BUZZER_PIN 16 // Buzzer pin

// Wi-Fi Credentials
const char* ssid = "STEM19EE";   // Replace with your Wi-Fi SSID
const char* password = "1919016111@H"; // Replace with your Wi-Fi password

// Thresholds
float temperatureThreshold = 30.0;  // Temperature threshold in °C
float humidityThreshold = 60.0;     // Humidity threshold in %
float airQualityThreshold = 100.0;  // Example air quality threshold (PPM)
int flameThreshold = 0;             // No threshold for flame in digital mode (Just HIGH or LOW)

// Web Server
WebServer server(80);

// Initialize Sensors and Buzzer
void initSensors() {
  dht.begin();
  pinMode(FLAME_PIN, INPUT);  // Flame sensor as digital input
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer initially
}

// Fetch Sensor Data and Return as JSON
String getSensorDataJson() {
  // Read data from DHT22 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Read data from MQ135 (Air quality)
  float airQuality = gasSensor.getPPM(); // Gas concentration (ppm)

  // Read data from Flame sensor (digital HIGH/LOW)
  bool flameDetected = digitalRead(FLAME_PIN) == HIGH;  // HIGH means flame detected

  // Check for threshold exceedance and identify which parameter exceeded
  String exceededParameter = "";
  bool thresholdExceeded = false;

  // Check if temperature exceeds the threshold
  if (temperature > temperatureThreshold) {
    exceededParameter = "Temperature";
    thresholdExceeded = true;
  }
  // Check if humidity exceeds the threshold
  else if (humidity > humidityThreshold) {
    exceededParameter = "Humidity";
    thresholdExceeded = true;
  }
  // Check if air quality exceeds the threshold
  else if (airQuality > airQualityThreshold) {
    exceededParameter = "Air Quality";
    thresholdExceeded = true;
  }
  // Check if flame is detected
  else if (flameDetected) {
    exceededParameter = "Flame Detected";
    thresholdExceeded = true;
  }

  // Turn on buzzer if any threshold is exceeded
  digitalWrite(BUZZER_PIN, thresholdExceeded ? HIGH : LOW);

  // Print readings to Serial Monitor
  Serial.println("==== Sensor Readings ====");
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  Serial.print("Air Quality (PPM): ");
  Serial.println(airQuality);

  Serial.print("Flame Detected: ");
  Serial.println(flameDetected ? "Yes" : "No");

  Serial.print("Threshold Exceeded: ");
  Serial.println(thresholdExceeded ? "Yes" : "No");
  if (thresholdExceeded) {
    Serial.print("Exceeded Parameter: ");
    Serial.println(exceededParameter);
  }
  Serial.println("========================");

  // Generate JSON for sending to the endpoint
  String json = "{";
  json += "\"temperature\": " + String(temperature, 1) + ",";
  json += "\"humidity\": " + String(humidity, 1) + ",";
  json += "\"airQuality\": " + String(airQuality, 1) + ",";
  json += "\"flameDetected\": " + String(flameDetected ? "true" : "false") + ",";
  json += "\"thresholdExceeded\": " + String(thresholdExceeded ? "true" : "false") + ",";
  json += "\"exceededParameter\": \"" + exceededParameter + "\"";  // Include which parameter exceeded
  json += "}";

  return json;
}

// Handle Data Request and Send Sensor Data
void handleSensorData() {
  String data = getSensorDataJson();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", data);
}

// Connect to Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Start Web Server
void startServer() {
  server.on("/data", handleSensorData);
  server.begin();
  Serial.println("Web Server started");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWiFi();

  // Initialize sensors
  initSensors();

  // Start the web server
  startServer();
}

void loop() {
  // Handle client requests
  server.handleClient();
}
