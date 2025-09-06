// Include AWS IoT Certificates
#include "aws_iot_certs.h"

// Add the Wifi Library
#include "WiFi.h"

// Add the DFH Library
#include "DHT.h"

// Import for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Import MQTT and Wifi Secure used for AWS IoT
#include <WiFiClientSecure.h>
#include <MQTT.h>

// Import the Arduino Json Library
// Used for sending payload information to AWS IoT Core. 
#include <ArduinoJson.h>

// Intiallize the WifiClientSecure
WiFiClientSecure net = WiFiClientSecure();

// Setup MQTT Client with 512 packet size
MQTTClient client = MQTTClient(512);

// Defining our AWS IoT Configuration
#define DEVICE_NAME "Your Things Here"
#define AWS_IOT_ENDPOINT "Your Endpoint Here"
#define AWS_IOT_TOPIC "$aws/things/your_things/shadow/update"
#define AWS_MAX_RECONNECT_TRIES 10

// Define Publish and Describe Topics
// Make sure to add your topics names
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
//esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// Set Screen Specifications
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1

// Executing the Screen Specifications of the OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the Network Specifications
#define WIFI_NETWORK "Your_Wifi_Here"
#define WIFI_PASS "Your_Password_Here"
#define WIFI_TIMEOUT_MS 20000

// Define the DHT Type
#define DHTTYPE DHT22
// Define the GPIO Pin Associated to the ESP32
#define DHTPIN 4

// Initialize DHT22 Sensor
DHT dht(DHTPIN, DHTTYPE);

//---------------------------------------------------------
// DHT22 Sensor Function
void initializeDHT22Sensor() {
  dht.begin();  
  // Get the Temperature and Humidity Data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(temperature, humidity, true);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temperature, humidity, false);
  
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  // Display Temperature and Humidity on OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.println("Temp/Humidity");
  display.print(temperature);
  display.print(" / ");
  display.print(humidity);
  display.display();
  
  // Create a JSON document for AWS IoT
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["heat_index_c"] = hic;
  doc["heat_index_f"] = hif;
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

// End of Function
//------------------------------------------------------
// AWS Connection Function
void connectToAWSIoT() {
  // Adding the IoT Certificates for our AWS IoT Thing
  net.setCACert(AWS_ROOT_CA_CERT);
  net.setCertificate(AWS_CLIENT_CERT);
  net.setPrivateKey(AWS_PRIVATE_KEY);
  
  // Associating the Endpoint and Port Number
  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  
  // Setting up Retry Count
  int retries = 0;
  Serial.print("Connecting to AWS IoT");
  
  // Attempting Connection in While Loop
  while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES) {
    Serial.print(".");
    delay(100);
    retries++;
  }
  
  // Setup If Else Statement to handle connections and failures. 
  if (!client.connected()) {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Connected to AWS!");
    display.display();
    Serial.println("AWS Timed Out!");
    return;
  } else {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Connected to AWS!");
    display.display();
    Serial.println("Connected to AWS IoT Thing!");
    
    // Subscribe to the AWS IoT Device 
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  }
}

// End of Function
//------------------------------------------------------

// Connection to Wifi Function
void connectToWifi() {
  Serial.print("Connecting to Wifi");
  // In order to connect to an existing network we need to utilize station mode. 
  WiFi.mode(WIFI_STA);
  
  // Connect to the Network Using SSID and Pass. 
  WiFi.begin(WIFI_NETWORK, WIFI_PASS);
  
  // Store the time it takes for Wifi to connect. 
  unsigned long startAttemptTime = millis();
  
  // The While loop utilize the Wifi Status to check if its connect as well as makes sure that the timeout was not reached. 
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {   
    Serial.print(".");
    
    // Delay so this while loop does not run so fast. 
    delay(100);
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to Connect to Wifi");
    esp_deep_sleep_start();
  } else {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.write(3);
    display.println(" Connected!");
    display.println(WiFi.localIP());
    display.display();
  }
}

// End Function
//---------------------------------------

void setup() {
  Serial.begin(9600);
  connectToWifi();
  delay(2000);
  connectToAWSIoT();
}

void loop() {
  initializeDHT22Sensor();
  client.loop();
  delay(9000);
}
