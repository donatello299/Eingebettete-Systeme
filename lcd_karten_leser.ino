#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

// Pin definitions for the LEDs
const int access = A1; // Pin for access granted indicator
const int denied = A2; // Pin for access denied indicator

// RFID module pin definitions
#define Reset_PIN  9        // Reset pin for RFID module
#define SPI_Verbindung  10  // SPI connection pin for RFID module

// Create an instance of the RFID module
MFRC522 mfrc522(SPI_Verbindung, Reset_PIN);
MFRC522::MIFARE_Key key;

// LCD pin definitions
const int rs = 6, en = 7, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create an instance of the LCD

void setup() {
    // Configure LED pins as outputs
    pinMode(access, OUTPUT);
    pinMode(denied, OUTPUT);

    // Initialize the LCD display
    lcd.begin(16, 2);            // Set the LCD size to 16 columns and 2 rows
    lcd.setCursor(0, 0);         // Position the cursor to the first row
    lcd.print("Willkommen!!!");  // Display a welcome message
    lcd.setCursor(0, 1);         // Move the cursor to the second row
    lcd.print("Karte scannen");  // Prompt user to scan a card
    lcd.autoscroll();            // Enable text scrolling

    // Initialize serial communication
    Serial.begin(9600);
    while (!Serial);             // Wait for the serial port to connect

    // Initialize the RFID module
    SPI.begin();                 // Start SPI communication
    mfrc522.PCD_Init();          // Initialize RFID reader
}

// Function to indicate access granted
void accessGranted() {
    lcd.clear();                 // Clear the LCD display
    analogWrite(access, 200);    // Turn on the access granted LED
    lcd.begin(16, 2);            // Reinitialize the LCD
    Serial.println("Zugriff erlaubt"); // Print access message to serial monitor
    lcd.setCursor(0, 0);         // Position the cursor
    lcd.print("Zugriff erlaubt"); // Display access granted message
    lcd.autoscroll();            // Enable text scrolling
    delay(3000);                 // Keep the message for 3 seconds
    setup();                     // Reinitialize setup
    analogWrite(access, 0);      // Turn off the LED
}

// Function to indicate access denied
void accessDenied() {
    Serial.println("Zugriff nicht erlaubt"); // Print denied message to serial monitor
    analogWrite(denied, 200);                // Turn on the access denied LED
    lcd.clear();                             // Clear the LCD display
    lcd.begin(16, 2);                        // Reinitialize the LCD
    lcd.setCursor(0, 0);                     // Position the cursor
    lcd.print("Zugriff verweigert");         // Display access denied message
    lcd.autoscroll();                        // Enable text scrolling
    lcd.setCursor(0, 1);                     // Move to the second row
    delay(1000);
    lcd.print("Bitte erneut");               // Suggest retrying
    delay(3000);                             // Keep the message for 3 seconds
    setup();                                 // Reinitialize setup
    analogWrite(denied, 0);                  // Turn off the LED
}

void loop() {
    // Check if a new card is present
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(50); // Small delay before retrying
        return;    // Exit if no card is found
    }

    // Read and print the card UID
    Serial.print(F("Karten-UID: "));
    String content = ""; // String to store UID
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        // Format and append UID bytes
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase(); // Convert UID to uppercase for comparison
    Serial.println();      // Print a newline

    // Check if the card UID matches the allowed UID
    if (content.substring(1) == "29 AA BF 7E") { // Replace with your card's UID
        accessGranted(); // Call access granted function
    } else {
        accessDenied(); // Call access denied function
    }
}
