// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

uint16_t keymap_to_keycode_hook_mouse(uint16_t keycode);
void matrix_scan_user_hook_mouse(void);
void post_process_record_mouse(uint16_t keycode, keyrecord_t* record);
bool process_record_mouse(uint16_t keycode, keyrecord_t* record);
void set_mouse_gesture_threshold(uint16_t val);