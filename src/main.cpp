/**
 * mac id : E8:68:E7:09:1A:CC
 * esp-now with esp 32
 * mode: reaceive data from mutiple boards (many-to-one)
 * note slave read sensor co, sensor sound
 * note 1 -slave1
 * authors:oopsan
 */

#include <esp_now.h>
#include <WiFi.h>
#include <WiFiType.h>
#include "WiFi.h"
#include "FS.h"
#include"Adafruit_Sensor.h"

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xCC, 0x7B, 0x5C, 0x28, 0xB4, 0x6C};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
    int id; // must be unique for each sender board
    int MQ7SensorValue;
    int MAX9814SensorValue;
} struct_message;

// Create a struct_message called myData
struct_message note1Slave;

int MAX9814SensorValueRead;
int MQ7SensorValueRead;
int sendSS1 = 0;

// Create peer interface
esp_now_peer_info_t peerInfo;

// gpio pin
const int MQ7Sensor = 34;
const int MAX9814Sensor = 35;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == ESP_NOW_SEND_SUCCESS)
    {
        sendSS1++;
    }
}

void setup()
{
    // Init Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }
    // init gpio
    pinMode(MQ7Sensor, INPUT);
    pinMode(MAX9814Sensor, INPUT);
}

void loop()
{
    // Set values to send
    note1Slave.id = 1;
    MQ7SensorValueRead = analogRead(MQ7Sensor);
    note1Slave.MQ7SensorValue = MQ7SensorValueRead;
    MAX9814SensorValueRead = analogRead(MAX9814Sensor);
    note1Slave.MAX9814SensorValue = MAX9814SensorValueRead;


    Serial.print("MQ7SensorValue: ");
    Serial.println(MQ7SensorValueRead);
    Serial.print("MAX9814SensorValue:");
    Serial.println(MAX9814SensorValueRead);
    Serial.print("send: ");
    Serial.println(sendSS1);

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&note1Slave, sizeof(note1Slave));

    if (result == ESP_OK)
    {
        Serial.println("Sent with success");
    }
    else
    {
        Serial.println("Error sending the data");
    }
    delay(2000);
}
