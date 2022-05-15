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

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) {
		len++;
    }
	return len;
}

unsigned str_uint64_to_decstr_len(uint64_t number) {
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

char* str_uint64_to_decstr(uint64_t number, char* buffer, size_t buffer_length, unsigned *number_length) {
    unsigned number_str_length;
    number_str_length = *number_length = str_uint64_to_decstr_len(number);

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

char* str_uint64_to_hexstr(uint64_t number, uint8_t length, char* buffer, size_t buffer_length) {
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
