// esclavo.ino
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  // Mostrar la dirección MAC del esclavo
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT); // Dirección MAC del Bluetooth (no WiFi)
  Serial.printf("Dirección MAC del esclavo: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void loop() {

}
