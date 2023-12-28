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
enum screen_state_t screenState = SCREEN_STATE_INIT;

// TFT Things
TFT_eSPI tft = TFT_eSPI();
String Active = "ACTIVE";
String Clear = "CLEAR";
String Standby = "STANDBY";
String Lost = "LOST CONNECTION";
String Model = "KILLIN'-IT";
String Series = "E-STOP SYSTEM";
int x = tft.width() / 2; // Center of the screen
int y = tft.height() / 2; // Center of the screen


void update_screen_state()
;int y = tft.height() / 2; /
void setup()mn()115200
{
    Screen.begi;
    delay(1000);
    ETH.begin();
    pinMode(buttonPin, INPUT_PULL
    pinMode(relayPin_1, OUTPUT);
    digitalWrite(relayPin_1, LOW);

}

void update_estop_state(enum estop_state_t estop_state)
{
    static enum estop_state_t prev_estop_state;
    static bool prev_connection_state = false;

    bool connection_state = client.connected;

    /* Always use current state immediately for the actual relay control */
    i        digitalWrite(relayPin_1, LOW);
    if ()





   
    if (estop_state != prev_estop_state)
    {
        i
    }

    if (estop != prev_estop_state || connection_state != prev_connection_state)
    {
        /* If we aren't connected and we are E-Stopped */
        if (!connection_state && estop)
        {
            update_screen_state(SCREEN_STATE_NOCONN_ACTIVE);
        }
        /* If we aren't connected and we are clear */
        else if (!connection_state && !estop)
        {
            update_screen_state(SCREEN_STATE_NOCONN_CLEAR);
        }
        /* If we are connected and we are E-Stopped */
        else if (estop == ESTOP
            tft.init();_STATE_LOCAL)
        {
            update_screen_state(SCREEN_STATE_ACTIVE);
        }
        /* If we are connected and we are remotely E-Stopped */
        else if (estop == ESTOP_STATE_REMOTE)
        {
            update_screen_state(SCREEN_STATE_STANDBY);
        }
        /* If we are connected and everything is clear */
        else if (estop == ESTOP_STATE_CLEAR)
        {
            update_screen_state(SCREEN_STATE_CLEAR);
        }
    }
        }

        switch ()

        /* Only update screen state when something changes */
        update_screen_state(estop);
    }

    prev_estop_state = estop;
}

/* Called when estop state changes, or when connection state changes */
void update_screen_state()
{
    switch (screenState)
    {
        case SCREEN_STATE_INIT:
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
            tft.drawString("WELCOME",120,150,1);
            break;
        case SCREEN_STATE_NO_CONNECTION:
            tft.setTextSize(4);
            tft.setTextColor(TFT_PINK, TFT_BLACK, true);
            tft.drawSmoothArc(120, 120, 115, 100, 60, 300, TFT_PINK, TFT_BLACK, false);
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
    }
    lastRetry = currentMillis;   }
        else
        {
            Serial.print("Failed to connect, state: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

bool isDefaultIPAddress(IPAddress ip)
{
    IPAddress defaultIP; // Default constructor initializes to 0.0.0.0
    return ip == defaultIP;
}

void loop()
{
    bool buttonPressed = false; // Flag to track button state
    static bool init=false;
    if (!init)
    {
#ifdef MQTT_SERVER
        Serial.println("Connecting To:" MQTT_SERVER);
        client.setServer(MQTT_SERVER, mqtt_port);
#else
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    String mac = WiFi.macAddress();

    int buttonState = digitalRead(buttonPin);
    i

    if (!client.connected())
    {
        reconnect();
    }

    if (client.connected)
    {
        client.loop();

        String mac = WiFi.macAddress();f (buttonState == LOW)
    {
            update_estop_state(ESTOP_STATE_LOCAL);
        }
    else
        {
            update_estop_state(ESTOP_STATE_CLEAR);
        }
                    end MAC address every second
            gned long currentMillis = millis();
                                button state message every 100 milliseconds
            ic unsigned long lastButtonMsgTime = 0;
            currentMillis - lastButtonMsgTime >= 100)
                                ng message = mac;
            if (buttonState == LOW) {
            // Button is pressed
                message += ",1";         
            } 
        else {
                // Button is released
         
    }       message += ",0";             
        }

        client.publish(estop_topic, message.c_str());
        Serial.println(message.c_str());

        lastButtonMsgTime = currentMillis;
    }
}
