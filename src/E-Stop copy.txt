#include <Arduino.h>
#include <ETH.h>
#include "PubSubClient.h"
#include "TFT_eSPI.h"

//#define MQTT_SERVER "10.1.10.181" // Uncomment for static IP CSD

// MQTT configuration
// const char* mqtt_server = "10.1.10.181"; //  change this for local IP
const int mqtt_port = 1883;
const char* mqtt_username = "csd_design";
const char* mqtt_password = "csd_design";
const char* estop_topic = "estop";
const char* relay_control_topic = "relay";

// GPIO configuration
const int buttonPin = 34;
const int relayPin_1 = 33;
const int relayPin_2 = 0;
const int relayPin_3 = 1;
const int relayPin_4 = 4;
const int relayPin_5 = 32;
const int relayPin_6 = 16;

// Global variables
WiFiClient ethClient;
PubSubClient client(ethClient);
int lastButtonState = HIGH;
bool messageSent = false;
unsigned long lastMacSendTime = 0;

// TFT Things
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite clear = TFT_eSprite(&tft);
TFT_eSprite active = TFT_eSprite(&tft);
TFT_eSprite standby = TFT_eSprite(&tft);
String Active = "ACTIVE";
String Clear = "CLEAR";
String Standby = "STANDBY";
String Model = "KILLIN'-IT";
String Series = "E-STOP SYSTEM";
int x = tft.width() / 2; // Center of the screen
int y = tft.height() / 2; // Center of the screen

void setup() {
    Serial.begin(115200);
    tft.init();
    delay(1000);
    tft.fillScreen(TFT_BLACK);
    ETH.begin();
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(relayPin_1, OUTPUT);
    digitalWrite(relayPin_1, LOW);
    tft.setRotation(0);
    tft.setTextDatum(MC_DATUM);
    // tft.createSprite(240, 240);
    // tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK,true);
    tft.setTextSize(3);
    tft.drawString("KILLIN'-IT",120,120,1);
    tft.setTextSize(2);
    tft.drawString("ESTOP SYSTEM",120,150,1);
    // tft.pushSprite(0, 0);



        //Clear Sprite Create
    clear.createSprite(240, 240);
    clear.fillSprite(TFT_BLUE);
    clear.setTextSize(4);
    clear.setTextColor(TFT_GREEN, TFT_BLACK, true);
    clear.drawSmoothArc(120, 120, 110, 100, 60, 300, TFT_GREEN, TFT_TRANSPARENT, false);
    clear.setTextPadding(180);
    clear.drawString(Clear, x, 195);
    clear.setTextDatum(MC_DATUM);
    clear.setTextColor(TFT_WHITE, TFT_TRANSPARENT,true);
    clear.setTextSize(3);
    clear.drawString("KILLIN'-IT",120,120);
    clear.setTextSize(2);
    clear.drawString("ESTOP SYSTEM",120,150);
    clear.pushSprite(0,0);

    // Active Sprite Create
    active.createSprite(240, 240);
    active.setTextSize(4);
    active.setTextColor(TFT_RED, TFT_BLACK, true);
    active.drawSmoothArc(120, 120, 110, 100, 60, 300, TFT_RED, TFT_TRANSPARENT, false);
    active.setTextPadding(180);
    active.drawString(Active, x, 195);
    active.setTextDatum(MC_DATUM);
    active.setTextColor(TFT_WHITE, TFT_TRANSPARENT,true);
    active.setTextSize(3);
    active.drawString("KILLIN'-IT",120,120);
    active.setTextSize(2);
    active.drawString("ESTOP SYSTEM",120,150);

    // Standby Sprite Create
    standby.createSprite(240, 240);
    standby.setTextSize(4);
    standby.setTextColor(TFT_YELLOW, TFT_BLACK, true);
    standby.drawSmoothArc(120, 120, 110, 100, 60, 300, TFT_YELLOW, TFT_TRANSPARENT, false);
    standby.setTextPadding(180);
    standby.drawString(Standby, x, 195);
    standby.setTextDatum(MC_DATUM);
    standby.setTextColor(TFT_WHITE, TFT_TRANSPARENT,true);
    standby.setTextSize(3);
    standby.drawString("KILLIN'-IT",120,120);
    standby.setTextSize(2);
    standby.drawString("ESTOP SYSTEM",120,150);


}



void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';  // Null-terminate the payload
    String message = String((char*)payload);
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Received message: " + message);

    // if (String(topic) == relay_control_topic) {
    //     digitalWrite(relayPin_1, message == "on" ? HIGH : LOW);
    //     Serial.println(message == "on" ? "Relay turned on" : "Relay turned off");
    // } 

  int buttonState = digitalRead(buttonPin);

if (String(topic) == relay_control_topic) {
    if (message == "on") {
        if ((message == "on") && (buttonState == LOW)) {
            digitalWrite(relayPin_1, HIGH);
            //active.pushSprite(0, 0);
            tft.setTextSize(4);
            tft.setTextColor(TFT_RED, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_RED, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Active, x, 195);
            Serial.println("Relay turned on");
        } else {
            digitalWrite(relayPin_1, HIGH);
            //standby.pushSprite(0, 0);
            tft.setTextSize(4);
            tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_YELLOW, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Standby, x, 195);
            Serial.println("I'm Good, Others Are Not");
        }
    } else {
        digitalWrite(relayPin_1, LOW);
        //clear.pushSprite(0, 0);
        tft.setTextSize(4);
        tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
        tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_GREEN, TFT_BLACK, false);
        tft.setTextPadding(180);
        tft.drawString(Clear, x, 195);
        Serial.println("Relay turned off");
    }
    
    }

}


void reconnect() {
    while (!client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            client.subscribe(relay_control_topic);
        } else {
            Serial.print("Failed to connect, state: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}



bool isDefaultIPAddress(IPAddress ip) {
    IPAddress defaultIP; // Default constructor initializes to 0.0.0.0
    return ip == defaultIP;
}

void loop() {

    // img.setTextDatum(MC_DATUM);
    // img.setTextColor(TFT_WHITE, TFT_TRANSPARENT,false);
    // img.setTextSize(3);
    // img.drawString("KILLIN'-IT",120,120);
    // img.setTextSize(2);
    // img.drawString("ESTOP SYSTEM",120,150);
    // img.pushSprite(0, 0);

bool buttonPressed = false; // Flag to track button state


  static bool init=false;
  if (!init)
  {
#ifdef MQTT_SERVER
    Serial.println("Connecting To:" MQTT_SERVER);
    client.setServer(MQTT_SERVER, mqtt_port);
#else
    if (isDefaultIPAddress(ETH.gatewayIP())) return;
    Serial.println("Connecting To:" + ETH.gatewayIP().toString());
    client.setServer(ETH.gatewayIP(), mqtt_port);
#endif
    client.setCallback(callback);
    init=true;
  }
    

    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    String mac = WiFi.macAddress();
    int buttonState = digitalRead(buttonPin);

    // Send MAC address every second
    unsigned long currentMillis = millis();

    // Send button state message every 100 milliseconds
    static unsigned long lastButtonMsgTime = 0;
    if (currentMillis - lastButtonMsgTime >= 100) {
        String message = mac;
        if (buttonState == LOW) {
            // Button is pressed
            message += ",1";         
        } 
        else {
            // Button is released
            message += ",0";             
        }

        client.publish(estop_topic, message.c_str());
        Serial.println(message.c_str());


        lastButtonMsgTime = currentMillis;
    }


}


