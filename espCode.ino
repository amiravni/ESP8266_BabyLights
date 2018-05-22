/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.
*/

#include "defines.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include "DHTesp.h"


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = MYAUTH;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = MYSSID;
char pass[] = MYPASS;

DHTesp dht;
long dhtLastTime = 0;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMOFLEDS, STRIPPIN, NEO_GRB + NEO_KHZ800);
int redDay = 255;
int greenDay = 128;
int blueDay = 0;
int redNight = 255;
int greenNight = 0;
int blueNight = 0;
int ledsNum = NUMOFLEDS/2;
float power = 1.0;
bool isDay = false;


void TurnDayLights() {
  for (int iii=0;iii<ledsNum;iii++) {
    strip.setPixelColor(iii, strip.Color( (int)(power*(float)redDay), (int)(power*(float)greenDay),(int)(power*(float)blueDay) )); 
  }
  for (int iii=(ledsNum);iii<NUMOFLEDS;iii++) {
    strip.setPixelColor(iii, strip.Color(0, 0, 0) ); 
  }    
  strip.show();
}

void TurnNightLights() {
  for (int iii=0;iii<ledsNum;iii++) {
    strip.setPixelColor(iii, strip.Color( (int)(power*(float)redNight), (int)(power*(float)greenNight),(int)(power*(float)blueNight) )); 
  }
  for (int iii=(ledsNum);iii<NUMOFLEDS;iii++) {
    strip.setPixelColor(iii, strip.Color(0, 0, 0) ); 
  }  
  strip.show();
  //Serial.println("Night lights are on");
}

void refreshLeds() {
  if (isDay) TurnDayLights();
  else TurnNightLights();
}

BLYNK_WRITE(V0)
{
  Serial.print("Timer (V0): Got a value: ");
  Serial.println(param.asStr());

  if (param.asInt() == 0)  isDay = false;
  else  isDay = true;
  refreshLeds();
}

BLYNK_WRITE(V1)
{
  Serial.print("Button (V1): Got a value: ");
  Serial.println(param.asStr());
  if (param.asInt() == 0)  isDay = false;
  else  isDay = true; 
  refreshLeds();
}

BLYNK_WRITE(V2)
{
  Serial.print("Power Slider (V2): Got a value: ");
  Serial.println(param.asStr());
  power = param.asFloat(); 
  refreshLeds(); 
}

BLYNK_WRITE(V3)
{
  Serial.print("LEDS# Slider (V3): Got a value: ");
  Serial.println(param.asStr());
  if (param.asInt() < NUMOFLEDS) ledsNum = param.asInt();
  else   ledsNum = NUMOFLEDS;
  refreshLeds();
  //ledsNum = min(NUMOFLEDS,param.asInt());  
}

BLYNK_WRITE(V4)
{
  Serial.print("Day Color (V4): Got a value: ");
  Serial.print(param[0].asStr());
  Serial.print(" , ");
  Serial.print(param[1].asStr());
  Serial.print(" , ");
  Serial.println(param[2].asStr());      
  redDay = param[0].asInt();
  greenDay = param[1].asInt();
  blueDay = param[2].asInt();  
  refreshLeds();
}

BLYNK_WRITE(V5)
{
  Serial.print("Night Color (V5): Got a value: ");
  Serial.print(param[0].asStr());
  Serial.print(" , ");
  Serial.print(param[1].asStr());
  Serial.print(" , ");
  Serial.println(param[2].asStr()); 
  redNight = param[0].asInt();
  greenNight = param[1].asInt();
  blueNight = param[2].asInt();  
  refreshLeds();
}

void sendSensor()
{
  float h = dht.getHumidity();
  float t = dht.getTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V7, h);
  Blynk.virtualWrite(V6, t);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  strip.begin();
  refreshLeds();
  dht.setup(DHTPIN); 
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
}

void loop()
{
  Blynk.run();
  if (( dhtLastTime == 0 ) || ((millis() - dhtLastTime) > DT_FOR_DHT_MSEC) ) {
    dhtLastTime = millis();
    sendSensor();
  }
}
