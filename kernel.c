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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8
 
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (data));
}

static inline void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}

static inline void outl(uint16_t port, uint32_t data) {
    asm volatile ("outl %1, %0" : : "dN" (port), "a" (data));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)  {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color)  {
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str)  {
	size_t len = 0;
	while (str[len]) {
		len++;
    }
	return len;
}

void serial_enable(int device) {
	outb(device + 1, 0x00);
	outb(device + 3, 0x80);
	outb(device + 0, 0x03);
	outb(device + 1, 0x00);
	outb(device + 3, 0x03);
	outb(device + 2, 0xC7);
	outb(device + 4, 0x0B);
}

void serial_initialize(void) {
	serial_enable(SERIAL_PORT_A);
	serial_enable(SERIAL_PORT_B);
}

int serial_rcvd(int device) {
	return inb(device + 5) & 1;
}

char serial_recv(int device) {
	while (serial_rcvd(device) == 0) ;
	return inb(device);
}

char serial_recv_async(int device) {
	return inb(device);
}

int serial_transmit_empty(int device) {
	return inb(device + 5) & 0x20;
}

void serial_send(int device, char c) {
	while (serial_transmit_empty(device) == 0);
	outb(device, c);
}

void serial_write(int device, const char *data, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i) {
		serial_send(device, data[i]);
	}
}

void serial_writestring(int device, const char *data) {
    serial_write(device, data, strlen(data));
}
 
void terminal_initialize(void)  {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color)  {
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) {
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
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
    }
}
 
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void console_writestring(const char* data) {
    terminal_writestring(data);
    serial_writestring(SERIAL_PORT_A, data);
}

unsigned writenumber_uint64_get_len(uint64_t number) {
    static uint8_t maxdigits[65] = {
            1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6,
            7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11,
            12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15,
            16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20,
    };

    static uint64_t powers[] = {
            0U, 1U, 10U, 100U, 1000U, 10000U, 100000U, 1000000U, 10000000U,
            100000000U, 1000000000U, 10000000000U, 100000000000U,
            1000000000000U, 10000000000000U, 100000000000000U,
            1000000000000000U, 10000000000000000U, 100000000000000000U,
            1000000000000000000U, 10000000000000000000U,
    };

    if (number == 0) {
        return 1;
    }

    unsigned bits = sizeof(number) * 8 - __builtin_clzll(number);
    unsigned digits = maxdigits[bits];

    if (number < powers[digits]) {
        --digits;
    }

    return digits;
}

char* writenumber_uint64(uint64_t number, char* buffer, size_t buffer_length, unsigned *number_length) {
    unsigned number_str_length;
    number_str_length = *number_length = writenumber_uint64_get_len(number);

    if (number_str_length > buffer_length) {
        number_str_length = buffer_length;
    }

    if (number_str_length == 0) {
        return NULL;
    }

    do {
        buffer[number_str_length - 1] = (char)((number % 10U) + '0');
    } while(number_str_length-- > 0 && (number /= 10U) > 0);

    if (number_str_length == 0 && number > 0) {
        return NULL;
    }

    return buffer;
}

char* writenumber_hex(uint64_t number, uint8_t length, char* buffer, size_t buffer_length) {
    if (length == 0) {
        return NULL;
    }

    if (length > buffer_length) {
        length = buffer_length;
    }

    int index;
    for(index = length - 1; index >= 0; index--) {
        int digit = number & 0xF;

        if (digit <= 9) {
            buffer[index] = '0' + digit;
        } else {
            buffer[index] = 'A' + (digit - 10);
        }
        
        number >>= 4;
    }

    return buffer;
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    outl(0xCF8, address);

    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    return (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint32_t pci_config_read_long(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset) | ((uint32_t)0x80000000));
 
    outl(0xCF8, address);
    return inl(0xCFC);
}

uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t function) {
    return pci_config_read_long(bus, device, function, 0x00) >> 16;
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function) {
    return pci_config_read_long(bus, device, function, 0x00);
}

uint8_t pci_get_class(uint8_t bus, uint8_t device, uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 24) & 0xFF;
}

uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 16) & 0xFF;
}

uint8_t pci_get_prog_if(uint8_t bus, uint8_t device, uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 8) & 0xFF;
}

uint8_t pci_get_rev_id(uint8_t bus, uint8_t device, uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 0) & 0xFF;
}

uint32_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function) {
    return ((pci_config_read_long(bus, device, function, 0x0C)) >> 16) & 0xFF;
}

void pci_print_dev_info(uint8_t bus, uint8_t device, uint8_t function) {
    char
        buffer1[25] = { 0 },
        buffer2[25] = { 0 },
        buffer3[25] = { 0 },
        buffer4[25] = { 0 },
        buffer5[25] = { 0 },
        buffer6[25] = { 0 },
        buffer7[25] = { 0 },
        buffer8[25] = { 0 };
    unsigned out = 0;

    uint16_t pci_dev_vendor_id = pci_get_vendor_id(bus, device, function);
    uint16_t pci_dev_dev_id = pci_get_device_id(bus, device, function);
    uint8_t pci_dev_class = pci_get_class(bus, device, function);
    uint8_t pci_dev_subclass = pci_get_subclass(bus, device, function);
    uint8_t pci_dev_rev_id = pci_get_rev_id(bus, device, function);
    
    console_writestring("[");
    console_writestring(writenumber_hex((uint64_t)bus, 2, buffer1, sizeof(buffer1)));
    console_writestring(":");
    console_writestring(writenumber_hex((uint64_t)device, 2, buffer2, sizeof(buffer2)));
    console_writestring(":");
    console_writestring(writenumber_hex((uint64_t)function, 2, buffer3, sizeof(buffer3)));
    console_writestring("] ");
    console_writestring("ID: ");
    console_writestring(writenumber_hex((uint64_t)pci_dev_vendor_id, 4, buffer4, sizeof(buffer4)));
    console_writestring(":");
    console_writestring(writenumber_hex((uint64_t)pci_dev_dev_id, 4, buffer5, sizeof(buffer5)));
    console_writestring(", Class: 0x");
    console_writestring(writenumber_hex((uint64_t)pci_dev_class, 2, buffer6, sizeof(buffer6)));
    console_writestring(", SubClass: 0x");
    console_writestring(writenumber_hex((uint64_t)pci_dev_subclass, 2, buffer7, sizeof(buffer7)));
    console_writestring(", Rev: ");
    console_writestring(writenumber_uint64((uint64_t)pci_dev_rev_id, buffer8, sizeof(buffer8), &out));
    console_writestring("\n");
}

unsigned pci_check_function(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t pci_vendor_id = pci_get_vendor_id(bus, device, function);
    if (pci_vendor_id == 0xFFFF) {
        return 0;
    }

    pci_print_dev_info(bus, device, function);

    return 1;
}

unsigned pci_check_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    uint16_t pci_vendor_id = pci_get_vendor_id(bus, device, function);
    if (pci_vendor_id == 0xFFFF) {
        return 0;
    }
    
    pci_print_dev_info(bus, device, function);
    
    uint8_t header_type = pci_get_header_type(bus, device, function);
    if ((header_type & 0x80) != 0) {
        // It's a multi-function device, so check remaining functions
        for (function = 1; function < 8; function++) {
            pci_check_function(bus, device, function);
        }
    }

    return 1;
}

void pci_check_all_buses(void) {
    uint16_t bus;
    uint8_t device;

    for (bus = 0; bus < 256; bus++) {
        device = 0;
        for (device = 0; device < 32; device++) {
            pci_check_device(bus, device);
        }
    }
}

void kernel_main(void) {
	terminal_initialize();
	serial_initialize();
 
	console_writestring("SCANNING PCI BUS...\n");

    pci_check_all_buses();
    
	console_writestring("SCAN COMPLETED\n");
}
