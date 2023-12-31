#include <Arduino.h>
#include <ETH.h>
#include "PubSubClient.h"
#include "TFT_eSPI.h"

#include "AnimatedGIF.h"
#include "SFX_GIF.h"
#include "GIFDraw.h"
AnimatedGIF gif;
#define GIF_IMAGE SFX_GIF
#define USE_DMA

enum screen_state_t
{
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
    ESTOP_STATE_INIT = -1,
    ESTOP_STATE_CLEAR = 0,
    ESTOP_STATE_LOCAL,
    ESTOP_STATE_REMOTE,
};

// #define MQTT_SERVER "10.1.10.181" // Uncomment for static IP CSD

// MQTT configuration
// const char* mqtt_server = "10.1.10.181"; //  change this for local IP
const int mqtt_port = 1883;
const char *mqtt_username = "csd_design";
const char *mqtt_password = "csd_design";
const char *estop_topic = "estop";
const char *relay_control_topic = "relay";

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
bool remoteEstop = false;
bool messageSent = false;
unsigned long lastMacSendTime = 0;

// TFT Things
// TFT_eSPI tft = TFT_eSPI();
String Active = "ACTIVE";
String Clear = "CLEAR";
String Standby = "STANDBY";
String Lost = "NO CONNECTION";
String Model = "KILLIN'-IT";
String Series = "E-STOP SYSTEM";
int NoConnActiveColor = 0xfb20;
int NoConnClearColor = TFT_BLUE;
int ClearColor = TFT_GREEN;
int ActiveColor = TFT_RED;
int StandbyColor = TFT_YELLOW;
int ArcOD = 120;
int ArcID = 100;
int StartAng = 60;
int EndAng = 300;
int TextPad = 170;
int x = tft.width() / 2;  // Center of the screen
int y = tft.height() / 2; // Center of the screen

// Sprite Creation
TFT_eSprite start_s = TFT_eSprite(&tft);
TFT_eSprite boot_s = TFT_eSprite(&tft);
TFT_eSprite noconn_a_s = TFT_eSprite(&tft);
TFT_eSprite noconn_c_s = TFT_eSprite(&tft);
TFT_eSprite active_s = TFT_eSprite(&tft);
TFT_eSprite standby_s = TFT_eSprite(&tft);
TFT_eSprite clear_s = TFT_eSprite(&tft);

void start_sprite()
{
    start_s.createSprite(240, 240);
    start_s.fillSprite(TFT_BLUE);
    start_s.setTextDatum(MC_DATUM);
    start_s.setTextColor(TFT_WHITE, TFT_BLACK, true);
    start_s.setTextSize(3);
    start_s.drawString("STRICTLY FX", 120, 80, 1);
    start_s.setTextSize(2);
    start_s.drawString(Model, 120, 120, 1);
    start_s.setTextSize(1);
    start_s.drawString("CSD Design", 120, 200, 1);
}

void boot_sprite()
{
    boot_s.createSprite(240, 240);
}

void noconn_active_sprite()
{
    noconn_a_s.createSprite(240, 240);
}

void noconn_clear_sprite()
{
    noconn_c_s.createSprite(240, 240);
}

void active_sprite()
{
    active_s.createSprite(240, 240);
}

void standby_sprite()
{
    standby_s.createSprite(240, 240);
}

void clear_sprite()
{
    clear_s.createSprite(240, 240);
}

void update_estop_state();
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
    gif.begin();
}

void play_gif()
{
    (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw));
    {
        tft.setSwapBytes(true);
        tft.startWrite();
        while (gif.playFrame(true, NULL))
        {
            yield();
        }
        gif.close();
        tft.endWrite();
    }
}

void update_estop_state()
{
    static bool prev_connection_state = false;
    static enum estop_state_t prev_estop_state = ESTOP_STATE_INIT;
    enum estop_state_t estop_state;

    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW)
    {
        estop_state = ESTOP_STATE_LOCAL;
    }
    else if (remoteEstop)
    {
        estop_state = ESTOP_STATE_REMOTE;
    }
    else
    {
        estop_state = ESTOP_STATE_CLEAR;
    }

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
        // tft.setFreeFont(&FreeMonoBold12pt7b);
        tft.fillScreen(TFT_BLACK);
        tft.setRotation(0);
        play_gif();
        tft.fillScreen(TFT_BLACK);
        delay(100);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
        tft.setTextSize(3);
        tft.drawString("STRICTLY FX", 120, 80, 1);
        tft.setTextSize(2);
        tft.drawString(Model, 120, 120, 1);
        tft.setTextSize(1);
        tft.drawString("CSD Design", 120, 200, 1);
        break;
    case SCREEN_STATE_BOOT:
        tft.fillScreen(TFT_BLACK);
        delay(100);
        tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
        tft.setTextSize(3);
        tft.drawString(Model, 120, 100, 1);
        tft.setTextSize(2);
        tft.setTextPadding(TextPad);
        tft.drawString(Series, 120, 130, 1);
        break;
    case SCREEN_STATE_NOCONN_ACTIVE:
        tft.setTextSize(4);
        tft.setTextColor(NoConnActiveColor, TFT_BLACK, true);
        tft.drawSmoothArc(x, y, ArcOD, ArcID, StartAng, EndAng, NoConnActiveColor, TFT_BLACK, true);
        tft.setTextPadding(TextPad);
        tft.drawString("NO", 121, 160);
        tft.drawString("NETWORK", 121, 195);
        break;
    case SCREEN_STATE_NOCONN_CLEAR:
        tft.setTextSize(4);
        tft.setTextColor(NoConnClearColor, TFT_BLACK, true);
        tft.drawSmoothArc(x, y, ArcOD, ArcID, StartAng, EndAng, NoConnClearColor, TFT_BLACK, true);
        tft.setTextPadding(TextPad);
        tft.drawString("NO", 121, 160);
        tft.drawString("NETWORK", 121, 195);
        break;
    case SCREEN_STATE_STANDBY:
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
        tft.setTextSize(3);
        tft.drawString(Model, 120, 100, 1);
        tft.setTextSize(2);
        tft.setTextPadding(TextPad);
        tft.drawString(Series, 120, 130, 1);
        tft.setTextSize(4);
        tft.setTextColor(StandbyColor, TFT_BLACK, true);
        tft.drawSmoothArc(x, y, ArcOD, ArcID, StartAng, EndAng, StandbyColor, TFT_BLACK, true);
        tft.setTextPadding(TextPad);
        tft.drawString(Standby, x, 195);
        break;
    case SCREEN_STATE_CLEAR:
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
        tft.setTextSize(3);
        tft.drawString(Model, 120, 100, 1);
        tft.setTextSize(2);
        tft.setTextPadding(TextPad);
        tft.drawString(Series, 120, 130, 1);
        tft.setTextSize(4);
        tft.setTextColor(ClearColor, TFT_BLACK, true);
        tft.drawSmoothArc(x, y, ArcOD, ArcID, StartAng, EndAng, ClearColor, TFT_BLACK, true);
        tft.setTextPadding(TextPad);
        tft.drawString(Clear, x, 195);
        break;
    case SCREEN_STATE_ACTIVE:
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
        tft.setTextSize(3);
        tft.drawString(Model, 120, 100, 1);
        tft.setTextSize(2);
        tft.setTextPadding(TextPad);
        tft.drawString(Series, 120, 130, 1);
        tft.setTextSize(4);
        tft.setTextColor(ActiveColor, TFT_BLACK, true);
        tft.drawSmoothArc(x, y, ArcOD, ArcID, StartAng, EndAng, ActiveColor, TFT_BLACK, true);
        tft.setTextPadding(TextPad);
        tft.drawString(Active, x, 195);
        break;
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0'; // Null-terminate the payload
    String message = String((char *)payload);
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Received message: " + message);

    if (String(topic) == relay_control_topic)
    {
        if (message == "on")
        {
            remoteEstop = true;
        }
        else
        {
            remoteEstop = false;
        }

        update_estop_state();
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
    static bool init = false;

    if (!init)
    {
#ifdef MQTT_SERVER
        Serial.println("Connecting To:" MQTT_SERVER);
        client.setServer(MQTT_SERVER, mqtt_port);
        client.setCallback(callback);
        init = true;
#else
        if (!isDefaultIPAddress(ETH.gatewayIP()))
        {
            Serial.println("Connecting To:" + ETH.gatewayIP().toString());
            client.setServer(ETH.gatewayIP(), mqtt_port);
            client.setCallback(callback);
            init = true;
        }
#endif
    }

    update_estop_state();

    if (!init)
        return;

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
            int buttonState = digitalRead(buttonPin);
            if (buttonState == LOW)
            {
                // Button is pressed
                message += ",1";
            }
            else
            {
                // Button is released
                message += ",0";
            }

            client.publish(estop_topic, message.c_str());
            Serial.println(message.c_str());

            lastButtonMsgTime = currentMillis;
        }
    }
}
