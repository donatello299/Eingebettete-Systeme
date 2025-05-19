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

const String validUIDs [] = {"29AABF7E"};

bool isAuthorized(String uid) {
    for (String valid : validUIDs) {
        if (uid == valid) return true;
    }
    return false;
}

void setup() {
    // Configure LED pins as outputs
    pinMode(access, OUTPUT);
    pinMode(denied, OUTPUT);

    // Initialize the LCD display
    showWelcome();

    // Initialize serial communication
    Serial.begin(9600);
    while (!Serial);             // Wait for the serial port to connect

    // Initialize the RFID module
    SPI.begin();                 // Start SPI communication
    mfrc522.PCD_Init();          // Initialize RFID reader
}

void showWelcome() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Willkommen !!!");
    lcd.setCursor(0, 1);
    lcd.print("Karte zeigen");
}

// Function to indicate access granted
void accessGranted() {
    analogWrite(access, 200);    // Turn on the access granted LED
    lcd.setCursor(0, 0);         // Position the cursor
    lcd.clear();
    Serial.println("Zugriff erlaubt"); // Print access message to serial monitor
    lcd.print("Zugriff erlaubt"); // Display access granted message
    delay(3000);                 // Keep the message for 3 seconds               
    analogWrite(access, 0);      // Turn off the LED
    showWelcome();                // Reinitialize welcome
}

// Function to indicate access denied
void accessDenied() {
    analogWrite(denied, 200);                // Turn on the access denied LED
    lcd.clear();                             // Clear the LCD display
    lcd.setCursor(0, 0);                     // Position the cursor
    lcd.print("Zugriff verweigert");         // Display access denied message
    lcd.setCursor(0, 1);                     // Move to the second row
    lcd.print("Bitte erneut");               // Suggest retrying
    Serial.println("Zugriff nicht erlaubt"); // Print denied message to serial monitor
    delay(3000);                             // Keep the message for 3 seconds
    analogWrite(denied, 0);                  // Turn off the LED
    showWelcome();                           // Reinitialize welcome
}

void loop() {
    // Check if a new card is present
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(50); // Small delay before retrying
        return;    // Exit if no card is found
    }

    // Read and print the card UID
    Serial.print(F("Karten-UID: "));

String content = "";
for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) content += "0";
    content += String(mfrc522.uid.uidByte[i], HEX);
}
content.toUpperCase();

if (isAuthorized(content)) {
    accessGranted();
} else {
    accessDenied();
}

mfrc522.PICC_HaltA();      
mfrc522.PCD_StopCrypto1();
}
