// copyright 2023 sekigon-gonnoc
// spdx-license-identifier: gpl-2.0-or-later

#pragma once

void activate_tap_dances(void);
void dynamic_tap_dance_task(void);

enum layer_names {
    _QWERTY,
    _QWERTY_OSX,
    _QWERTY_GAMING,
    _LOWER,
    _RAISE,
    _ADJUST,
    _MODS,
    _MODS_OSX,
};

// "tap-hold"
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

// custom tap dance
enum {
    TD_ESC,
    TD_ESC_OSX,
    TD_TAB,
    TD_O,
    TD_P,
    TD_L,
    TD_ENT,
    TD_SCLN,
    TD_LCTL,
    TD_LGUI,
    TD_LALT,
    TD_RALT,
    TD_RALT_OSX,
    TD_BSPC,
    TD_BSPC_OSX,
    TD_DEL,
    TD_DEL_OSX,
    TD_LEFT,
    TD_LEFT_OSX,
    TD_RIGHT,
    TD_RIGHT_OSX
};

extern bool is_hold_tapdance_disabled;

extern void tap_dance_tap_hold_finished_unprotected(tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_finished_layout(tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

// allow call multiple tap dance simultaneously
// e.g: TD_DEL/TD_DEL_OSX
#define ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_unprotected, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }
