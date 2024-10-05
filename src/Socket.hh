#ifndef SOCKET_H
#define SOKCET_H

union Connection {
    unsigned char ip[4];
};

class Socket {
public:
    Socket();
    ~Socket();

public:
    bool canReceive();
    bool receive(void* data, int dataSize, Connection* connection);
    bool send(void const* data, int dataSize, Connection connection = { 127, 0, 0, 1 });

private:
    static constexpr unsigned short s_port = 4786;
    void* m_handle;
};

#endif