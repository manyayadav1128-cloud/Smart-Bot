#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h>

// WiFi Credentials
#define WLAN_SSID       "ssid"
#define WLAN_PASS       "password"

// Adafruit IO Credentials
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  port number
#define AIO_USERNAME    "username"
#define AIO_KEY         "key"

// speed
unsigned long startTime = 0;
unsigned long endTime = 0;
bool objectDetected = false;

float objectLength = 0.10;   

// DHT Settings
#define DHTPIN D4
#define DHTTYPE DHT11   // Change to DHT22 if using DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi Client
WiFiClient client;

// MQTT Client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temperatureFeed = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/R1");
Adafruit_MQTT_Publish humidityFeed = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/G1");
Adafruit_MQTT_Publish speedFeed = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/S1");


#define IR1 D0    // Object Detection
#define IR2 D1    // Left Edge
#define IR3 D2    // Right Edge
#define IR4 D3    // Speed

// ---------- L298N ----------
#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8

void MQTT_connect();
void setup(){
  Serial.begin(115200);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  dht.begin();
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
}
void loop(){
  MQTT_connect();
  // object detection
  if(digitalRead(IR1)==0){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  delay(250);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(250);
  }
  if(digitalRead(IR1)==1){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  // edge detection
  if((digitalRead(IR2)==1)&&(digitalRead(IR3)==1)){ //stop&backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    delay(250);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  if((digitalRead(IR2)==1)&&(digitalRead(IR3)==0)){ //right
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  if((digitalRead(IR2)==0)&&(digitalRead(IR3)==1)){ //left
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  if((digitalRead(IR2)==0)&&(digitalRead(IR3)==0)){ //forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }  
   //Speed
  // Object enters sensor
  if (digitalRead(IR4) == LOW && !objectDetected) {
    startTime = millis();
    objectDetected = true;
  }

  // Object leaves sensor
  if (digitalRead(IR4) == HIGH && objectDetected) {
    endTime = millis();

    float timeSec = (endTime - startTime) / 1000.0;

    if (timeSec > 0) {
      float speed = objectLength / timeSec;

      Serial.print("Speed = ");
      Serial.print(speed);
      Serial.println(" m/s");
      speedFeed.publish(speed);  

    }

    objectDetected = false;
    delay(200);
  }

  // DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read DHT sensor!");
    delay(2000);
    return;
  }
  else{Serial.println("Ok");
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  temperatureFeed.publish(t);
  humidityFeed.publish(h);

  delay(2000);  // Upload every 10 seconds

}
void MQTT_connect() {

  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to Adafruit IO... ");

  while ((ret = mqtt.connect()) != 0) {

    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying in 5 seconds...");

    mqtt.disconnect();
    delay(5000);
  }

  Serial.println("Connected!");
}