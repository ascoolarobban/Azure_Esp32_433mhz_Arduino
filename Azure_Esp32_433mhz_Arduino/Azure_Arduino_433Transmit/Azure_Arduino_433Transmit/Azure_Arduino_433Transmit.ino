#include <Adafruit_Sensor.h>
#include <RH_ASK.h>
#include <DHT.h>
#include <SPI.h>
#define DHT_PIN 8
#define DHT_TYPE DHT11

char msg[60];
//433 hastighet/mottagare/sändare.
RH_ASK dhlCargo(2000, 5, 4, 0);
//Termo/hygrometer pin 8 , typ11.
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  //startar upp fabriken. 
  Serial.begin(115200);
  dhlCargo.init();
  dht.begin();
}

void loop() {
  
  char temperature[10];
  char humidity [10];
  //stoppar in mätningar till temperature/humidity.
  dtostrf(dht.readTemperature(), 3, 1, temperature);
  dtostrf(dht.readHumidity(), 3, 1, humidity);
  //värden till json-format.
  sprintf(msg, "{\"temp\": %s, \"hum\": %s}", temperature, humidity);


  Serial.println(msg);
  //skickar meddelandet.
  dhlCargo.send((uint8_t *)msg, strlen(msg));
  dhlCargo.waitPacketSent();
 
  delay(5000);

}
