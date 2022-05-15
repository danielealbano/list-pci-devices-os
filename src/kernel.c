// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal with the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//   1. Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimers.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimers in the
//      documentation and/or other materials provided with the distribution.
//   3. Neither the names of the ToAruOS Kernel Development Team, Kevin Lange,
//      nor the names of its contributors may be used to endorse
//      or promote products derived from this Software without specific prior
//      written permission.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// WITH THE SOFTWARE.

// The code is based on the OSDev wiki examples and reuses a simple wrapper to
// handle the serial port from
// https://github.com/stevej/osdev/blob/master/kernel/devices/serial.c

#include <stddef.h>
#include <stdint.h>

#include "inout.h"
#include "str.h"
#include "terminal.h"
#include "serial.h"
#include "console.h"
#include "pci.h"

#define KERNEL_CONSOLE_SERIAL_PORT SERIAL_PORT_A

void kernel_serial_initialize() {
	serial_enable(KERNEL_CONSOLE_SERIAL_PORT);
    console_set_serial_port(KERNEL_CONSOLE_SERIAL_PORT);
}
 
void kernel_terminal_initialize()  {
    terminal_initialize();
}

void kernel_main() {
	kernel_terminal_initialize();

    terminal_writestring("INITIALIZING SERIAL PORT 0");
	kernel_serial_initialize();
 
	console_writestring("SCANNING PCI BUS...\n");

    pci_check_all_buses();
    
	console_writestring("SCAN COMPLETED\n");
}
