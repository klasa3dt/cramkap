#include <stdint.h>

struct Socket {
    uint64_t handle;
};

static const uint16_t DEFAULT_PORT = 777;
struct Connection {
    uint8_t ipaddress[4] = { 127, 0, 0, 1 };
    uint16_t port = DEFAULT_PORT;
};

bool CreateSocket(Socket* sock);
bool Send(const Socket* sock, const Connection* connection, const void* data, size_t dataSize);
bool ListenPort(const Socket* sock, uint16_t port = DEFAULT_PORT);
bool CanReceive(const Socket* sock, long timeout_microseconds = 1);
bool Receive(const Socket* sock, Connection* connection, void* data, size_t dataSize);
const char* GetMyIp();
Connection ConvertToConnection(const char* ip);