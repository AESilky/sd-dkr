/**
 * CBM - Control Bus Master Operations.
 *
 * Copyright 2023-26 AESilky
 * SPDX-License-Identifier: MIT License
 *
 */

#include "dbusm.h"
#include "generated/dbusm.pio.h"

#include "board.h"
#include "pio_sm.h"

#include <stddef.h>

#include "shell.h"

// ====================================================================
// Data Section
// ====================================================================
#define PIO_BUS_CLKDIV 8.f

static volatile bool _modinit_called;

static pio_sm_pocfg _cbm_rd_pocfg;
static pio_sm_pocfg _cbm_wr_pocfg;

// ====================================================================
// Local/Private Method Declarations
// ====================================================================


// ====================================================================
// Run-After/Delay/Sleep Methods
// ====================================================================


// ====================================================================
// Message Handler Methods
// ====================================================================


// ====================================================================
// Local/Private Methods
// ====================================================================


static pio_sm_pocfg _cbm_rd_pio_init(PIO pio, uint sm, uint dbpin, uint ctrlpin, uint waitpin) {
    pio_sm_pocfg smpocfg = pio_sm_configure(
        pio, sm, &cbm_in_program, cbm_in_program_get_default_config, 16.0f, PIO_FIFO_JOIN_NONE,
        8, true, false,
        0, false, false,
        dbpin, 8,
        0, 0,
        0, 0,
        ctrlpin, 3,
        waitpin
    );
    return smpocfg;
}

static pio_sm_pocfg _cbm_wr_pio_init(PIO pio, uint sm, uint dbpin, uint ctrlpin, uint waitpin) {
    // pio_sm_set_enabled(pio, sm, false);

    // pio_sm_pocfg smpocfg;
    // smpocfg.pio = pio;
    // smpocfg.sm = sm;

    // // install the program in the PIO shared instruction space
    // const pio_program_t* pio_prgm = &cbm_out_program;
    // smpocfg.offset = pio_add_program(pio, pio_prgm);
    // if (smpocfg.offset < 0) {
    //     return smpocfg;      // the program could not be added
    // }

    // smpocfg.sm_cfg = cbm_out_program_get_default_config(smpocfg.offset);
    // sm_config_set_in_pin_base(&smpocfg.sm_cfg, dbpin); // for the Data Bus
    // sm_config_set_out_pins(&smpocfg.sm_cfg, dbpin, 8); // for the Data Bus Pindirs
    // sm_config_set_set_pins(&smpocfg.sm_cfg, dbpin, 8); // for the Data Bus Pindirs
    // sm_config_set_jmp_pin(&smpocfg.sm_cfg, waitpin);   // for wait-states
    // sm_config_set_sideset_pin_base(&smpocfg.sm_cfg, ctrlpin);
    // sm_config_set_sideset(&smpocfg.sm_cfg, 2, true, false); // 2 pins, optional, not pindirs
    // sm_config_set_clkdiv(&smpocfg.sm_cfg, PIO_BUS_CLKDIV);

    // pio_gpio_init(pio, ctrlpin);        // WR-
    // pio_gpio_init(pio, ctrlpin + 1);    // MS-
    // pio_gpio_init(pio, ctrlpin + 2);    // WAIT-

    // pio_sm_init(pio, sm, smpocfg.offset, &smpocfg.sm_cfg);
    pio_sm_pocfg smpocfg = pio_sm_configure(
        pio, sm, &cbm_out_program, cbm_out_program_get_default_config, 16.0f, PIO_FIFO_JOIN_NONE,
        0, false, false,
        8, true, false,
        dbpin, 8,
        0, 0,
        0, 0,
        ctrlpin, 3,
        waitpin
    );
    return smpocfg;
}


// ====================================================================
// Public Methods
// ====================================================================

uint8_t dbusm_rd() {
    // To Read from the Bus, clear the IRQ bit then read from the Input FIFO
    pio_interrupt_clear(_cbm_rd_pocfg.pio, 4);
    uint32_t v = pio_sm_get_blocking(_cbm_rd_pocfg.pio, _cbm_rd_pocfg.sm);
    return ((uint8_t)((v & 0xFF000000) >> 24));
}

void dbusm_wr(uint8_t v) {
    // To Write to the Bus, write the value to the PIO-SM Output FIFO
    pio_sm_put_blocking(_cbm_wr_pocfg.pio, _cbm_wr_pocfg.sm, (uint32_t)v);
}



// ====================================================================
// Initialization/Start-Up Methods
// ====================================================================


int dbusm_modinit() {
    if (_modinit_called) {
        board_panic("!!! dbusm_modinit - called more than once !!!");
    }
    _modinit_called = true;
    int retval = 0;

    // Initialize the state machines
    _cbm_wr_pocfg = _cbm_wr_pio_init(PIO_BUS_CTRL, PIO_BCM_WR_SM, DATA0, CTRL_WR, CTRL_WAITRQ);
    if (_cbm_wr_pocfg.offset < 0) {
        return (_cbm_wr_pocfg.offset); // Indicate error
    }
    _cbm_rd_pocfg = _cbm_rd_pio_init(PIO_BUS_CTRL, PIO_BCM_RD_SM, DATA0, CTRL_RD, CTRL_WAITRQ);
    if (_cbm_rd_pocfg.offset < 0) {
        return (_cbm_rd_pocfg.offset); // Indicate error
    }
    // Start them
    pio_sm_set_enabled(_cbm_wr_pocfg.pio, _cbm_wr_pocfg.sm, true);
    pio_sm_set_enabled(_cbm_rd_pocfg.pio, _cbm_rd_pocfg.sm, true);

    return retval;
}