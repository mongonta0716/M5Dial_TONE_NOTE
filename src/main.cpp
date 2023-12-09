#include <SPIFFS.h>
#include <M5Dial.h>

#include  "keyconvination.hpp"

#include "Wire.h"

#include "USB.h"
#include "USBHIDMouse.h"

USBHIDMouse Mouse;
USBHIDKeyboard keyboard;

#include <Avatar.h>
#include "formatString.hpp"

#include <Wire.h>

#define I2C_ADDR 0x55

using namespace m5avatar;

Avatar avatar;
float rotation = 0.0f;

int8_t layer_no = 0;

enum Layer {
  ADOBE_ILLUSTRATOR1,
  ADOBE_ILLUSTRATOR2,
  MUSIC,
  LAYER_END
};


uint16_t keycode_layout[LAYER_END][8] = { 
 { KEY_ESC, 'v', 'a', 'b', 'p', 't', SHIFT('c'), SHIFT('+') },
 { KEY_ESC, 'v', 'a', 'b', 'p', 't', SHIFT(HID_KEY_C), SHIFT(HID_KEY_KEYPAD_ADD) },
 { KEY_ESC, 'v', 'a', 'b', 'p', 't', SHIFT(HID_KEY_C), SHIFT(HID_KEY_KEYPAD_ADD) },
};

const char* jpgfilename[LAYER_END] = {
  "/KeyLayout1.jpg",
  "/KeyLayout2.jpg",
  "/KeyLayout3.jpg",
};
 

void changeImage(int8_t layer_no) {
  M5.Display.drawJpgFile(jpgfilename[layer_no]);
}

uint16_t convertKeyCode(uint8_t keycode) {
  uint8_t char_value;
  switch(keycode) {
    // TONE&NOTEからのキーコードを変換
    case 39:
      char_value = 0;
      rotation = 0.0f;
      break;
    case 30:
      char_value = 1;
      rotation = 90.0f;
      break;
    case 31:
      char_value = 2;
      rotation = 180.0f;
      break;
    case 32:
      char_value = 3;
      layer_no++;
      if (layer_no >= LAYER_END) layer_no = 0;
      changeImage(layer_no);
      return layer_no;
    case 33:
      char_value = 4;
      break;
    case 34:
      char_value = 5;
      break;
    case 35:
      char_value = 6;
      break;
    case 36:
      char_value = 7;
      break;
    case 37:
      char_value = 8;
      layer_no--;
      if (layer_no < 0) layer_no = 0;
      changeImage(layer_no);
      return layer_no;
    case 38:
      char_value = 9;
      layer_no++;
      if (layer_no >= LAYER_END) layer_no = LAYER_END - 1;
      changeImage(layer_no);
      return layer_no;
    default:
      char_value = 10;
      break;
  }
  uint16_t keyconvination = keycode_layout[layer_no][char_value];

  key_conv(&keyboard, keyconvination);
  //keyboard.press(keyconvination & 0x00FF);
  //keyboard.releaseAll();
  return keyconvination;
}

void onReceive(int len){
  uint8_t buff[20];
  M5_LOGI("onReceive[%d]: ", len);
  while(Wire.available()){
    Wire.readBytes(buff, len);
    M5.Display.setCursor(100, 100);
    for (int i=0; i< len; i++) { 
      M5.Log.printf("%d", buff[i]);
      if (buff[0] == 37) {
        rotation = rotation - 10.0f;
      } else if (buff[0] == 38) {
        rotation = rotation + 10.0f;
      }
      //avatar.setRotation(rotation);
      //char data = convertKeyCode(buff[0]);
      std::string s = formatString("%4x", convertKeyCode(buff[0]));
      //avatar.setSpeechText(s.c_str());
    }
  }
  M5.Log.println();
}


void setup()
{
    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);
    SPIFFS.begin();
    M5.Display.setRotation(1);
    Mouse.begin();
    USB.begin();
    keyboard.begin();
    //avatar.init();
    //avatar.setPosition(0, -40);
    M5.Display.setFileStorage(SPIFFS);
    changeImage(layer_no);
    M5.Speaker.begin();
    M5.Speaker.tone(2000, 500);
    delay(500);
    Wire.onReceive(onReceive);
    Wire.begin((uint8_t)I2C_ADDR, 15, 13, 400000U);
}

long oldPosition = -999;

int prev_x = -1;
int prev_y = -1;
static m5::touch_state_t prev_state;

int alpha_count = 0;

bool touched = false;
bool first_move = false;

void loop()
{
    M5Dial.update();
    auto t = M5Dial.Touch.getDetail();

    if (prev_state != t.state)
    {
        prev_state = t.state;
        static constexpr const char *state_name[16] = {
            "none", "touch", "touch_end", "touch_begin",
            "___", "hold", "hold_end", "hold_begin",
            "___", "flick", "flick_end", "flick_begin",
            "___", "drag", "drag_end", "drag_begin"};
        Serial.println(state_name[t.state]);
        if (t.state == m5::touch_state_t::touch)
        {
            touched = true;
            first_move = true;
            prev_x = t.x;
            prev_y = t.y;
            Serial.println("TOUCH X:" + String(t.x) + " / " + "Y:" + String(t.y));
        }
        if (t.state == m5::touch_state_t::touch_end)
        {
            Serial.println("LEFT CLICK!!!");
            Mouse.click(MOUSE_LEFT);
        }
        if (t.state == m5::touch_state_t::none)
        {
            touched = false;
        }
    }
    if (touched && (prev_x != t.x || prev_y != t.y))
    {
        int8_t dx = (int16_t)t.x - (int16_t)prev_x;
        int8_t dy = (int16_t)t.y - (int16_t)prev_y;

        if (first_move)
        {
            first_move = false;
            Serial.println("FIRST MOVE");
        }
        else
        {
            Serial.println("MOVE  X:" + String(t.x) + " / " + "Y:" + String(t.y) + " / " + "DX:" + String(dx) + " / " + "DY:" + String(dy));
            Mouse.move(dx * 5, dy * 5, 0, 0);
        }
        prev_x = t.x;
        prev_y = t.y;
    }

    if (M5Dial.BtnA.wasPressed())
    {
        Serial.println("RIGHT CLICK");
        Mouse.click(MOUSE_RIGHT);
    }

    long newPosition = M5Dial.Encoder.read();
    if (newPosition != oldPosition)
    {
        int8_t dw = newPosition - oldPosition;
        Serial.println("W:" + String(newPosition) + " / " + "DW:" + String(dw));
        oldPosition = newPosition;
        Mouse.move(0, 0, dw, 0);
    }
}