#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8

void serial_enable(
    int port);

int serial_rcvd(
    int port);

char serial_recv(
    int port);

char serial_recv_async(
    int port);

int serial_transmit_empty(
    int port);

void serial_send(
    int port,
    char c);

void serial_write(
    int port,
    const char *data,
    uint32_t length);

void serial_writestring(
    int port,
    const char *data);
