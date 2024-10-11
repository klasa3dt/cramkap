#include "Socket.hh"

#include <string>
#include <assert.h>

#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <winsock.h>

struct NetworkInternal
{
    WSAData wsadata;

    NetworkInternal()
    {

        int result;

        result = WSAStartup(MAKEWORD(2, 2), &wsadata);
        if (result)
        {
            int error = WSAGetLastError();
        }

    }
    ~NetworkInternal()
    {
        WSACleanup();
    }
};

static NetworkInternal internal;

bool CreateSocket(Socket* sock)
{
    sock->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock->handle == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        return false;
    }

    return true;
}

const char* GetMyIp() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    struct hostent* host = gethostbyname(hostname);
    if (!host) {
        return "Nie ma skibidi";
    }

    const char* ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
    return ip;
}

Connection ConvertToConnection(const char* ip) {
    sockaddr_in sa;
    inet_pton (AF_INET, ip, &sa.sin_addr);
    
    Connection conn;
    conn.ipaddress[0] = sa.sin_addr.S_un.S_un_b.s_b1;
    conn.ipaddress[1] = sa.sin_addr.S_un.S_un_b.s_b2;
    conn.ipaddress[2] = sa.sin_addr.S_un.S_un_b.s_b3;
    conn.ipaddress[3] = sa.sin_addr.S_un.S_un_b.s_b4;
    conn.port = DEFAULT_PORT;
    
    return conn;
}

bool Send(const Socket* sock, const Connection* connection, const void* data, size_t dataSize) {
    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(connection->port);
    target.sin_addr.S_un.S_un_b.s_b1 = connection->ipaddress[0];
    target.sin_addr.S_un.S_un_b.s_b2 = connection->ipaddress[1];
    target.sin_addr.S_un.S_un_b.s_b3 = connection->ipaddress[2];
    target.sin_addr.S_un.S_un_b.s_b4 = connection->ipaddress[3];

    int result = sendto(sock->handle, (const char*)data, (int)dataSize, 0, (const sockaddr*)&target, sizeof(target));
    if (result == SOCKET_ERROR) {
        int error = WSAGetLastError();
        return false;
    }

    return true;
}

bool ListenPort(const Socket* sock, uint16_t port) {
    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


    int result = bind(sock->handle, (const sockaddr*)& target, sizeof(target));
    if (result == SOCKET_ERROR) {
        int error = WSAGetLastError();
        return false;
    }

    return true;
}

bool CanReceive(const Socket* sock, long timeout_microseconds) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock->handle, &readfds);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_microseconds;
    int result = select(0, &readfds, NULL, NULL, &timeout);
    if (result < 0) {
        int error = WSAGetLastError();
        printf("skibidi error: %d\n\n", error);
        assert(0);
        return false;
    }

    return FD_ISSET(sock->handle, &readfds);
}

bool Receive(const Socket* sock, Connection* connection, void* data, size_t dataSize) {
    sockaddr_in sender;
    int targetsize = sizeof(sender);
    int result = recvfrom(sock->handle, (char*)data, (int)dataSize, 0, (sockaddr*)& sender, &targetsize);
    if (result == SOCKET_ERROR) {
        int error = WSAGetLastError();
        return false;
    }

    connection->port = htons(sender.sin_port);
    connection->ipaddress[0] = sender.sin_addr.S_un.S_un_b.s_b1;
    connection->ipaddress[1] = sender.sin_addr.S_un.S_un_b.s_b2;
    connection->ipaddress[2] = sender.sin_addr.S_un.S_un_b.s_b3;
    connection->ipaddress[3] = sender.sin_addr.S_un.S_un_b.s_b4;

    return true;
}