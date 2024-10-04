#ifndef SOCKET_H
#define SOKCET_H

#include <stdint.h>
#include <stdarg.h>

#define DEFAULT_PORT 4786

struct Connection
{
    uint8_t ip[4] = { 127, 0, 0, 1 };
    uint16_t port = DEFAULT_PORT;
};

class Socket
{
public:
    Socket();
    ~Socket();

public:
    bool send(Connection const* connection, void const* data, size_t dataSize);
    bool listenPort(uint16_t port = DEFAULT_PORT);
    bool canReceive(long timeoutMicroseconds = 1);
    bool receive(Connection* connection, void* data, size_t dataSize);

private:
    void* m_handle;
};

#endif