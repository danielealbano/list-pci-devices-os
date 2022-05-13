list-pci-devices-os
===

An *Operating System* written for fun for the sole purpose of dumping out some internal registries of the PCI devices connected to an x86 machine.

The code is based on the OSDev wiki examples and https://github.com/stevej/osdev/blob/master/kernel/devices/serial.c for the serial support.

## Dependencies

Can be built on Linux and on WSL with Ubuntu 22.04, requires the following dependencies:

- build-essential
- gcc-multilib
- gcc-11-multilib
- xorriso
- qemu-user
- qemu-system-x86
- grub-common
- grub-pc-bin
- grub2-common

## Building it

To build it
```sh
make
```

## Running it

### On QEMU

To test it out on QEMU there is a shortcut in the Makefile.

```sh
make qemu
```

### Real hardware

The Operating System can boot on real hardware using the Legacy BIOS as it doesn't support UEFI.

It's possible to build out both an ISO using
```
make myos.iso
```

Or for an USB pendrive using
```
make myos.img
```
