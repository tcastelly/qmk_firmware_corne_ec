// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "full.h"

#include "virtser.h"
#include "cli.h"
#include "dynamic_config.h"
#include "quantizer_mouse.h"

user_config_t user_config;
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {{{KC_NO}}};

// To compile keymap_introspection.c, declear combo here
extern combo_t key_combos[COMBO_LEN_MAX];

int8_t virtser_send_wrap(uint8_t c) {
    virtser_send(c);
    return 0;
}

void keyboard_pre_init_user(void) {
    cli_init();
    print_set_sendchar(virtser_send_wrap);
    dynamic_config_init();
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    set_mouse_gesture_threshold(get_mouse_gesture_threshold());
}

void housekeeping_task_user(void) {
    dynamic_config_task();
    cli_exec();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool cont = process_record_dynamic_config(keycode, record) //
                && process_record_mouse(keycode, record);

    if (record->event.pressed) {
        switch (keycode) {
            case QK_KB_0 ... QK_KB_3:
                // Used for mouse gesture
                break;
            case QK_KB_4:
                send_bootstrap_macro();
                break;
            case QK_KB_5:
                set_keyboard_language(LANG_US);
                user_config.keyboard_lang_us_or_jp = 0;
                eeconfig_update_user(user_config.raw);
                break;
            case QK_KB_6:
                set_keyboard_language(LANG_JP);
                user_config.keyboard_lang_us_or_jp = 1;
                eeconfig_update_user(user_config.raw);
                break;
            case QK_KB_7:
                set_os_language(LANG_US);
                user_config.os_lang_us_or_jp = 0;
                eeconfig_update_user(user_config.raw);
                break;
            case QK_KB_8:
                set_os_language(LANG_JP);
                user_config.os_lang_us_or_jp = 1;
                eeconfig_update_user(user_config.raw);
                break;
        }
    }

    return cont;
}

void post_process_record_user(uint16_t keycode, keyrecord_t* record) {
    post_process_record_mouse(keycode, record);
}

uint16_t keymap_to_keycode_hook(uint16_t keycode) {
    return keymap_to_keycode_hook_mouse(keycode);
}

void matrix_scan_user(void) {
    matrix_scan_user_hook_mouse();
}

 void eeconfig_init_user(void) {
    user_config.keyboard_lang_us_or_jp = get_keyboard_language() == LANG_JP ? 1 : 0;
    user_config.os_lang_us_or_jp       = get_os_language() == LANG_JP ? 1 : 0;

    eeconfig_update_user(user_config.raw);
 }