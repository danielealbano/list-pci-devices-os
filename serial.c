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
#include "serial.h"

void serial_enable(int port) {
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, 0x03);
	outb(port + 1, 0x00);
	outb(port + 3, 0x03);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
}

int serial_rcvd(int port) {
	return inb(port + 5) & 1;
}

char serial_recv(int port) {
	while (serial_rcvd(port) == 0) ;
	return inb(port);
}

char serial_recv_async(int port) {
	return inb(port);
}

int serial_transmit_empty(int port) {
	return inb(port + 5) & 0x20;
}

void serial_send(int port, char c) {
	while (serial_transmit_empty(port) == 0);
	outb(port, c);
}

void serial_write(int port, const char *data, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i) {
		serial_send(port, data[i]);
	}
}

void serial_writestring(int port, const char *data) {
    serial_write(port, data, strlen(data));
}
