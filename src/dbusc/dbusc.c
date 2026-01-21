/**
 * DBUS - Databus Operations.
 *
 * Copyright 2023-26 AESilky
 * SPDX-License-Identifier: MIT License
 *
 */

#include "dbusc.h"

#include "board.h"


#include <stddef.h>

// ====================================================================
// Data Section
// ====================================================================

static volatile bool _modinit_called;

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


// ====================================================================
// Public Methods
// ====================================================================

extern uint8_t dbus_rd() {
    if (dbus_is_out()) {
        dbus_set_in();
    }
    uint32_t rawvalue = gpio_get_all();
    uint8_t value = (rawvalue & DATA_BUS_MASK) >> DATA_BUS_SHIFT;

    return value;
}

void dbus_wr(uint8_t data) {
    dbus_set_out();
    uint32_t bdval = data << DATA_BUS_SHIFT;
    gpio_put_masked(DATA_BUS_MASK, bdval);
}



// ====================================================================
// Initialization/Start-Up Methods
// ====================================================================


int dbusc_modinit() {
    if (_modinit_called) {
        board_panic("!!! dbusc_modinit: Called more than once !!!");
    }
    _modinit_called = true;

    int retval = 0;

    return (retval);
}
