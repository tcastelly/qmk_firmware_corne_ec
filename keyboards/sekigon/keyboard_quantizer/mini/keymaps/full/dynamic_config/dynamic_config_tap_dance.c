// copyright 2023 sekigon-gonnoc
// spdx-license-identifier: gpl-2.0-or-later

#include "quantum.h"
#include "dynamic_config_def.h"
#include "dynamic_config.h"
#include "process_tap_dance.h"
#include "dynamic_config_tap_dance.h"

bool is_hold_tapdance_disabled = false;

// Define a type for as many tap dance states as you need
typedef enum { TD_NONE, TD_UNKNOWN, TD_SINGLE_TAP, TD_SINGLE_HOLD, TD_DOUBLE_TAP, TD_DOUBLE_HOLD } td_state_t;

typedef struct {
    td_state_t          state;
    dtap_dance_t const *p_td;
    uint8_t             kc_remap_col;
} td_tap_t;

typedef struct {
    td_tap_t *td_tap;
    bool      start;
    uint16_t  kc;
} td_deffered_event_t;

tap_dance_action_t tap_dance_actions[TAPDANCE_LEN_MAX] = {}
//     [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
;

static td_tap_t    td_tap[TAPDANCE_LEN_MAX];
#define DEFFERED_EVENT_CNT 16
static td_deffered_event_t deffered_event[DEFFERED_EVENT_CNT];
static uint8_t             widx = 0;
static uint8_t             ridx = 0;

static td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) {
            return TD_SINGLE_TAP;
        }
        else {
            return TD_SINGLE_HOLD;
        }
    } else if (state->count == 2) {
        if (state->interrupted || !state->pressed) {
            return TD_DOUBLE_TAP;
        }
        else {
            return TD_DOUBLE_HOLD;
        }
    }

    return TD_DOUBLE_TAP;
}

static void push_deffered_event(uint16_t kc, bool pressed, td_tap_t *td_tap) {
    deffered_event[widx].td_tap = td_tap;
    deffered_event[widx].start  = pressed;
    deffered_event[widx].kc     = kc;
    widx                        = (widx + 1) & (DEFFERED_EVENT_CNT - 1);
}

static void td_finished(tap_dance_state_t *state, void *user_data) {
    td_tap_t *data = user_data;
    data->state    = cur_dance(state);
    uint16_t kc    = 0;

    switch (data->state) {
        case TD_SINGLE_TAP:
            kc = data->p_td->single_tap;
            break;
        case TD_SINGLE_HOLD:
            kc = data->p_td->single_hold;
            kc = kc ? kc : data->p_td->single_tap;
            if (kc != KC_NO) {
                data->kc_remap_col = dynamic_config_register_code(kc);
                return;
            }
            break;
        case TD_DOUBLE_TAP:
            kc = data->p_td->double_tap;
            if (kc == KC_NO) {
                kc = data->p_td->single_tap;
                if (kc != KC_NO) {
                    push_deffered_event(kc, true, data);
                    push_deffered_event(kc, false, data);
                }
            }
            break;
        case TD_DOUBLE_HOLD:
            kc = data->p_td->double_hold;
            kc = kc ? kc : data->p_td->double_tap;
            kc = kc ? kc : data->p_td->single_hold;
            kc = kc ? kc : data->p_td->single_tap;
            if (kc != KC_NO) {
                data->kc_remap_col = dynamic_config_register_code(kc);
                return;
            }
            break;
        default:
            break;
    }

    if (kc != KC_NO) {
        push_deffered_event(kc, true, data);
    }
}

static void td_reset(tap_dance_state_t *state, void *user_data) {
    td_tap_t *data = user_data;
    uint16_t  kc   = 0;

    switch (data->state) {
        case TD_SINGLE_TAP:
            kc = data->p_td->single_tap;
            break;
        case TD_SINGLE_HOLD:
            kc = data->p_td->single_hold;
            kc = kc ? kc : data->p_td->single_tap;
            if (kc != KC_NO && data->kc_remap_col != 0xff) {
                dynamic_config_unregister_code_col(data->kc_remap_col);
                data->kc_remap_col = 0xff;
                return;
            }
            break;
        case TD_DOUBLE_TAP:
            kc = data->p_td->double_tap;
            if (kc == KC_NO) {
                kc = data->p_td->single_tap;
                if (kc != KC_NO) {
                    push_deffered_event(kc, false, data);
                }
            }
            break;
        case TD_DOUBLE_HOLD:
            kc = data->p_td->double_hold;
            kc = kc ? kc : data->p_td->double_tap;
            kc = kc ? kc : data->p_td->single_hold;
            kc = kc ? kc : data->p_td->single_tap;
            if (kc != KC_NO && data->kc_remap_col != 0xff) {
                dynamic_config_unregister_code_col(data->kc_remap_col);
                data->kc_remap_col = 0xff;
                return;
            }
            break;
        default:
            break;
    }

    if (kc != KC_NO) {
        push_deffered_event(kc, false, data);
    }

    data->state = TD_NONE;
}

void activate_tap_dances(void) {
    uint32_t td_cnt = 0;
    for (int td = 0; td < MIN(p_config->tap_dance_len, TAPDANCE_LEN_MAX); td++) {
        reset_tap_dance(&tap_dance_actions[td_cnt].state);
        td_tap[td_cnt].state                = TD_NONE;
        td_tap[td_cnt].kc_remap_col         = 0xff;
        tap_dance_actions[td_cnt]           = (tap_dance_action_t)ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_finished, td_reset);
        // tap_dance_actions[td_cnt]           = (tap_dance_action_t)ACTION_TAP_DANCE_TAP_HOLD(p_config->p_tap_dance[td].single_tap, p_config->p_tap_dance[td].single_hold);
        td_tap[td_cnt].p_td                 = &p_config->p_tap_dance[td];
        tap_dance_actions[td_cnt].user_data = &td_tap[td_cnt];
        td_cnt++;
    }
}

void dynamic_tap_dance_task(void) {
    static bool avoid_recurse = false;
    if (!avoid_recurse) {
        avoid_recurse = true;

        while (widx != ridx) {
            uint16_t keycode        = deffered_event[ridx].kc;
            bool     pressed        = deffered_event[ridx].start;
            deffered_event[ridx].kc = KC_NO;
            if (pressed) {
                deffered_event[ridx].td_tap->kc_remap_col = dynamic_config_register_code(keycode);
            } else {
                if (deffered_event[ridx].td_tap->kc_remap_col != 0xff) {
                    dynamic_config_unregister_code_col(deffered_event[ridx].td_tap->kc_remap_col);
                    deffered_event[ridx].td_tap->kc_remap_col = 0xff;
                }
            }

            // update read index
            ridx = (ridx + 1) & (DEFFERED_EVENT_CNT - 1);
        }

        avoid_recurse = false;
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
            && !is_hold_tapdance_disabled
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

// allow call multiple tap dance simultaneously
// e.g: TD_DEL/TD_DEL_OSX
void tap_dance_tap_hold_finished_unprotected(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

// START tap-hold
void tap_dance_tap_hold_finished_layout(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    is_hold_tapdance_disabled = true;

    if (state->pressed) {
        layer_on(tap_hold->hold);
    }
}

void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;
}
// END tap-hold
