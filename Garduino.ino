/*        Created By: Efstathios "Stathis" Kapnidis
          Feel free to use however you please
          First Created on August 25, 2022
 */

#include "defines.h"
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFiNINA_Generic.h>
#include <DHT.h>;

// Helps with the Measure_Humidity_and_Temp() function
enum MATERIAL{AIR, SOIL};

//Constants
#define DHTPIN 7     // what pin the air humidity and temperature sensor is connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

#define LM35_PIN              A0 // Soil tempereature sensor
#define SOIL_HUMIDITY_PIN     A1 // Soil Humidity sensor

#define SOLENOIDVALVE_PIN  2 // 12V Solenoid valve


#define FIFTEEN_MINUTES_PER_DAY 96

// Initialize the client for write_data()
WiFiClient client;
int status = WL_IDLE_STATUS;

// please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP), length must be 8+
char server[]               =   "192.168.2.24";
int  port                   =    8086;
char path_and_querry[]      =   "/api/v2/write?bucket=Front_Garden/";

// Global Variables
float soil_temp;
float soil_humidity;
int fifteen_minutes_count;


// Prototypes
void write_data(char * raw_data);
void Measure_Humidity_and_Temp(enum MATERIAL);
void water_soil(unsigned long time_ms);
void printWiFiStatus();
void web_request(char * raw_data);

//
void setup()
{
  Serial.begin(9600);
  dht.begin();
  pinMode(2,OUTPUT);
  fifteen_minutes_count = 0;
  Serial.println(String("Macro: ") + FIFTEEN_MINUTES_PER_DAY);
}

void loop()
{
  Serial.println(String("Counter: ") + fifteen_minutes_count);
  if (fifteen_minutes_count == 0)
  {
    Measure_Humidity_and_Temp(AIR);
    Measure_Humidity_and_Temp(SOIL);
    water_soil(110000);
    delay(765000);
    fifteen_minutes_count++;
  }

  Measure_Humidity_and_Temp(AIR);
  Measure_Humidity_and_Temp(SOIL);
  Serial.println("Waiting for 15 minutes");
  delay(875000);
  fifteen_minutes_count++;
  if (fifteen_minutes_count >= FIFTEEN_MINUTES_PER_DAY)
    fifteen_minutes_count = 0;
}

void write_data(char *raw_data)
{
  while(true){
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE)
    {
      while (true);
    }

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED)
    {
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      status = WiFi.begin(ssid, pass);
      // wait 10 seconds for connection:
      delay(10000);
    }
    printWiFiStatus();
    // if you get a connection, report back via serial:
    if (client.connect(server, port))
    {
      web_request(raw_data);
      break;
    }
    else
    {
      status=WiFi.disconnect();
      Serial.println("Disconnected");
    }
  }
}

void Measure_Humidity_and_Temp(enum MATERIAL type)
{
  float humidity;
  float temp;
  char material[5];

  if (type == AIR)
  {
    //Read data and store it to variables humidity and temp
    humidity  = dht.readHumidity();
    temp      = dht.readTemperature() * 0.87;
    strcpy(material,"air");
  }
  else if (type == SOIL)
  {
    // Temperature
    int val;
    val = analogRead(LM35_PIN);
    float mv = (val/1024.0)*5000;
    temp = mv/10;

    // Humidity
    int sensorValue  = analogRead(SOIL_HUMIDITY_PIN);
    humidity = (100 - ((sensorValue - 450) / 6)) - 5;
    strcpy(material,"soil");
  }

  char  temp_str[9];
  char  humidity_str[9];
  dtostrf(temp,5,3,temp_str);
  dtostrf(humidity,5,3,humidity_str);

  Serial.print(material);
  Serial.print(": ");
  Serial.print("Humidity: ");
  Serial.print(humidity_str);
  Serial.print(" Temperature: ");
  Serial.println(temp_str);

  char *data_raw = (char*)malloc(155 * sizeof(char));
  sprintf(data_raw, "%s,source=garduino temp=%s,humidity=%s", material, temp_str, humidity_str);
  write_data(data_raw);
  free(data_raw);
}

void water_soil(unsigned long time_ms)
{
  digitalWrite(2, HIGH);
  Serial.println("Watering Soil");
  delay(time_ms);
  digitalWrite(2,LOW);
  Serial.println("Finished Watering Soil");
}

void printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("\t\t");
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("\t\t");
  Serial.print(F("Address: "));
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("\t\t");
  Serial.print(F("Signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}

void web_request(char * raw_data)
{
    // Make a HTTP request:
    client.println(String("POST ") + path_and_querry +" HTTP/1.1");
    client.println(String("Host: ") + server + ":" + String(port));
    client.println("User-Agent: arduino-agent");
    client.println("Accept: */*");
    client.println(String("Authorization: Token ") + DB_USER +":"+ DB_PASS);
    client.println("Content-Type: text/plain; charset=utf-8");
    client.println(String("Content-Length: ") + String(strlen(raw_data)));
    client.println();
    client.write(raw_data);
    client.stop();
    delay(5000);
}
