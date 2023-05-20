// copyright 2023 sekigon-gonnoc
// spdx-license-identifier: gpl-2.0-or-later

#include "ascii_to_keycode_lut.h"
#include "dynamic_config_macro.h"
#include "dynamic_config.h"
#include "dynamic_config_def.h"

static uint16_t    active_macro_step = 0;
static bool        macro_abort       = false;
static const char *p_macro_string    = NULL;

void set_macro_string(const char *str) { p_macro_string = str; }

bool process_record_dynamic_config_macro(uint16_t     keycode,
                                         keyrecord_t *record) {
    if (record->event.pressed) {
        if (keycode >= QK_UNICODE && keycode <= QK_UNICODEMAP_MAX) {
            uint8_t id         = keycode - QK_UNICODE;
            uint8_t macro_type = p_config->p_macro[id].macro->type;
            if (macro_type == MACRO_TYPE_MACRO) {
                p_macro_string    = p_config->p_macro[id].macro->str;
                macro_abort       = false;
                active_macro_step = 0;
            } else if (macro_type == MACRO_TYPE_COMMAND) {
                printf("command: %s\n", p_config->p_macro[id].macro->str);
            } else if (macro_type == MACRO_TYPE_UNICODE_STRING) {
                printf("send_string_u: %s\n", p_config->p_macro[id].macro->str);
            } else {
                printf("unknown macro\n");
            }
            return false;
        } else if (p_macro_string != NULL && keycode == KC_ESC) {
            macro_abort = true;
            return false;
        }
    }

    return true;
}

void process_macro_send_char(void) {
    if (p_macro_string == NULL) return;

    if (macro_abort) {
        p_macro_string    = NULL;
        active_macro_step = 0;
        clear_keyboard();
        macro_abort = false;
    }

    char data[4] = {0, 0, 0, 0};
    data[0]      = p_macro_string[active_macro_step++];
    data[1]      = 0;
    if (data[0] == 0) {
        p_macro_string    = NULL;
        active_macro_step = 0;
        return;
    }

    if (data[0] == SS_TAP_CODE || data[0] == SS_DOWN_CODE ||
        data[0] == SS_UP_CODE) {
        data[1] = data[0];
        data[0] = SS_QMK_PREFIX;
        data[2] = p_macro_string[active_macro_step++];
        if (data[2] == 0) {
            p_macro_string    = NULL;
            active_macro_step = 0;
            return;
        }
    } else if (data[0] == SS_DELAY_CODE) {
        data[1] = p_macro_string[active_macro_step++];
        data[2] = p_macro_string[active_macro_step++];

        if (data[1] == 0 || data[2] == 0) {
            p_macro_string    = NULL;
            active_macro_step = 0;
            return;
        }

        wait_ms(((((uint32_t)data[2]) << 8) | data[1]));
        return;
    }

    send_string(data);
}