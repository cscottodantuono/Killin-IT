#include <Arduino.h>
#include <ETH.h>
#include "PubSubClient.h"
#include "TFT_eSPI.h"

enum screen_state_t {
  SCREEN_STATE_INIT = -1,
  SCREEN_STATE_BOOT,
  SCREEN_STATE_NOCONN_CLEAR,
  SCREEN_STATE_NOCONN_ACTIVE,
  SCREEN_STATE_STANDBY,
  SCREEN_STATE_CLEAR,
  SCREEN_STATE_ACTIVE,
};

enum estop_state_t 
{
    ESTOP_STATE_CLEAR = 0,
    ESTOP_STATE_LOCAL,
    ESTOP_STATE_REMOTE,
};

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
String Active = "ACTIVE";
String Clear = "CLEAR";
String Standby = "STANDBY";
String Lost = "NO CONN";
String Model = "KILLIN'-IT";
String Series = "E-STOP SYSTEM";
int x = tft.width() / 2; // Center of the screen
int y = tft.height() / 2; // Center of the screen

void update_estop_state(enum estop_state_t estop_state);
void update_screen_state(enum screen_state_t screen_state);

void setup()
{
    Serial.begin(115200);
    update_screen_state(SCREEN_STATE_INIT);
    ETH.begin();
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(relayPin_1, OUTPUT);
    digitalWrite(relayPin_1, LOW);
    update_screen_state(SCREEN_STATE_BOOT);
}

void update_estop_state(enum estop_state_t estop_state)
{
    static enum estop_state_t prev_estop_state;
    static bool prev_connection_state = false;

    bool connection_state = client.connected();

    /* Always use current state immediately for the actual relay control */
    if (estop_state)
    {
        digitalWrite(relayPin_1, HIGH);
    }
    else
    {
        digitalWrite(relayPin_1, LOW);
    }

    if (estop_state != prev_estop_state || connection_state != prev_connection_state)
    {
        /* If we aren't connected and we are E-Stopped */
        if (!connection_state && estop_state)
        {
            update_screen_state(SCREEN_STATE_NOCONN_ACTIVE);
        }
        /* If we aren't connected and we are clear */
        else if (!connection_state && !estop_state)
        {
            update_screen_state(SCREEN_STATE_NOCONN_CLEAR);
        }
        /* If we are connected and we are E-Stopped */
        else if (estop_state == ESTOP_STATE_LOCAL)
        {
            update_screen_state(SCREEN_STATE_ACTIVE);
        }
        /* If we are connected and we are remotely E-Stopped */
        else if (estop_state == ESTOP_STATE_REMOTE)
        {
            update_screen_state(SCREEN_STATE_STANDBY);
        }
        /* If we are connected and everything is clear */
        else if (estop_state == ESTOP_STATE_CLEAR)
        {
            update_screen_state(SCREEN_STATE_CLEAR);
        }
    }

    prev_connection_state = connection_state;
    prev_estop_state = estop_state;
}

/* Called when estop state changes, or when connection state changes */
void update_screen_state(enum screen_state_t screenState)
{
    switch (screenState)
    {
        case SCREEN_STATE_INIT:
            tft.init();
            tft.fillScreen(TFT_BLACK);
            tft.setRotation(0);
            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_WHITE, TFT_BLACK,true);
            tft.setTextSize(3);
            tft.drawString(Model,120,120,1);
            tft.setTextSize(2);
            tft.drawString(Series,120,150,1);
            break;
        case SCREEN_STATE_BOOT:
            tft.setTextColor(TFT_DARKCYAN, TFT_BLACK,true);
            tft.setTextSize(3);
            tft.drawString(Model,120,120,1);
            tft.setTextSize(2);
            tft.setTextPadding(180);
            tft.drawString("WELCOME",120,150,1);
            break;
        case SCREEN_STATE_NOCONN_ACTIVE:
            tft.setTextSize(4);
            tft.setTextColor(TFT_PINK, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_PINK, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Lost, x, 195);
            break;
        case SCREEN_STATE_NOCONN_CLEAR:
            tft.setTextSize(4);
            tft.setTextColor(TFT_BLUE, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_BLUE, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Lost, x, 195);
            break;
        case SCREEN_STATE_STANDBY:
            tft.setTextSize(4);
            tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_YELLOW, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Standby, x, 195);
            break;
        case SCREEN_STATE_CLEAR:
            tft.setTextSize(4);
            tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_GREEN, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Clear, x, 195);
            break;
        case SCREEN_STATE_ACTIVE:
            tft.setTextSize(4);
            tft.setTextColor(TFT_RED, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_RED, TFT_BLACK, false);
            tft.setTextPadding(180);
            tft.drawString(Active, x, 195);
            break;    
    }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
    payload[length] = '\0';  // Null-terminate the payload
    String message = String((char*)payload);
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Received message: " + message);

    if (String(topic) == relay_control_topic)
    {
        int buttonState = digitalRead(buttonPin);
        if (buttonState == LOW)
        {
            update_estop_state(ESTOP_STATE_LOCAL);
        }
        else if (message == "on")
        {
            update_estop_state(ESTOP_STATE_REMOTE);
        }
        else
        {
            update_estop_state(ESTOP_STATE_CLEAR);
        }
    }
}

void reconnect()
{
    static unsigned long lastRetry = 0;
    unsigned long currentMillis = millis();

    // Try reconnecting every 2 seconds
    if (currentMillis - lastRetry >= 2000)
    {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
        {
            Serial.println("Connected to MQTT broker");
            client.subscribe(relay_control_topic);
        }
        else
        {
            Serial.print("Failed to connect, state: ");
            Serial.println(client.state());
        }
        lastRetry = currentMillis;
    }
}

bool isDefaultIPAddress(IPAddress ip)
{
    IPAddress defaultIP; // Default constructor initializes to 0.0.0.0
    return ip == defaultIP;
}

void loop()
{
    static bool init=false;
  
    if (!init)
    {
#ifdef MQTT_SERVER
        Serial.println("Connecting To:" MQTT_SERVER);
        client.setServer(MQTT_SERVER, mqtt_port);
    #else
        if (isDefaultIPAddress(ETH.gatewayIP())) break;
        Serial.println("Connecting To:" + ETH.gatewayIP().toString());
        client.setServer(ETH.gatewayIP(), mqtt_port);
    #endif
        client.setCallback(callback);
        init=true;
    }

    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW)
    {
        update_estop_state(ESTOP_STATE_LOCAL);
    }
    else
    {
        update_estop_state(ESTOP_STATE_CLEAR);
    }

    if (!init) return;

    if (!client.connected())
    {
        reconnect();
    }

    if (client.connected())
    {
        client.loop();

        String mac = WiFi.macAddress();

        // Send MAC address every second
        unsigned long currentMillis = millis();

        // Send button state message every 100 milliseconds
        static unsigned long lastButtonMsgTime = 0;
        if (currentMillis - lastButtonMsgTime >= 100)
        {
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
}
