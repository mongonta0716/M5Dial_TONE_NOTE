#ifndef _KEYCONVINATION_HPP_
#define _KEYCONVINATION_HPP_

#include <Arduino.h>
#include <USBHIDKeyboard.h>
#include <M5Unified.h>


enum Special_key {
    None      = 0b00000000,
    Shift     = 0b00000001,
    Ctrl      = 0b00000010,
    Alt       = 0b00000100,
    CtrlShift = 0b00001000,
    ShiftAlt  = 0b00010000,
    CtrlAlt   = 0b00100000
};

#define SHIFT(kc) (Special_key::Shift << 8 | (kc))
#define ALT(kc) (Special_key::Alt << 8 | (kc))
#define CTRL(kc) (Special_key::Shift << 8 | (kc))


void key_conv1(USBHIDKeyboard *usb_keyboard, uint8_t sp_key, uint8_t keycord) {
    switch (sp_key) {
        case Special_key::None:
            break;
        case Special_key::Shift:
            usb_keyboard->press(KEY_LEFT_SHIFT);
            break;
        case Special_key::Ctrl:
            usb_keyboard->press(KEY_LEFT_CTRL);
            break;
        case Special_key::Alt:
            usb_keyboard->press(KEY_LEFT_ALT);
            break;
        case Special_key::CtrlShift:
            usb_keyboard->press(KEY_LEFT_CTRL);
            usb_keyboard->press(KEY_LEFT_SHIFT);
            break;
        case Special_key::ShiftAlt:
            usb_keyboard->press(KEY_LEFT_SHIFT);
            usb_keyboard->press(KEY_LEFT_ALT);
            break;
        case Special_key::CtrlAlt:
            usb_keyboard->press(KEY_LEFT_CTRL);
            usb_keyboard->press(KEY_LEFT_ALT);
            break;
        default:
            M5_LOGE("Invalid keycode: %d\n", sp_key);
            break;
    }
    usb_keyboard->press(keycord);
    usb_keyboard->releaseAll();
}

void key_conv(USBHIDKeyboard *usb_keyvoard, uint16_t keycode) {
    key_conv1(usb_keyvoard, (keycode >> 8), keycode & 0x00FF);
}


#endif