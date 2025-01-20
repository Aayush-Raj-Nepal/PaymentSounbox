#include "WiFi.h"
#include "HTTPClient.h"
#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>

#define RX_PIN 16  // Change as per wiring
#define TX_PIN 17

const char* ssid = "Hardware_Hackathon-1"; // Change to your WiFi SSID
const char* password = "hackathon@2025"; // Change to your WiFi Password
const char* serverURL = "https://paymentsounbox.onrender.com/latest"; // Change to your API endpoint

HardwareSerial mySerial(1);  // Use UART1 on ESP32
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

    if (!myDFPlayer.begin(mySerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (true);
    }
    Serial.println("DFPlayer Mini initialized!");
    myDFPlayer.volume(25); // Set volume (0-30)

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected!");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverURL);
        int httpResponseCode = http.GET();

        if (httpResponseCode == 200) {
            String response = http.getString();
            Serial.print("Server Response: ");
            Serial.println(response);

            int amount = response.toInt();
            if (amount > 0) {
                playAmount(amount);
            }
        } else {
            Serial.print("Error in HTTP request: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }

    delay(2000); // Poll every 2 seconds
}

void playAmount(int amount) {
    myDFPlayer.play(31); // "You have received a payment of"
    delay(2000);
    
    if (amount >= 1000) {
        int thousands = amount / 1000;
        myDFPlayer.play(thousands); // Play "1000", "2000", etc.
        delay(1000);
        myDFPlayer.play(29);  // "thousand"
        delay(1000);
        amount %= 1000;
    }

    if (amount >= 100) {
        int hundreds = amount / 100;
        myDFPlayer.play(hundreds);
        delay(1000);
        myDFPlayer.play(28);  // "hundred"
        delay(1000);
        amount %= 100;
    }

    if (amount >= 20) {
        int tens = amount / 10;
        myDFPlayer.play(10 + (tens - 1)); // "20", "30", "40", ...
        delay(1000);
        amount %= 10;
    }

    if (amount > 0) {
        myDFPlayer.play(amount);
        delay(1000);
    }

    myDFPlayer.play(32); // "rupees"
    delay(1000);
    myDFPlayer.play(30); // "only"
    delay(1000);
}
