#include <WiFi.h> 
#include <WiFiClient.h> 

//ESP Web Server Library  
#include <WebServer.h> 

#include <PZEM004Tv30.h>
#define PZEM_SERIAL Serial2

PZEM004Tv30 pzem(PZEM_SERIAL, 16, 17);

//SSID and Password of your WiFi router 
const char* ssid = "wifiname"; 
const char* password = "12345678"; 

//IP Address Server yang terpasang XAMPP
const char *host = "192.168.203.166";

String url; 

//Declare a global object variable from the ESP32WebServer class. 
WebServer server(80); //Server on port 80 

void setup() {
  Serial.begin(115200);
   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  } 
 
  //If connection successful show IP address in serial monitor 
  Serial.println(""); 
  Serial.print("Connected to "); 
  Serial.println(ssid); 
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP 

}

void loop() {
  server.handleClient();          //Handle client requests 
  delay(10000);
//  ++value;

  // Membaca Sensor Analog -------------------------------------------------------
  //int datasensor=analogRead(36`);
  //Serial.println(datasensor);
  float Voltage = pzem.voltage();
  float Current = pzem.current();
  float Power = pzem.power();
  float Energy = pzem.energy();
  float Frequency = pzem.frequency();
  float powerf = pzem.pf();
   if(isnan(Voltage)){
        Serial.println("Error reading voltage");
    } else if (isnan(Current)) {
        Serial.println("Error reading current");
    } else if (isnan(Power)) {
        Serial.println("Error reading power");
    } else if (isnan(Energy)) {
        Serial.println("Error reading energy");
    } else if (isnan(Frequency)) {
        Serial.println("Error reading frequency");
    } else if (isnan(powerf)) {
        Serial.println("Error reading power factor");
    } else {

        // Print the values to the Serial console
        Serial.print("Voltage: ");      Serial.print(Voltage);      Serial.println("V");
        Serial.print("Current: ");      Serial.print(Current);      Serial.println("A");
        Serial.print("Power: ");        Serial.print(Power);        Serial.println("W");
        Serial.print("Energy: ");       Serial.print(Energy,3);     Serial.println("kWh");
        Serial.print("Frequency: ");    Serial.print(Frequency, 1); Serial.println("Hz");
        Serial.print("PF: ");           Serial.println(powerf);
    }

  Serial.print("connecting to ");
  Serial.println(host);

// Mengirimkan ke alamat host dengan port 80 -----------------------------------
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // This will send the request to the server
  client.print(String("GET http://your_hostname/coba-PA/koneksi.php?") +
               ("&Voltage=") + Voltage +
               ("&Current=") + Current +
               ("&Power=") + Power +
               ("&Energy=") + Energy +
               ("&Frequency=") + Frequency +
               ("&powerf=") + powerf +
               " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis(); 
  while (client.available() == 0) { 
    if (millis() - timeout > 1000) { 
      Serial.println(">>> Client Timeout !"); 
      client.stop(); 
      return; 
    } 
  } 

// Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}
