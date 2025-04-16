#include "Wire.h"
#include "U8g2lib.h"
#include <Wire.h>
#include "MFRC522_I2C.h"

#include "Tone32.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
String card = "";

MFRC522 mfrc522(0x28);

String readRFID() {
  String mfrc522ReadCode = "";
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {}
  else {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      mfrc522ReadCode += String(mfrc522.uid.uidByte[i], HEX);
    }
  }
  return mfrc522ReadCode;
}

void setup()
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  u8g2.setFont(u8g2_font_8x13_mf);
  u8g2.clearDisplay();
  Wire.begin();
  mfrc522.PCD_Init();

}


void loop()
{
  card = readRFID();
  if (card != "") {
    u8g2.firstPage();
    do {
      u8g2.setCursor(0, 0);
      u8g2.print(String(card).c_str());

      u8g2.sendBuffer();
    } while (u8g2.nextPage());
  }
  delay(100);
}
