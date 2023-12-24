#include <Arduino.h>
#include <ETH.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>

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
const int relayPin = 33;

// Global variables
WiFiClient ethClient;
PubSubClient client(ethClient);
int lastButtonState = HIGH;
bool messageSent = false;
unsigned long lastMacSendTime = 0;

// TFT Things
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"
  String Active = "ACTIVE";
  String Clear = "CLEAR";
  String Standby = "STANDBY";
  String Model = "KILLIN'-IT";
  String Series = "E-STOP SYSTEM";
  int x = tft.width() / 2; // Center of the screen
  int y = tft.height() / 2; // Center of the screen



void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';  // Null-terminate the payload
    String message = String((char*)payload);
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Received message: " + message);

    // if (String(topic) == relay_control_topic) {
    //     digitalWrite(relayPin, message == "on" ? HIGH : LOW);
    //     Serial.println(message == "on" ? "Relay turned on" : "Relay turned off");
    // } 

  int buttonState = digitalRead(buttonPin);

    if (String(topic) == relay_control_topic) {
if (message == "on") {
    if ((message == "on") && (buttonState == LOW)) {
        digitalWrite(relayPin, HIGH);
        tft.setTextSize(4);
        tft.setTextColor(TFT_RED, TFT_BLACK, true);
        tft.drawSmoothArc(120, 120, 120, 100, 60, 300, TFT_RED, TFT_TRANSPARENT, false);
        tft.setTextPadding(180);
        tft.drawString(Active, x, 195);
        Serial.println("Relay turned on");
    } else {
        digitalWrite(relayPin, HIGH);
        tft.setTextSize(4);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
        tft.drawSmoothArc(120, 120, 120, 100, 60, 300, TFT_YELLOW, TFT_TRANSPARENT, false);
        tft.setTextPadding(180);
        tft.drawString(Standby, x, 195);
        Serial.println("I'm Good, Others Are Not");
    }
} else {
    digitalWrite(relayPin, LOW);
    tft.setTextSize(4);
    tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
    tft.drawSmoothArc(120, 120, 120, 100, 60, 300, TFT_GREEN, TFT_TRANSPARENT, false);
    tft.setTextPadding(180);
    tft.drawString(Clear, x, 195);
    Serial.println("Relay turned off");
}}

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

void setup() {
    Serial.begin(115200);
    tft.init();
    delay(1000);
    tft.fillScreen(TFT_BLACK);
    ETH.begin();
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    tft.setRotation(0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK,true);
    tft.setTextSize(3);
    tft.drawString("KILLIN'-IT",120,120);
    tft.setTextSize(2);
    tft.drawString("ESTOP SYSTEM",120,150);

}

bool isDefaultIPAddress(IPAddress ip) {
    IPAddress defaultIP; // Default constructor initializes to 0.0.0.0
    return ip == defaultIP;
}

void loop() {

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


