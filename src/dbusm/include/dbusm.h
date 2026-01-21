/**
 * Data Bus Master operations.
 *
 * Provide functions that control (master) / follow (slave) a Module-Select, RD and WR pins.
 *
 * Copyright 2023-26 AESilky
 * SPDX-License-Identifier: MIT License
 *
*/
#ifndef DBUS_M_H_
#define DBUS_M_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "system_defs.h"

#include "pico/types.h" // 'uint' and other standard types

#include <stdbool.h>
#include <stdint.h>


extern uint8_t dbusm_rd();

extern void dbusm_wr(uint8_t v);

/**
 * @brief Initialize the module. Must be called once/only-once before module use.
 *
 * @return 0 If init good.
 */
extern int dbusm_modinit();

#ifdef __cplusplus
}
#endif
#endif // DBUS_M_H_
