#include "Socket.hh"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

struct Internal
{
    Internal()
    {
        WSAData wsadata;
        WSAStartup(MAKEWORD(2, 2), &wsadata);
    }

    ~Internal()
    {
        WSACleanup();
    }
} static internal;

Socket::Socket()
{
    m_handle = (void*)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

Socket::~Socket()
{
    closesocket((SOCKET)m_handle);
}

bool Socket::send(Connection const* connection, void const* data, size_t dataSize)
{
    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(connection->port);
    target.sin_addr.S_un.S_un_b.s_b1 = connection->ip[0];
    target.sin_addr.S_un.S_un_b.s_b2 = connection->ip[1];
    target.sin_addr.S_un.S_un_b.s_b3 = connection->ip[2];
    target.sin_addr.S_un.S_un_b.s_b4 = connection->ip[3];

    int result = sendto((SOCKET)m_handle, (char const*)data, (int)dataSize, 0, (sockaddr const*)&target, sizeof(target));

    return result != SOCKET_ERROR;
}

bool Socket::listenPort(uint16_t port)
{
    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    int result = bind((SOCKET)m_handle, (sockaddr const*)&target, sizeof(target));

    return result != SOCKET_ERROR;
}

bool Socket::canReceive(long timeoutMicroseconds)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET((SOCKET)m_handle, &readfds);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = timeoutMicroseconds;
    int result = select(0, &readfds, NULL, NULL, &timeout);

    if (result < 0)
    {
        return false;
    }

    return FD_ISSET((SOCKET)m_handle, &readfds);
}

bool Socket::receive(Connection* connection, void* data, size_t dataSize)
{
    sockaddr_in sender;
    int targetsize = sizeof(sender);
    int result = recvfrom((SOCKET)m_handle, (char*)data, (int)dataSize, 0, (sockaddr*)&sender, &targetsize);

    if (result == SOCKET_ERROR)
    {
        return false;
    }

    connection->port = htons(sender.sin_port);
    connection->ip[0] = sender.sin_addr.S_un.S_un_b.s_b1;
    connection->ip[1] = sender.sin_addr.S_un.S_un_b.s_b2;
    connection->ip[2] = sender.sin_addr.S_un.S_un_b.s_b3;
    connection->ip[3] = sender.sin_addr.S_un.S_un_b.s_b4;
}

#endif