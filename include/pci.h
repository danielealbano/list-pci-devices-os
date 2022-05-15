uint16_t pci_config_read_word(
    uint8_t bus,
    uint8_t device,
    uint8_t func,
    uint8_t offset);

uint32_t pci_config_read_long(
    uint8_t bus,
    uint8_t device,
    uint8_t func,
    uint8_t offset);

uint16_t pci_get_device_id(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

uint16_t pci_get_vendor_id(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

uint8_t pci_get_class(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

uint8_t pci_get_subclass(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

uint8_t pci_get_prog_if(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

uint8_t pci_get_rev_id(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

uint32_t pci_get_header_type(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

void pci_print_dev_info(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

unsigned pci_check_function(
    uint8_t bus,
    uint8_t device,
    uint8_t function);

unsigned pci_check_device(
    uint8_t bus,
    uint8_t device);

void pci_check_all_buses();
