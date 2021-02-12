#include "includes.h"
char* ssid = "Robin's Wi-Fi Network";
char* pass = "Batman123";
static char* connectionString = "HostName=iot-robban.azure-devices.net;DeviceId=esp32;SharedAccessKey=GiSqmjq/DLaGYKuhP/+nogcpoF+JZq7h6REFLs3pGrM=";
static bool _connected = false;


RH_ASK driver(2000, 4, 4, 0); 

void initWifi()
{
  //Anslutning med felhantering.
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  //Skriver ut lite bra-att-ha.
  Serial.println("\nIP Address: ");
  Serial.println(WiFi.localIP());
}

void initIotHub() {
  //kollar så att azure är med på banan.
  if (!Esp32MQTTClient_Init((const uint8_t * ) connectionString)) {
    _connected = false;
    return;
  }
  _connected = true;
}
float temp = 0;
float prev = 0;
float diff = 0.5;

void setup()
{
  //uppstart.
  Serial.begin(115200);
  if (!driver.init())
    Serial.println("init failed");
  initWifi();
  initIotHub();
  delay(200);

}
//mäter skillnad så att vi kan skicka vid 0.5 diff.
bool difference(){
  if(temp > (prev + diff) || temp < (prev-diff)){
    prev = temp;
    return true;
  }
  return false;
}
void loop() {
  char payload[60];
  //Storlek samt längd på meddelande.
  uint8_t buf[60];
  uint8_t buflen = sizeof(buf);
  //kollar om vi får meddelande ifrån sändare till  433mhz-mottagaren.
  if (driver.recv(buf, &buflen))
  {
    Serial.println((char*)buf);
    strcpy(payload, (char*)buf);
  }
  Serial.println(payload);
  /*tar vår payload och skickar temp-datat till tempvariabel, 
  för att sedan packetera om det nya värdet, som vid 0.5 grader diff skickas till azure.*/
  DynamicJsonDocument json(1024);
  deserializeJson(json, payload);
  //temp in i temp.
  temp = json["temp"];
  float hum = json["hum"];
  char messageToSend[128];
  //json med sluttemperatur.
  serializeJson (json,messageToSend);
if (difference()){
  //Skickar den nya temperaturen.
    if (Esp32MQTTClient_SendEvent(messageToSend))
  {
    Serial.println(messageToSend);
  }
  
}
  delay(5 * 1000);
}
