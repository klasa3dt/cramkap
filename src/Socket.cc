#include "Socket.hh"

#ifdef _WIN32
#include <winsock2.h>

struct Internal {
    Internal() {
        WSAData wsadata;
        WSAStartup(MAKEWORD(2, 2), &wsadata);
    }

    ~Internal() {
        WSACleanup();
    }
} static internal;

Socket::Socket() {
    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(s_port);
    target.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    m_handle = (void*)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind((SOCKET)m_handle, (sockaddr const*)&target, sizeof(target));
}

Socket::~Socket() {
    closesocket((SOCKET)m_handle);
}

bool Socket::canReceive() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET((SOCKET)m_handle, &fds);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;

    if (0 > select(0, &fds, nullptr, nullptr, &timeout)) {
        return false;
    }

    return FD_ISSET((SOCKET)m_handle, &fds);
}

bool Socket::receive(void* data, int dataSize, Connection* connection) {
    sockaddr_in sender;
    int size = sizeof(sender);

    if (SOCKET_ERROR == recvfrom((SOCKET)m_handle, (char*)data, dataSize, 0, (sockaddr*)&sender, &size)) {
        return false;
    }

    connection->ip[0] = sender.sin_addr.S_un.S_un_b.s_b1;
    connection->ip[1] = sender.sin_addr.S_un.S_un_b.s_b2;
    connection->ip[2] = sender.sin_addr.S_un.S_un_b.s_b3;
    connection->ip[3] = sender.sin_addr.S_un.S_un_b.s_b4;

    return true;
}

bool Socket::send(void const* data, int dataSize, Connection connection) {
    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(s_port);
    target.sin_addr.S_un.S_un_b.s_b1 = connection.ip[0];
    target.sin_addr.S_un.S_un_b.s_b2 = connection.ip[1];
    target.sin_addr.S_un.S_un_b.s_b3 = connection.ip[2];
    target.sin_addr.S_un.S_un_b.s_b4 = connection.ip[3];

    return SOCKET_ERROR != sendto((SOCKET)m_handle, (char const*)data, dataSize, 0, (sockaddr const*)&target, sizeof(target));
}

#endif