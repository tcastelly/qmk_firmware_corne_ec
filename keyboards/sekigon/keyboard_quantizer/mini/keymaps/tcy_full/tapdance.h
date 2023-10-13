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
  QWERTY = QK_USER_0,
  QWERTY_OSX = QK_USER_1,
  QWERTY_GAMING = QK_USER_2,
  LOWER = QK_USER_3,
  RAISE = QK_USER_4,
  ADJUST = QK_USER_5,
  ESC = QK_USER_6,
  ACCENT_GRAVE = QK_USER_7,
  ACCENT_CIRCUM = QK_USER_8,
  ACCENT_TREMA = QK_USER_9,
  ACCENT_E_GRAVE = QK_USER_10,
  ACCENT_A_GRAVE = QK_USER_11,

  TOGGLE_OLED = QK_USER_12,
  TOGGLE_OLED_WPM = QK_USER_13,
  TOGGLE_OLED_LAYOUT = QK_USER_14,
  TOGGLE_OLED_DEFAULT = QK_USER_15,

  // to be used with RALT already pressed
  ACCENT_I_CIRC_RALT = QK_USER_16,
  ACCENT_O_CIRC_RALT = QK_USER_17,
  ACCENT_U_AIGU_RALT = QK_USER_18,
  ACCENT_C_RALT = QK_USER_19,
  ACCENT_A_GRAVE_RALT = QK_USER_20,

  // Jetbrains macro
  JET_FIND = QK_USER_21,
  JET_RNM = QK_USER_22,

  NEW_SAFE_RANGE = SAFE_RANGE,
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

extern bool is_hold_tapdance_disabled;

extern void td_ralt_reset (tap_dance_state_t *state, void *user_data);

extern void td_ralt_finished (tap_dance_state_t *state, void *user_data);

extern void td_ralt_osx_reset (tap_dance_state_t *state, void *user_data);

extern void td_ralt_osx_finished (tap_dance_state_t *state, void *user_data);

extern void tap_dance_tap_hold_finished_unprotected(tap_dance_state_t *state, void *user_data);

extern void td_lgui_reset (tap_dance_state_t *state, void *user_data);

extern void td_lgui_finished (tap_dance_state_t *state, void *user_data);

extern void td_lalt_reset (tap_dance_state_t *state, void *user_data);

extern void td_lalt_finished (tap_dance_state_t *state, void *user_data);

extern void td_lctl_reset (tap_dance_state_t *state, void *user_data);

extern void td_lctl_finished (tap_dance_state_t *state, void *user_data);

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
