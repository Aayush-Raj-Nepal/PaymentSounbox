#include <WiFi.h>
#include <HTTPClient.h>
#include "DFRobotDFPlayerMini.h"

#define BUTTON_PIN 4 // Change to your button GPIO pin
#define DEBOUNCE_TIME 200 // Debounce time for stable input

HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

const char* ssid = "Hardware_Hackathon-1"; // Change to your WiFi SSID
const char* password = "hackathon@2025"; // Change to your WiFi Password
const char* serverURL = "https://paymentsounbox.onrender.com/latest"; // Change to your API endpoint


volatile unsigned long lastPressTime = 0;
volatile int clickCount = 0;

void IRAM_ATTR handleButtonPress() {
    unsigned long currentTime = millis();
    if (currentTime - lastPressTime > DEBOUNCE_TIME) {
        clickCount++;
    }
    lastPressTime = currentTime;
}

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

    if (!myDFPlayer.begin(mySerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (true);
    }
    myDFPlayer.volume(25);

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void loop() {
    static unsigned long lastPollTime = 0;
    static unsigned long buttonStartTime = 0;
    static bool buttonProcessing = false;

    // ✅ 1. Poll server every 2s
    if (millis() - lastPollTime >= 2000) {
        pollLatestTransaction();
        lastPollTime = millis();
    }

    // ✅ 2. Process button clicks (after a small delay to check multiple clicks)
    if (clickCount > 0 && !buttonProcessing) {
        buttonStartTime = millis();
        buttonProcessing = true;
    }

    if (buttonProcessing && millis() - buttonStartTime > 500) {
        if (clickCount == 1) {
            Serial.println("Single Click: Replay Last Transaction");
            fetchAndPlay("/replay");
        } else if (clickCount == 2) {
            Serial.println("Double Click: Get Today's Total Payments");
            fetchAndPlay("/total-today");
        } else if (clickCount == 3) {
            Serial.println("Triple Click: Get This Month's Total Payments");
            fetchAndPlay("/total-month");
        }
        clickCount = 0;
        buttonProcessing = false;
    }
}

// ✅ Poll latest transaction
void pollLatestTransaction() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverURL + "/latest");
        int httpResponseCode = http.GET();

        if (httpResponseCode == 200) {
            String response = http.getString();
            Serial.print("Latest Transaction: ");
            Serial.println(response);

            int amount = response.toInt();
            if (amount > 0) {
                playAmount(amount);
            }
        } else {
            Serial.print("Error: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
}

// ✅ Fetch and Play audio for special button features
void fetchAndPlay(String endpoint) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverURL + endpoint);
        int httpResponseCode = http.GET();

        if (httpResponseCode == 200) {
            String response = http.getString();
            Serial.print("Response from ");
            Serial.print(endpoint);
            Serial.print(": ");
            Serial.println(response);

            int amount = response.toInt();
            if (amount > 0) {
                playAmount(amount);
            }
        } else {
            Serial.print("Error: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
}

// ✅ Play the amount using DFPlayer Mini
void playAmount(int amount) {
    myDFPlayer.play(32); // "You have received a payment of"
    delay(2500);

    if (amount >= 1000) {
        myDFPlayer.play(amount / 1000);
        delay(1000);
        myDFPlayer.play(29); // "Thousand"
        delay(1000);
        amount %= 1000;
    }

    if (amount >= 100) {
        myDFPlayer.play(amount / 100);
        delay(1000);
        myDFPlayer.play(28); // "Hundred"
        delay(1000);
        amount %= 100;
    }

    if (amount >= 20) {
        myDFPlayer.play(10 + (amount / 10 - 1));
        delay(1000);
        amount %= 10;
    }

    if (amount > 0) {
        myDFPlayer.play(amount);
        delay(1000);
    }

    myDFPlayer.play(30); // "Only"
    delay(1000);
    myDFPlayer.play(31); // "Rupees"
    delay(1000);
}
