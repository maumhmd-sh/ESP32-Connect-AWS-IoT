## 🛠️ Setup and Configuration Guide

Follow these steps to connect your ESP32 to AWS IoT Core and run the example code successfully.

---

### 1. Configure AWS IoT Core

1. **Create a New Thing**  
   - Log in to your AWS account and navigate to **AWS IoT Core**.  
   - Click on **Manage → Things → Create things** and follow the steps to create a new IoT device (Thing).

2. **Generate and Download Certificates and Keys**  
   - During Thing creation, generate the device certificates and private key.  
   - Download the following files:  
     - Device certificate (`.crt`)  
     - Private key (`.key`)  
     - Amazon Root CA certificate (`root-CA.crt`)

3. **Attach an Appropriate Policy**  
   - Create a new IoT policy or attach an existing one to allow MQTT operations such as `Connect`, `Publish`, `Subscribe`, and `Receive`.

---

### 2. Prepare Arduino IDE

1. **Install ESP32 Board Support**  
   - Open **Arduino IDE → File → Preferences**.  
   - In "Additional Boards Manager URLs", add:  
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```  
   - Open **Tools → Board → Board Manager**, search for ESP32, and install.

2. **Install Required Libraries**  
   - Go to **Sketch → Include Library → Manage Libraries…**  
   - Install the following libraries if not already present:  
     - `WiFi.h`  
     - `PubSubClient.h`  
     - Any other sensor libraries used (e.g., `MPU6050`)

---

### 3. Upload Code to ESP32

1. Open `main.ino` in Arduino IDE.  
2. Replace placeholders with your **Wi-Fi SSID, Wi-Fi password, and AWS IoT Core credentials** (certificate paths, endpoint, and topic names).  
3. Select the correct ESP32 board and port under **Tools → Board** and **Tools → Port**.  
4. Click **Upload** to flash the code to your ESP32.

---

### 4. Monitor Output

1. Open **Tools → Serial Monitor** in Arduino IDE.  
2. Set baud rate (usually 115200).  
3. Observe the output:  
   - ESP32 connecting to Wi-Fi  
   - Connecting to AWS IoT Core  
   - Publishing data to your MQTT topic  
   - Receiving messages from subscribed topics  

Example Serial Monitor output:

Connecting to WiFi...
Connected to WiFi
Connecting to AWS IoT Core...
Connected to AWS IoT Core
Publishing data to topic 'esp32/data': {"temperature": 22.5, "humidity": 60}
Subscribed to topic 'esp32/commands'
Received message: {"action": "LED_ON"}


This output indicates successful communication with both Wi-Fi and AWS IoT Core, along with data publishing and command reception.

---

## 📚 Learn More

For a detailed tutorial on connecting an ESP32 to AWS IoT Core, refer to this guide:

[ESP32 + AWS IoT Core Tutorial](https://esp32io.com/tutorials/esp32-aws-iot?utm_source=chatgpt.com)

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
