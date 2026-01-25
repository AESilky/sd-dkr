# sd-dkr

Disk, Keyboard, and Real Time Clock for parallel bus (like Z80) using an RP2040 (Pico).

## Git Submodules

This uses Git submodules for the Shell and SD Card support. When cloning or updating
the submodules need to be initialized. The following are the typical steps to do this.

### Cloning

   1. git clone git@github.com:AESilky/sd-dkr.git
   2. git submodule update --init --recursive

### Update to the latest Shell and SD Card versions

    * git submodule update --init --recursive

### Update only Shell or SD Card libraries

    * git submodule update --remote -- sd_card
    * git submodule update --remote -- shell

