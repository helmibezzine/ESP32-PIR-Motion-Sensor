#include <WiFi.h>
#include <UrlEncode.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduCAM.h>

const char* ssid = "wifi_ssid";
const char* password = "wifipassword";

String mobile_number = "whatsapp_phone_number (...********)";
String api_key = "your_api_key";

int led_pin = 4;  
int sensor_pin = 15; 

ArduCAM myCAM(OV2640, CS); 

void sendMessage(String message, String image_path){
 String API_URL = "http://api.whatabot.net/whatsapp/sendMessage?text=" + urlEncode(message) + "&apikey=" + api_key + "&phone=" + mobile_number;

 if(image_path != ""){
    File image_file = SPIFFS.open(image_path, "r");
    if(!image_file){
        Serial.println("Error opening image file");
        return;
    }
    String image_base64 = "";
    while(image_file.available()){
        image_base64 += (char)image_file.read();
    }
    image_file.close();
    API_URL += "&image=" + urlEncode(image_base64);
 }

  WiFiClient client;    
  HTTPClient http;
  http.begin(client, API_URL);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int http_response_code = http.GET();
  if (http_response_code == 200){
    Serial.println("Whatsapp message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(http_response_code);
  }

  http.end();
}

void setup() {
  
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(sensor_pin, INPUT); 
  pinMode(led_pin, OUTPUT); 
  digitalWrite (led_pin, LOW);
 
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  myCAM.begin();
  myCAM.set_format(JPEG);
}

void loop() {
  int state = digitalRead(sensor_pin); 
    if (state == HIGH) { // if motion detected
    digitalWrite(led_pin, HIGH);  // turn LED ON
    char timestamp[20];
    sprintf(timestamp, "%lu", millis());
    String message = "Motion detected at: " + String(timestamp);
    String image_path = "";
    if(myCAM.capture()){
        image_path = "/capture.jpg";
        myCAM.save_to_file(image_path);
    }
    sendMessage(message, image_path); //send message
    delay(5000);
  } 
  else {
    digitalWrite(led_pin, LOW); // turn LED OFF if we have no motion
  }
  delay(10); // this speeds up the simulation
}

