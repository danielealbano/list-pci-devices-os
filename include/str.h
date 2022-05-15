size_t str_len(
    const char* str);

unsigned str_uint64_to_decstr_len(
    uint64_t number);

char* str_uint64_to_decstr(
    uint64_t number,
    char* buffer,
    size_t buffer_length,
    unsigned *number_length);

char* str_uint64_to_hexstr(
    uint64_t number,
    uint8_t length,
    char* buffer,
    size_t buffer_length);
