/**
 * Commends: Data Bus Operations
 *
 * Shell commands for the Programmable Device.
 *
 * Copyright 2023-26 AESilky
 * SPDX-License-Identifier: MIT License
 *
 */

#include "cmds.h"
#include "dbusm.h"

#include "util.h"

#include "shell/include/shell.h"
#include "shell/cmd/cmd_t.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>


const cmd_handler_entry_t cmds_dbm_rd_entry;
const cmd_handler_entry_t cmds_dbm_wr_entry;


static int _exec_dbm_rd(int argc, char** argv, const char* unparsed) {
    int retval = 0;
    if (argc != 1) {
        // We don't take any arguments.
        cmd_help_display(&cmds_dbm_rd_entry, HELP_DISP_USAGE);
        return (-1);
    }
    uint8_t v = dbusm_rd();
    shell_printf("%02X\n", v);

    return (retval);
}

static int _exec_dbm_wr(int argc, char** argv, const char* unparsed) {
    int retval = 0;
    if (argc != 2) {
        // We take 1 argument.
        cmd_help_display(&cmds_dbm_wr_entry, HELP_DISP_USAGE);
        return (-1);
    }
    argv++;
    bool valid;
    uint32_t v = uint_from_hexstr(*argv, &valid);
    if (!valid) {
        shell_printferr("Value error - '%s' is not a valid hex value.\n", *argv);
        return (-1);
    }
    dbusm_wr(lowByte(v));

    return (retval);
}


const cmd_handler_entry_t cmds_dbm_rd_entry = {
    _exec_dbm_rd,
    4,
    "dbmrd",
    NULL,
    "RD from the Bus."
};

const cmd_handler_entry_t cmds_dbm_wr_entry = {
    _exec_dbm_wr,
    4,
    "dbmwr",
    "byte(hex)",
    "WR a Byte to the Bus."
};



void dbusmcmds_modinit(void) {
    cmd_register(&cmds_dbm_rd_entry);
    cmd_register(&cmds_dbm_wr_entry);
}
