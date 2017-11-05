/*

 Nov 2017 by Lucy Rogers
 
Cheerlights bus using WeMoS D1 ESP-8266 board and Andy Stanford-Clark's Cheerlights MQTT feed iot.eclipse.org.

Cheerlights parts influenced by LeRoy Miller's code https://github.com/kd8bxp/2017-Halloween-Cheerlights-Ghost.

Other parts based Andy Stanford-Clark's Rehau VOCGlowOrbv2.ino
https://github.com/DrLucyRogers/Fartometer/blob/master/RehauVOCGlowOrbv2.ino

Copyright (c) 2017 IBM Corporation
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 

 use board "WeMos D1 R2 & mini"
 CPU 160MHz
 4M (3M SPIFFS)
 upload speed 115200
 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// remember to change MQTT_KEEPALIVE to 60 in the header file ~/Arduino/libraries/PubSubClient/src

/////////////////////////////////////////////////////////////////////////////////////////////

// Update these with values suitable for your network.
const char* wifi_ssid = "****";
const char* wifi_password = "****";


String color;
int brightness = 150;

/////////////////////////////////////////////////////////////////////////////////////////////


#define BROKER "iot.eclipse.org"
#define CLIENTID ""
#define PASSWORD ""


//change this if the flash time becomes annoying
#define FLASH_TIME 2000 // ms

// subscribe to this for commands:
#define COMMAND_TOPIC "cheerlights"

//WiFiClientSecure espClient;
WiFiClient espClient;
PubSubClient client(espClient);

//Change this if using different number of neopixels or different pin
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(8, 4); // eight pixels, on pin 4
// pin 4 is D2 on the WeMoS D1 mini

// flashes this colour when connecting to wifi:
static uint32_t wifi_colour = pixel.Color(5, 0, 5); // magenta
// flashes this colour when connecting to MQTT:
static uint32_t mqtt_colour = pixel.Color(0, 5, 5); // cyan

static uint32_t current_LED = 0x000000; // black
static uint32_t LED_state = current_LED;

int next_flash;
boolean flashing = false;


static uint32_t magenta = pixel.Color(255, 0, 255);
static uint32_t cyan = pixel.Color(0, 255, 255);
static uint32_t white = pixel.Color(255, 255, 255);
static uint32_t warmwhite = pixel.Color(253, 245, 230);
static uint32_t oldlace = pixel.Color(253, 245, 230);
static uint32_t black = pixel.Color(0, 0, 0);
static uint32_t off = pixel.Color(0, 0, 0);
static uint32_t red = pixel.Color(255, 0, 0);
static uint32_t green = pixel.Color(0, 255, 0);
static uint32_t blue = pixel.Color(0, 0, 255);
static uint32_t yellow = pixel.Color(255, 255, 0);
static uint32_t purple = pixel.Color(102,51,204);
static uint32_t orange = pixel.Color(255,153,0);
static uint32_t pink = pixel.Color(255, 192, 203);


void setup() {
  
  Serial.begin(9600);

  pixel.begin();
  //pixel.setBrightness(64);
  pixel.show(); // Initialize all pixels to 'off'
  
  setup_wifi();
  client.setServer(BROKER, 1883);
  client.setCallback(callback);
 
}

void setup_wifi() {
  // connecting to wifi
 set_pixels(wifi_colour); 
  
  // We start by connecting to a WiFi network   
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  //WiFi.begin(wifi_ssid);

  wait_for_wifi();
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  char content[10];
  int count;
  char count_str[3];

 color = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    color+=(char)payload[i];
  }

  
  Serial.print("Message arrived: ");

  strncpy(content, (char *)payload, length);
  content[length] = '\0';
  
  Serial.print("Cheerlights colour:");
  Serial.print(content);
  Serial.println("");

  uint32_t value = strtol(content, 0, 10);

    if (color == "white") {
        set_pixels(white);
    } else if (color == "green") {
        set_pixels(green);
    } else if (color == "red") {
        set_pixels(red);
    } else if (color == "blue") {
        set_pixels(blue);
    } else if (color == "yellow") {
        set_pixels(yellow);
    } else if (color == "purple") {
        set_pixels(purple);    
    } else if (color == "pink") {
        set_pixels(pink);    
    } else if (color == "cyan") {
        set_pixels(cyan);    
    } else if (color == "magenta") {
        set_pixels(magenta);           
    } else if (color == "warmwhite") {
        set_pixels(oldlace);  
    } else if (color == "oldlace") {
        set_pixels(oldlace);  
    } else if (color == "orange") {
        set_pixels(orange);  

        
    } else {
    set_pixels(off);
  }


}


void wait_for_wifi()
{
  Serial.println("waiting for Wifi");
  
  // connecting to wifi - magenta
  set_pixels(wifi_colour);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    toggle_pixel();
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}


void reconnect() {

  if (WiFi.status() != WL_CONNECTED) {
      wait_for_wifi();
  }
  
  // Loop until we're reconnected to the broker
  set_pixels(mqtt_colour);
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    
    // Attempt to connect
    if (client.connect(CLIENTID)) {
      Serial.println("connected");
     
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      toggle_pixel();
      //delay(5000);
    }
  }
  
  set_pixels(0); // clear pixel when connected (black)
 
  // subscribe to the command topic
  client.subscribe(COMMAND_TOPIC);
      
}


void loop() {
  int reading;
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // service the flashing if that's what we're doing
  if (flashing && millis() > next_flash)
  {
    toggle_pixel();
    next_flash = millis() + FLASH_TIME;
  }

  
}

void set_pixels(uint32_t colour) {
  _set_pixels(colour);
  // Update current_LED with what the user last requested,
  // so we can toggle it to black and back again.
  current_LED = colour;

}

uint32_t _current_colour;

void _set_pixels(uint32_t colour) {
  for (int i = 0; i < pixel.numPixels(); i++) {
    pixel.setPixelColor(i, colour);
  }

 // Store current actual LED colour
  // (which may be black if toggling code turned it off.)
  _current_colour = colour;
  pixel.show();
}

void toggle_pixel() {

  if (_current_colour == 0) 
  {
    // if it's black, set it to the stored colour
    _set_pixels(current_LED);
  } else {
    // otherwise set it to black
    _set_pixels(0);
  }
}

