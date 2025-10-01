#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // ถ้าไม่ขึ้น ลองเปลี่ยนเป็น 0x3F

int ledPin = 13;          // LED ต่อขา 13
int motorPin = 9;         // Motor ต่อขา 9
bool motorFlag = false;   // ใช้บอกว่าต้องสั่นมอเตอร์

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  Wire.begin(1);           // ตั้ง Address = 1
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready...");
}

void loop() {
  // ถ้ามีคำสั่งให้สั่น
  if (motorFlag) {
    digitalWrite(motorPin, HIGH);  // เปิดมอเตอร์
    delay(500);                     // สั่น 0.5 วินาที
    digitalWrite(motorPin, LOW);    // ปิดมอเตอร์
    motorFlag = false;              // รีเซ็ต flag
  }
}

// ฟังก์ชันรับข้อมูลจาก Master
void receiveEvent(int howMany) {
  String command = "";
  while (Wire.available()) {
    char c = Wire.read();
    command += c;
  }

  Serial.print("รับคำสั่ง: ");
  Serial.println(command);

  if (command == "LED_ON") {
    digitalWrite(ledPin, HIGH);
    motorFlag = true;
  } 
  else if (command == "LED_OFF") {
    digitalWrite(ledPin, LOW);
    motorFlag = true;
  }
  else if (command == "Motor_on") {
    motorFlag = true;  // ตั้ง flag ให้ loop() เปิดมอเตอร์
  }

  if (Serial1.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    if (message == "CHEESE_DISPLAYED") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Safe Cheese");
    } 
    else if (message == "CHEESE_HIDDEN") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CHEESE is missing");
    }
    else if (message.startsWith("RANDOM:")) {
      String sNum = message.substring(7);
      sNum.trim();
      int n = sNum.toInt();
      if (n >= 1 && n <= 9) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Random result:");
        lcd.setCursor(0, 1);
        lcd.print(n);
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RANDOM invalid");
      }
    }
    else if (message.startsWith("RANGE_SET:")) {
      String sMax = message.substring(10);
      sMax.trim();
      int m = sMax.toInt();
      if (m >= 1 && m <= 9) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Range: 1-");
        lcd.print(m);
      }
    }
}
