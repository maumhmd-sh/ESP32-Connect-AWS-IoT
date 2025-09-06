// Include Library for MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Include AWS IoT Certificates
#include "aws_iot_certs.h"

// Add the Wifi Library
#include "WiFi.h"

// Import MQTT and Wifi Secure used for AWS IoT
#include <WiFiClientSecure.h>
#include <MQTT.h>

// Import the Arduino Json Library
// Used for sending payload information to AWS IoT Core.
#include <ArduinoJson.h>

Adafruit_MPU6050 mpu;

// Initialize the WifiClientSecure
WiFiClientSecure net = WiFiClientSecure();

// Setup MQTT Client with 512 packet size
MQTTClient client = MQTTClient(512);

// Defining our AWS IoT Configuration
#define DEVICE_NAME "MPU6050"
#define AWS_IOT_ENDPOINT "YOUR-ENDPOINT.amazon.com"
#define AWS_IOT_TOPIC "$aws/things/YOUR-NAME-SHEADOW/shadow/update"
#define AWS_MAX_RECONNECT_TRIES 10

// Define Publish and Describe Topics
// Make sure to add your topics names
#define AWS_IOT_PUBLISH_TOPIC "mpu6050/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "mpu6050/sub"

// Define the Network Specifications
#define WIFI_NETWORK "YOUR_SSID"
#define WIFI_PASS "YOUR_SSID_PASSWORD"
#define WIFI_TIMEOUT_MS 20000

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // akan menunggu hingga konsol serial terbuka

  Serial.println("TRYING POGRAM CONNECT TO AWS IOT");

  // Coba inisialisasi MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Rentang akselerometer diatur ke: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  // Setup koneksi ke WiFi
  connectToWifi();

  // Setup koneksi ke AWS IoT
  connectToAWSIoT();

  Serial.println("");
  delay(100);
}

void loop() {
  // Dapatkan sensor events dengan bacaan
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Cetak nilai-nilai MPU6050
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Gryo X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" C");

  // Kirim data ke AWS IoT
  sendToAWSIoT(a, g, temp);

  Serial.println("");
  delay(500);
}

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
  Serial.print("Menghubungkan ke AWS IoT");

  // Attempting Connection in While Loop
  while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES) {
    Serial.print(".");
    delay(100);
    retries++;
  }

  // Setup If Else Statement to handle connections and failures.
  if (!client.connected()) {
    Serial.println("Disocnnect from AWS IOT!");
    return;
  } else {
    Serial.println("Connected too AWS IOT!");

    // Subscribe to the AWS IoT Device
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  }
}

// Kirim data ke AWS IoT
void sendToAWSIoT(sensors_event_t a, sensors_event_t g, sensors_event_t temp) {
  // Create a JSON document for AWS IoT
  StaticJsonDocument<200> doc;
  doc["acceleration_x"] = a.acceleration.x;
  doc["acceleration_y"] = a.acceleration.y;
  doc["acceleration_z"] = a.acceleration.z;
  doc["gyro_x"] = g.gyro.x;
  doc["gyro_y"] = g.gyro.y;
  doc["gyro_z"] = g.gyro.z;
  doc["temperature"] = temp.temperature;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

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
    Serial.println(" Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}
