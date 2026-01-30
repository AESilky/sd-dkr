/**
 * DBUS - Databus Operations.
 *
 * Copyright 2023-26 AESilky
 * SPDX-License-Identifier: MIT License
 *
 */

#include "dbusc.h"
#include "generated/dbusc.pio.h"

#include "board.h"
#include "cmt_t.h"
#include "msgpost.h"
#include "pio_sm.h"
#include "shell.h"

#include <stddef.h>

// ====================================================================
// Data Section
// ====================================================================

static volatile bool _modinit_called;

static pio_sm_pocfg _cb_monrd_pocfg;
static pio_sm_pocfg _cb_monwr_pocfg;
static pio_sm_pocfg _cb_waitclr_pocfg;

// ====================================================================
// Local/Private Method Declarations
// ====================================================================

static void _wait_clear();

// ====================================================================
// Run-After/Delay/Sleep Methods
// ====================================================================


// ====================================================================
// Message Handler Methods
// ====================================================================

void _rdreq_handler(cmt_msg_t* msg) {
    static uint8_t v;
    dbus_set_out();
    uint32_t bdval = v << DATA_BUS_SHIFT;
    // Put the data on the bus
    gpio_put_masked(DATA_BUS_MASK, bdval);
    // Clear WAIT to allow Host to run
    _wait_clear();
    dbus_set_in();
    v--;
}

void _wrreq_handler(cmt_msg_t* msg) {
    dbus_set_in();
    uint32_t rawvalue = gpio_get_all();
    // Clear WAIT to allow Host to run
    _wait_clear();
    // Adjust the value and print it
    uint8_t value = (rawvalue & DATA_BUS_MASK) >> DATA_BUS_SHIFT;
    shell_printf("\nWR: %02X\n", value);
}


// ====================================================================
// IRQ Methods
// ====================================================================

/**
 * @brief IRQ Handler for RD Request.
 *
 */
void __isr _irq_pio_rdreq_handler() {
    io_rw_32 pio_irqbits = _cb_monrd_pocfg.pio->irq;
    pio_interrupt_clear(_cb_monrd_pocfg.pio, PIO_RDRQ_IRQ);
    //
    // Initialize and post the message
    //
    cmt_msg_t msg;
    cmt_exec_init(&msg, _rdreq_handler);
    msg.data.value32u = pio_irqbits;
    postAPPMsg(&msg);
}

/**
 * @brief IRQ Handler for RD Request.
 *
 */
void __isr _irq_pio_wrreq_handler() {
    io_rw_32 pio_irqbits = _cb_monrd_pocfg.pio->irq;
    pio_interrupt_clear(_cb_monwr_pocfg.pio, PIO_WRRQ_IRQ);
    //
    // Initialize and post the message
    //
    cmt_msg_t msg;
    cmt_exec_init(&msg, _wrreq_handler);
    msg.data.value32u = pio_irqbits;
    postAPPMsg(&msg);
}


// ====================================================================
// Local/Private Methods
// ====================================================================

static pio_sm_pocfg _cb_monrd_pio_init(PIO pio, uint sm, uint mspin, uint rdpin, uint waitpin) {
    pio_sm_pocfg smpocfg = pio_sm_configure(
        pio, sm, &cb_monrd_program, cb_monrd_program_get_default_config, 1.0f, PIO_FIFO_JOIN_NONE,
        0, true, false,
        0, true, false,
        mspin, 1,
        0, 0,
        waitpin, 1,
        0, 0,
        rdpin
    );
    return smpocfg;
}

static pio_sm_pocfg _cb_monwr_pio_init(PIO pio, uint sm, uint mspin, uint wrpin, uint waitpin) {
    pio_sm_pocfg smpocfg = pio_sm_configure(
        pio, sm, &cb_monwr_program, cb_monwr_program_get_default_config, 1.0f, PIO_FIFO_JOIN_NONE,
        0, true, false,
        0, true, false,
        mspin, 1,
        0, 0,
        waitpin, 1,
        0, 0,
        wrpin
    );
    return smpocfg;
}

static pio_sm_pocfg _cb_waitclr_pio_init(PIO pio, uint sm, uint waitpin) {
    pio_sm_pocfg smpocfg = pio_sm_configure(
        pio, sm, &cb_waitclr_program, cb_waitclr_program_get_default_config, 1.0f, PIO_FIFO_JOIN_NONE,
        0, false, false,
        0, false, false,
        0, 0,
        0, 0,
        waitpin, 1,
        0, 0,
        0
    );
    return smpocfg;
}

static void _wait_clear() {
    // To clear WAIT-, clear the interrupt bit that the PIOSM is waiting on.
    pio_interrupt_clear(_cb_waitclr_pocfg.pio, PIO_WAIT_CLR);

}

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

    // Initialize the Data Bus (Initially set to input)
    gpio_set_function(DATA0, GPIO_FUNC_SIO);
    gpio_set_dir(DATA0, GPIO_IN);
    gpio_set_pulls(DATA0, true, false); // Pull-Up
    gpio_set_drive_strength(DATA0, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA1, GPIO_FUNC_SIO);
    gpio_set_dir(DATA1, GPIO_IN);
    gpio_set_pulls(DATA1, true, false); // Pull-Up
    gpio_set_drive_strength(DATA1, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA2, GPIO_FUNC_SIO);
    gpio_set_dir(DATA2, GPIO_IN);
    gpio_set_pulls(DATA2, true, false); // Pull-Up
    gpio_set_drive_strength(DATA2, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA3, GPIO_FUNC_SIO);
    gpio_set_dir(DATA3, GPIO_IN);
    gpio_set_pulls(DATA3, true, false); // Pull-Up
    gpio_set_drive_strength(DATA3, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA4, GPIO_FUNC_SIO);
    gpio_set_dir(DATA4, GPIO_IN);
    gpio_set_pulls(DATA4, true, false); // Pull-Up
    gpio_set_drive_strength(DATA4, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA5, GPIO_FUNC_SIO);
    gpio_set_dir(DATA5, GPIO_IN);
    gpio_set_pulls(DATA5, true, false); // Pull-Up
    gpio_set_drive_strength(DATA5, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA6, GPIO_FUNC_SIO);
    gpio_set_dir(DATA6, GPIO_IN);
    gpio_set_pulls(DATA6, true, false); // Pull-Up
    gpio_set_drive_strength(DATA6, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_function(DATA7, GPIO_FUNC_SIO);
    gpio_set_dir(DATA7, GPIO_IN);
    gpio_set_pulls(DATA7, true, false); // Pull-Up
    gpio_set_drive_strength(DATA7, GPIO_DRIVE_STRENGTH_4MA);

    // Initialize the state machines
    _cb_monrd_pocfg = _cb_monrd_pio_init(PIO_BUS_CTRL, PIO_BC_RD_SM, CTRL_MODSEL, CTRL_RD, CTRL_WAITRQ);
    if (_cb_monrd_pocfg.offset < 0) {
        return (_cb_monrd_pocfg.offset); // Indicate error
    }
    _cb_monwr_pocfg = _cb_monwr_pio_init(PIO_BUS_CTRL, PIO_BC_WR_SM, CTRL_MODSEL, CTRL_WR, CTRL_WAITRQ);
    if (_cb_monwr_pocfg.offset < 0) {
        return (_cb_monwr_pocfg.offset); // Indicate error
    }
    _cb_waitclr_pocfg = _cb_waitclr_pio_init(PIO_BUS_CTRL, PIO_BC_WAIT_SM, CTRL_WAITRQ);
    if (_cb_waitclr_pocfg.offset < 0) {
        return (_cb_waitclr_pocfg.offset); // Indicate error
    }
    // Set up for the interrupts generated by the PIOs
    irq_set_exclusive_handler(PIO_RD_REQ_IRQ, _irq_pio_rdreq_handler); // Set the IRQ handler
    irq_set_enabled(PIO_RD_REQ_IRQ, false); // Disable the IRQ for now
    pio_set_irqn_source_enabled(PIO_BUS_CTRL, PIO_IRQ_RDRQ_IDX, PIO_IRQ_RDRQ_BIT, true); // Interrupt on IRQ-Bit0 set
    irq_set_exclusive_handler(PIO_WR_REQ_IRQ, _irq_pio_wrreq_handler); // Set the IRQ handler
    irq_set_enabled(PIO_WR_REQ_IRQ, false); // Disable the IRQ for now
    pio_set_irqn_source_enabled(PIO_BUS_CTRL, PIO_IRQ_WRRQ_IDX, PIO_IRQ_WRRQ_BIT, true); // Interrupt on IRQ-Bit1 set

    // Start them
    pio_sm_set_enabled(_cb_monwr_pocfg.pio, _cb_monwr_pocfg.sm, true);
    pio_sm_set_enabled(_cb_monrd_pocfg.pio, _cb_monrd_pocfg.sm, true);
    pio_sm_set_enabled(_cb_waitclr_pocfg.pio, _cb_waitclr_pocfg.sm, true);
    irq_set_enabled(PIO_RD_REQ_IRQ, true); // Enable the IRQ now
    irq_set_enabled(PIO_WR_REQ_IRQ, true); // Enable the IRQ now

    return (retval);
}
