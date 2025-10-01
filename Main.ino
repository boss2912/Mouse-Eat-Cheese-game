// --- 1) ไลบรารี ---
#include <UTFT.h>
#include <SD.h>
#include <Keypad.h>
#include <Wire.h>

// --- 2) ค่าคงที่จอและรูป ---
const int SCREEN_WIDTH  = 320;
const int SCREEN_HEIGHT = 480;
const int IMAGE_WIDTH   = 345;
const int IMAGE_HEIGHT  = 680;

// --- 3) จอ, SD, Keypad ---
UTFT myGLCD(ILI9486, 38, 39, 40, 41);  // RS, WR, CS, RST (ตาม UTFT)
const int SD_CS = 53;                  // Arduino Mega: SD CS = 53

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};
byte rowPins[ROWS] = {A8, A9, A10, A11};
byte colPins[COLS] = {A12, A13, A14, A15};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// --- 4) ตัวแปรและประกาศฟังก์ชัน ---
bool isImageVisible = false;
int rangeMax = 7;  // เริ่มต้นช่วงสุ่ม 1..7

void displayCheeseImage();
void clearImageArea();
void bmpDraw(const char* filename, int x, int y);
uint16_t read16(File &f);
uint32_t read32(File &f);

void setup() {
  Wire.begin();

  myGLCD.InitLCD();
  myGLCD.clrScr();

  // แนะนำบน Mega: คง SPI เป็นโหมด Master
  pinMode(10, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);

  // seed การสุ่ม (ถ้า A0 ถูกใช้งาน ให้เปลี่ยนเป็น randomSeed(micros());)
  randomSeed(analogRead(A0));

  if (!SD.begin(SD_CS)) {
    myGLCD.setColor(255, 0, 0);
    myGLCD.print(F("SD Card init failed!"), CENTER, 150);
    while (true);
  }

  displayCheeseImage();
  isImageVisible = true;
  Serial1.println("CHEESE_DISPLAYED");

  // แจ้งช่วงสุ่มเริ่มต้นให้ฝั่ง LCD (ทางเลือก)
  Serial1.print("RANGE_SET:");
  Serial1.println(rangeMax);
}

void loop() {
 // ส่งไปที่ Slave #1
  Wire.beginTransmission(1);
  Wire.write("Hello 1");
  Wire.endTransmission();
  delay(500);

  // ส่งไปที่ Slave #2
  Wire.beginTransmission(2);
  Wire.write("Hello 2");
  Wire.endTransmission();
  delay(500);

  //  ส่งไปที่ Slave #3
  Wire.beginTransmission(3);
  Wire.write("Hello 3");
  Wire.endTransmission();
  delay(500);
  
  char customKey = customKeypad.getKey();
  if (customKey) {
    if (customKey == 'A' && isImageVisible == true) {
      clearImageArea();
      isImageVisible = false;
      Serial1.println("CHEESE_HIDDEN"); 
    }
    else if (customKey == 'B' && isImageVisible == false) {
      displayCheeseImage();
      isImageVisible = true;
      Serial1.println("CHEESE_DISPLAYED");
    }
    // ปุ่มตัวเลข 1..9 = ตั้งช่วงสุ่ม 1..rangeMax
    else if (customKey >= '1' && customKey <= '9') {
      rangeMax = customKey - '0';  // '1'..'9' -> 1..9
      Serial1.print("RANGE_SET:");
      Serial1.println(rangeMax);
      // หากอยากแสดงบนจอ TFT ด้วย:
      // myGLCD.setColor(255, 255, 255);
      // myGLCD.print("Range 1-", 0, 0);
      // myGLCD.printNumI(rangeMax, 80, 0);
    }
    // ปุ่ม D = สุ่ม 1..rangeMax แล้วส่งผล
    else if (customKey == 'D') {
      int n = random(1, rangeMax + 1); // 1..rangeMax
      Serial1.print("RANDOM:");
      Serial1.println(n);              // ลงท้าย \n (println)
      // หากอยากแสดงผลสุ่มบน TFT ด้วย:
      // myGLCD.setColor(255, 255, 0);
      // myGLCD.print("Random:", 0, 20);
      // myGLCD.printNumI(n, 70, 20);
    }
  }
}

void displayCheeseImage() {
  int x = (SCREEN_WIDTH - IMAGE_WIDTH) / 2;
  int y = (SCREEN_HEIGHT - IMAGE_HEIGHT) / 2;
  bmpDraw("cheese.bmp", x, y);
}

void clearImageArea() {
  myGLCD.clrScr();
}

#define BUFFPIXEL 20
void bmpDraw(const char* filename, int x, int y) {
  File bmpFile; int bmpWidth, bmpHeight; uint8_t bmpDepth; uint32_t bmpImageoffset; uint32_t rowSize;
  uint8_t sdbuffer[3*BUFFPIXEL]; uint8_t buffidx = sizeof(sdbuffer); boolean goodBmp = false;
  boolean flip = true; int w, h, row, col; uint8_t r, g, b;
  if ((x >= myGLCD.getDisplayXSize()) || (y >= myGLCD.getDisplayYSize())) return;
  if ((bmpFile = SD.open(filename)) == NULL) { myGLCD.setColor(255, 255, 0); myGLCD.print("File not found!", CENTER, 150); return; }
  if (read16(bmpFile) == 0x4D42) {
    read32(bmpFile); (void)read32(bmpFile); bmpImageoffset = read32(bmpFile); read32(bmpFile);
    bmpWidth = read32(bmpFile); bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) {
      bmpDepth = read16(bmpFile);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {
        goodBmp = true; rowSize = (bmpWidth * 3 + 3) & ~3;
        if (bmpHeight < 0) { bmpHeight = -bmpHeight; flip = false; }
        w = bmpWidth; h = bmpHeight;
        if ((x + w - 1) >= myGLCD.getDisplayXSize()) w = myGLCD.getDisplayXSize() - x;
        if ((y + h - 1) >= myGLCD.getDisplayYSize()) h = myGLCD.getDisplayYSize() - y;
        for (row = 0; row < h; row++) {
          if (flip) { uint32_t pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize; if (bmpFile.position() != pos) bmpFile.seek(pos); buffidx = sizeof(sdbuffer); }
          for (col = 0; col < w; col++) {
            if (buffidx >= sizeof(sdbuffer)) { bmpFile.read(sdbuffer, sizeof(sdbuffer)); buffidx = 0; }
            b = sdbuffer[buffidx++]; g = sdbuffer[buffidx++]; r = sdbuffer[buffidx++];
            myGLCD.setColor(r, g, b); myGLCD.drawPixel(x + col, y + row);
          }
        }
      }
    }
  }
  bmpFile.close();
  if (!goodBmp) { myGLCD.setColor(255, 0, 0); myGLCD.print("BMP format error", CENTER, 150); }
}
uint16_t read16(File &f){ uint16_t r; ((uint8_t *)&r)[0]=f.read(); ((uint8_t *)&r)[1]=f.read(); return r; }
uint32_t read32(File &f){ uint32_t r; ((uint8_t *)&r)[0]=f.read(); ((uint8_t *)&r)[1]=f.read(); ((uint8_t *)&r)[2]=f.read(); ((uint8_t *)&r)[3]=f.read(); return r; }
