// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "full.h"

#include "virtser.h"
#include "os_detection.h"
#include "cli.h"
#include "dynamic_config.h"
#include "quantizer_mouse.h"
#include "tapdance.c"

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
    static os_variant_t detected_os = OS_UNSURE;
    if (detected_os != detected_host_os()) {
        dynamic_config_activate_default_apps();
        detected_os = detected_host_os();
    }

    dynamic_config_task();
    cli_exec();
}

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    return pre_process_record_mouse(keycode, record);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool cont = process_record_dynamic_config(keycode, record) //
                && process_record_mouse(keycode, record);

    tap_dance_action_t *action;
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

        // start custom
        switch (keycode) {
            case QWERTY:
                if (record->event.pressed) {
                    layer_move(_QWERTY);
                }
                return false;
                break;

            case QWERTY_OSX:
                if (record->event.pressed) {
                    layer_move(_QWERTY_OSX);
                }
                return false;
                break;

            case QWERTY_GAMING:
                if (record->event.pressed) {
                    layer_move(_QWERTY_GAMING);
                }
                return false;
                break;

            case LOWER:
                if (record->event.pressed) {
                    is_hold_tapdance_disabled = true;
                    layer_on(_LOWER);
                    update_tri_layer(_LOWER, _RAISE, _ADJUST);
                } else {
                    layer_off(_LOWER);
                    update_tri_layer(_LOWER, _RAISE, _ADJUST);
                    is_hold_tapdance_disabled = false;
                }
                return false;
                break;

            case RAISE:
                if (record->event.pressed) {
                    is_hold_tapdance_disabled = true;
                    layer_on(_RAISE);
                    update_tri_layer(_LOWER, _RAISE, _ADJUST);
                } else {
                    layer_off(_RAISE);
                    update_tri_layer(_LOWER, _RAISE, _ADJUST);
                    is_hold_tapdance_disabled = false;
                }
                return false;
                break;

            case KC_LALT:
            case KC_LGUI:
            case KC_LSFT:
                if (record->event.pressed) {
                    is_hold_tapdance_disabled = true;
                } else {
                    is_hold_tapdance_disabled = false;
                }
                return true;
                break;

            case ACCENT_CIRCUM:
                if (record->event.pressed) {
                    register_code(KC_RALT);
                    register_code(KC_6);
                } else {
                    unregister_code(KC_6);
                    unregister_code(KC_RALT);
                }
                break;

            case ACCENT_TREMA:
                if (record->event.pressed) {
                    register_code(KC_RALT);
                    register_code(KC_LSFT);
                    register_code(KC_QUOT);
                } else {
                    unregister_code(KC_QUOT);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_RALT);
                }
                break;

            case ACCENT_GRAVE:
                if (record->event.pressed) {
                    register_code(KC_RALT);
                    register_code(KC_GRV);
                } else {
                    unregister_code(KC_GRV);
                    unregister_code(KC_RALT);
                }
                break;

            case ACCENT_E_GRAVE:
                if (record->event.pressed) {
                    register_code(KC_RALT);
                    register_code(KC_GRV);
                } else {
                    unregister_code(KC_GRV);
                    unregister_code(KC_RALT);
                    register_code(KC_E);
                    unregister_code(KC_E);
                }
                break;

                // to be used with RALT already pressed
            case ACCENT_A_GRAVE_RALT:
                if (record->event.pressed) {
                    register_code(KC_GRV);
                } else {
                    unregister_code(KC_GRV);
                    unregister_code(KC_RALT);
                    register_code(KC_A);
                    unregister_code(KC_A);

                    // will be unregister by `td_ralt_reset`
                    register_code(KC_RALT);
                }
                break;

            case ACCENT_I_CIRC_RALT:
                if (record->event.pressed) {
                    register_code(KC_6);
                } else {
                    unregister_code(KC_6);
                    unregister_code(KC_RALT);
                    register_code(KC_I);
                    unregister_code(KC_I);

                    // will be unregister by `td_ralt_reset`
                    register_code(KC_RALT);
                }
                break;

            case ACCENT_O_CIRC_RALT:
                if (record->event.pressed) {
                    register_code(KC_6);
                } else {
                    unregister_code(KC_6);
                    unregister_code(KC_RALT);
                    register_code(KC_O);
                    unregister_code(KC_O);

                    // will be unregister by `td_ralt_reset`
                    register_code(KC_RALT);
                }
                break;

            case ACCENT_U_AIGU_RALT:
                if (record->event.pressed) {
                    register_code(KC_GRV);
                } else {
                    unregister_code(KC_GRV);
                    unregister_code(KC_RALT);
                    register_code(KC_U);
                    unregister_code(KC_U);

                    // will be unregister by `td_ralt_reset`
                    register_code(KC_RALT);
                }
                break;

            case ACCENT_C_RALT:
                if (record->event.pressed) {
                    register_code(KC_COMM);
                } else {
                    unregister_code(KC_COMM);
                    unregister_code(KC_RALT);

                    // will be unregister by `td_ralt_reset`
                    register_code(KC_RALT);
                }
                break;

            case ACCENT_A_GRAVE:
                if (record->event.pressed) {
                    register_code(KC_RALT);
                    register_code(KC_GRV);
                } else {
                    unregister_code(KC_GRV);
                    unregister_code(KC_RALT);
                    register_code(KC_A);
                    unregister_code(KC_A);
                }
                break;

            case JET_RNM:
                if (record->event.pressed) {
                    register_code(KC_LSFT);
                    register_code(KC_F6);

                    unregister_code(KC_LSFT);
                    unregister_code(KC_F6);
                }
                return false;
                break;

            case JET_FIND:
                if (record->event.pressed) {
                    register_code(KC_LALT);
                    register_code(KC_F1);

                    unregister_code(KC_F1);
                    unregister_code(KC_LALT);
                    tap_code(KC_1);
                }
                return false;
                break;

            case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
            case TD(TD_ESC):
            case TD(TD_ESC_OSX):
            case TD(TD_TAB):
            case TD(TD_P):
            case TD(TD_L):
            case TD(TD_ENT):
            case TD(TD_SCLN):
            case TD(TD_BSPC):
            case TD(TD_BSPC_OSX):
            case TD(TD_DEL):
            case TD(TD_DEL_OSX):
            case TD(TD_LEFT):
            case TD(TD_LEFT_OSX):
            case TD(TD_RIGHT):
            case TD(TD_RIGHT_OSX):
                if ((keycode == TD(TD_ESC) || keycode == TD(TD_ESC_OSX)) && !record->event.pressed) {
                    layer_off(_ESC);
                    layer_off(_ESC_OSX);
                    is_hold_tapdance_disabled = false;
                }

                action = &tap_dance_actions[TD_INDEX(keycode)];
                if (!record->event.pressed && action->state.count && !action->state.finished) {
                    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                    tap_code16(tap_hold->tap);
                }
                break;
        }
    }

    return cont;
}

void post_process_record_user(uint16_t keycode, keyrecord_t* record) {
    post_process_record_mouse(keycode, record);
}

 void eeconfig_init_user(void) {
    user_config.keyboard_lang_us_or_jp = get_keyboard_language() == LANG_JP ? 1 : 0;
    user_config.os_lang_us_or_jp       = get_os_language() == LANG_JP ? 1 : 0;

    eeconfig_update_user(user_config.raw);
 }
