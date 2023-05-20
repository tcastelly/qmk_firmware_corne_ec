// copyright 2023 sekigon-gonnoc
// spdx-license-identifier: gpl-2.0-or-later

#include "quantum.h"
#include "dynamic_config_def.h"
#include "dynamic_config.h"
#include "process_tap_dance.h"

// Define a type for as many tap dance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD
} td_state_t;

typedef struct {
    td_state_t          state;
    dtap_dance_t const *p_td;
} td_tap_t;

typedef struct {
    td_state_t event;
    bool       start;
    uint16_t   kc;
} td_deffered_event_t;

tap_dance_action_t    tap_dance_actions[TAPDANCE_LEN_MAX] = {};
static td_tap_t       td_tap[TAPDANCE_LEN_MAX];
#define DEFFERED_EVENT_CNT 16
static td_deffered_event_t deffered_event[DEFFERED_EVENT_CNT];
static uint8_t             widx = 0;
static uint8_t             ridx = 0;

static td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed)
            return TD_SINGLE_TAP;
        else
            return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        if (!state->pressed)
            return TD_DOUBLE_TAP;
        else
            return TD_DOUBLE_HOLD;
    } else
        return TD_UNKNOWN;
}

static void push_deffered_event(uint16_t kc, bool pressed, td_state_t event) {
    deffered_event[widx].event = event;
    deffered_event[widx].start = pressed;
    deffered_event[widx].kc    = kc;
    widx                       = (widx + 1) & (DEFFERED_EVENT_CNT - 1);
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
                set_kc_no_remap(kc);
                action_exec((keyevent_t){.key     = {.row = 0, .col = 0},
                                         .pressed = true,
                                         .time    = (timer_read() | 1)});
                return;
            }
            break;
        case TD_DOUBLE_TAP:
            kc = data->p_td->double_tap;
            if (kc == KC_NO) {
                kc = data->p_td->single_tap;
                if (kc != KC_NO) {
                    push_deffered_event(kc, true, data->state);
                    push_deffered_event(kc, false, data->state);
                }
            }
            break;
        case TD_DOUBLE_HOLD:
            kc = data->p_td->double_hold;
            kc = kc ? kc : data->p_td->double_tap;
            kc = kc ? kc : data->p_td->single_hold;
            kc = kc ? kc : data->p_td->single_tap;
            if (kc != KC_NO) {
                set_kc_no_remap(kc);
                action_exec((keyevent_t){.key     = {.row = 0, .col = 0},
                                         .pressed = true,
                                         .time    = (timer_read() | 1)});
                return;
            }
            break;
        default:
            break;
    }

    if (kc != KC_NO) {
        push_deffered_event(kc, true, data->state);
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
            if (kc != KC_NO) {
                set_kc_no_remap(kc);
                action_exec((keyevent_t){.key     = {.row = 0, .col = 0},
                                         .pressed = false,
                                         .time    = (timer_read() | 1)});
                return;
            }
            break;
        case TD_DOUBLE_TAP:
            kc = data->p_td->double_tap;
            if (kc == KC_NO) {
                kc = data->p_td->single_tap;
                if (kc != KC_NO) {
                    push_deffered_event(kc, false, data->state);
                }
            }
            break;
        case TD_DOUBLE_HOLD:
            kc = data->p_td->double_hold;
            kc = kc ? kc : data->p_td->double_tap;
            kc = kc ? kc : data->p_td->single_hold;
            kc = kc ? kc : data->p_td->single_tap;
            if (kc != KC_NO) {
                set_kc_no_remap(kc);
                action_exec((keyevent_t){.key     = {.row = 0, .col = 0},
                                         .pressed = false,
                                         .time    = (timer_read() | 1)});
                return;
            }
            break;
        default:
            break;
    }

    if (kc != KC_NO) {
        push_deffered_event(kc, false, data->state);
    }

    data->state = TD_NONE;
}

void activate_tap_dances(void) {
    uint32_t td_cnt = 0;
    for (int td = 0; td < MIN(p_config->tap_dance_len, TAPDANCE_LEN_MAX);
         td++) {
        reset_tap_dance(&tap_dance_actions[td_cnt].state);
        td_tap[td_cnt].state = TD_NONE;
        tap_dance_actions[td_cnt]           = (tap_dance_action_t)ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_finished, td_reset);
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
            set_kc_no_remap(deffered_event[ridx].kc);
            deffered_event[ridx].kc = KC_NO;
            bool pressed            = deffered_event[ridx].start;
            ridx                    = (ridx + 1) & (DEFFERED_EVENT_CNT - 1);

            action_exec((keyevent_t){.key     = {.row = 0, .col = 0},
                                     .pressed = pressed,
                                     .time    = (timer_read() | 1)});
        }

        avoid_recurse = false;
    }
}