#include <stddef.h>
#include <stdint.h>

#include "inout.h"
#include "str.h"
#include "console.h"

#include "pci.h"

uint16_t pci_config_read_word(
    uint8_t bus,
    uint8_t device,
    uint8_t func,
    uint8_t offset) {
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

uint32_t pci_config_read_long(
    uint8_t bus,
    uint8_t device,
    uint8_t func,
    uint8_t offset) {
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

uint16_t pci_get_device_id(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return pci_config_read_long(bus, device, function, 0x00) >> 16;
}

uint16_t pci_get_vendor_id(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return pci_config_read_long(bus, device, function, 0x00);
}

uint8_t pci_get_class(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 24) & 0xFF;
}

uint8_t pci_get_subclass(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 16) & 0xFF;
}

uint8_t pci_get_prog_if(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 8) & 0xFF;
}

uint8_t pci_get_rev_id(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return (pci_config_read_long(bus, device, function, 0x8) >> 0) & 0xFF;
}

uint32_t pci_get_header_type(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    return ((pci_config_read_long(bus, device, function, 0x0C)) >> 16) & 0xFF;
}

void pci_print_dev_info(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
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
    console_writestring(str_uint64_to_hexstr((uint64_t)bus, 2, buffer1, sizeof(buffer1)));
    console_writestring(":");
    console_writestring(str_uint64_to_hexstr((uint64_t)device, 2, buffer2, sizeof(buffer2)));
    console_writestring(":");
    console_writestring(str_uint64_to_hexstr((uint64_t)function, 2, buffer3, sizeof(buffer3)));
    console_writestring("] ");
    console_writestring("ID: ");
    console_writestring(str_uint64_to_hexstr((uint64_t)pci_dev_vendor_id, 4, buffer4, sizeof(buffer4)));
    console_writestring(":");
    console_writestring(str_uint64_to_hexstr((uint64_t)pci_dev_dev_id, 4, buffer5, sizeof(buffer5)));
    console_writestring(", Class: 0x");
    console_writestring(str_uint64_to_hexstr((uint64_t)pci_dev_class, 2, buffer6, sizeof(buffer6)));
    console_writestring(", SubClass: 0x");
    console_writestring(str_uint64_to_hexstr((uint64_t)pci_dev_subclass, 2, buffer7, sizeof(buffer7)));
    console_writestring(", Rev: ");
    console_writestring(str_uint64_to_decstr((uint64_t)pci_dev_rev_id, buffer8, sizeof(buffer8), &out));
    console_writestring("\n");
}

unsigned pci_check_function(
    uint8_t bus,
    uint8_t device,
    uint8_t function) {
    uint32_t pci_vendor_id = pci_get_vendor_id(bus, device, function);
    if (pci_vendor_id == 0xFFFF) {
        return 0;
    }

    pci_print_dev_info(bus, device, function);

    return 1;
}

unsigned pci_check_device(
    uint8_t bus,
    uint8_t device) {
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

void pci_check_all_buses() {
    uint16_t bus;
    uint8_t device;

    for (bus = 0; bus < 256; bus++) {
        device = 0;
        for (device = 0; device < 32; device++) {
            pci_check_device(bus, device);
        }
    }
}
