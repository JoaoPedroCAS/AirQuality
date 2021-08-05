  #include <Adafruit_Sensor.h>

#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include<Wire.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include <SDS011.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

#define DHTTYPE    DHT11
const int DHTPIN = 3; 
const int DISPLAY_ADDRESS_PIN = 0x3c;
const int DISPLAY_SDA_PIN = 4;
const int DISPLAY_SCL_PIN = 15;
const int LORA_SCK_PIN = 5;
const int LORA_MISO_PIN = 19;
const int LORA_MOSI_PIN = 27;
const int LORA_CS_PIN = 18;
const int LORA_RST_PIN = 14;
const int LORA_DIO0_PIN = 26;
const int BAND = 433E6;
const int fontHeight = 16;
static const PROGMEM u1_t NWKSKEY[16] = {0xE0, 0x29, 0xF8, 0x8D, 0x9A, 0xFD, 0x30, 0xAA, 0x98, 0x82, 0xA1, 0x2E, 0x80, 0x57, 0x09, 0x68};
static const u1_t PROGMEM APPSKEY[16] = {0xE3, 0x9E, 0xA3, 0x53, 0x2B, 0x2D, 0x24, 0x17, 0xC7, 0x03, 0x24, 0x67, 0x58, 0x2F, 0x56, 0x2A};
static const u4_t DEVADDR = 0x260D5EE2;


DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature;

float p10, p25;
int err;

SDS011 my_sds;

#ifdef ESP32
HardwareSerial port(2);
#endif

// Função que inicializa o radio lora
bool loraBegin()
{
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_CS_PIN);
  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);
  return LoRa.begin(BAND);
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  my_sds.begin(&port);
  Serial.println("Iniciando Transmissor.");
  if(!loraBegin()) 
  {
    // Se não deu certo, exibimos falha de lora na serial
    Serial.println("LoRa failed!");
    // E deixamos em loop infinito
    while (1);
  }
  delay(500);
  Serial.println("Sensor SDS011 em conjunto com DHT11");
  delay(1500);

}

void loop()
{
  err = my_sds.read(&p25, &p10);
  if (!err) {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
    humidity = dht.readHumidity();
    Serial.print("Umidade Relativa = ");
    Serial.print(humidity, 0);
    Serial.print("%  ");
    temperature = dht.readTemperature();
    Serial.print("Temperatura = ");
    Serial.print(temperature, 0); 
    Serial.println("ºC  ");

    LoRa.beginPacket();
    LoRa.print(temperature);
    LoRa.print("|");
    LoRa.print(humidity);
    LoRa.print("|");
    LoRa.print(p25);
    LoRa.print("|");
    LoRa.print(p10);
    LoRa.print("|");
    LoRa.endPacket();
  }
  delay(100);
    
}
