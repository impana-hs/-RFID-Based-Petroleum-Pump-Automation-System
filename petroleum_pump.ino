```cpp
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9
#define RELAY_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad Setup
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 6, 7};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Authorized RFID UID
byte authorizedUID[4] = {0x53, 0xA1, 0xB2, 0xC4};

float balance = 1000.0;
float fuelPrice = 100.0;

void setup() {
  Serial.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  lcd.setCursor(0,0);
  lcd.print("Petrol Pump");
  lcd.setCursor(0,1);
  lcd.print("Automation");
  delay(2000);
  lcd.clear();
}

void loop() {

  lcd.setCursor(0,0);
  lcd.print("Scan RFID Card");

  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

  bool authorized = true;

  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
      authorized = false;
    }
  }

  lcd.clear();

  if (authorized) {

    lcd.print("Access Granted");
    delay(1500);

    lcd.clear();
    lcd.print("Balance:");
    lcd.print(balance);
    delay(2000);

    lcd.clear();
    lcd.print("Fuel Qty(L):");

    String qtyString = "";

    while (true) {
      char key = keypad.getKey();

      if (key) {

        if (key >= '0' && key <= '9') {
          qtyString += key;
          lcd.setCursor(0,1);
          lcd.print(qtyString);
        }

        if (key == '#') {
          break;
        }
      }
    }

    int litres = qtyString.toInt();
    float amount = litres * fuelPrice;

    lcd.clear();

    if (amount <= balance) {

      balance -= amount;

      lcd.print("Dispensing...");
      digitalWrite(RELAY_PIN, LOW);

      delay(litres * 1000);

      digitalWrite(RELAY_PIN, HIGH);

      lcd.clear();
      lcd.print("Fuel Delivered");
      delay(2000);

      lcd.clear();
      lcd.print("Bal:");
      lcd.print(balance);
      delay(3000);

    } else {

      lcd.print("Low Balance");
      delay(3000);
    }

  } else {

    lcd.print("Access Denied");
    delay(3000);
  }

  lcd.clear();
}
```
