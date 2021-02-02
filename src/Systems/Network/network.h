#pragma once
#include "psp2/net/net.h"
#include "psp2/net/netctl.h"
#include <string>

class CNetwork
{
    public:
    CNetwork();
    ~CNetwork();

    void Init();
    void Connect(std::string ip_addr);
    void CreateServer();

    void Listen();

    bool Server() {return m_server_socket != -1;};
    bool Client() {return m_client_socket != -1;};

    private:
    int m_server_socket = -1;
    int m_client_socket = -1;

    SceNetCtlInfo m_info;
    SceNetInAddr m_local_address;

    SceNetSockaddrIn m_server_addr;
};