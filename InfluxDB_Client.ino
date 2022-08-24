#include "defines.h"
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFiNINA_Generic.h>

void write_data(char * raw_data);

// please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP), length must be 8+
char server[]               =   "192.168.2.24";
int  port                   =    8086;
char path_and_querry[]      =   "/api/v2/write?bucket=Front_Garden/";
char *data_raw = (char*)malloc(155 * sizeof(char));

WiFiClient client; // Initialize the client for write_data()
int status = WL_IDLE_STATUS;

void setup()
{
  float soil_temp = 27.72;
  float soil_humidity = 55.5;
  char soil_temp_str[9];
  char soil_humidity_str[9];

  dtostrf(soil_temp,5,3,soil_temp_str);
  dtostrf(soil_humidity,5,3,soil_humidity_str);
  sprintf(data_raw, "%s,source=garduino temp=%s,humidity=%s", "soil", soil_temp_str, soil_humidity_str);
  write_data(data_raw);
}

void loop()
{
  delay(5000);
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
  }
}
