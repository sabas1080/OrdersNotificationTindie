/************************************************************
TindieHardwareNotification.ino
Tindie API Example Notification Pending Orders
Andres Sabas @ The Inventor's House
Original Creation Date: May 14, 2016


This example demonstrates how to use the TCP client 
functionality of the WiFi101 library to get 
to a Tindie API on 
https://www.tindie.com

Development environment specifics:
  IDE: Arduino 1.6.7
  Hardware Platform:
  Arduino MKR1000

This code is beerware; if you see me (or any other The Inventor's House 
member) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
************************************************************/

#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>

char ssid[] = "..........."; //  your network SSID (name)
char pass[] = "...........";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.tindie.com";    // name address for Google (using DNS)

// Tindie User:
String Tindie_Username = "........"; // Your username Tindie
// Tindie private APIKey:
String Tindie_Key = ".............."; // Your private key Tindie API

char json_string[512];
StaticJsonBuffer<512> jsonBuffer;
int header_flag = 0;
int length = 0;

  
// Initialize the WiFi client Secure library
// with the IP address and port of the server
// that you want to connect to (port 443 is default for HTTPS):
WiFiSSLClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /api/v1/order/?format=json&shipped=false&username=" + Tindie_Username + "&api_key=" + Tindie_Key + " HTTP/1.1");
    client.println("Host: www.tindie.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char current_char = client.read();
    //Serial.write(current_char);
    if (current_char == '{') {
      header_flag = 1;
    }

    if (header_flag == 1) {
        json_string[length] = current_char;
        length++;     
      }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    Serial.println(json_string);

  JsonObject& root = jsonBuffer.parseObject(json_string);
  if (!root.success()) {
    Serial.println(F("parseObject() failed"));
    return;
  }
  
  int orders = root["meta"]["total_count"];
  Serial.print("Pending Orders: ");
  Serial.println(orders);

  if(orders>0){
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(6, LOW);
    delay(100);
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(6, LOW);
  }
  
  header_flag = 0;

    // do nothing forevermore:
    while (true);
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
