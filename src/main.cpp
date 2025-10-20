#include <Arduino.h>
#include <LiquidCrystal.h>

// ========================================
// 💡 Arduino LCD Transmitter – Refined Version
// ========================================

// --- Constants ---
constexpr uint8_t TX_RATE = 20; // bits per second
constexpr uint8_t TX_DATA = 3;
constexpr uint8_t BUTTON_PIN = 2;
constexpr uint8_t LCD_D4 = 4;
constexpr uint8_t LCD_D5 = 5;
constexpr uint8_t LCD_D6 = 6;
constexpr uint8_t LCD_D7 = 7;
constexpr uint8_t LCD_RS = 8;
constexpr uint8_t LCD_EN = 9;
constexpr uint8_t LCD_SIZE = 16;
constexpr uint8_t LCD_LINES = 2;

// --- Message ---
const char *message =
    "je suis geek et j'aime programmer des microcontroleurs Arduino pour "
    "transmettre des donnees de facon professionnelle";
// Split message by group of 16 characters

// --- LCD Object ---
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// --- Function Declarations ---
void initHardware();
void transmitMessage(const char *msg);
void transmitMessageGroup(const char *msg, size_t len);
void transmitByte(char c, int index);
void clearLine(uint8_t line);

// ========================================
// Setup
// ========================================
void setup() {
  initHardware();

  // Display message and status

  lcd.setCursor(0, 1);
  lcd.print("TX Demo Ready");
}

// ========================================
// Main Loop
// ========================================
void loop() {
  // Wait for button press (active LOW)
  if (digitalRead(BUTTON_PIN) == LOW) {
    lcd.setCursor(0, 0);
    size_t msg_len = strlen(message);
    uint8_t group_count = (msg_len + LCD_SIZE - 1) /
                          LCD_SIZE; // Ceiling division.  (n + d - 1) / d is a
                                    // classic ceiling division trick:

    for (uint8_t i = 0; i < group_count; i++) {
      // Calculate group start and length
      size_t start = i * LCD_SIZE;
      size_t group_len = min((size_t)LCD_SIZE, msg_len - start);

      // Display group (truncated to fit LCD)
      lcd.setCursor(0, 0);
      for (size_t j = 0; j < group_len && j < 16; j++) {
        lcd.print(message[start + j]);
      }

      // Show progress
      clearLine(1);
      lcd.setCursor(0, 1);
      lcd.print("TX ");
      lcd.print(i + 1);
      lcd.print("/");
      lcd.print(group_count);
      lcd.print(" ");

      delay(500);
      // Transmit this group directly (no buffer needed)
      transmitMessageGroup(message + start, group_len);
    }
    clearLine(1);
    lcd.setCursor(0, 1);
    lcd.print("Done!");
    delay(1000);

    clearLine(0);
    clearLine(1);
    lcd.print("TX Demo Ready");
  }
}

// ========================================
// Initialize Hardware
// ========================================
void initHardware() {
  pinMode(TX_DATA, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin(LCD_SIZE, LCD_LINES);
  lcd.clear();
}

// ========================================
// Transmit Message Group (Memory Optimized)
// ========================================
void transmitMessageGroup(const char *msg, size_t len) {
  for (size_t i = 0; i < len; i++) {
    clearLine(1); // clear bit line before each byte
    transmitByte(msg[i], i);
  }
  digitalWrite(TX_DATA, LOW);
}

// ========================================
// Transmit Whole Message
// ========================================
void transmitMessage(const char *msg) {
  size_t msg_len = strlen(msg);

  for (size_t i = 0; i < msg_len; i++) {
    clearLine(1); // clear bit line before each byte
    transmitByte(msg[i], i);
  }

  digitalWrite(TX_DATA, LOW);
}

// ========================================
// Transmit One Byte
// ========================================
void transmitByte(char c, int index) {
  lcd.setCursor(index, 0);
  lcd.cursor();

  for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
    bool tx_bit = bitRead(c, 7 - bit_idx);
    digitalWrite(TX_DATA, tx_bit);

    // Show bit on line 2
    lcd.setCursor(bit_idx, 1);
    lcd.print(tx_bit ? '1' : '0');

    // Move cursor to show current char
    lcd.setCursor(index, 0);
    lcd.cursor();

    delay(1000 / TX_RATE);
  }

  lcd.noCursor();
  digitalWrite(TX_DATA, LOW);
}

// ========================================
// Utility: Clear a Specific LCD Line
// ========================================
void clearLine(uint8_t line) {
  lcd.setCursor(0, line);
  lcd.print("                "); // 16 spaces
  lcd.setCursor(0, line);
}
