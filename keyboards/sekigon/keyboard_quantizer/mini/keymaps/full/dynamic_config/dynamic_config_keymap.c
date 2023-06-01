// copyright 2023 sekigon-gonnoc
// spdx-license-identifier: gpl-2.0-or-later

#include "dynamic_config.h"
#include "dynamic_config_def.h"
#include "quantum.h"

// use kc_no remap for action injection
static uint16_t kc_no_remap = KC_NO;

void set_kc_no_remap(uint16_t kc) { kc_no_remap = kc; }

uint16_t dynamic_config_keymap_keycode_to_keycode(uint8_t layer, uint16_t keycode) {
    if (layer >= MAX_LAYER) return keycode;

    // Search for the keymap starting from the bottom of the configuration
    // settings. Settings written in the bottom takes priority
    for (int app = (int16_t)*p_active_app_cnt - 1; app >= 0; app--) {
        if (p_active_apps[app] >= p_config->app_len) break;

        const application_t *p_app      = &p_config->p_app[p_active_apps[app]];
        int                  keymap_len = p_app->keymap_len;
        for (int km = 0; km < keymap_len; km++) {
            if (p_app->p_keymap[km].layer != layer) {
                continue;
            }

            const dkeymap_t *p_keymap = &p_app->p_keymap[km];
            int              keylen   = p_keymap->keys_len;
            for (int key = 0; key < keylen; key++) {
                if (p_keymap->p_map[key].from == keycode) {
                    return p_keymap->p_map[key].to;
                }
            }
        }
    }

    return layer == 0 ? keycode : KC_TRNS;
}

// override keymap_key_to_keycode
uint16_t keymap_key_to_keycode(uint8_t layer, keypos_t key) {
    if (key.row == 0 && key.col == 0) return kc_no_remap;

    return dynamic_config_keymap_keycode_to_keycode(layer, key.row * MATRIX_COLS + key.col);
}


#if !defined(TAPPING_TERM_PER_KEY)
#    error "Option not satisfied"
#endif
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    for (int idx = 0; idx < p_config->per_key_option_len; idx++) {
        if (p_config->p_per_key_option[idx].key == keycode) {
            return p_config->p_per_key_option[idx].tapping_term;
        }
    }

    return p_config->default_values.tapping_term;
}


#if !defined(QUICK_TAP_TERM_PER_KEY)
#    error "Option not satisfied"
#endif
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    for (int idx = 0; idx < p_config->per_key_option_len; idx++) {
        if (p_config->p_per_key_option[idx].key == keycode) {
            return p_config->p_per_key_option[idx].quick_tap_term;
        }
    }

    return p_config->default_values.quick_tap_term;
}


#if !defined(PERMISSIVE_HOLD_PER_KEY)
#    error "Option not satisfied"
#endif
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    for (int idx = 0; idx < p_config->per_key_option_len; idx++) {
        if (p_config->p_per_key_option[idx].key == keycode) {
            return p_config->p_per_key_option[idx].permissive_hold;
        }
    }

    return p_config->default_values.permissive_hold;
}


#if !defined(HOLD_ON_OTHER_KEY_PRESS_PER_KEY)
#    error "Option not satisfied"
#endif
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    for (int idx = 0; idx < p_config->per_key_option_len; idx++) {
        if (p_config->p_per_key_option[idx].key == keycode) {
            return p_config->p_per_key_option[idx].hold_on_other_key_press;
        }
    }

    return p_config->default_values.hold_on_other_key_press;
}

#if !defined(RETRO_TAPPING_PER_KEY)
#    error "Option not satisfied"
#endif
bool get_retro_tapping(uint16_t keycode, keyrecord_t *record) {
    for (int idx = 0; idx < p_config->per_key_option_len; idx++) {
        if (p_config->p_per_key_option[idx].key == keycode) {
            return p_config->p_per_key_option[idx].retro_tapping;
        }
    }

    return p_config->default_values.retro_tapping;
}

#if !defined(LAYER_STATE_16BIT)
#    error "Option not satisfied"
#endif