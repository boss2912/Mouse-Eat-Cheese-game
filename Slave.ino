#include <Wire.h>

int ledPin = 13;          // LED ต่อขา 13
int motorPin = 9;         // Motor ต่อขา 9
bool motorFlag = false;   // ใช้บอกว่าต้องสั่นมอเตอร์

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  Wire.begin(8);           // ตั้ง Address = 8
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
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
}
