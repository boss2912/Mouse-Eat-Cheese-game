#include <Wire.h>
#include <Keypad.h>

// กำหนด Keypad 4x4
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // ต่อขา Row
byte colPins[COLS] = {5, 4, 3, 2}; // ต่อขา Column

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Wire.begin();   // เริ่ม Master
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();  // อ่านค่าปุ่มกด

  if (key) { // ถ้ามีการกดจริง
    Serial.print("กดปุ่ม: ");
    Serial.println(key);

    if (key == '1') {
      // ส่งคำสั่งไปยัง Slave #8 ให้เปิด LED
      Wire.beginTransmission(8);
      Wire.write("LED_ON");
      Wire.endTransmission();
    }
    else if (key == '0') {
      // ส่งคำสั่งไปยัง Slave #8 ให้ปิด LED
      Wire.beginTransmission(8);
      Wire.write("LED_OFF");
      Wire.endTransmission();
    }
    else if (key >= '0' && key <= '9') {
      // กดตัวเลข 0–9 → ส่งคำสั่งให้ Motor สั่น
      Wire.beginTransmission(8);
      Wire.write("Motor_on");
      Wire.endTransmission();
    }
  }
}