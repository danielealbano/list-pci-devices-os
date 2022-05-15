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

#include "str.h"
#include "inout.h"
#include "terminal.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

static inline uint8_t terminal_entry_color(
	enum vga_color fg,
	enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t terminal_entry(
	unsigned char uc,
	uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = terminal_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = terminal_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(
	uint8_t color) {
	terminal_color = color;
}
 
void terminal_putentryat(
	char c,
	uint8_t color,
	size_t x,
	size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = terminal_entry(c, color);
}
 
void terminal_putchar(
	char c) {
    if (c != '\n') {
	    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_row = 0;
            }
        }
    } else {
        terminal_column = 0;

        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}
 
void terminal_write(
	const char* data,
	size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
    }
}
 
void terminal_writestring(
	const char* data) {
	terminal_write(data, str_len(data));
}
