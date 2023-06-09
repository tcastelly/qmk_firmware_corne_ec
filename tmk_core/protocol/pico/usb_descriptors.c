/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Modifications for QMK and RP2040 by sekigon-gonnoc
 */

#include "tusb.h"
#include "report.h"
#include "usb_descriptors.h"
#include "util.h"

#ifndef SERIAL_NUMBER
#define SERIAL_NUMBER 123456
#endif

/* A combination of interfaces must have a unique product id, since PC will save
 * device driver after the first plug. Same VID/PID with different interface e.g
 * MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID                                                        \
    (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
     _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
    .bLength         = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB          = 0x0200,

    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and
    // protocol must be IAD (1)
    .bDeviceClass    = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor  = VENDOR_ID,
    .idProduct = PRODUCT_ID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct      = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#ifdef MIDI_ENABLE
#    define MIDI_DESC_LEN TUD_MIDI_DESC_LEN
#else
#    define MIDI_DESC_LEN 0
#endif

#define CONFIG_TOTAL_LEN                                             \
    (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_DESC_LEN * 4 + \
     TUD_HID_INOUT_DESC_LEN + MIDI_DESC_LEN)

#define EPNUM_HID_KEYBOARD_IN 0x81
#define EPNUM_HID_MOUSE_IN 0x82
#define EPNUM_HID_EXTRA_IN 0x83
#define EPNUM_HID_RAW_IN 0x84
#define EPNUM_HID_RAW_OUT 0x04
#define EPNUM_HID_CONSOLE_IN 0x85

#define EPNUM_CDC_NOTIF 0x86
#define EPNUM_CDC_OUT 0x07
#define EPNUM_CDC_IN 0x87

#ifdef MIDI_ENABLE
#    define EPNUM_MIDI_IN 0x88
#    define EPNUM_MIDI_OUT 0x08
#endif

static uint8_t const desc_keyboard[] = {TUD_HID_REPORT_DESC_KEYBOARD()};

static uint8_t const desc_mouse[] = {TUD_HID_REPORT_DESC_MOUSE()};

static uint8_t const desc_extra[] = {
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER)),
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL(HID_REPORT_ID(REPORT_ID_SYSTEM)),
};

static uint8_t const desc_raw[] = {HID_REPORT_DESC_RAW()};

static uint8_t const desc_console[] = {HID_REPORT_DESC_CONSOLE()};

uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf) {
    switch (itf) {
        case ITF_NUM_HID_KEYBOARD:
            return desc_keyboard;
        case ITF_NUM_HID_MOUSE:
            return desc_mouse;
        case ITF_NUM_HID_EXTRA:
            return desc_extra;
        case ITF_NUM_HID_RAW:
            return desc_raw;
        case ITF_NUM_HID_CONSOLE:
            return desc_console;
        default:
            return NULL;
    }
}

uint8_t const desc_fs_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, protocol, report descriptor len, EP In
    // address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KEYBOARD, 2, HID_ITF_PROTOCOL_KEYBOARD,
                       sizeof(desc_keyboard), EPNUM_HID_KEYBOARD_IN,
                       CFG_TUD_HID_EP_BUFSIZE, 10),
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID_RAW, 2, HID_ITF_PROTOCOL_NONE,
                             sizeof(desc_raw), EPNUM_HID_RAW_OUT,
                             EPNUM_HID_RAW_IN, CFG_TUD_HID_EP_BUFSIZE, 1),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_MOUSE, 2, HID_ITF_PROTOCOL_MOUSE,
                       sizeof(desc_mouse), EPNUM_HID_MOUSE_IN,
                       CFG_TUD_HID_EP_BUFSIZE, 1),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_EXTRA, 2, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_extra), EPNUM_HID_EXTRA_IN,
                       CFG_TUD_HID_EP_BUFSIZE, 10),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_CONSOLE, 2, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_console), EPNUM_HID_CONSOLE_IN,
                       CFG_TUD_HID_EP_BUFSIZE, 10),

    // Interface number, string index, EP notification address and size, EP data
    // address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 2, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT,
                       EPNUM_CDC_IN, 64),

#ifdef MIDI_ENABLE
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 2, EPNUM_MIDI_OUT, EPNUM_MIDI_IN, 64)
#endif
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // for multiple configurations

    return desc_fs_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    STR(MANUFACTURER),           // 1: Manufacturer
    STR(PRODUCT),                // 2: Product
    STR(SERIAL_NUMBER),          // 3: Serials, should use chip ID
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}
