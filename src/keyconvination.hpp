#ifndef _KEYCONVINATION_HPP_
#define _KEYCONVINATION_HPP_

#include <Arduino.h>
#include <USBHIDKeyboard.h>
#include <M5Unified.h>

enum class Special_key {
    None,
    Shift,
    Ctrl,
    Alt,
    CtrlShift,
    ShiftAlt,
    CtrlAlt
};



void key_conv1(USBHIDKeyboard *usb_keyboard, Special_key sp_key, uint8_t keycord) {
    switch (sp_key) {
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
    usb_keyboard->write(keycord);
    usb_keyboard->releaseAll();
}



#endif