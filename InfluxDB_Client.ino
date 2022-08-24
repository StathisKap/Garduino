#include "defines.h"
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFiNINA_Generic.h>
#include <DHT.h>;

enum MATERIAL{AIR, SOIL};

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

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


// Prototypes
void write_data(char * raw_data);
void Measure_Humidity_and_Temp(enum MATERIAL);


void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  Measure_Humidity_and_Temp(SOIL);
  Measure_Humidity_and_Temp(AIR);
  delay(10000); //Delay 2 sec.
}

void write_data(char *raw_data)
{
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

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
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
    delay(5000);
    client.stop();
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
    temp      = dht.readTemperature();
    strcpy(material,"air");
  }
  else if (type == SOIL)
  {
    humidity  = dht.readHumidity() - 2;
    temp      = dht.readTemperature() - 2;
    strcpy(material,"soil");
  }

  char  temp_str[9];
  char  humidity_str[9];
  dtostrf(temp,5,3,temp_str);
  dtostrf(humidity,5,3,humidity_str);

  char *data_raw = (char*)malloc(155 * sizeof(char));
  sprintf(data_raw, "%s,source=garduino temp=%s,humidity=%s", material, temp_str, humidity_str);
  write_data(data_raw);
}
