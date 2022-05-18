/*
 * 
 * Description: Implementing I2C protocol using a TMP100 temperature sensor and an ESP32 MCU to communicate with a remote device 
 * Author: David Kumar 
 * 
 */

#include<Wire.h>
#include "WiFi.h"

#define WIFI_NETWORK ""
#define WIFI_PASSWORD ""
#define WIFI_TIMEOUT_MS 20000

//Configure slave address for TMP100 sensor
#define address 0x4F // equivalent to 76 

WiFiServer server(80); // connecting to port 80

// establishing initial WiFi connection 
void connectToWifi(){
  
  Serial.println("Trying to connect to WiFi");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long attemptTime = millis();

  // keeps looping until WiFi is connected 
  while (WiFi.status() != WL_CONNECTED && millis() - attemptTime < WIFI_TIMEOUT_MS){
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Connection Failed.");
  }
  else{
    Serial.println("Connection Successful!");
    server.begin();
    Serial.println(WiFi.localIP());

  }
}

// establishing WiFi client connection 
void connectClient(float cTemp, float fTemp){

  WiFiClient client = server.available(); 

  if (client){
    
    while(client.connected()){

      if (client.available()){
                
        // sending a standard http response header
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");

        client.println("<!DOCTYPE html><html>");
        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

        client.println("<p>Temperature in Celsius: ");
        client.print(cTemp);
        client.println(" C</p>");
        client.print("<p>Temperature in Fahrenheit: ");
        client.print(fTemp);
        client.println(" F</p>");
        client.println("</body>");
        client.println("</html>");

      }
    }
    // ending the cient connection 
    client.stop();
  }

  delay(500);
}

void setup() {
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial communication, set baud rate = 9600
  Serial.begin(9600);
  
  connectToWifi();
  // Start I2C Transmission
  Wire.beginTransmission(address);
  // Select configuration register
  Wire.write(0x01);
  // Set continuous conversion, comparator mode, 12-bit resolution
  Wire.write(0x60);
  // Stop I2C Transmission
  Wire.endTransmission();
  //delay(300);  
}

void loop(){

  unsigned int data[2];
  
  // Start I2C Transmission
  Wire.beginTransmission(address);
  // Select data register
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();
  // Request 2 bytes of data
  Wire.requestFrom(address, 2);

  // Read 2 bytes of data
  if(Wire.available() == 2){
   
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
    
  // Convert the data
  float cTemp = (((data[0] * 256) + (data[1] & 0xF0)) / 16) * 0.0625;
  float fTemp = cTemp * 1.8 + 32;
  
  // Output data to serial monitor
  Serial.print("Temperature in Celsius : ");
  Serial.print(cTemp);
  Serial.println(" C");
  Serial.print("Temperature in Fahrenheit : ");
  Serial.print(fTemp);
  Serial.println(" F");

  // passing temperature data to WiFi client 
  connectClient(cTemp, fTemp);
  delay(500);

}
