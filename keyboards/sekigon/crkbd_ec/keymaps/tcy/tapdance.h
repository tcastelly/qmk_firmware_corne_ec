#ifndef TAPDANCE_H
#define TAPDANCE_H

enum layer_names {
    _QWERTY,
    _QWERTY_OSX,
    _QWERTY_GAMING,
    _LOWER,
    _RAISE,
    _ADJUST,
    _ESC,
    _ESC_OSX,
    _NUM_PADS,
    _ACCENTS_RALT
};

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  QWERTY_OSX,
  QWERTY_GAMING,
  LOWER,
  RAISE,
  ADJUST,
  ESC,
  ACCENT_GRAVE,
  ACCENT_CIRCUM,
  ACCENT_TREMA,
  ACCENT_E_GRAVE,
  ACCENT_A_GRAVE,

  // to be used with RALT already pressed
  ACCENT_I_CIRC_RALT,
  ACCENT_O_CIRC_RALT,
  ACCENT_U_AIGU_RALT,
  ACCENT_C_RALT,
  ACCENT_A_GRAVE_RALT,

  // Jetbrains macro
  JET_FIND,
  JET_RNM,
};

// default tap dance
enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2, // should use tap_dance_tap_hold_t instead
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5, //send two single taps
  TRIPLE_TAP = 6,
  TRIPLE_HOLD = 7,
  TRIPLE_SINGLE_TAP = 8
};

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

typedef struct {
  bool is_press_action;
  int state;
} tap;

// "tap-hold"
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

#endif

extern uint16_t last_hold_t;

extern bool is_hold_tapdance_disabled;

extern void td_ralt_reset (qk_tap_dance_state_t *state, void *user_data);

extern void td_ralt_finished (qk_tap_dance_state_t *state, void *user_data);

extern void td_ralt_osx_reset (qk_tap_dance_state_t *state, void *user_data);

extern void td_ralt_osx_finished (qk_tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_finished_unprotected(qk_tap_dance_state_t *state, void *user_data);

extern void td_lgui_reset (qk_tap_dance_state_t *state, void *user_data);

extern void td_lgui_finished (qk_tap_dance_state_t *state, void *user_data);

extern void td_lalt_reset (qk_tap_dance_state_t *state, void *user_data);

extern void td_lalt_finished (qk_tap_dance_state_t *state, void *user_data);

extern void td_lctl_reset (qk_tap_dance_state_t *state, void *user_data);

extern void td_lctl_finished (qk_tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_reset(qk_tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_finished(qk_tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_reset_layout(qk_tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_finished_layout(qk_tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

// allow call multiple tap dance simultaneously
// e.g: TD_DEL/TD_DEL_OSX
#define ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_unprotected, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }
