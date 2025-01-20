#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>

HardwareSerial mySerial(1);  // Use UART1 on ESP32
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(115200); 
    mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (Change if needed)

    if (!myDFPlayer.begin(mySerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (true);
    }
    Serial.println("DFPlayer Mini initialized!");
    myDFPlayer.volume(25); // Set volume (0-30)

    Serial.println("Enter amount (1-99999) in Serial Monitor:");
}

void loop() {
  for (int i =28 ; i <= 32; i++) {
    Serial.print("Playing: ");
    Serial.println(i);
    myDFPlayer.play(i);
    delay(2000); // Wait for each file to play
}
    // if (Serial.available()) {
    //     int amount = Serial.parseInt();
    //     while (Serial.available()) Serial.read();  // ✅ Clears the buffer

    //     Serial.print("Received amount: ");
    //     Serial.println(amount);
    //     playAmount(amount);
    // }
}

void playAmount(int amount) {
    myDFPlayer.play(32); // Play "You have received a payment of"
    delay(2500);

    if (amount >= 1000) {
        int thousands = amount / 1000;
        myDFPlayer.play(thousands); // Play "1000", "2000", etc.
        delay(1000);
        myDFPlayer.play(29);  // Play "thousand"
        delay(1000);
        amount %= 1000;
    }

    if (amount >= 100) {
        int hundreds = amount / 100;
        myDFPlayer.play(hundreds); // Play "1", "2", ... "9"
        delay(1000);
        myDFPlayer.play(28);  // Play "hundred"
        delay(1000);
        amount %= 100;
    }

    if (amount >= 20) {
        int tens = amount / 10;
        myDFPlayer.play(19 + (tens - 1)); // Play "20", "30", ... "90"
        delay(1000);
        amount %= 10;
    }

    if (amount > 0) {
        myDFPlayer.play(amount); // Play "1", "2", ... "9"
        delay(1000);
    }

    myDFPlayer.play(30); // Play "rupees"
    delay(1000);
    myDFPlayer.play(31); // Play "Only"
    delay(1000);
}
