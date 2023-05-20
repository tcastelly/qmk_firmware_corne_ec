// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "c1.h"
#include "ch.h"
#include "hal.h"

// 1ms repeat timer for USB frame
extern void sof_timer(void *);

static virtual_timer_t vt;
static volatile bool   c1_stop_flag;
static volatile bool   c1_restart_flag;

static void timer_cb(virtual_timer_t *_vt, void *_) {
    // Start USB frame
    sof_timer(NULL);
}

void c1_before_flash_operation(void) {
    c1_stop_flag = true;
    while (c1_stop_flag) {
        continue;
    }
}

void c1_after_flash_operation(void) {
    c1_restart_flag = true;
    while (c1_restart_flag) {
        continue;
    }
}

static void __no_inline_not_in_flash_func(c1_trap_for_flash_operation)(void) {
    if (c1_stop_flag) {
        chVTReset(&vt);
        c1_stop_flag = false;
        while (!c1_restart_flag) {
            continue;
        }
        c1_restart_flag = false;
        chVTSetContinuous(&vt, TIME_MS2I(1), timer_cb, NULL);
    }
}

// Main process for core1
static THD_WORKING_AREA(wa_c1_main_task_wrapper, 2048);
static THD_FUNCTION(c1_main_task_wrapper, arg) {
    while (1) {
        c1_main_task();
        c1_trap_for_flash_operation();
        chThdSleepMicroseconds(125);
    }
}

// Entry point of core1
void c1_main(void) {
    chSysWaitSystemState(ch_sys_running);
    chInstanceObjectInit(&ch1, &ch_core1_cfg);
    chSysUnlock();

    // USB host stack uses PIO and DMA
    hal_lld_peripheral_unreset(RESETS_ALLREG_PIO0);
    hal_lld_peripheral_unreset(RESETS_ALLREG_PIO1);
    hal_lld_peripheral_unreset(RESETS_ALLREG_DMA);

    // Initialize USB host stack
    c1_usbh();

    // Start main task
    chThdCreateStatic(wa_c1_main_task_wrapper, sizeof(wa_c1_main_task_wrapper), NORMALPRIO + 1, c1_main_task_wrapper, NULL);
}

// Start 1ms timer
void c1_start_timer(void) {
    chVTObjectInit(&vt);
    chVTSetContinuous(&vt, TIME_MS2I(1), timer_cb, NULL);
}